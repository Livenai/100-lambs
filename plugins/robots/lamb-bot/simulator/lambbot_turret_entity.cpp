/**
 * @file <argos3/plugins/robots/lamb-bot/simulator/lambbot_turret_entity.cpp>
 *
 * @author Carlo Pinciroli - <ilpincy@gmail.com>
 */

#include "lambbot_turret_entity.h"
#include <argos3/core/utility/math/angles.h>
#include <argos3/core/simulator/physics_engine/physics_engine.h>
#include <argos3/core/simulator/space/space.h>

namespace argos {

   /****************************************/
   /****************************************/

   CLambBotTurretEntity::CLambBotTurretEntity(CComposableEntity* pc_parent) :
      CEntity(pc_parent),
      m_psAnchor(NULL) {
      Reset();
      Disable();
   }
   
   /****************************************/
   /****************************************/

   CLambBotTurretEntity::CLambBotTurretEntity(CComposableEntity* pc_parent,
                                              const std::string& str_id,
                                              SAnchor& s_anchor) :
      CEntity(pc_parent, str_id),
      m_psAnchor(&s_anchor) {
      Reset();
      Disable();
   }
   
   /****************************************/
   /****************************************/

   void CLambBotTurretEntity::Init(TConfigurationNode& t_tree) {
   }

   /****************************************/
   /****************************************/

   void CLambBotTurretEntity::Reset() {
      m_unMode = MODE_OFF;
      m_cDesRot = CRadians::ZERO;
      m_cOldRot = CRadians::ZERO;
      m_fDesRotSpeed = 0.0;
      m_fCurRotSpeed = 0.0;
      if (m_psAnchor) {
         m_psAnchor->OffsetOrientation = CQuaternion();
      }
   }

   /****************************************/
   /****************************************/

   void CLambBotTurretEntity::Update() {
      /* Calculate rotation speed */
      CRadians cZAngle, cYAngle, cXAngle;
      m_psAnchor->OffsetOrientation.ToEulerAngles(cZAngle, cYAngle, cXAngle);
      m_fCurRotSpeed =
         NormalizedDifference(cZAngle, m_cOldRot).GetValue() *
         CPhysicsEngine::GetInverseSimulationClockTick();
      /* Save rotation for next time */
      m_cOldRot = cZAngle;
   }

   /****************************************/
   /****************************************/

   CRadians CLambBotTurretEntity::GetRotation() const {
      CRadians cZAngle, cYAngle, cXAngle;
      m_psAnchor->OffsetOrientation.ToEulerAngles(cZAngle, cYAngle, cXAngle);
      return cZAngle;
   }

   /****************************************/
   /****************************************/

   Real CLambBotTurretEntity::GetRotationSpeed() const {
      return m_fCurRotSpeed;
   }

   /****************************************/
   /****************************************/

   const CRadians& CLambBotTurretEntity::GetDesiredRotation() const {
      return m_cDesRot;
   }

   /****************************************/
   /****************************************/

   Real CLambBotTurretEntity::GetDesiredRotationSpeed() const {
      return m_fDesRotSpeed;
   }

   /****************************************/
   /****************************************/

   void CLambBotTurretEntity::SetDesiredRotation(const CRadians& c_rotation) {
      m_cDesRot = c_rotation;
      m_cDesRot.SignedNormalize();
   }

   /****************************************/
   /****************************************/

   void CLambBotTurretEntity::SetDesiredRotationSpeed(Real f_speed) {
      m_fDesRotSpeed = f_speed;
   }

   /****************************************/
   /****************************************/

   REGISTER_STANDARD_SPACE_OPERATIONS_ON_ENTITY(CLambBotTurretEntity);

   /****************************************/
   /****************************************/

}
