/**
 * @file <argos3/plugins/robots/lamb-bot/simulator/lambbot_distance_scanner_default_actuator.h>
 *
 * @author Carlo Pinciroli - <ilpincy@gmail.com>
 */

#ifndef LAMBBOT_DISTANCE_SCANNER_DEFAULT_ACTUATOR_H
#define LAMBBOT_DISTANCE_SCANNER_DEFAULT_ACTUATOR_H

#include <string>
#include <map>

namespace argos {
   class CLambBotDistanceScannerDefaultActuator;
}

#include <argos3/plugins/robots/lamb-bot/control_interface/ci_lambbot_distance_scanner_actuator.h>
#include <argos3/plugins/robots/lamb-bot/simulator/lambbot_entity.h>
#include <argos3/plugins/robots/lamb-bot/simulator/lambbot_distance_scanner_equipped_entity.h>
#include <argos3/core/simulator/actuator.h>

namespace argos {

   class CLambBotDistanceScannerDefaultActuator : public CSimulatedActuator,
                                                  public CCI_LambBotDistanceScannerActuator {

   public:

  	  static const Real RPM_TO_RADIANS_PER_SEC;

      CLambBotDistanceScannerDefaultActuator();
      virtual ~CLambBotDistanceScannerDefaultActuator() {}

      virtual void SetRobot(CComposableEntity& c_entity);

      virtual void SetAngle(const CRadians& c_angle);
      virtual void SetRPM(Real f_rpm);

      virtual void Enable();
      virtual void Disable();

      virtual void Update();
      virtual void Reset();

   private:

      CLambBotDistanceScannerEquippedEntity* m_pcDistanceScannerEquippedEntity;

      CRadians m_cDesiredRotation;
      Real     m_fDesiredRotationSpeed;
      UInt8    m_unDesiredMode;

   };

}

#endif
