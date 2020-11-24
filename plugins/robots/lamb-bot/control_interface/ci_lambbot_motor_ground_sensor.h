/**
 * @file <argos3/plugins/robots/lamb-bot/control_interface/ci_lambbot_motor_ground_sensor.h>
 *
 * @brief This file provides the interface of the lambbot motor ground sensor.
 *
 * On the robot, there are two kinds of ground sensors.
 *
 * The BASE ground sensors are located on the main PCB, intertwined with the
 * proximity sensors and evenly spaced. They are 8 and are useful for detecting
 * holes.
 *
 * The MOTOR ground sensors are built on the motor PCB and located close to the
 * motors. They are 4 and useful to detect changes in color on the ground (i.e.,
 * for line following).
 *
 * This sensor provides access to the motor sensors. The readings are in the following
 * order (seeing the robot from TOP, battery socket is the BACK):
 *
 *       front
 *
 * l|w          r|w
 * e|h   1  0   i|h
 * f|e          g|e
 * t|e   2  3   h|e
 *  |l          t|l
 *
 *       back
 *
 * @author Carlo Pinciroli <ilpincy@gmail.com>
 */

#ifndef CCI_LAMBBOT_MOTOR_GROUND_SENSOR_H
#define CCI_LAMBBOT_MOTOR_GROUND_SENSOR_H

namespace argos {
   class CCI_LambBotMotorGroundSensor;
}

#include <argos3/core/control_interface/ci_sensor.h>
#include <argos3/core/utility/math/vector2.h>
#include <argos3/core/utility/math/range.h>
#include <vector>

namespace argos {

   class CCI_LambBotMotorGroundSensor : public CCI_Sensor {

   public:

      struct SReading {
         Real Value;
         CVector2 Offset;

         SReading() :
            Value(0.0f) {}

         SReading(Real f_value,
                  const CVector2& c_offset) :
            Value(f_value),
            Offset(c_offset) {}
      };

      typedef std::vector<SReading> TReadings;

   public:

      CCI_LambBotMotorGroundSensor();

      virtual ~CCI_LambBotMotorGroundSensor() {}

      const TReadings& GetReadings() const;

#ifdef ARGOS_WITH_LUA
      virtual void CreateLuaState(lua_State* pt_lua_state);

      virtual void ReadingsToLuaState(lua_State* pt_lua_state);
#endif

   protected:

      TReadings m_tReadings;

   };

   std::ostream& operator<<(std::ostream& c_os, const CCI_LambBotMotorGroundSensor::SReading& s_reading);
   std::ostream& operator<<(std::ostream& c_os, const CCI_LambBotMotorGroundSensor::TReadings& t_readings);

}

#endif
