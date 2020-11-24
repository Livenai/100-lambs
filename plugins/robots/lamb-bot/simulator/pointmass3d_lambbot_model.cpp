/**
 * @file <argos3/plugins/simulator/physics_engines/dynamics2d/dynamics2d_differentialsteering_control.cpp>
 *
 * @author Carlo Pinciroli - <ilpincy@gmail.com>
 */

#include "pointmass3d_lambbot_model.h"
#include <argos3/core/utility/math/cylinder.h>

namespace argos {

   static const Real LAMBBOT_RADIUS                   = 0.085036758f;
   static const Real LAMBBOT_INTERWHEEL_DISTANCE      = 0.14f;
   static const Real LAMBBOT_HEIGHT                   = 0.146899733f;

   enum LAMBBOT_WHEELS {
      LAMBBOT_LEFT_WHEEL = 0,
      LAMBBOT_RIGHT_WHEEL = 1
   };

   /****************************************/
   /****************************************/

   CPointMass3DLambBotModel::CPointMass3DLambBotModel(CPointMass3DEngine& c_engine,
                                                      CLambBotEntity& c_lambbot) :
      CPointMass3DModel(c_engine, c_lambbot.GetEmbodiedEntity()),
      m_cWheeledEntity(c_lambbot.GetWheeledEntity()),
      m_fCurrentWheelVelocity(m_cWheeledEntity.GetWheelVelocities()) {
      /* Register the origin anchor update method */
      RegisterAnchorMethod(GetEmbodiedEntity().GetOriginAnchor(),
                           &CPointMass3DLambBotModel::UpdateOriginAnchor);
      /* Get initial rotation */
      CRadians cTmp1, cTmp2;
      GetEmbodiedEntity().GetOriginAnchor().Orientation.ToEulerAngles(m_cYaw, cTmp1, cTmp2);
   }

   /****************************************/
   /****************************************/

   void CPointMass3DLambBotModel::Reset() {
      CPointMass3DModel::Reset();
      CRadians cTmp1, cTmp2;
      GetEmbodiedEntity().GetOriginAnchor().Orientation.ToEulerAngles(m_cYaw, cTmp1, cTmp2);
      m_fAngularVelocity = 0.0;
   }

   /****************************************/
   /****************************************/


   void CPointMass3DLambBotModel::UpdateFromEntityStatus() {
      m_cVelocity.Set((m_fCurrentWheelVelocity[LAMBBOT_RIGHT_WHEEL] + m_fCurrentWheelVelocity[LAMBBOT_LEFT_WHEEL])*0.5, 0.0, 0.0);
      m_cVelocity.RotateZ(m_cYaw);
      m_fAngularVelocity = (m_fCurrentWheelVelocity[LAMBBOT_RIGHT_WHEEL] - m_fCurrentWheelVelocity[LAMBBOT_LEFT_WHEEL]) / LAMBBOT_INTERWHEEL_DISTANCE;
   }

   /****************************************/
   /****************************************/

   void CPointMass3DLambBotModel::Step() {
      m_cPosition += m_cVelocity * m_cPM3DEngine.GetPhysicsClockTick();
      m_cYaw += CRadians(m_fAngularVelocity * m_cPM3DEngine.GetPhysicsClockTick());
   }

   /****************************************/
   /****************************************/

   void CPointMass3DLambBotModel::CalculateBoundingBox() {
      GetBoundingBox().MinCorner.Set(
         GetEmbodiedEntity().GetOriginAnchor().Position.GetX() - LAMBBOT_RADIUS,
         GetEmbodiedEntity().GetOriginAnchor().Position.GetY() - LAMBBOT_RADIUS,
         GetEmbodiedEntity().GetOriginAnchor().Position.GetZ());
      GetBoundingBox().MaxCorner.Set(
         GetEmbodiedEntity().GetOriginAnchor().Position.GetX() + LAMBBOT_RADIUS,
         GetEmbodiedEntity().GetOriginAnchor().Position.GetY() + LAMBBOT_RADIUS,
         GetEmbodiedEntity().GetOriginAnchor().Position.GetZ() + LAMBBOT_HEIGHT);
   }

   /****************************************/
   /****************************************/

   bool CPointMass3DLambBotModel::CheckIntersectionWithRay(Real& f_t_on_ray,
                                                           const CRay3& c_ray) const {
      CCylinder m_cShape(LAMBBOT_RADIUS,
                         LAMBBOT_HEIGHT,
                         m_cPosition,
                         CVector3::Z);
      bool bIntersects = m_cShape.Intersects(f_t_on_ray, c_ray);
      return bIntersects;
   }

   /****************************************/
   /****************************************/

   void CPointMass3DLambBotModel::UpdateOriginAnchor(SAnchor& s_anchor) {
      s_anchor.Position = m_cPosition;
      s_anchor.Orientation = CQuaternion(m_cYaw, CVector3::Z);
   }

   /****************************************/
   /****************************************/
   
   REGISTER_STANDARD_POINTMASS3D_OPERATIONS_ON_ENTITY(CLambBotEntity, CPointMass3DLambBotModel);

   /****************************************/
   /****************************************/

}
