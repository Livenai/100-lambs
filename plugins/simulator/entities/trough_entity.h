#ifndef TROUGH_ENTITY_H
#define TROUGH_ENTITY_H

namespace argos {
   class CTroughEntity;
}

#include <argos3/core/simulator/entity/embodied_entity.h>
#include <argos3/core/simulator/entity/composable_entity.h>
#include <string>

namespace argos {

    enum Trough_type{WATER, FOOD};
class CTroughEntity : public CComposableEntity {
public:

    ENABLE_VTABLE();

    CTroughEntity();

    virtual void Init(TConfigurationNode& t_tree);
    virtual void Reset();


    inline CEmbodiedEntity& GetEmbodiedEntity() {
        return *m_pcEmbodiedEntity;
    }

    inline const CEmbodiedEntity& GetEmbodiedEntity() const {
        return *m_pcEmbodiedEntity;
    }


    inline const Trough_type GetType() const{
         return trough_type;
     }

     inline const CVector3& GetSize() const {
        return m_cSize;
     }

     inline void SetSize(const CVector3& c_size) {
        m_cSize = c_size;
     }

    inline Real GetScale() {
        return scale;
    }

    inline void SetScale(Real c_scale) {
        scale = c_scale;
    }

    inline CVector2 GetPos() {
        return pos2d ;
    }


    virtual std::string GetTypeDescription() const {
        return "trough";
    }

private:
    CEmbodiedEntity*    m_pcEmbodiedEntity;
    CVector3            m_cSize;
    Real                m_fMass;
    Real                scale;
    Trough_type trough_type;
    CVector3 pos;
    CVector2 pos2d;

};

}

#endif
