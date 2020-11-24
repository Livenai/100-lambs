/**
 * @file <argos3/plugins/simulator/robots/lamb-bot/simulator/pointmass3d/pointmass3d_lambbot_model.h>
 *
 * @author Carlo Pinciroli - <cpinciro@ulb.ac.be>
 */

#ifndef POINTMASS3D_LAMBBOT_H
#define POINTMASS3D_LAMBBOT_H

namespace argos {
   class CPointMass3DEngine;
   class CPointMass3DLambBotModel;
   class CLambBotEntity;
}

#include <argos3/plugins/simulator/physics_engines/pointmass3d/pointmass3d_model.h>
#include <argos3/core/simulator/entity/embodied_entity.h>
#include <argos3/plugins/robots/lamb-bot/simulator/lambbot_entity.h>

namespace argos {

   class CPointMass3DLambBotModel : public CPointMass3DModel {

   public:
      
      CPointMass3DLambBotModel(CPointMass3DEngine& c_engine,
                               CLambBotEntity& c_lambbot);

      virtual ~CPointMass3DLambBotModel() {}
      
      virtual void Reset();

      virtual void UpdateFromEntityStatus();
      virtual void Step();

      virtual void CalculateBoundingBox();

      virtual bool CheckIntersectionWithRay(Real& f_t_on_ray,
                                            const CRay3& c_ray) const;

      virtual void UpdateOriginAnchor(SAnchor& s_anchor);

   private:

      /** Reference to the wheeled entity */
      CWheeledEntity& m_cWheeledEntity;

      /** The yaw of the lamb-bot entity */
      CRadians m_cYaw;

      /** The yaw of the lamb-bot entity */
      Real m_fAngularVelocity;

      /** Current wheel velocity */
      const Real* m_fCurrentWheelVelocity;
   };

}

#endif
