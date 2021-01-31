#ifndef GALIB_LAMB_FLOOR_LOOP_FUNCTIONS_H
#define GALIB_LAMB_FLOOR_LOOP_FUNCTIONS_H

#include <argos3/core/simulator/loop_functions.h>
#include <argos3/core/simulator/simulator.h>
#include <argos3/core/simulator/space/space.h>//TODO puedo omitir el import?
#include <argos3/core/utility/configuration/argos_configuration.h>
#include "galib_lamb.h"
#include <iostream>
#include <fstream>
#include <stdio.h>

#include <ga/GARealGenome.h>
#include <ga/GARealGenome.C>

#define BOOST_FILESYSTEM_NO_DEPRECATED
#include <boost/filesystem.hpp>
// #include <Python.h>



using namespace argos;
namespace fs = boost::filesystem;
class CLambLoopFunctions : public CLoopFunctions {

public:
    CLambLoopFunctions();

    virtual ~CLambLoopFunctions() {}
    //se llama al comienzo de la simulacion y le los parametros del archivo de configuración
    void Init(TConfigurationNode& t_tree);
    void Destroy();
    void Reset();

    CColor GetFloorColor(const CVector2& floor_pos);//TODO borrar con el sistema nuevo
    void PostStep();

    inline void SetTrial(size_t un_trial) { m_unCurrentTrial = un_trial; }
    void ConfigureFromGenome(const GARealGenome& c_genome);
    Real Performance();

private:
    std::vector<CGalibLamb*> lambs;
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

    size_t m_unCurrentTrial;



};

#endif
