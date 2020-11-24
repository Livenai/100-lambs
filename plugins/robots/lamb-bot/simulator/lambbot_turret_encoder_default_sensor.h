/**
 * @file <argos3/plugins/robots/lamb-bot/simulator/lambbot_turret_encoder_default_sensor.h>
 *
 * @author Carlo Pinciroli - <ilpincy@gmail.com>
 */

#ifndef LAMBBOT_TURRET_ENCODER_DEFAULT_SENSOR_H
#define LAMBBOT_TURRET_ENCODER_DEFAULT_SENSOR_H

#include <string>
#include <map>

namespace argos {
   class CLambBotTurretEncoderDefaultSensor;
}

#include <argos3/plugins/robots/lamb-bot/control_interface/ci_lambbot_turret_encoder_sensor.h>
#include <argos3/plugins/robots/lamb-bot/simulator/lambbot_turret_encoder_default_sensor.h>
#include <argos3/plugins/robots/lamb-bot/simulator/lambbot_turret_entity.h>
#include <argos3/core/simulator/sensor.h>

namespace argos {

   class CLambBotTurretEncoderDefaultSensor : public CCI_LambBotTurretEncoderSensor,
                                              public CSimulatedSensor {

   public:

      CLambBotTurretEncoderDefaultSensor();

      virtual ~CLambBotTurretEncoderDefaultSensor() {}

      virtual void SetRobot(CComposableEntity& c_entity);

      virtual void Update();

      virtual void Reset();

   private:

      CLambBotTurretEntity* m_pcTurretEntity;

   };

}

#endif
