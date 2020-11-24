/**
 * @file <argos3/plugins/robots/lamb-bot/control_interface/ci_lambbot_encoder_sensor.h>
 *
 * @brief This file provides the definition of the lambbot encoder sensor. It

 * This file provides the definition of the lambbot encoder sensor. It
 * returns the distance covered by the wheels in the last control step.
 * Moreover, the interaxis distance of each robot is unique.
 * On the lambbot there is a sensor that, at start-up, measure this distance,
 * i.e. the external distance between the two wheels.
 * This value is measured once at start-up and then is left constant.
 *
 * @author Carlo Pinciroli - <ilpincy@gmail.com>
 */

#ifndef CCI_LAMBBOT_ENCODER_SENSOR_H
#define CCI_LAMBBOT_ENCODER_SENSOR_H

/* To avoid dependency problems when including */
namespace argos {
   class CCI_LambBotEncoderSensor;
}

#include <argos3/core/control_interface/ci_sensor.h>

namespace argos {

   class CCI_LambBotEncoderSensor : public CCI_Sensor {

   public:

      /**
       * The DTO of the encoder
       * It returns the distance covered by the wheels in the last timestep
       * In addition it returns the value of the interaxis distance.
       * i.e. the external distance between the two wheels.
       * This value is measured once at start-up and then is left constant.
       */
      struct SReading {
         Real CoveredDistanceLeftWheel;
         Real CoveredDistanceRightWheel;
         Real WheelAxisLength;

         SReading() :
            CoveredDistanceLeftWheel(0.0f),
            CoveredDistanceRightWheel(0.0f),
            WheelAxisLength(0.0f) {}

         SReading(Real f_covered_distance_left_wheel,
                  Real f_covered_distance_right_wheel,
                  Real f_wheel_axis_length) :
            CoveredDistanceLeftWheel(f_covered_distance_left_wheel),
            CoveredDistanceRightWheel(f_covered_distance_right_wheel),
            WheelAxisLength(f_wheel_axis_length) {
         }

      };

      /**
       * Destructor
       */
      virtual ~CCI_LambBotEncoderSensor() {}

      /**
       * @brief Returns the reading of the encoder sensor
       * Returns the reading of the encoder sensor
       */
      const SReading& GetReading() const;

#ifdef ARGOS_WITH_LUA
      virtual void CreateLuaState(lua_State* pt_lua_state);

      virtual void ReadingsToLuaState(lua_State* pt_lua_state);
#endif

   protected:

      SReading m_sReading;
   };

}

#endif
