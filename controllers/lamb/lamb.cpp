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
    max_vel(30.0f),
    rot_vel(15.0f),
    speed_factor(1),
    alpha(2),
    beta(0.5),
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
   GetNodeAttributeOrDefault(t_node, "velocity", max_vel, max_vel);
   GetNodeAttributeOrDefault(t_node, "rot_velocity", rot_vel, rot_vel);
   GetNodeAttributeOrDefault(t_node, "alpha", alpha, alpha);
   GetNodeAttributeOrDefault(t_node, "beta", beta, beta);
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
                                .leaf<IsAtRandomPosition>(this)
                                .leaf<Walk>(this)
                            .end()
                            .leaf<Wait>(this)
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
    // rb_act->ClearData();
    //cada tick de la simulacion solo los individuos cuyo timer haya llegado a 0 ejecutaran el BT
    bt_timer = (id_num%(UInt8)bt_interval)+1;

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

    if(show_debug){
        // RLOG<<rot.z.GetValue()<<std::endl;

        LOG << "---------------\n";
        RLOG << "\nPosicion: ("<< pos<<")\n";
        string priority;
        switch (current_state) {
            case 0: priority = "beber"; break;
            case 1: priority = "comer"; break;
            case 2: priority = "descansar"; break;
            case 3: priority = "social ERROR"; break;
            case 4: priority = "pasear"; break;
            default: priority = "ERROR";
        }
        LOG<<"priority: "<<priority<<endl;
        //Estos contadores son para regular la duración de la actividad
        LOG <<"w: "<< stats[water]<<", f: "<<stats[food]<<", r: "<< stats[rest]
            <<", walk: "<< stats[walk]<<"\n";
        // LOG<<"random_pos: "<<random_pos<<endl;
    }

    SendPosition();
    PollMessages();

}


void CLamb::TurnLeft() {
     wheels_act->SetLinearVelocity(speed_factor*rot_vel/2, speed_factor*rot_vel);
 }
void CLamb::TurnRight() {
     wheels_act->SetLinearVelocity(speed_factor*rot_vel, speed_factor*rot_vel/2);
 }

void CLamb::MoveForward() {
     wheels_act->SetLinearVelocity(speed_factor*max_vel, speed_factor*max_vel);
  }

void CLamb::Stop()
{ wheels_act->SetLinearVelocity(0.0f, 0.0f); }

//Prepara el mensaje para enviar la posición actual cuando acabe el control step
void CLamb::SendPosition(){
    CByteArray mess_data;
    mess_data << id_num;
    mess_data << pos.GetX();
    mess_data << pos.GetY();
    mess_data << rot.z.GetValue();
    mess_data << speed_factor;
    rb_act->SetData(mess_data);
}


void CLamb::PollMessages(){
    neightbors.clear();
    CCI_RangeAndBearingSensor::TReadings messages = rb_sens->GetReadings();
    CCI_RangeAndBearingSensor::TReadings::iterator m;
    for(m = messages.begin() ; m != messages.end(); ++m){
        Neightbor_Info n;
        UInt8 id_neight = m->Data.PopFront<UInt8>();
        n.pos.SetX(m->Data.PopFront<Real>());
        n.pos.SetY(m->Data.PopFront<Real>());
        //se obtiene la direccion del vecino como un vector con magnitud 1
        n.vel = CVector2(1,0).Rotate(CRadians(m->Data.PopFront<Real>()));
        //se multiplica el vector direccion por la velocidad
        n.vel *= m->Data.PopFront<Real>();
        neightbors[id_neight] = n;
    }
        if(show_debug){
            LOG<< neightbors.size()<<" vecinos:\n";
            for (map<UInt8,Neightbor_Info>::iterator it=neightbors.begin(); it!=neightbors.end(); ++it){
                LOG<<"..id: "<<it->first << endl;
                LOG<<"....pos: ("<< it->second.pos.GetX()<<", " <<it->second.pos.GetY()<<")\n";
                LOG<<"....rot: "<< it->second.vel.Angle().GetValue()<<" rads\n";
                LOG<<"....vel: "<< it->second.vel.Length()<<"\n";
            }
        }
}

//Cálculo del vector de dirección teniendo en cuenta la direccion del objetivo y la de
//los obstaculos a la vista. Versión simplificada de Artificial Potential Fields
CVector2 CLamb::simplifiedAPF(CVector2 target){
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


// TODO añadir giros mas pequeños para prevenir oscilacion,
// o tal vez trastear con los parametros
void CLamb::GoToPoint(CVector2 target) {
    // if(show_debug){
    //     LOG<<"POS:"<< pos<<endl;
    //     LOG<<"TARGET POS:"<< target<<endl;
    // }
    CVector2  v = simplifiedAPF(target);
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
    // if(show_debug)
    //     LOG<<"CLOSEST_POS: "<<closest<<endl;

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
