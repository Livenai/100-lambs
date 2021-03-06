/**
 * @file <argos3/plugins/robots/lamb-bot/simulator/lambbot_light_rotzonly_sensor.h>
 *
 * @author Carlo Pinciroli - <ilpincy@gmail.com>
 */

#ifndef LAMBBOT_LIGHT_ROTZONLY_SENSOR_H
#define LAMBBOT_LIGHT_ROTZONLY_SENSOR_H

#include <string>
#include <map>

namespace argos {
   class CLambBotLightRotZOnlySensor;
   class CLightSensorEquippedEntity;
}

#include <argos3/plugins/robots/lamb-bot/control_interface/ci_lambbot_light_sensor.h>
#include <argos3/core/utility/math/range.h>
#include <argos3/core/utility/math/rng.h>
#include <argos3/core/simulator/space/space.h>
#include <argos3/core/simulator/sensor.h>

namespace argos {

   class CLambBotLightRotZOnlySensor : public CSimulatedSensor,
                                      public CCI_LambBotLightSensor {

   public:

      CLambBotLightRotZOnlySensor();

      virtual ~CLambBotLightRotZOnlySensor() {}

      virtual void SetRobot(CComposableEntity& c_entity);

      virtual void Init(TConfigurationNode& t_tree);

      virtual void Update();

      virtual void Reset();

     void Enable();

     void Disable();

      /**
       * Returns true if the rays must be shown in the GUI.
       * @return true if the rays must be shown in the GUI.
       */
      inline bool IsShowRays() {
         return m_bShowRays;
      }

      /**
       * Sets whether or not the rays must be shown in the GUI.
       * @param b_show_rays true if the rays must be shown, false otherwise
       */
      inline void SetShowRays(bool b_show_rays) {
         m_bShowRays = b_show_rays;
      }

   protected:

      /** Is this sensor currently enabled? */
      bool             m_bEnabled;
      /** Reference to embodied entity associated to this sensor */
      CEmbodiedEntity* m_pcEmbodiedEntity;

      /** Reference to light sensor equipped entity associated to this sensor */
      CLightSensorEquippedEntity* m_pcLightEntity;

      /** Reference to controllable entity associated to this sensor */
      CControllableEntity* m_pcControllableEntity;

      /** Flag to show rays in the simulator */
      bool m_bShowRays;

      /** Random number generator */
      CRandom::CRNG* m_pcRNG;

      /** Whether to add noise or not */
      bool m_bAddNoise;

      /** Noise range */
      CRange<Real> m_cNoiseRange;

      /** Reference to the space */
      CSpace& m_cSpace;
   };

}

#endif
