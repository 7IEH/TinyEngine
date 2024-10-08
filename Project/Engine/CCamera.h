#pragma once

#include "CComponent.h"
#include "CFrustum.h"

enum class PROJ_TYPE
{
    ORTHOGRAPHIC, // 직교 투영
    PERSPECTIVE,  // 원근 투영
};

class CCamera : public CComponent
{
private:
    CFrustum m_Frustum;

    PROJ_TYPE m_ProjType; // 투영 방식

    // 원근투영(Perspective)
    float m_FOV; // 시야 각(Filed Of View)

    // 직교투영(Orthographic)
    float m_Width; // 직교투영 가로 길이
    float m_Scale; // 직교투영 배율

    // Both
    float m_AspectRatio; // 종횡비, 투영 가로세로 비율
    float m_Near;        // 투영 최소 거리
    float m_Far;         // 투영 최대 거리

    UINT m_LayerMask;

    int m_iCamPriority;

    bool m_bHDRI;

    // 변환 행렬
    Matrix m_matPrevView;
    Matrix m_matView;
    Matrix m_matViewInv;
    Matrix m_matProj;
    Matrix m_matProjInv;

    // 물체 분류
    map<ULONG64, vector<tInstObj>> m_mapInstGroup_D; // Deferred
    map<ULONG64, vector<tInstObj>> m_mapInstGroup_F; // Forward ( Opaque, Mask )
    map<INT_PTR, vector<tInstObj>> m_mapSingleObj;   // Single Object

    vector<CGameObject*> m_vecSkybox;
    vector<CGameObject*> m_vecDecal;
    vector<tInstObj> m_vecTransparent;
    vector<CGameObject*> m_vecPostProcess;

    vector<CGameObject*> m_vecShadow;

public:
    PROJ_TYPE GetProjType() const { return m_ProjType; }
    void SetProjType(PROJ_TYPE _Type) { m_ProjType = _Type; }

    float GetScale() const { return m_Scale; }
    void SetScale(float _Scale) { m_Scale = _Scale; }

    float GetFOV() const { return m_FOV; }
    void SetFOV(float _FOV) { m_FOV = _FOV; }

    float GetAspectRatio() const { return m_AspectRatio; }

    float GetNear() const { return m_Near; }
    void SetNear(float _near) { m_Near = _near; }

    float GetFar() const { return m_Far; }
    void SetFar(float _far) { m_Far = _far; }

    UINT GetLayerMask() const { return m_LayerMask; }
    void LayerMask(UINT _LayerMask) { m_LayerMask = _LayerMask; }
    void LayerMask(UINT _LayerIdx, bool _bMask);
    void LayerMask(CLevel* _CurLevel, const wstring& _strLayerName, bool _bMask);
    void LayerMaskAll() { m_LayerMask = 0xffffffff; }

    int GetCameraPriority() const { return m_iCamPriority; }
    void SetCameraPriority(int _Priority) { m_iCamPriority = _Priority; }

    bool IsHDRI() const { return m_bHDRI; }
    void SetHDRI(bool _Enable) { m_bHDRI = _Enable; }

    const Matrix& GetViewMat() const { return m_matView; }
    const Matrix& GetViewInvMat() const { return m_matViewInv; }
    const Matrix& GetProjMat() const { return m_matProj; }
    const Matrix& GetProjInvMat() const { return m_matProjInv; }

    void Resize(Vec2 Resolution);

    tRay GetRay();

public:
    virtual void begin() override;
    virtual void finaltick() override;

    void SortObject();
    void SortShadowMapObject(UINT _MobilityType = 0);

public:
    void render_Deferred();
    void render_Forward();
    void render_Postprocess();
    void render_DepthOnly(Ptr<CTexture> _DepthMapTex);

private:
    void render_Inst(const map<ULONG64, vector<tInstObj>>& _Group);

    void render_Decal();
    void render_SSAO();
    void render_Light();
    void render_Merge();
    void render_Transparent();

    void render_Clear();

private:
    void render(vector<CGameObject*>& _vecObj);
    void render_OutLine();
    void render_IDMap();
    void render_DepthOnly(vector<CGameObject*>& _vecObj);

public:
    virtual UINT SaveToLevelFile(FILE* _File) override;
    virtual UINT LoadFromLevelFile(FILE* _File) override;

    CLONE(CCamera);

public:
    CCamera();
    CCamera(const CCamera& origin);
    virtual ~CCamera();
};
