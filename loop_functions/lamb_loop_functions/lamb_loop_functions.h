#ifndef LAMB_FLOOR_LOOP_FUNCTIONS_H
#define LAMB_FLOOR_LOOP_FUNCTIONS_H

#include <argos3/core/simulator/loop_functions.h>
#include <argos3/core/simulator/simulator.h>
#include <argos3/core/simulator/space/space.h>//TODO puedo omitir el import?
#include <argos3/core/utility/configuration/argos_configuration.h>
#include "footbot_lamb.h"
#include <iostream>
#include <fstream>
#include <stdio.h>


using namespace argos;

class CLambLoopFunctions : public CLoopFunctions {

public:
    CLambLoopFunctions();

    virtual ~CLambLoopFunctions() {}
    //se llama al comienzo de la simulacion y le los parametros del archivo de configuración
    void Init(TConfigurationNode& t_tree);
    void Destroy();
    void Reset();

    CColor GetFloorColor(const CVector2& floor_pos);
    void PostStep();
private:
    std::vector<CFootBotLamb*> lambs;
    std::vector<std::ofstream> files;

    // radio del circulo que representa la zona donde duermen las ovejas;
    Real radius;
    CVector2 bed_pos;
    CColor bed_color, floor_color;

    UInt32 log_interval;
    UInt32 log_timer;
    UInt8 number_logs;
    UInt32 ticks_per_second;
    Real time;


};

#endif
