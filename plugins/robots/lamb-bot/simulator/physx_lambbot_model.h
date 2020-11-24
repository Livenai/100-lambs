/**
 * @file <argos3/plugins/robots/lamb-bot/simulator/physx_lambbot_model.h>
 *
 * @author Carlo Pinciroli - <ilpincy@gmail.com>
 */

#ifndef PHYSX_LAMBBOT_MODEL_H
#define PHYSX_LAMBBOT_MODEL_H

namespace argos {
   class CPhysXEngine;
   class CPhysXLambBotModel;
   class CLambBotEntity;
}

#include <argos3/plugins/simulator/physics_engines/physx/physx_single_body_object_model.h>

namespace argos {

   class CPhysXLambBotModel : public CPhysXSingleBodyObjectModel {

   public:

      CPhysXLambBotModel(CPhysXEngine& c_engine,
                         CLambBotEntity& c_entity);

      virtual void UpdateFromEntityStatus();

   private:

      CLambBotEntity& m_cLambBotEntity;
   };

}

#endif
