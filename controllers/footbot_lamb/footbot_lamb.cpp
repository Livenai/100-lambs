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
    speed(2.5f),
    rot_speed(2.5f),
    alpha(2),
    beta(0.5),
    ping_interval(3),
    hp_interval(1),
    bt_interval(0.5)
    {
        CFootBotLamb::SetIdNum(this);
        rng = CRandom::CreateRNG( "argos" );
}

/****************************************/
/****************************************/

void CFootBotLamb::Init(TConfigurationNode& t_node) {

   // Get sensor/actuator handles
   wheels_act   = GetActuator<CCI_DifferentialSteeringActuator >("differential_steering");
   proxi_sens   = GetSensor  <CCI_FootBotProximitySensor       >("footbot_proximity"    );
   pos_sens     = GetSensor  <CCI_PositioningSensor            >("positioning"          );
   rb_act       = GetActuator<CCI_RangeAndBearingActuator      >("range_and_bearing"    );
   rb_sens      = GetSensor  <CCI_RangeAndBearingSensor        >("range_and_bearing"    );


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
   GetNodeAttributeOrDefault(t_node, "linear_speed", speed, speed);
   GetNodeAttributeOrDefault(t_node, "rot_speed", rot_speed, rot_speed);
   GetNodeAttributeOrDefault(t_node, "alpha", alpha, alpha);
   GetNodeAttributeOrDefault(t_node, "beta", beta, beta);
   GetNodeAttributeOrDefault(t_node, "ping_interval", ping_interval, ping_interval);
   ping_interval *= ticks_per_second;
   GetNodeAttributeOrDefault(t_node, "hp_dec_interval", hp_interval, hp_interval);
   hp_interval *= ticks_per_second;
   GetNodeAttributeOrDefault(t_node, "bt_interval", bt_interval, bt_interval);
   bt_interval *= ticks_per_second;

//TODO borrar esta caca
   switch (rng->Uniform(CRange<SInt32>(0,3))) {
        case 1:
            water_pos = food_pos;
        break;
        case 2:
            water_pos = bed_pos;
        break;
   }

   //TODO hardcoded
   proxi_limit = 0.25;
   robot_radius = 0.085036758f;

   CRadians offset = CRadians::TWO_PI / NUM_SAMPLE_POINTS;
   CVector2 sp(0, robot_radius);
   for(size_t i = 0; i < NUM_SAMPLE_POINTS; i++){
       sample_points.push_back(sp);
       sample_points_norm.push_back(sp);
       sample_points_norm[i].Normalize();
       sp.Rotate(offset);
   }


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
//TODO el reinicio no funciona correctamente siempre, es por el arbol
void CFootBotLamb::Reset() {
    mess_count = 0;
    clear_message = false;
    // rb_act->ClearData();
    ping_timer = ping_interval;
    hp_timer = hp_interval;
    bt_timer = (id_num%(UInt8)bt_interval)+1;

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
    if(--bt_timer <=0){
        bt.update();
        bt_timer = bt_interval;
        // LOG <<id_num<<endl;
    }

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
{ wheels_act->SetLinearVelocity(speed/2, speed); }

void CFootBotLamb::TurnRight()
{ wheels_act->SetLinearVelocity(speed, speed/2); }

void CFootBotLamb::MoveForward()
{ wheels_act->SetLinearVelocity(speed, speed); }

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

//Calculo del vector de direccion teniendo en cuenta la direccion del objetivo y la de
//los obstaculos a la vista. Alternativa simplificada a CalculateGradient de APF
CVector2 CFootBotLamb::CalculateDirection(CVector2 target){
    // inicialmente la direccion es la del objetivo
    CVector2 direction = target - pos;
    direction.Normalize();
    direction *= beta;
    //a la direccion inicial se le suman vectores en direccion contraria a los obstaculos
    //detectados
    for(auto r :proxi_readings){
        if(r.Value > 0){
            //resta de un vector inversamente proporcional a la distacia
            direction -= CVector2(r.Value*alpha , r.Angle + rot.z);
        }
    }
    return direction;
}

/****************************************/
/****************************************/

HPState CFootBotLamb::GetHPState(UInt32 health_stat){
    if (health_stat > HP_STAT_BAD)
        return HPState::GOOD;
    else if (health_stat > HP_STAT_CRITIC)
        return HPState::BAD;
    else
        return HPState::CRITIC;
}

bool CFootBotLamb::IsInPlace(CVector2 point){
    Real dis = (pos - point).Length();
    if( dis < radius)
        return true;
    else
        return false;
}

void CFootBotLamb::SetIdNum(CFootBotLamb* robot){
    robot->id_num = id_counter ++;

}

CVector3 CFootBotLamb::GetPos(){
    return CVector3(pos.GetX(),pos.GetY(), 0);
}

CVector3 CFootBotLamb::GetDirection(){
    return CVector3(0,1,0);
}

void CFootBotLamb::Destroy(){
    CFootBotLamb::id_counter --;
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
    if(lamb->IsInPlace(*target_pos)){
        lamb->Stop();
        return Status::Success;
    }

    CVector2  v = lamb->CalculateDirection(*target_pos);
    CRadians angle_remaining = (v.Angle() - lamb->rot.z).SignedNormalize();

    LOG<<"Angulo objetivo: "<< v.Angle().GetValue() * CRadians::RADIANS_TO_DEGREES<< endl;
    LOG<<"Aungulo restante: "<< angle_remaining.GetValue() * CRadians::RADIANS_TO_DEGREES<< endl;
    LOG<<"vector: "<< v<< endl;

    if(angle_remaining.GetAbsoluteValue() > ANGLE_THRESHOLD){
        if(angle_remaining < CRadians::ZERO){
            lamb->TurnRight();
            // LOG<<"->"<<endl;
            return Status::Running;
        }
        else {
            lamb->TurnLeft();
            // LOG<<"<-"<<endl;
            return Status::Running;
        }
    }
    else{
        lamb->MoveForward();
        // LOG<<"^"<<endl;
        return Status::Running;
    }
}


CFootBotLamb::NodeFootBot::Status CFootBotLamb::Advance::update(){
    if(lamb->IsInPlace(*target_pos)) {
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
    if(lamb->GetHPState(*health_stat) != HPState::GOOD)
        return Status::Success;
    else
        return Status::Failure;
}


CFootBotLamb::NodeFootBot::Status CFootBotLamb::ConditionIsInPlace::update(){
    if(lamb->IsInPlace(*target_pos))
        return Status::Success;
    else
        return Status::Failure;
}

CFootBotLamb::NodeFootBot::Status CFootBotLamb::ConditionAligned::update(){
    CRadians angle_remaining = (*target_pos - lamb->pos).Angle() - lamb->rot.z;

    if(angle_remaining.GetAbsoluteValue() > ANGLE_THRESHOLD)
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
