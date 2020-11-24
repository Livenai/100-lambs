/**
 * @file <argos3/plugins/robots/lamb-bot/simulator/lambbot_gripper_default_actuator.h>
 *
 * @author Carlo Pinciroli - <ilpincy@gmail.com>
 */

#ifndef LAMBBOT_GRIPPER_DEFAULT_ACTUATOR_H
#define LAMBBOT_GRIPPER_DEFAULT_ACTUATOR_H

#include <string>
#include <map>

namespace argos {
   class CLambBotGripperDefaultActuator;
}

#include <argos3/plugins/robots/lamb-bot/control_interface/ci_lambbot_gripper_actuator.h>
#include <argos3/plugins/simulator/entities/gripper_equipped_entity.h>
#include <argos3/core/simulator/actuator.h>

namespace argos {

   class CLambBotGripperDefaultActuator : public CSimulatedActuator,
                                          public CCI_LambBotGripperActuator {

   public:

      CLambBotGripperDefaultActuator();

      virtual ~CLambBotGripperDefaultActuator() {}

      virtual void SetRobot(CComposableEntity& c_entity);

      virtual void Update();
      virtual void Reset();

      virtual void EnableCheckForObjectGrippedRoutine() {}
      virtual void DisableCheckForObjectGrippedRoutine() {}

   private:

      CGripperEquippedEntity* m_pcGripperEquippedEntity;

   };

}

#endif
