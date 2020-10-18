#include "paint_floor_loop_functions.h"
CPaintFloorLoopFunctions::CPaintFloorLoopFunctions():
    CLoopFunctions(),
    water_color(133,246,251),
    food_color(194,254,56),
    bed_color(212,54,126),
    floor_color(CColor::GRAY50){}

void CPaintFloorLoopFunctions::Init(TConfigurationNode& t_tree) {
    // TODO controlar que estos parametros se proporcionen
    //Leyendo las posiciones del bebedero, comedero y cama
    TConfigurationNode arena_conf = GetNode(CSimulator::GetInstance().GetConfigurationRoot(), "arena");
    GetNodeAttribute(arena_conf, "radius", radius);
    radius *= radius;
    GetNodeAttribute(arena_conf, "water_pos", water_pos);
    GetNodeAttribute(arena_conf, "food_pos", food_pos);
    GetNodeAttribute(arena_conf, "bed_pos", bed_pos);

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
        return water_color;

    disY = floor_pos.GetY()- food_pos.GetY();
    disY *= disY;
    disX = floor_pos.GetX()- food_pos.GetX();
    disX *= disX;
    if((disY+disX) < radius)
        return food_color;

    disY = floor_pos.GetY()- bed_pos.GetY();
    disY *= disY;
    disX = floor_pos.GetX()- bed_pos.GetX();
    disX *= disX;
    if((disY+disX) < radius)
        return bed_color;

    else
        return floor_color;
}


/****************************************/
/****************************************/

REGISTER_LOOP_FUNCTIONS(CPaintFloorLoopFunctions, "paint_floor_loop_functions")
