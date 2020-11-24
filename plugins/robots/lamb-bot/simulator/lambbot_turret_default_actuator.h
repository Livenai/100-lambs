/**
 * @file <argos3/plugins/robots/lamb-bot/simulator/lambbot_turret_default_actuator.h>
 *
 * @author Carlo Pinciroli - <ilpincy@gmail.com>
 */

#ifndef LAMBBOT_TURRET_DEFAULT_ACTUATOR_H
#define LAMBBOT_TURRET_DEFAULT_ACTUATOR_H

#include <string>
#include <map>

namespace argos {
   class CLambBotTurretDefaultActuator;
}

#include <argos3/plugins/robots/lamb-bot/control_interface/ci_lambbot_turret_actuator.h>
#include <argos3/plugins/robots/lamb-bot/simulator/lambbot_entity.h>
#include <argos3/plugins/robots/lamb-bot/simulator/lambbot_turret_entity.h>
#include <argos3/core/simulator/actuator.h>

namespace argos {

   class CLambBotTurretDefaultActuator : public CSimulatedActuator,
                                         public CCI_LambBotTurretActuator {

   public:

      CLambBotTurretDefaultActuator();
      virtual ~CLambBotTurretDefaultActuator() {}

      virtual void SetRobot(CComposableEntity& c_entity);

      virtual void SetRotation(const CRadians& c_angle);
      virtual void SetRotationSpeed(SInt32 n_speed_pulses);
      virtual void SetMode(ETurretModes e_mode);

      virtual void Update();
      virtual void Reset();

   private:

      CLambBotTurretEntity* m_pcTurretEntity;
      UInt32 m_unDesiredMode;

   };

}

#endif
