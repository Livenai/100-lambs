/**
 * @file <argos3/plugins/robots/lamb-bot/control_interface/ci_lambbot_turret_encoder_sensor.h>
 *
 * @brief This file provides the common interface definition of the lambbot turret encoder
 * sensor. The sensor provides a measure of the rotation of the turret.
 *
 * The turret rotation is expressed in radians [-pi,pi], counter-clockwise positive when
 * looking from above.
 *
 * @author Carlo Pinciroli - <ilpincy@gmail.com>
 */

#ifndef CCI_LAMBBOT_TURRET_ENCODER_SENSOR_H
#define CCI_LAMBBOT_TURRET_ENCODER_SENSOR_H

namespace argos {
   class CCI_LambBotTurretEncoderSensor;
}

#include <argos3/core/control_interface/ci_sensor.h>
#include <argos3/core/utility/math/vector2.h>

namespace argos {

   class CCI_LambBotTurretEncoderSensor : virtual public CCI_Sensor {

   public:

      static const CRange<CRadians> ANGULAR_RANGE;

   public:

      virtual ~CCI_LambBotTurretEncoderSensor() {}

      const CRadians& GetRotation() const;

#ifdef ARGOS_WITH_LUA
      virtual void CreateLuaState(lua_State* pt_lua_state);

      virtual void ReadingsToLuaState(lua_State* pt_lua_state);
#endif

   protected:

      CRadians m_cRotation;

      friend class CCI_LambBotTurretActuator;
      friend class CRealLambBotTurretActuator;

   };

}

#endif
