#include "galib_lamb_loop_functions.h"
CLambLoopFunctions::CLambLoopFunctions():
    CLoopFunctions(),
    m_unCurrentTrial(0),
    bed_color(212,54,126),
    floor_color(CColor::GRAY70),
    log_interval(60),
    number_logs(0){}

void CLambLoopFunctions::Init(TConfigurationNode& t_tree) {
    TConfigurationNode experiment_conf = GetNode(CSimulator::GetInstance().GetConfigurationRoot(), "framework");
    experiment_conf = GetNode(experiment_conf,"experiment");
    GetNodeAttribute(experiment_conf, "ticks_per_second", ticks_per_second);

    GetNodeAttributeOrDefault(t_tree, "log_interval", log_interval, log_interval);
    time = 1603889708.5339026;//Para que coincida con los primeros logs reales y se puedan reproducir a la vez

    CSpace::TMapPerType robot_map = CSimulator::GetInstance().GetSpace().GetEntitiesByType("lamb-bot");
    GetNodeAttributeOrDefault(t_tree, "number_of_logs", number_logs, robot_map.size());
    if(number_logs > robot_map.size())
        number_logs = robot_map.size();
    fs::path dir("tracking_logs");
    if(number_logs > 0 && !fs::exists(dir))
        fs::create_directory(dir);

    string sufixes[8] = {"Am", "Az", "Bl", "Li", "Na", "Ro", "Ve", "Ng"};
    char filename[80], log[80];
    auto it = robot_map.begin();
    for (UInt8 i = 0; i < number_logs; i++){
        CLambBotEntity *robot = any_cast<CLambBotEntity*>(it->second);
        CCI_Controller *lamb = &(robot->GetControllableEntity().GetController());
        lambs.push_back( dynamic_cast<CGalibLamb*>(lamb) );
        sprintf( filename,"tracking_logs/%s_%s.json", lamb->GetId().c_str(),sufixes[i%8].c_str());
        files.push_back(std::ofstream(filename, std::ios::trunc));
        //introduciendo la primera posicion
        //TODO el tiempo sin decimales porque la configuración local del ordenador introduce una coma
        // en lugar de un punto y deja de funcionar el json
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
    CGalibLamb::SetTroughs();

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

void CLambLoopFunctions::ConfigureFromGenome(const GARealGenome& c_genome){
  return;//TODO
}
Real CLambLoopFunctions::Performance(){
  // Py_Initialize();
  //
	// PyRun_SimpleString("print('Hello World from Embedded Python!!!')");
  //
	// Py_Finalize();

  return 0;
}

/****************************************/
/****************************************/

REGISTER_LOOP_FUNCTIONS(CLambLoopFunctions, "lamb_loop_functions")
