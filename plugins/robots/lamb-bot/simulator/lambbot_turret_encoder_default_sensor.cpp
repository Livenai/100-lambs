/**
 * @file <argos3/plugins/robots/lamb-bot/simulator/lambbot_turret_encoder_default_sensor.cpp>
 *
 * @author Carlo Pinciroli - <ilpincy@gmail.com>
 */

#include "lambbot_turret_encoder_default_sensor.h"
#include <argos3/core/simulator/entity/composable_entity.h>

namespace argos {

   /****************************************/
   /****************************************/

   CLambBotTurretEncoderDefaultSensor::CLambBotTurretEncoderDefaultSensor() :
      m_pcTurretEntity(NULL) {}

   /****************************************/
   /****************************************/

   void CLambBotTurretEncoderDefaultSensor::SetRobot(CComposableEntity& c_entity) {
      m_pcTurretEntity = &(c_entity.GetComponent<CLambBotTurretEntity>("turret"));
      m_pcTurretEntity->Enable();
   }

   /****************************************/
   /****************************************/

   void CLambBotTurretEncoderDefaultSensor::Update() {
      m_cRotation = m_pcTurretEntity->GetRotation();
   }

   /****************************************/
   /****************************************/

   void CLambBotTurretEncoderDefaultSensor::Reset() {
      m_cRotation = CRadians::ZERO;
   }

   /****************************************/
   /****************************************/

   REGISTER_SENSOR(CLambBotTurretEncoderDefaultSensor,
                   "lambbot_turret_encoder", "default",
                   "Carlo Pinciroli [ilpincy@gmail.com]",
                   "1.0",
                   "The lamb-bot turret encoder sensor.",
                   "This sensor accesses the lamb-bot turret encoder. For a complete\n"
                   "description of its usage, refer to the ci_lambbot_turret_encoder_sensor\n"
                   "file.\n\n"
                   "REQUIRED XML CONFIGURATION\n\n"
                   "  <controllers>\n"
                   "    ...\n"
                   "    <my_controller ...>\n"
                   "      ...\n"
                   "      <sensors>\n"
                   "        ...\n"
                   "        <lambbot_turret_encoder implementation=\"default\" />\n"
                   "        ...\n"
                   "      </sensors>\n"
                   "      ...\n"
                   "    </my_controller>\n"
                   "    ...\n"
                   "  </controllers>\n\n"
                   "OPTIONAL XML CONFIGURATION\n\n"
                   "None for the time being.\n",
                   "Usable"
      );

}
