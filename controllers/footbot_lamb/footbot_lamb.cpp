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
    m_pcWheels(NULL),
    m_pcProximity(NULL),
    m_pcPosSens(NULL),
    m_pcRBSens(NULL),
    m_pcRBAct(NULL),
    m_cAlpha(10.0f),
    m_fDelta(0.5f),
    m_fWheelVelocity(2.5f),
    m_cGoStraightAngleRange(-ToRadians(m_cAlpha),
                           ToRadians(m_cAlpha)),
    ping_interval(3),
    hp_interval(1),
    mess_count(0),
    clear_message(false),
    pos(),
    rot(),
    water( HP_STAT_FULL),
    food( HP_STAT_FULL),
    energy( HP_STAT_FULL) {
        CFootBotLamb::SetIdNum(this);
        rng = CRandom::CreateRNG( "argos" );
}

/****************************************/
/****************************************/

void CFootBotLamb::Init(TConfigurationNode& t_node) {
   // Get sensor/actuator handles
   m_pcWheels      = GetActuator<CCI_DifferentialSteeringActuator >("differential_steering");
   m_pcProximity   = GetSensor  <CCI_FootBotProximitySensor       >("footbot_proximity"    );
   m_pcPosSens     = GetSensor  <CCI_PositioningSensor            >("positioning"          );
   m_pcRBAct       = GetActuator<CCI_RangeAndBearingActuator      >("range_and_bearing"    );
   m_pcRBSens      = GetSensor  <CCI_RangeAndBearingSensor        >("range_and_bearing"    );


   //leyendo del archivo de configuración
   TConfigurationNode experiment_conf = GetNode(CSimulator::GetInstance().GetConfigurationRoot(), "framework");
   experiment_conf = GetNode(experiment_conf,"experiment");
   UInt32 ticks_per_second;
   GetNodeAttribute(experiment_conf, "ticks_per_second", ticks_per_second);

   // Parse the configuration file
   GetNodeAttributeOrDefault(t_node, "alpha", m_cAlpha, m_cAlpha);
   m_cGoStraightAngleRange.Set(-ToRadians(m_cAlpha), ToRadians(m_cAlpha));
   GetNodeAttributeOrDefault(t_node, "delta", m_fDelta, m_fDelta);
   GetNodeAttributeOrDefault(t_node, "velocity", m_fWheelVelocity, m_fWheelVelocity);


   GetNodeAttributeOrDefault(t_node, "ping_interval", ping_interval, ping_interval);
   ping_interval *= ticks_per_second;
   GetNodeAttributeOrDefault(t_node, "hp_dec_interval", hp_interval, hp_interval);
   hp_interval *= ticks_per_second;
   //TODO datos obligatorios
   TConfigurationNode arena_conf = GetNode(CSimulator::GetInstance().GetConfigurationRoot(), "arena");
   //TODO comprobar valores de los parametros
   GetNodeAttribute(arena_conf, "radius", radius);
   radius *= radius;
   GetNodeAttribute(arena_conf, "water_pos", water_pos);
   GetNodeAttribute(arena_conf, "food_pos", food_pos);
   GetNodeAttribute(arena_conf, "bed_pos", bed_pos);

   random_pos = CVector2(-3, -3);
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
                            .composite<BrainTree::Selector>()
                                .leaf<ConditionAligned>(this, "water")//TODO notAligned
                                .leaf<Aligne>(this, "water")
                            .end()
                            .leaf<Advance>(this, "water")
                            .leaf<IncreaseHP>(this, "water")
                        .end()
                    .end()
                .end()//Fin de sequencia para beber
                // Secuencia para comer
                .composite<BrainTree::Sequence>()
                    .leaf<ConditionDepletion>(this, "food")
                    .composite<BrainTree::Selector>()
                        .composite<BrainTree::Sequence>()//Está en el sitio?
                            .leaf<ConditionIsInPlace>(this, "food")
                            .leaf<IncreaseHP>(this, "food")
                        .end()
                        .composite<BrainTree::Sequence>()//Ir al sitio
                            .composite<BrainTree::Selector>()
                                .leaf<ConditionAligned>(this, "food")//TODO notAligned
                                .leaf<Aligne>(this, "food")
                            .end()
                            .leaf<Advance>(this, "food")
                            .leaf<IncreaseHP>(this, "food")
                        .end()
                    .end()
                .end()//Fin de sequencia para comer
                // Secuencia para dormir
                .composite<BrainTree::Sequence>()
                    .leaf<ConditionDepletion>(this, "energy")
                    .composite<BrainTree::Selector>()
                        .composite<BrainTree::Sequence>()//Está en el sitio?
                            .leaf<ConditionIsInPlace>(this, "energy")
                            .leaf<IncreaseHP>(this, "energy")
                        .end()
                        .composite<BrainTree::Sequence>()//Ir al sitio
                            .composite<BrainTree::Selector>()
                                .leaf<ConditionAligned>(this, "energy")//TODO notAligned
                                .leaf<Aligne>(this, "energy")
                            .end()
                            .leaf<Advance>(this, "energy")
                            .leaf<IncreaseHP>(this, "energy")
                        .end()
                    .end()
                .end()//Fin de sequencia para dormir
                // Secuencia para ir a un sitio aleatorio
                .composite<BrainTree::Sequence>()
                    .leaf<SelectRandomPos>(this,"random")
                    .composite<BrainTree::Selector>()
                        .leaf<ConditionIsInPlace>(this, "random") //Está en el sitio?
                        .composite<BrainTree::Sequence>()//Ir al sitio
                            .composite<BrainTree::Selector>()
                                .leaf<ConditionAligned>(this, "random")//TODO notAligned
                                .leaf<Aligne>(this, "random")
                            .end()
                            .leaf<Advance>(this, "random")
                        .end()
                    .end()
                .end()//Fin de sequencia para ir a un sitio aleatorio
           .end()
           .build();

   Reset();
}

void CFootBotLamb::Reset() {
    mess_count = 0;
    clear_message = false;
    // m_pcRBAct->ClearData();
    ping_timer = ping_interval;
    hp_timer = hp_interval;

    neightbors.clear();
    water = HP_STAT_BAD;
    food = HP_STAT_FULL;
    energy = HP_STAT_FULL;
    // water = HP_STAT_FULL;
    // food = HP_STAT_FULL;
    // energy = HP_STAT_FULL;
}

/****************************************/
/****************************************/

void CFootBotLamb::ControlStep() {
    bt.update();
    if(--hp_timer <=0){
        if (water > 0)  water --;
        if (food > 0)  food --;
        if (energy > 0)  energy --;
        hp_timer = hp_interval;
    }


    // OBTENEMOS EL ID DEL ROBOT, LEEMOS EL SENSOR DE POSICION E IMPRIMIMOS
    readings = m_pcPosSens->GetReading();
    pos = CVector2(readings.Position.GetX(), readings.Position.GetY());
    readings.Orientation.ToEulerAngles(rot.z, rot.y, rot.x);
    // RLOG << food << endl;

    /* Get readings from proximity sensor */
    // const CCI_FootBotProximitySensor::TReadings& tProxReads = m_pcProximity->GetReadings();
    // /* Sum them together */
    // CVector2 cAccumulator;
    // for(size_t i = 0; i < tProxReads.size(); ++i) {
    //     cAccumulator += CVector2(tProxReads[i].Value, tProxReads[i].Angle);
    // }
    // cAccumulator /= tProxReads.size();
    // /* If the angle of the vector is small enough and the closest obstacle
    // * is far enough, continue going straight, otherwise curve a little
    // */
    // CRadians cAngle = cAccumulator.Angle();
    //
    // if(m_cGoStraightAngleRange.WithinMinBoundIncludedMaxBoundIncluded(cAngle) &&
    //   cAccumulator.Length() < m_fDelta ) {
    //   /* Go straight */
    //   m_pcWheels->SetLinearVelocity(m_fWheelVelocity, m_fWheelVelocity);
    // }
    // else {
    //   /* Turn, depending on the sign of the angle */
    //   if(cAngle.GetValue() > 0.0f) {
         // m_pcWheels->SetLinearVelocity(m_fWheelVelocity, 0.0f);
    //   }
    //   else {
    //      m_pcWheels->SetLinearVelocity(0.0f, m_fWheelVelocity);
    //   }
    // }

    //FIXME pone a 0 el mensaje, es un apaño,
    //no soy capaz de que deje de enviarse en cada step
    if(clear_message){
        m_pcRBAct->ClearData();
        clear_message = false;
    }

    if(--ping_timer <=0){
        Ping();
        ping_timer = ping_interval;
    }
    PollMessages();
}

void CFootBotLamb::TurnLeft()
{ m_pcWheels->SetLinearVelocity(0.0f, m_fWheelVelocity); }

void CFootBotLamb::TurnRight()
{ m_pcWheels->SetLinearVelocity(m_fWheelVelocity, 0.0f); }

void CFootBotLamb::MoveForward()
{ m_pcWheels->SetLinearVelocity(m_fWheelVelocity, m_fWheelVelocity); }

void CFootBotLamb::Stop()
{ m_pcWheels->SetLinearVelocity(0.0f, 0.0f); }

void CFootBotLamb::Ping(){
    neightbors.clear();

    UInt8 *data_b = new UInt8[26];
    data_b[0] = id_num;
    data_b[1] = (UInt8) CODE_PING;

    m_pcRBAct->SetData(CByteArray(data_b, 26));
    delete data_b;
    clear_message = true; //para enviarlo una sola vez
}


void CFootBotLamb::SendPosition(){

    CByteArray mess_data;
    mess_data << id_num;
    mess_data << (UInt8) CODE_PING_REPLY;
    mess_data << pos.GetX();
    mess_data << pos.GetY();
    m_pcRBAct->SetData(mess_data);

    clear_message = true; //para enviarlo una sola vez
}


void CFootBotLamb::PollMessages(){
    CCI_RangeAndBearingSensor::TReadings messages = m_pcRBSens->GetReadings();
    CCI_RangeAndBearingSensor::TReadings::iterator m;
    for (m = messages.begin() ; m != messages.end(); ++m){
        switch (m->Data[1]) {
            case CODE_PING:
                SendPosition();
                break;
            case CODE_PING_REPLY:
                Neightbor_Info n = neightbors[m->Data.PopFront<UInt8>()];
                m->Data.PopFront<UInt8>(); //descartar el codigo del mensaje (Data[1])
                n.pos.SetX(m->Data.PopFront<Real>());
                n.pos.SetY(m->Data.PopFront<Real>());
                n.range = m->Range;
                n.bearing = m->HorizontalBearing;


                // RLOG<<" recibe de "<<n.id<<", pos: ("<< n.pos.GetX()<<", " <<n.pos.GetY()<<"), range: " << n.range<< ", bearing: "<< n.bearing<<")\n";
                break;
        }
    }
    // if(messages.size() > 0)
    //     RLOG<<" tiene "<<neightbors.size()<<" vecinos\n";
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
    disY = pos.GetY()- point.GetY();
    disY *= disY;
    disX = pos.GetX()- point.GetX();
    disX *= disX;
    if((disY+disX) < radius)
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


CFootBotLamb::NodeFootBot::Status CFootBotLamb::Aligne::update(){
    angle_to_target = (*target_pos - lamb->pos).Angle()-lamb->rot.z;
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
    LOG<<lamb->GetId()<<" w : "<< lamb->water
        <<" f: "<<lamb->food
        <<" e: "<<lamb->energy<<endl;
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
    angle_to_target = (*target_pos - lamb->pos).Angle() - lamb->rot.z;

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

/* binds the class to the string argument to use this controller
    in the configuration file
 */
REGISTER_CONTROLLER(CFootBotLamb, "footbot_lamb_controller")
