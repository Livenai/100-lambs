#ifndef PAINT_FLOOR_LOOP_FUNCTIONS_H
#define PAINT_FLOOR_LOOP_FUNCTIONS_H

#include <argos3/core/simulator/loop_functions.h>
#include <argos3/core/simulator/simulator.h>
#include <argos3/core/simulator/space/space.h>//TODO puedo omitir el import?
#include <argos3/core/utility/configuration/argos_configuration.h>

using namespace argos;

class CPaintFloorLoopFunctions : public CLoopFunctions {

public:
    CPaintFloorLoopFunctions();

    virtual ~CPaintFloorLoopFunctions() {}
    //se llama al comienzo de la simulacion y le los parametros del archivo de configuración
    void Init(TConfigurationNode& t_tree);

    virtual void Reset();


    virtual CColor GetFloorColor(const CVector2& floor_pos);
private:
    // radio de las manchas que representan el agua, comida, etc;
    Real radius;
    CVector2 bed_pos;
    CColor bed_color, floor_color;
};

#endif
