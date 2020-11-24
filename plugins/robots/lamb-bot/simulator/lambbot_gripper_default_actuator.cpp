/**
 * @file <argos3/plugins/robots/lamb-bot/simulator/lambbot_gripper_default_actuator.cpp>
 *
 * @author Carlo Pinciroli - <ilpincy@gmail.com>
 */

#include "lambbot_gripper_default_actuator.h"
#include <argos3/core/utility/logging/argos_log.h>
#include <argos3/core/simulator/entity/composable_entity.h>

namespace argos {

   /****************************************/
   /****************************************/

   CLambBotGripperDefaultActuator::CLambBotGripperDefaultActuator() :
      m_pcGripperEquippedEntity(NULL) {}

   /****************************************/
   /****************************************/

   void CLambBotGripperDefaultActuator::SetRobot(CComposableEntity& c_entity) {
      m_pcGripperEquippedEntity = &(c_entity.GetComponent<CGripperEquippedEntity>("gripper"));
      m_pcGripperEquippedEntity->Enable();
   }

   /****************************************/
   /****************************************/

   void CLambBotGripperDefaultActuator::Update() {
      m_pcGripperEquippedEntity->SetLockState(Abs(m_cAperture) / CRadians::PI_OVER_TWO);
   }

   /****************************************/
   /****************************************/

   void CLambBotGripperDefaultActuator::Reset() {
      Unlock();
   }

   /****************************************/
   /****************************************/

}

REGISTER_ACTUATOR(CLambBotGripperDefaultActuator,
                  "lambbot_gripper", "default",
                  "Carlo Pinciroli [ilpincy@gmail.com]",
                  "1.0",
                  "The lambbot gripper actuator.",
                  "This actuator controls the lamb-bot gripper. For a complete description of its\n"
                  "usage, refer to the ci_lambbot_gripper_actuator.h file.\n\n"
                  "REQUIRED XML CONFIGURATION\n\n"
                  "  <controllers>\n"
                  "    ...\n"
                  "    <my_controller ...>\n"
                  "      ...\n"
                  "      <actuators>\n"
                  "        ...\n"
                  "        <lambbot_gripper implementation=\"default\" />\n"
                  "        ...\n"
                  "      </actuators>\n"
                  "      ...\n"
                  "    </my_controller>\n"
                  "    ...\n"
                  "  </controllers>\n\n"
                  "OPTIONAL XML CONFIGURATION\n\n"
                  "None.\n",
                  "Usable"
   );

