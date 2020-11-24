#include "lamb_qtuser_functions.h"

/****************************************/
/****************************************/

CLambQTUserFunctions::CLambQTUserFunctions(): controller(NULL){
}

void CLambQTUserFunctions::Init(TConfigurationNode& t_tree) {
    bool show_id = true;
    GetNodeAttributeOrDefault(t_tree, "show_id", show_id, show_id);
    if(show_id)
        RegisterUserFunction<CLambQTUserFunctions,CLambBotEntity>(&CLambQTUserFunctions::Draw);
}


/****************************************/
/****************************************/

void CLambQTUserFunctions::Draw(CLambBotEntity& c_entity) {
   /* The position of the text is expressed wrt the reference point of the robot
    * The reference point is the center of the base of the robot.
    */
   DrawText(CVector3(0.0, 0.0, 0.3),   // position
            c_entity.GetId().c_str()); // text
}

void CLambQTUserFunctions::EntitySelected(CEntity& c_entity) {
   /* Make sure the entity is a lamb-bot */
   CLambBotEntity* pcFB = dynamic_cast<CLambBotEntity*>(&c_entity);
   if(!pcFB) return;
   controller = dynamic_cast<CLamb*>(&pcFB->GetControllableEntity().GetController());
   controller->ShowDebugInformation(true);
}

void CLambQTUserFunctions::EntityDeselected(CEntity& c_entity) {
   /* Make sure that a controller was set (should always be true...) */
   if(!controller) return;
   controller->ShowDebugInformation(false);
   controller = NULL;
}
/****************************************/
/****************************************/

REGISTER_QTOPENGL_USER_FUNCTIONS(CLambQTUserFunctions, "lamb_qtuser_functions")
