/* Include the controller definition */
#include "footbot_lamb.h"
/* Function definitions for XML parsing */
#include <argos3/core/utility/configuration/argos_configuration.h>
/* 2D vector definition */
#include <argos3/core/utility/math/vector2.h>

/****************************************/
/****************************************/
UInt8 CFootBotLamb::id_counter = 0;

CFootBotLamb::CFootBotLamb() :
    wheels_act(NULL),
    proxi_sens(NULL),
    pos_sens(NULL),
    rb_sens(NULL),
    rb_act(NULL),
    normal_speed(2.5f),
    alpha(2),
    beta(0.5),
    ping_interval(3),
    hp_interval(1)
    {
        CFootBotLamb::SetIdNum(this);
        rng = CRandom::CreateRNG( "argos" );
}

/****************************************/
/****************************************/

void CFootBotLamb::Init(TConfigurationNode& t_node) {
   // Get sensor/actuator handles
   wheels_act      = GetActuator<CCI_DifferentialSteeringActuator >("differential_steering");
   proxi_sens   = GetSensor  <CCI_FootBotProximitySensor >("footbot_proximity"    );
   pos_sens     = GetSensor  <CCI_PositioningSensor      >("positioning"          );
   rb_act       = GetActuator<CCI_RangeAndBearingActuator>("range_and_bearing"    );
   rb_sens      = GetSensor  <CCI_RangeAndBearingSensor  >("range_and_bearing"    );


   //Configuracion del experimento.
   //Es necesario saber los tics por segundo para calcular con que intervalos
   //se realizan las acciones
   TConfigurationNode experiment_conf = GetNode(CSimulator::GetInstance().GetConfigurationRoot(), "framework");
   experiment_conf = GetNode(experiment_conf,"experiment");
   UInt32 ticks_per_second;
   GetNodeAttribute(experiment_conf, "ticks_per_second", ticks_per_second);

   //Configuracion de la arena:
   //posicion de el agua, la comida y la cama
   TConfigurationNode arena_conf = GetNode(CSimulator::GetInstance().GetConfigurationRoot(), "arena");
   //TODO comprobar valores de los parametros
   GetNodeAttribute(arena_conf, "radius", radius);
   GetNodeAttribute(arena_conf, "water_pos", water_pos);
   GetNodeAttribute(arena_conf, "food_pos", food_pos);
   GetNodeAttribute(arena_conf, "bed_pos", bed_pos);

   // Configuracion del controller
   GetNodeAttributeOrDefault(t_node, "velocity", normal_speed, normal_speed);
   GetNodeAttributeOrDefault(t_node, "alpha", alpha, alpha);
   GetNodeAttributeOrDefault(t_node, "beta", beta, beta);
   GetNodeAttributeOrDefault(t_node, "ping_interval", ping_interval, ping_interval);
   ping_interval *= ticks_per_second;
   GetNodeAttributeOrDefault(t_node, "hp_dec_interval", hp_interval, hp_interval);
   hp_interval *= ticks_per_second;

   random_pos = CVector2(-3, -3);//FIXME

   //para el metodo de calcular gradiente
   sample_points[0] = CVector2(0, 0.25);
   sample_points[1] = CVector2(0, -0.25);
   sample_points[2] = CVector2(0.25,0);
   sample_points[3] = CVector2(-0.25, 0);

   bt = BrainTree::Builder()
            .composite<BrainTree::Selector>()
                // Secuencia para beber
                .composite<BrainTree::Sequence>()
                    .leaf<ConditionDepletion>(this, "water")
                    .composite<BrainTree::Selector>()
                        .composite<BrainTree::Sequence>()//Está en el sitio?
                            .leaf<ConditionIsInPlace>(this, "water")
                            .leaf<IncreaseHP>(this, "water")
                        .end()
                        .composite<BrainTree::Sequence>()//Ir al sitio
                            .leaf<GoTo>(this, "water")
                            // .composite<BrainTree::Selector>()
                            //     .leaf<ConditionAligned>(this, "water")
                            //     .leaf<Aligne>(this, "water")
                            // .end()
                            // .leaf<Advance>(this, "water")
                            // .leaf<IncreaseHP>(this, "water")
                        .end()
                    .end()
                .end()//Fin de sequencia para beber
           .end()
           .build();

   Reset();
}
//TODO el reinicio no funciona correctamente, es por el arbol
void CFootBotLamb::Reset() {
    mess_count = 0;
    clear_message = false;
    // rb_act->ClearData();
    ping_timer = ping_interval;
    hp_timer = hp_interval;

    neightbors.clear();
    water = HP_STAT_BAD;
    // water = HP_STAT_FULL; //FIXME
    food = HP_STAT_FULL;
    energy = HP_STAT_FULL;
}

/****************************************/
/****************************************/

void CFootBotLamb::ControlStep() {

    pos_readings = pos_sens->GetReading();
    pos = CVector2(pos_readings.Position.GetX(), pos_readings.Position.GetY());
    pos_readings.Orientation.ToEulerAngles(rot.z, rot.y, rot.x);

    proxi_readings = proxi_sens->GetReadings();

    //actualizacion del arbol de decision
    bt.update();

    //decremento de los HP
    if(--hp_timer <=0){
        if (water > 0)  water --;
        if (food > 0)  food --;
        if (energy > 0)  energy --;
        hp_timer = hp_interval;
    }

    // RLOG << food << endl;

    //FIXME pone a 0 el mensaje de salida, es un apaño,
    //no soy capaz de que deje de enviarse en cada step
    if(clear_message){
        rb_act->ClearData();
        clear_message = false;
    }

    if(--ping_timer <=0){
        Ping();
        ping_timer = ping_interval;
    }
    PollMessages();
}

void CFootBotLamb::TurnLeft()
{ wheels_act->SetLinearVelocity(- normal_speed, normal_speed); }

void CFootBotLamb::TurnRight()
{ wheels_act->SetLinearVelocity(normal_speed, - normal_speed); }

void CFootBotLamb::MoveForward()
{ wheels_act->SetLinearVelocity(normal_speed, normal_speed); }

void CFootBotLamb::Stop()
{ wheels_act->SetLinearVelocity(0.0f, 0.0f); }

void CFootBotLamb::Ping(){
    neightbors.clear();

    UInt8 *data_b = new UInt8[26];
    data_b[0] = id_num;
    data_b[1] = (UInt8) CODE_PING;

    rb_act->SetData(CByteArray(data_b, 26));
    delete data_b;
    clear_message = true; //para enviarlo una sola vez
}

//Prepara el mensaje para enviar la posición actual cuando acabe el control step
void CFootBotLamb::SendPosition(){

    CByteArray mess_data; //TODO alojamiento dinamico de memoria
    mess_data << id_num;
    mess_data << (UInt8) CODE_PING_REPLY;
    mess_data << pos.GetX();
    mess_data << pos.GetY();
    rb_act->SetData(mess_data);

    clear_message = true; //para enviarlo una sola vez
}


void CFootBotLamb::PollMessages(){
    CCI_RangeAndBearingSensor::TReadings messages = rb_sens->GetReadings();
    CCI_RangeAndBearingSensor::TReadings::iterator m;
    for (m = messages.begin() ; m != messages.end(); ++m){
        switch (m->Data[1]) {
            case CODE_PING:
                SendPosition();
                //Aunque se llame varias veces en el bucle solo se envia una vez
                break;
            case CODE_PING_REPLY:
                // RLOG<<" recibe de "<<m->Data[0];
                Neightbor_Info n = neightbors[m->Data.PopFront<UInt8>()];
                m->Data.PopFront<UInt8>(); //descartar el codigo del mensaje (Data[1])
                n.pos.SetX(m->Data.PopFront<Real>());
                n.pos.SetY(m->Data.PopFront<Real>());
                n.range = m->Range;
                n.bearing = m->HorizontalBearing;


                // LOG<<", pos: ("<< n.pos.GetX()<<", " <<n.pos.GetY()<<"), range: " << n.range<< ", bearing: "<< n.bearing<<")\n";
                // break;
        }
    }
    // if(messages.size() > 0)
    //     RLOG<<" tiene "<<neightbors.size()<<" vecinos\n";
}

//Calculo del vector gradiente usando Artificial Potential Fields
CVector2 CFootBotLamb::CalculateGradient(CVector2 *target){
    // obtenemos las vectores que representan la distancia y direccion a los obstaculos
    //desde el centro del robot
    vector<CVector2> obstacles;
    for(size_t i = 0; i < proxi_readings.size(); i++){
        if(proxi_readings[i].Value > 0){
            //distancia desde el sensors en metros
            Real dis = -log(proxi_readings[i].Value)/10;
            dis += 0.085036758; // + el radio del robot
            obstacles.push_back(CVector2(dis , proxi_readings[i].Angle+ CRadians::PI_OVER_TWO));
        }
    }


    //Calculo de la fuerza repulsiva en el centro del robot
    Real f_rep = 0;
    if(obstacles.size() > 0){
        Real min_dis = obstacles[0].Length();
        for(size_t i = 1; i<obstacles.size(); i++){
            if(obstacles[1].Length() < min_dis)
                min_dis = obstacles[1].Length();
        }
        //0,5 es la maxima distancia medible TODO hardcoded
        f_rep =  alpha *((1/min_dis)-(1/0.5));
    }
    //calculo de la fuerza atractiva y suma de ambas
    Real f_atr =  beta *(pos-(*target)).SquareLength();
    Real f = f_rep + f_atr;


    //Calculo de la fuerza repulsiva en 4 puntos alrededor del robot(sample_points)
    Real f_rep_sp[4], f_atr_sp[4], f_sp[4];
    for (size_t i = 0; i < 4; i++){
        f_rep_sp[i] = 0;
        if(obstacles.size() > 0){
            Real min_dis = (obstacles[0]-sample_points[i]).Length();
            for(size_t j = 1; j<obstacles.size(); j++){
                Real dis = (obstacles[i]-sample_points[i]).Length();
                if(dis < min_dis)
                    min_dis = dis;
            }
            //0,5 es la maxima distancia medible TODO hardcoded
            f_rep_sp[i] =  alpha *((1/min_dis)-(1/0.5));
        }
        //calculo de la fuerza atractiva y suma de ambas
        f_atr_sp[i] =  beta *(pos+sample_points[i]-(*target)).SquareLength();
        f_sp[i] = f_rep_sp[i] + f_atr_sp[i];
    }

    CVector2 gradient = CVector2(0,0);
    for(size_t i = 0; i < 4 ; i++){
        gradient += (f - f_sp[i])*sample_points[i].Normalize();
    }

    return gradient;
}

/****************************************/
/****************************************/

HPState CFootBotLamb::getHPState(UInt32 health_stat){
    if (health_stat > HP_STAT_BAD)
        return HPState::GOOD;
    else if (health_stat > HP_STAT_CRITIC)
        return HPState::BAD;
    else
        return HPState::CRITIC;
}

bool CFootBotLamb::isInPlace(CVector2 point){
    Real dis = (pos - point).Length();
    if( dis < radius)
        return true;
    else
        return false;
}

void CFootBotLamb::SetIdNum(CFootBotLamb* robot){
    robot->id_num = id_counter ++;
}


/*******************************************************
********************************************************
*******************************************************/
//Metodos update de los nodos del behavior trees
CFootBotLamb::NodeFootBot::Status CFootBotLamb::IncreaseHP::update(){
    *health_stat = HP_STAT_FULL;
    return Status::Success;
}


CFootBotLamb::NodeFootBot::Status CFootBotLamb::GoTo::update(){
    CVector2  v = lamb->CalculateGradient(target_pos);
    LOG<< v<< endl;
    CRadians angle_to_target(0);
    if(angle_to_target.GetAbsoluteValue() > ANGLE_THRESHOLD){
        if(angle_to_target < CRadians::ZERO){
            lamb->TurnRight();
            return Status::Running;
        }
        else {
            lamb->TurnLeft();
            return Status::Running;
        }
    }
    else{
        lamb->Stop();
        return Status::Success;
    }
}


CFootBotLamb::NodeFootBot::Status CFootBotLamb::Advance::update(){
    if(lamb->isInPlace(*target_pos)) {
        lamb->Stop();
        return Status::Success;
    } else{
        lamb->MoveForward();
        return Status::Running;
    }
}


CFootBotLamb::NodeFootBot::Status CFootBotLamb::SelectRandomPos::update(){
    lamb->random_pos = CVector2(lamb->rng->Uniform(CRange<Real>(-3,3)),
                        lamb->rng->Uniform(CRange<Real>(-3,3)));
    LOG<<lamb->random_pos<<endl;
    return Status::Success;
}


CFootBotLamb::NodeFootBot::Status CFootBotLamb::ConditionDepletion::update(){
    // LOG<<lamb->GetId()<<" w : "<< lamb->water
    //     <<" f: "<<lamb->food
    //     <<" e: "<<lamb->energy<<endl;
    if(lamb->getHPState(*health_stat) != HPState::GOOD)
        return Status::Success;
    else
        return Status::Failure;
}


CFootBotLamb::NodeFootBot::Status CFootBotLamb::ConditionIsInPlace::update(){
    if(lamb->isInPlace(*target_pos))
        return Status::Success;
    else
        return Status::Failure;
}

CFootBotLamb::NodeFootBot::Status CFootBotLamb::ConditionAligned::update(){
    CRadians angle_to_target = (*target_pos - lamb->pos).Angle() - lamb->rot.z;

    if(angle_to_target.GetAbsoluteValue() > ANGLE_THRESHOLD)
        return Status::Failure;
    else
        return Status::Success;
}

//TODO borrar este nodo del arbol mas adelante
CFootBotLamb::NodeFootBot::Status CFootBotLamb::PrintNode::update(){
    // LOG<<lamb->GetId()<<" ALINEADO"<<endl;
    return Status::Success;
}

/* Asocia la clase a la cadena de caracteres para poder usar el controller desde el archivo de configuracion
 */
REGISTER_CONTROLLER(CFootBotLamb, "footbot_lamb_controller")
