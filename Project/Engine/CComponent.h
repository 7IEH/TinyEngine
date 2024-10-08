#pragma once
#include "CEntity.h"

#include "CGameObject.h"

#define GET_OTHER_COMPONENT(Type)                                                                                                                    \
    C##Type* Type()                                                                                                                                  \
    {                                                                                                                                                \
        return m_Owner->Type();                                                                                                                      \
    }

class CComponent : public CEntity
{
private:
    const COMPONENT_TYPE m_Type;
    CGameObject* m_Owner;

public:
    virtual void begin() {}
    virtual void tick() {}
    virtual void finaltick() = 0;
    virtual void UpdateData(){};

public:
    COMPONENT_TYPE GetComponentType() const { return m_Type; }
    CGameObject* GetOwner() const { return m_Owner; }

public:
    GET_OTHER_COMPONENT(Transform);
    GET_OTHER_COMPONENT(Animator2D);
    GET_OTHER_COMPONENT(Animator);
    GET_OTHER_COMPONENT(Light2D);
    GET_OTHER_COMPONENT(Light);
    GET_OTHER_COMPONENT(Camera);
    GET_OTHER_COMPONENT(Rigidbody2D);
    GET_OTHER_COMPONENT(BoxCollider2D);
    GET_OTHER_COMPONENT(CircleCollider2D);
    GET_OTHER_COMPONENT(PolygonCollider2D);
    GET_OTHER_COMPONENT(EdgeCollider2D);
    GET_OTHER_COMPONENT(Rigidbody);
    GET_OTHER_COMPONENT(BoxCollider);
    GET_OTHER_COMPONENT(SphereCollider);
    GET_OTHER_COMPONENT(CapsuleCollider);
    GET_OTHER_COMPONENT(MeshCollider);
    GET_OTHER_COMPONENT(CharacterController);
    GET_OTHER_COMPONENT(SoundListener);
    GET_OTHER_COMPONENT(MeshRender);
    GET_OTHER_COMPONENT(TileMap);
    GET_OTHER_COMPONENT(ParticleSystem);
    GET_OTHER_COMPONENT(SkyBox);
    GET_OTHER_COMPONENT(Decal);
    GET_OTHER_COMPONENT(LandScape);
    GET_OTHER_COMPONENT(TextRender);

public:
    virtual UINT SaveToLevelFile(FILE* _File) = 0;
    virtual UINT LoadFromLevelFile(FILE* _File) = 0;

    virtual CComponent* Clone() = 0;

public:
    CComponent(COMPONENT_TYPE _Type);
    CComponent(const CComponent& origin);
    virtual ~CComponent();

    friend class CGameObject;
};
