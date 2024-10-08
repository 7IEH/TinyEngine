#pragma once
#include "CCollider.h"

class CSphereCollider : public CCollider
{
private:
    float m_Radius;

public:
    virtual void begin() override;
    virtual void finaltick() override;

public:
    float GetRadius() const { return m_Radius; }
    void SetRadius(float _Radius);

public:
    virtual UINT SaveToLevelFile(FILE* _File) override;
    virtual UINT LoadFromLevelFile(FILE* _File) override;

    CLONE(CSphereCollider);

public:
    CSphereCollider();
    virtual ~CSphereCollider();

    friend class CPhysicsMgr;
};
