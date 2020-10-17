#include "paint_floor_loop_functions.h"
CPaintFloorLoopFunctions::CPaintFloorLoopFunctions():
    CLoopFunctions(),
    radius(0.5),
    water_pos(0, 0),
    food_pos(3, 3),
    bed_pos(-3, -3){}

void CPaintFloorLoopFunctions::Init(TConfigurationNode& t_tree) {
    //lee los paramatros del archivo de configuracion con extesion .argos
    if(NodeExists(t_tree, "params")){
        TConfigurationNode params = GetNode(t_tree, "params");
        //TODO comprobar valores de los parametros
        GetNodeAttributeOrDefault(params, "radius", radius, radius);
        radius *= radius;
        GetNodeAttributeOrDefault(params, "water_pos", water_pos, water_pos);
        GetNodeAttributeOrDefault(params, "food_pos", food_pos, food_pos);
        GetNodeAttributeOrDefault(params, "bed_pos", bed_pos, bed_pos);
    }

}

/****************************************/
/****************************************/

void CPaintFloorLoopFunctions::Reset() {
}


CColor CPaintFloorLoopFunctions::GetFloorColor(const CVector2& floor_pos){
    Real disY = floor_pos.GetY()- water_pos.GetY();
    disY *= disY;
    Real disX = floor_pos.GetX()- water_pos.GetX();
    disX *= disX;
    if((disY+disX) < radius)
        return CColor::CYAN;

    disY = floor_pos.GetY()- food_pos.GetY();
    disY *= disY;
    disX = floor_pos.GetX()- food_pos.GetX();
    disX *= disX;
    if((disY+disX) < radius)
        return CColor::GREEN;

    disY = floor_pos.GetY()- bed_pos.GetY();
    disY *= disY;
    disX = floor_pos.GetX()- bed_pos.GetX();
    disX *= disX;
    if((disY+disX) < radius)
        return CColor::MAGENTA;

    else
        return CColor::GRAY50;
}


/****************************************/
/****************************************/

REGISTER_LOOP_FUNCTIONS(CPaintFloorLoopFunctions, "paint_floor_loop_functions")
