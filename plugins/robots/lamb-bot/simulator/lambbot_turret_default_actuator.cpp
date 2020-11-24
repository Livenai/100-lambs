/**
 * @file <argos3/plugins/robots/lamb-bot/simulator/lambbot_turret_default_actuator.cpp>
 *
 * @author Carlo Pinciroli - <ilpincy@gmail.com>
 */

#include "lambbot_turret_default_actuator.h"

namespace argos {

	 const Real RPM_TO_RADIANS_PER_SEC = ARGOS_PI / 30.0f;

   /****************************************/
   /****************************************/

   CLambBotTurretDefaultActuator::CLambBotTurretDefaultActuator() :
      m_pcTurretEntity(NULL),
      m_unDesiredMode(CLambBotTurretEntity::MODE_OFF) {}

   /****************************************/
   /****************************************/

   void CLambBotTurretDefaultActuator::SetRobot(CComposableEntity& c_entity) {
      m_pcTurretEntity = &(c_entity.GetComponent<CLambBotTurretEntity>("turret"));
      m_pcTurretEntity->Enable();
   }

   /****************************************/
   /****************************************/

   void CLambBotTurretDefaultActuator::SetRotation(const CRadians& c_angle) {
      m_pcTurretEntity->SetDesiredRotation(c_angle);
   }

   /****************************************/
   /****************************************/

   void CLambBotTurretDefaultActuator::SetRotationSpeed(SInt32 n_speed_pulses) {
      m_pcTurretEntity->SetDesiredRotationSpeed(RPM_TO_RADIANS_PER_SEC * n_speed_pulses);
   }

   /****************************************/
   /****************************************/

   void CLambBotTurretDefaultActuator::SetMode(ETurretModes e_mode) {
      m_unDesiredMode = e_mode;
   }

   /****************************************/
   /****************************************/

   void CLambBotTurretDefaultActuator::Update() {
      m_pcTurretEntity->SetMode(m_unDesiredMode);
   }

   /****************************************/
   /****************************************/

   void CLambBotTurretDefaultActuator::Reset() {
      m_unDesiredMode = CLambBotTurretEntity::MODE_OFF;
   }

   /****************************************/
   /****************************************/

   REGISTER_ACTUATOR(CLambBotTurretDefaultActuator,
                     "lambbot_turret", "default",
                     "Carlo Pinciroli [ilpincy@gmail.com]",
                     "1.0",
                     "The lamb-bot turret actuator.",
                     "This actuator controls the lamb-bot turret. For a complete\n"
                     "description of its usage, refer to the ci_lambbot_turret_actuator\n"
                     "file.\n\n"
                     "REQUIRED XML CONFIGURATION\n\n"
                     "  <controllers>\n"
                     "    ...\n"
                     "    <my_controller ...>\n"
                     "      ...\n"
                     "      <actuators>\n"
                     "        ...\n"
                     "        <lambbot_turret implementation=\"default\" />\n"
                     "        ...\n"
                     "      </actuators>\n"
                     "      ...\n"
                     "    </my_controller>\n"
                     "    ...\n"
                     "  </controllers>\n\n"
                     "OPTIONAL XML CONFIGURATION\n\n"
                     "None for the time being.\n",
                     "Usable"
      );

}
