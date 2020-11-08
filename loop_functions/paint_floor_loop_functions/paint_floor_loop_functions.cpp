#include "paint_floor_loop_functions.h"
#include "footbot_lamb.h"
CPaintFloorLoopFunctions::CPaintFloorLoopFunctions():
    CLoopFunctions(),
    bed_color(212,54,126),
    floor_color(CColor::GRAY70){}

void CPaintFloorLoopFunctions::Init(TConfigurationNode& t_tree) {
    TConfigurationNode arena_conf = GetNode(CSimulator::GetInstance().GetConfigurationRoot(), "arena");
    GetNodeAttribute(arena_conf, "radius", radius);
    GetNodeAttribute(arena_conf, "bed_pos", bed_pos);
    // CFootBotLamb::SetTroughs();



}

/****************************************/
/****************************************/

void CPaintFloorLoopFunctions::Reset() {
}


CColor CPaintFloorLoopFunctions::GetFloorColor(const CVector2& floor_pos){
    Real dis = (floor_pos - bed_pos).Length();
    if(dis < radius)
        return bed_color;
    else
        return floor_color;
}


/****************************************/
/****************************************/

REGISTER_LOOP_FUNCTIONS(CPaintFloorLoopFunctions, "paint_floor_loop_functions")
