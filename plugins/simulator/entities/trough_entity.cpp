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
                m_cSize(1.50,0.5,0.25),
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
        m_cSize *= scale;
        std::string type;
        GetNodeAttributeOrDefault(t_tree, "type", type, type);
        if(type == "water")
            trough_type = WATER;
        else if(type == "food")
            trough_type = FOOD;
        /* Create embodied entity using parsed data */
        m_pcEmbodiedEntity = new CEmbodiedEntity(this);
        AddComponent(*m_pcEmbodiedEntity);
        TConfigurationNode body = GetNode(t_tree, "body");
        GetNodeAttribute(body, "position", pos);
        pos2d = CVector2(pos.GetX(), pos.GetY());
        m_pcEmbodiedEntity->Init(body);
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
