#pragma once
#include "CComponent.h"
#include "CMesh.h"
#include "CMaterial.h"

class CRenderComponent : public CComponent
{
private:
    Ptr<CMesh> m_Mesh;

    Ptr<CMaterial> m_SharedMtrl;
    Ptr<CMaterial> m_DynamicMtrl;
    Ptr<CMaterial> m_CurMtrl;

public:
    Ptr<CMesh> GetMesh() const { return m_Mesh; }
    void SetMesh(Ptr<CMesh> _Mesh) { m_Mesh = _Mesh; }

    Ptr<CMaterial> GetMaterial() const { return m_CurMtrl; }
    Ptr<CMaterial> GetSharedMaterial() const { return m_SharedMtrl; }
    Ptr<CMaterial> GetDynamicMaterial();
    void SetMaterial(Ptr<CMaterial> _Mtrl);

    void RestoreMaterial();

public:
    virtual void finaltick(){};
    virtual void UpdateData() = 0;
    virtual void render() = 0;
    virtual void render(Ptr<CMaterial> _mtrl) = 0;

public:
    virtual void SaveToLevelFile(FILE* _File) override;
    virtual void LoadFromLevelFile(FILE* _File) override;

public:
    CRenderComponent(COMPONENT_TYPE _Type);
    virtual ~CRenderComponent();
};
