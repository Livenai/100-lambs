#include "lamb.h"
#include <argos3/core/utility/configuration/argos_configuration.h>
#include <argos3/core/utility/math/vector2.h>

/****************************************/
/****************************************/
UInt8 CLamb::id_counter = 0;
vector<CVector2> CLamb::water_troughs;
vector<CVector2> CLamb::food_troughs;

CLamb::CLamb() :
    wheels_act(NULL),
    proxi_sens(NULL),
    pos_sens(NULL),
    rb_sens(NULL),
    rb_act(NULL),
    speed(2.5f),
    alpha(2),
    beta(0.5),
    ping_interval(3),
    bt_interval(0.5),
    show_debug(false),
    threshold_distance(0.5),
    random_pos(),
    bed_pos()
    {
        CLamb::SetIdNum(this);
        rng = CRandom::CreateRNG( "argos" );

        colors[water]  = CColor::BLUE;
        colors[food]   = CColor::RED;
        colors[rest]   = CColor::GREEN;
        colors[social] = CColor::MAGENTA;
        colors[walk]   = CColor::YELLOW;
}

/****************************************/
/****************************************/

void CLamb::Init(TConfigurationNode& t_node) {

   // Get sensor/actuator handles
   wheels_act = GetActuator<CCI_DifferentialSteeringActuator >("differential_steering");
   leds       = GetActuator<CCI_LEDsActuator                 >("leds"                 );
   proxi_sens = GetSensor  <CCI_LambBotProximitySensor       >("lambbot_proximity"    );
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

   //TODO eliminar el concepto de bed_pos,
   GetNodeAttribute(arena_conf, "bed_pos", bed_pos);

   // Configuracion del controller
   GetNodeAttributeOrDefault(t_node, "linear_speed", speed, speed);
   GetNodeAttributeOrDefault(t_node, "alpha", alpha, alpha);
   GetNodeAttributeOrDefault(t_node, "beta", beta, beta);
   GetNodeAttributeOrDefault(t_node, "ping_interval", ping_interval, ping_interval);
   ping_interval *= ticks_per_second;
   GetNodeAttributeOrDefault(t_node, "bt_interval", bt_interval, bt_interval);
   bt_interval *= ticks_per_second;
   GetNodeAttributeOrDefault(t_node, "threshold_distance", threshold_distance, threshold_distance);

   //configuracion del arbol de comportamiento
   UInt32 seed = rng->Uniform(CRange<UInt32>(0,10000));
   bt = BrainTree::Builder()
                    .composite<BrainTree::RandomWeightedSelector>((double *)&transition_matrix, 4, seed)
                        // Secuencia para beber
                        .composite<BrainTree::Sequence>()
                            .leaf<SetLedColor>(this, water)
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
                            .leaf<SetLedColor>(this, food)
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
                            .leaf<SetLedColor>(this, rest)
                            .composite<BrainTree::ActiveSelector>()
                                .composite<BrainTree::Sequence>()
                                    .leaf<CanRest>(this)//esta al lado de la cama?
                                    .leaf<Rest>(this)
                                .end()
                                .leaf<GoToRest>(this)
                            .end()
                        .end()//Fin de sequencia para dormir
                        .composite<BrainTree::Sequence>()//sequencia para pasear
                            .leaf<SetLedColor>(this, walk)
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


//TODO habría que reiniciar el arbol tambien?
void CLamb::Reset() {
    pos_readings = pos_sens->GetReading();
    pos = CVector2(pos_readings.Position.GetX(), pos_readings.Position.GetY());
    mess_count = 0;
    clear_message = false;
    // rb_act->ClearData();
    ping_timer = ping_interval;
    bt_timer = (id_num%(UInt8)bt_interval)+1;

    neightbors.clear();

    for(Stat_type s: {water, food, rest, social})
        stats[s] = 0;

}


void CLamb::ShowDebugInformation(bool show){
    this->show_debug = show;
}

/****************************************/
/****************************************/

void CLamb::ControlStep() {

    pos_readings = pos_sens->GetReading();
    pos = CVector2(pos_readings.Position.GetX(), pos_readings.Position.GetY());
    pos_readings.Orientation.ToEulerAngles(rot.z, rot.y, rot.x);

    proxi_readings = proxi_sens->GetReadings();

    //actualizacion del arbol de decision
    if(--bt_timer <=0){
        bt.update();
        bt_timer = bt_interval;
    }


    //Pone a 0 el mensaje de salida despues de que se halla enviado
    //Es un apaño, no soy capaz de que deje de enviarse en cada step
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
        RLOG <<"w: "<< stats[water]<<", f: "<<stats[food]<<", r: "<< stats[rest]
        <<", walk: "<< stats[walk]<<"\n";
        LOG<<"priority: "<<current_state<<endl;
        // LOG<<"random_pos: "<<random_pos<<endl;
        // RLOG << GetCorrectedPos()<<endl;
    }
}

void CLamb::TurnLeft()
{ wheels_act->SetLinearVelocity(speed/2, speed); }

void CLamb::TurnRight()
{ wheels_act->SetLinearVelocity(speed, speed/2); }

void CLamb::MoveForward()
{ wheels_act->SetLinearVelocity(speed, speed); }

void CLamb::Stop()
{ wheels_act->SetLinearVelocity(0.0f, 0.0f); }

void CLamb::Ping(){
    neightbors.clear();

    UInt8 *data_b = new UInt8[26];
    data_b[0] = id_num;
    data_b[1] = (UInt8) CODE_PING;

    rb_act->SetData(CByteArray(data_b, 26));
    delete data_b;
    clear_message = true; //para enviarlo una sola vez
}

//Prepara el mensaje para enviar la posición actual cuando acabe el control step
void CLamb::SendPosition(){

    CByteArray mess_data;
    mess_data << id_num;
    mess_data << (UInt8) CODE_PING_REPLY;
    mess_data << pos.GetX();
    mess_data << pos.GetY();
    rb_act->SetData(mess_data);

    clear_message = true; //para enviarlo una sola vez
}


void CLamb::PollMessages(){
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
CVector2 CLamb::CalculateDirection(CVector2 target){
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


// TODO añadir giros mas pequeños para prevenir oscilacion?
void CLamb::GoToPoint(CVector2 target) {
    if(show_debug){
        LOG<<"POS:"<< pos<<endl;
        LOG<<"TARGET POS:"<< target<<endl;
    }
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

//FIXME calcular como distancia de un punto a un rectangulo
Real CLamb::distanceToTrough(CVector2 trough){
    return (pos - trough).Length();
}

Real CLamb::distanceToPoint(CVector2 point){
    return (pos - point).Length();
}


CVector2 CLamb::GetClosestTrough(Stat_type stat){
    vector<CVector2> *targets;
    if (stat == water)
        targets = &water_troughs;
    else
        targets = &food_troughs;

    CVector2 closest = (*targets)[0];
    Real min = (closest - pos).Length();
    for(auto t: *targets){
        if((t - pos).Length() < min)
            closest = t;
    }
    if(show_debug)
        LOG<<"CLOSEST_POS: "<<closest<<endl;

    return closest;
}

//TODO más adelante devolvera la posicion mas cercana donde haya visto dormir a
//otro cordero
CVector2 CLamb::GetClosestRestingPlace(){
    return bed_pos;
}


void CLamb::SetIdNum(CLamb* robot){
    robot->id_num = id_counter ++;
}

void CLamb::SetTroughs(){
    CSpace::TMapPerType troughs = CSimulator::GetInstance().GetSpace().GetEntitiesByType("trough");
    for(auto it: troughs){
        CTroughEntity *t = any_cast<CTroughEntity*>(it.second);
        if(t->GetType() == Trough_type::WATER)
            water_troughs.push_back(t->GetPos());
        else
            food_troughs.push_back(t->GetPos());
    }
}

//Convierte las cordenadas de la arena en las del corral
CVector2 CLamb::GetCorrectedPos(){

    Real x = pos.GetX()*2000;
    Real y = -pos.GetY()*2000;

    return CVector2(x, y);
}


void CLamb::Destroy(){
    CLamb::id_counter --;
}

//Nodos del behavior trees definidos en otro archivo
//lambbot_lamb_bt_nodes.cpp

/* Asocia la clase a la cadena de caracteres para poder usar el controller desde el archivo de configuracion
 */
REGISTER_CONTROLLER(CLamb, "lamb_controller")
