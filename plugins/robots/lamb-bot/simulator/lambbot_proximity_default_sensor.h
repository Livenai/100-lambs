/**
 * @file <argos3/plugins/robots/lamb-bot/simulator/lambbot_proximity_default_sensor.h>
 *
 * @author Carlo Pinciroli - <ilpincy@gmail.com>
 */

#ifndef LAMBBOT_PROXIMITY_DEFAULT_SENSOR_H
#define LAMBBOT_PROXIMITY_DEFAULT_SENSOR_H

#include <string>
#include <map>

namespace argos {
   class CLambBotProximityDefaultSensor;
}

#include <argos3/plugins/robots/lamb-bot/control_interface/ci_lambbot_proximity_sensor.h>
#include <argos3/plugins/robots/generic/simulator/proximity_default_sensor.h>

namespace argos {

   class CLambBotProximityDefaultSensor : public CCI_LambBotProximitySensor,
                                          public CSimulatedSensor {

   public:

      CLambBotProximityDefaultSensor();

      virtual ~CLambBotProximityDefaultSensor();

      virtual void SetRobot(CComposableEntity& c_entity);

      virtual void Init(TConfigurationNode& t_tree);

      virtual void Update();

      virtual void Reset();

   private:

      CProximityDefaultSensor* m_pcProximityImpl;

   };

}

#endif
