#include "trough_entity.h"
#include <argos3/core/utility/math/matrix/rotationmatrix3.h>
#include <argos3/core/simulator/space/space.h>
#include <argos3/core/simulator/simulator.h>

namespace argos {

/****************************************/
/****************************************/

CTroughEntity::CTroughEntity():
                CComposableEntity(NULL),
                m_pcEmbodiedEntity(NULL),
                m_fMass(0.0f),
                scale(1),
                trough_type(WATER) {}

/****************************************/
/****************************************/

void CTroughEntity::Init(TConfigurationNode& t_tree) {
    try {
        /* Parse XML */
        CComposableEntity::Init(t_tree);
        GetNodeAttributeOrDefault(t_tree, "scale", scale, scale);
        std::string type;
        GetNodeAttributeOrDefault(t_tree, "type", type, type);
        if(type == "water")
            trough_type = WATER;
        else if(type == "food")
            trough_type = FOOD;
        /* Create embodied entity using parsed data */
        m_pcEmbodiedEntity = new CEmbodiedEntity(this);
        AddComponent(*m_pcEmbodiedEntity);
        m_pcEmbodiedEntity->Init(GetNode(t_tree, "body"));
        m_pcEmbodiedEntity->SetMovable(false);


        UpdateComponents();
    }
    catch(CARGoSException& ex) {
        THROW_ARGOSEXCEPTION_NESTED("Failed to initialize trough entity \"" << GetId() << "\".", ex);
    }
}

/****************************************/
/****************************************/

void CTroughEntity::Reset() {
    /* Reset all components */
    CComposableEntity::Reset();
    /* Update components */
    UpdateComponents();
}

/****************************************/
/****************************************/


/****************************************/
/****************************************/
//TODO cambiar descripcion si eso
REGISTER_ENTITY(CTroughEntity,
    "trough",
    "Carlo Pinciroli [ilpincy@gmail.com]",
    "1.0",
    "A trough for water or food",
    //TODO descripcion y configuracion
    "descripcion larga TODO",
    "Usable"
);

/****************************************/
/****************************************/

REGISTER_STANDARD_SPACE_OPERATIONS_ON_COMPOSABLE(CTroughEntity);

/****************************************/
/****************************************/

}
