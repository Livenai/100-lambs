#include "lamb_loop_functions.h"
CLambLoopFunctions::CLambLoopFunctions():
    CLoopFunctions(),
    bed_color(212,54,126),
    floor_color(CColor::GRAY70),
    log_interval(60),
    number_logs(0){}

void CLambLoopFunctions::Init(TConfigurationNode& t_tree) {
    TConfigurationNode experiment_conf = GetNode(CSimulator::GetInstance().GetConfigurationRoot(), "framework");
    experiment_conf = GetNode(experiment_conf,"experiment");
    GetNodeAttribute(experiment_conf, "ticks_per_second", ticks_per_second);

    GetNodeAttributeOrDefault(t_tree, "log_interval", log_interval, log_interval);
    time = 1603889708.5339026;

    CSpace::TMapPerType robot_map = CSimulator::GetInstance().GetSpace().GetEntitiesByType("foot-bot");
    GetNodeAttributeOrDefault(t_tree, "number_lambs_to_log", number_logs, robot_map.size());
    if(number_logs > robot_map.size())
        number_logs = robot_map.size();

    string sufixes[8] = {"Am", "Az", "Bl", "Li", "Na", "Ro", "Ve", "Ng"};
    char filename[80], log[80];
    auto it = robot_map.begin();
    for (UInt8 i = 0; i < number_logs; i++){
        CFootBotEntity *footbot = any_cast<CFootBotEntity*>(it->second);
        CCI_Controller *lamb = &(footbot->GetControllableEntity().GetController());
        lambs.push_back( dynamic_cast<CFootBotLamb*>(lamb) );
        sprintf( filename,"tracking_logs/%s_%s.json", lamb->GetId().c_str(),sufixes[i%8].c_str());
        files.push_back(std::ofstream(filename, std::ios::trunc));

        //introduciendo la primera posicion
        CVector2 pos = lambs[i]->GetCorrectedPos();
        sprintf( log,"{ \"%.0f\": {\"x\": %d, \"y\": %d}", time, (int) pos.GetX(), (int) pos.GetY() );
        files[i]<<log;

        it ++;
    }
    //posicion de la "cama"
    //TODO esta parte sera eliminada ya que el comportamiento real de los corderos
    //es dormir cerca de otros y no en un punto especifico
    TConfigurationNode arena_conf = GetNode(CSimulator::GetInstance().GetConfigurationRoot(), "arena");
    GetNodeAttribute(arena_conf, "radius", radius);
    GetNodeAttribute(arena_conf, "bed_pos", bed_pos);
    CFootBotLamb::SetTroughs();

    Reset();
}


void CLambLoopFunctions::Destroy(){
    for(UInt8 i = 0; i < number_logs; i++){
        files[i]<<"}";
        files[i].close();
    }
}

/****************************************/
/****************************************/

void CLambLoopFunctions::Reset() {
    log_timer = log_interval * ticks_per_second;
}


CColor CLambLoopFunctions::GetFloorColor(const CVector2& floor_pos){
    Real dis = (floor_pos - bed_pos).Length();
    if(dis < radius)
        return bed_color;
    else
        return floor_color;
}

void CLambLoopFunctions::PostStep(){
    if(--log_timer <= 0){
        time += log_interval;
        char log[80];
        CVector2 pos;
        for(UInt8 i = 0; i < number_logs ; i ++){
            pos = lambs[i]->GetCorrectedPos();
            sprintf( log,",  \"%.0f\": {\"x\": %d, \"y\": %d}", time, (int)pos.GetX(), (int)pos.GetY() );
            files[i]<<log;
        }
        log_timer = log_interval * ticks_per_second;
    }
}


/****************************************/
/****************************************/

REGISTER_LOOP_FUNCTIONS(CLambLoopFunctions, "lamb_loop_functions")
