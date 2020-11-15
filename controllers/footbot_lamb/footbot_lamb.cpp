#include "footbot_lamb.h"
#include <argos3/core/utility/configuration/argos_configuration.h>
#include <argos3/core/utility/math/vector2.h>

/****************************************/
/****************************************/
UInt8 CFootBotLamb::id_counter = 0;
vector<CVector2> CFootBotLamb::water_troughs;
vector<CVector2> CFootBotLamb::food_troughs;
// vector<CVector2> CFootBotLamb::beds;

CFootBotLamb::CFootBotLamb() :
    wheels_act(NULL),
    proxi_sens(NULL),
    pos_sens(NULL),
    rb_sens(NULL),
    rb_act(NULL),
    speed(2.5f),
    alpha(2),
    beta(0.5),
    ping_interval(3),
    hp_interval(1),
    bt_interval(0.5),
    show_debug(false),
    random_pos()
    {
        CFootBotLamb::SetIdNum(this);
        rng = CRandom::CreateRNG( "argos" );

        colors[water]  = CColor::BLUE;
        colors[food]   = CColor::RED;
        colors[rest]   = CColor::MAGENTA;
        colors[social] = CColor::GREEN;
        colors[walk]   = CColor::YELLOW;
}

/****************************************/
/****************************************/

void CFootBotLamb::Init(TConfigurationNode& t_node) {

   // Get sensor/actuator handles
   wheels_act = GetActuator<CCI_DifferentialSteeringActuator >("differential_steering");
   leds       = GetActuator<CCI_LEDsActuator                 >("leds"                 );
   proxi_sens = GetSensor  <CCI_FootBotProximitySensor       >("footbot_proximity"    );
   pos_sens   = GetSensor  <CCI_PositioningSensor            >("positioning"          );
   rb_act     = GetActuator<CCI_RangeAndBearingActuator      >("range_and_bearing"    );
   rb_sens    = GetSensor  <CCI_RangeAndBearingSensor        >("range_and_bearing"    );


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

   //FIXME la cama se configura aqui pero los comederos y bebederos en otro sitio
   //puede paracer poco organizado
   CVector2 bed_pos;
   GetNodeAttribute(arena_conf, "bed_pos", bed_pos);
   beds.push_back(bed_pos);

   // Configuracion del controller
   GetNodeAttributeOrDefault(t_node, "linear_speed", speed, speed);
   GetNodeAttributeOrDefault(t_node, "alpha", alpha, alpha);
   GetNodeAttributeOrDefault(t_node, "beta", beta, beta);
   GetNodeAttributeOrDefault(t_node, "ping_interval", ping_interval, ping_interval);
   ping_interval *= ticks_per_second;
   GetNodeAttributeOrDefault(t_node, "hp_dec_interval", hp_interval, hp_interval);
   hp_interval *= ticks_per_second;
   GetNodeAttributeOrDefault(t_node, "bt_interval", bt_interval, bt_interval);
   bt_interval *= ticks_per_second;

   //configuracion del arbol de comportamiento
   bt = BrainTree::Builder()
                    .composite<BrainTree::ActiveSelector>()
                        // Secuencia para beber
                        .composite<BrainTree::Sequence>()
                            .leaf<NeedWater>(this) //Tiene sed?
                            .composite<BrainTree::ActiveSelector>()
                                .composite<BrainTree::Sequence>()
                                    .leaf<CanDrink>(this)//esta al lado del bebedero
                                    .leaf<Drink>(this)
                                .end()
                                .leaf<GoToWater>(this)
                            .end()
                        .end()//Fin de sequencia para beber
                        // Secuencia para comer
                        .composite<BrainTree::Sequence>()
                            .leaf<NeedFood>(this) //Tiene hambre?
                            .composite<BrainTree::ActiveSelector>()
                                .composite<BrainTree::Sequence>()
                                    .leaf<CanEat>(this)//esta al lado del comedero
                                    .leaf<Eat>(this)
                                .end()
                                .leaf<GoToFood>(this)
                            .end()
                        .end()//Fin de sequencia para comer
                        // Secuencia para dormir
                        .composite<BrainTree::Sequence>()
                            .leaf<NeedRest>(this) //Tiene sueño?
                            .composite<BrainTree::ActiveSelector>()
                                .composite<BrainTree::Sequence>()
                                    .leaf<CanSleep>(this)//esta al lado de la cama?
                                    .leaf<Sleep>(this)
                                .end()
                                .leaf<GoToBed>(this)
                            .end()
                        .end()//Fin de sequencia para dormir
                        .composite<BrainTree::Sequence>()//sequencia para pasear
                            .leaf<SelectRandomPos>(this)
                            .composite<BrainTree::ActiveSelector>()
                                .leaf<IsAtRandomPos>(this)
                                .leaf<GoToRandomPos>(this)
                            .end()
                        .end()//Fin de la sequencia para pasear
                    .end()
                .build();

   Reset();
}


//TODO habría que reiniciar el arbol tambien
void CFootBotLamb::Reset() {
    mess_count = 0;
    clear_message = false;
    // rb_act->ClearData();
    ping_timer = ping_interval;
    hp_timer = hp_interval;
    bt_timer = (id_num%(UInt8)bt_interval)+1;

    neightbors.clear();

    for(Stat_type s: {water, food, rest, social})
        stats[s] = rng->Uniform(CRange<UInt32>(0,STAT_FULL));

}


void CFootBotLamb::ShowDebugInformation(bool show){
    this->show_debug = show;
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
        UpdatePriority();
        bt.update();
        bt_timer = bt_interval;
    }

    //decremento de los HP
    if(--hp_timer <=0){
        if (stats[water]  > 0)  stats[water]  --;
        if (stats[food]   > 0)  stats[food]   --;
        if (stats[rest]   > 0)  stats[rest]   --;
        if (stats[social] > 0)  stats[social] --;
        if (stats[walk]   > 0)  stats[walk]   --;
        hp_timer = hp_interval;
    }

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

    if(show_debug){
        // RLOG <<"w: "<< stats[water]<<", f: "<<stats[food]<<", r: "<< stats[rest]
        // <<", walk: "<< stats[walk]<<"\n";
        // LOG<<"priority: "<<current_priority<<endl;
        // LOG<<"random_pos: "<<random_pos<<endl;
        RLOG << GetCorrectedPos()<<endl;
    }
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

    CByteArray mess_data;
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
    for(m = messages.begin() ; m != messages.end(); ++m){
        switch (m->Data[1]) {
            case CODE_PING:
                //SendPosition prepara el mensaje que se enviara en el proximo tick
                //no importa que se llame más de una vez en el bucle, solo se envia una vez
                SendPosition();
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


void CFootBotLamb::GoTo(CVector2 target) {
    CVector2  v = CalculateDirection(target);
    CRadians angle_remaining = (v.Angle() - rot.z).SignedNormalize();

    // RLOG<<"Angulo objetivo: "<< v.Angle().GetValue() * CRadians::RADIANS_TO_DEGREES<< endl;
    // RLOG<<"Aungulo restante: "<< angle_remaining.GetValue() * CRadians::RADIANS_TO_DEGREES<< endl;
    // RLOG<<"vector: "<< v<< endl;

    if(angle_remaining.GetAbsoluteValue() > ANGLE_THRESHOLD){
        if(angle_remaining < CRadians::ZERO)
            TurnRight();
        else
            TurnLeft();
    }
    else
        MoveForward();
}

/****************************************/
/****************************************/

void CFootBotLamb::UpdatePriority(){

    if( stats[current_priority] >= STAT_FULL ||
        (stats[walk] <= STAT_CRITIC && stats[current_priority] > STAT_HIGH)){
        current_priority = walk;
    }

    for(Stat_type s : {water, food, rest}){
        if( stats[s] <= STAT_BAD){
            current_priority = s;
            break;
        }
    }

    for(Stat_type s : {water, food, rest}){
        if( stats[s] <= STAT_CRITIC){
            current_priority = s;
            break;
        }
    }

    leds->SetAllColors(colors[current_priority]);
}

//FIXME muy cutrillo
bool CFootBotLamb::IsInPlace(CVector2 target){
    Real dis = (pos - target).Length();
    return dis < radius;
}


CVector2 CFootBotLamb::GetClosestPoint(vector<CVector2> *targets){
    CVector2 closest = (*targets)[0];
    Real min = (closest - pos).Length();
    for(auto t: *targets){
        if((t - pos).Length() < min)
            closest = t;
    }
    current_target = closest;
    return closest;
}


void CFootBotLamb::SetIdNum(CFootBotLamb* robot){
    robot->id_num = id_counter ++;
}

void CFootBotLamb::SetTroughs(){
    CSpace::TMapPerType troughs = CSimulator::GetInstance().GetSpace().GetEntitiesByType("trough");
    for(auto it: troughs){
        CTroughEntity *t = any_cast<CTroughEntity*>(it.second);
        if(t->GetType() == Trough_type::WATER)
            water_troughs.push_back(t->GetPos());
        else
            food_troughs.push_back(t->GetPos());
    }
}

CVector2 CFootBotLamb::GetCorrectedPos(){

    Real x = pos.GetX()*2000;
    Real y = -pos.GetY()*2000;

    return CVector2(x, y);
}

CVector3 CFootBotLamb::GetDirection(){
    return CVector3(0,1,0);
}

void CFootBotLamb::Destroy(){
    CFootBotLamb::id_counter --;
}

//Nodos del behavior trees definidos en otro archivo
//footbot_lamb_bt_nodes.cpp

/* Asocia la clase a la cadena de caracteres para poder usar el controller desde el archivo de configuracion
 */
REGISTER_CONTROLLER(CFootBotLamb, "footbot_lamb_controller")
