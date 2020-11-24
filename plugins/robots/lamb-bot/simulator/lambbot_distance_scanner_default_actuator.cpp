/**
 * @file <argos3/plugins/robots/lamb-bot/simulator/lambbot_distance_scanner_default_actuator.cpp>
 *
 * @author Carlo Pinciroli - <ilpincy@gmail.com>
 */

#include "lambbot_distance_scanner_default_actuator.h"

namespace argos {

	 const Real CLambBotDistanceScannerDefaultActuator::RPM_TO_RADIANS_PER_SEC = ARGOS_PI / 30.0f;

   /****************************************/
   /****************************************/

   CLambBotDistanceScannerDefaultActuator::CLambBotDistanceScannerDefaultActuator() :
      m_fDesiredRotationSpeed(0.0f),
      m_unDesiredMode(CLambBotDistanceScannerEquippedEntity::MODE_OFF) {}

   /****************************************/
   /****************************************/

   void CLambBotDistanceScannerDefaultActuator::SetRobot(CComposableEntity& c_entity) {
      m_pcDistanceScannerEquippedEntity = &(c_entity.GetComponent<CLambBotDistanceScannerEquippedEntity>("distance_scanner"));
      m_pcDistanceScannerEquippedEntity->Enable();
   }

   /****************************************/
   /****************************************/

   void CLambBotDistanceScannerDefaultActuator::SetAngle(const CRadians& c_angle) {
      m_unDesiredMode = CLambBotDistanceScannerEquippedEntity::MODE_POSITION_CONTROL;
      m_cDesiredRotation = c_angle;
   }

   /****************************************/
   /****************************************/

   void CLambBotDistanceScannerDefaultActuator::SetRPM(Real f_rpm) {
      m_unDesiredMode = CLambBotDistanceScannerEquippedEntity::MODE_SPEED_CONTROL;
      m_fDesiredRotationSpeed = f_rpm * RPM_TO_RADIANS_PER_SEC;
   }

   /****************************************/
   /****************************************/

   void CLambBotDistanceScannerDefaultActuator::Enable() {
      m_unDesiredMode = CLambBotDistanceScannerEquippedEntity::MODE_POSITION_CONTROL;
   }

   /****************************************/
   /****************************************/

   void CLambBotDistanceScannerDefaultActuator::Disable() {
      m_unDesiredMode = CLambBotDistanceScannerEquippedEntity::MODE_OFF;
   }

   /****************************************/
   /****************************************/

   void CLambBotDistanceScannerDefaultActuator::Update() {
      m_pcDistanceScannerEquippedEntity->SetMode(m_unDesiredMode);
      if(m_unDesiredMode == CLambBotDistanceScannerEquippedEntity::MODE_POSITION_CONTROL) {
         m_pcDistanceScannerEquippedEntity->SetRotation(m_cDesiredRotation);
      }
      else if(m_unDesiredMode == CLambBotDistanceScannerEquippedEntity::MODE_SPEED_CONTROL) {
         m_pcDistanceScannerEquippedEntity->SetRotationSpeed(m_fDesiredRotationSpeed);
      }
   }

   /****************************************/
   /****************************************/

   void CLambBotDistanceScannerDefaultActuator::Reset() {
      m_cDesiredRotation = CRadians::ZERO;
      m_fDesiredRotationSpeed = 0.0f;
      m_unDesiredMode = CLambBotDistanceScannerEquippedEntity::MODE_OFF;
   }

   /****************************************/
   /****************************************/

   REGISTER_ACTUATOR(CLambBotDistanceScannerDefaultActuator,
                     "lambbot_distance_scanner", "default",
                     "Carlo Pinciroli [ilpincy@gmail.com]",
                     "1.0",
                     "The lamb-bot distance scanner actuator.",
                     "This actuator controls the lamb-bot distance scanner. For a complete\n"
                     "description of its usage, refer to the ci_lambbot_distance_scanner_actuator\n"
                     "file.\n\n"
                     "REQUIRED XML CONFIGURATION\n\n"
                     "  <controllers>\n"
                     "    ...\n"
                     "    <my_controller ...>\n"
                     "      ...\n"
                     "      <actuators>\n"
                     "        ...\n"
                     "        <lambbot_distance_scanner implementation=\"default\" />\n"
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
