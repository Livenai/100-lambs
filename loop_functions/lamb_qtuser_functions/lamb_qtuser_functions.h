#ifndef LAMB_QTUSER_FUNCTIONS_H
#define LAMB_QTUSER_FUNCTIONS_H

#include <argos3/plugins/simulator/visualizations/qt-opengl/qtopengl_user_functions.h>
#include <argos3/plugins/robots/foot-bot/simulator/footbot_entity.h>
#include "footbot_lamb.h"

using namespace argos;

class CLambQTUserFunctions : public CQTOpenGLUserFunctions {

public:

   CLambQTUserFunctions();

   virtual ~CLambQTUserFunctions() {}

   void Init(TConfigurationNode& t_tree);

   void Draw(CFootBotEntity& c_entity);

   void EntitySelected(CEntity& c_entity);

   void EntityDeselected(CEntity& c_entity);
private:
    CFootBotLamb* controller;
};

#endif
