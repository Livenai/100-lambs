#ifndef LAMB_QTUSER_FUNCTIONS_H
#define LAMB_QTUSER_FUNCTIONS_H

#include <argos3/plugins/simulator/visualizations/qt-opengl/qtopengl_user_functions.h>
#include <argos3/plugins/robots/lamb-bot/simulator/lambbot_entity.h>
#include "lamb.h"

using namespace argos;

class CLambQTUserFunctions : public CQTOpenGLUserFunctions {

public:

   CLambQTUserFunctions();

   virtual ~CLambQTUserFunctions() {}

   void Init(TConfigurationNode& t_tree);

   void Draw(CLambBotEntity& c_entity);

   void EntitySelected(CEntity& c_entity);

   void EntityDeselected(CEntity& c_entity);
private:
    CLamb* controller;
};

#endif
