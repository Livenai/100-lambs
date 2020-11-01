#ifndef DYNAMICS2D_TROUGH_MODEL_H
#define DYNAMICS2D_TROUGH_MODEL_H

namespace argos {
   class CDynamics2DStretchableObjectModel;
   class CDynamics2DTroughModel;
}

#include <argos3/plugins/simulator/physics_engines/dynamics2d/dynamics2d_stretchable_object_model.h>
#include "trough_entity.h"

namespace argos {

   class CDynamics2DTroughModel : public CDynamics2DStretchableObjectModel {

   public:

      CDynamics2DTroughModel(CDynamics2DEngine& c_engine,
                          CTroughEntity& c_entity);
      virtual ~CDynamics2DTroughModel() {}
   };

}

#endif
