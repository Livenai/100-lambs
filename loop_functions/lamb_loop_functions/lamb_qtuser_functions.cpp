#include "lamb_qtuser_functions.h"

/****************************************/
/****************************************/

CLambQTUserFunctions::CLambQTUserFunctions(): controller(NULL){
}

void CLambQTUserFunctions::Init(TConfigurationNode& t_tree) {
    bool show_id = true;
    GetNodeAttributeOrDefault(t_tree, "show_id", show_id, show_id);
    if(show_id)
        RegisterUserFunction<CLambQTUserFunctions,CFootBotEntity>(&CLambQTUserFunctions::Draw);
}


/****************************************/
/****************************************/

void CLambQTUserFunctions::Draw(CFootBotEntity& c_entity) {
   /* The position of the text is expressed wrt the reference point of the footbot
    * For a foot-bot, the reference point is the center of its base.
    * See also the description in
    * $ argos3 -q foot-bot
    */
   DrawText(CVector3(0.0, 0.0, 0.3),   // position
            c_entity.GetId().c_str()); // text
}

void CLambQTUserFunctions::EntitySelected(CEntity& c_entity) {
   /* Make sure the entity is a foot-bot */
   CFootBotEntity* pcFB = dynamic_cast<CFootBotEntity*>(&c_entity);
   if(!pcFB) return;
   /* It's a foot-bot; extract its controller */
   controller = dynamic_cast<CFootBotLamb*>(&pcFB->GetControllableEntity().GetController());
   /* Tell that foot-bot that it is selected */
   controller->ShowDebugInformation(true);
}

void CLambQTUserFunctions::EntityDeselected(CEntity& c_entity) {
   /* Make sure that a controller was set (should always be true...) */
   if(!controller) return;
   /* Tell the foot-bot that it is deselected */
   controller->ShowDebugInformation(false);
   /* Forget the controller */
   controller = NULL;
}
/****************************************/
/****************************************/

REGISTER_QTOPENGL_USER_FUNCTIONS(CLambQTUserFunctions, "lamb_qtuser_functions")
