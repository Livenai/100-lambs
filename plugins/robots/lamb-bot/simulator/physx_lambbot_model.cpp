/**
 * @file <argos3/plugins/robots/lamb-bot/simulator/physx_lambbot_model.cpp>
 *
 * @author Carlo Pinciroli - <ilpincy@gmail.com>
 */

#include "physx_lambbot_model.h"
#include <argos3/plugins/robots/lamb-bot/simulator/lambbot_entity.h>

namespace argos {

   /****************************************/
   /****************************************/
   
   CPhysXLambBotModel::CPhysXLambBotModel(CPhysXEngine& c_engine,
                                                      CLambBotEntity& c_entity) :
      CPhysXSingleBodyObjectModel(c_engine, c_entity),
      m_cLambBotEntity(c_entity) {
      /* Calculate base center */
      SetARGoSReferencePoint(physx::PxVec3(0.0f, 0.0f, 0.0f));
      /* Get position and orientation in this engine's representation */
      physx::PxVec3 cPos;
      CVector3ToPxVec3(GetEmbodiedEntity().GetOriginAnchor().Position, cPos);
      physx::PxQuat cOrient;
      CQuaternionToPxQuat(GetEmbodiedEntity().GetOriginAnchor().Orientation, cOrient);
      /* Create the transform
       * 1. a translation up by half body height
       * 2. a rotation around the base
       * 3. a translation to the final position
       */
      physx::PxTransform cTranslation1(physx::PxVec3(0.0f, 0.0f, 0.0f));
      physx::PxTransform cRotation(cOrient);
      physx::PxTransform cTranslation2(cPos);
      physx::PxTransform cFinalTrans = cTranslation2 * cRotation * cTranslation1;
      /* Calculate bounding box */
      CalculateBoundingBox();
   }

   /****************************************/
   /****************************************/

   void CPhysXLambBotModel::UpdateFromEntityStatus() {
   }

   /****************************************/
   /****************************************/

   REGISTER_STANDARD_PHYSX_OPERATIONS_ON_ENTITY(CLambBotEntity, CPhysXLambBotModel);

   /****************************************/
   /****************************************/

}
