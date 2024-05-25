#pragma once
#include "CComponent.h"
#include "CFrustum.h"

enum class PROJ_TYPE
{
    ORTHOGRAPHIC, // ���� ����
    PERSPECTIVE,  // ���� ����
};

class CCamera : public CComponent
{
private:
    CFrustum m_Frustum;

    PROJ_TYPE m_ProjType; // ���� ���

    // ��������(Perspective)
    float m_FOV; // �þ� ��(Filed Of View)

    // ��������(Orthographic)
    float m_Width; // �������� ���� ����
    float m_Scale; // �������� ����

    // Both
    float m_AspectRatio; // ��Ⱦ��, ���� ���μ��� ����
    float m_Near;        // ���� �ּ� �Ÿ�
    float m_Far;         // ���� �ִ� �Ÿ�

    UINT m_LayerMask;

    int m_iCamPriority;

    tRay m_Ray; // ���콺 ������ ���ϴ� ����

    bool m_bHDRI;

    // ��ȯ ���
    Matrix m_matView;
    Matrix m_matViewInv;
    Matrix m_matProj;
    Matrix m_matProjInv;

    // ��ü �з�
    vector<CGameObject*> m_vecDeferred;
    vector<CGameObject*> m_vecDecal;
    vector<CGameObject*> m_vecOpaque;
    vector<CGameObject*> m_vecMaked;
    vector<CGameObject*> m_vecTransparent;
    vector<CGameObject*> m_vecPostProcess;

    vector<CGameObject*> m_vecShadow;

public:
    PROJ_TYPE GetProjType() const { return m_ProjType; }
    void SetProjType(PROJ_TYPE _Type) { m_ProjType = _Type; }

    float GetScale() const { return m_Scale; }
    void SetScale(float _Scale) { m_Scale = _Scale; }

    float GetFOV() const { return m_FOV; }
    void SetFOV(float _FOV) { m_FOV = _FOV; }

    float GetNear() const { return m_Near; }
    void SetNear(float _near) { m_Near = _near; }

    float GetFar() const { return m_Far; }
    void SetFar(float _far) { m_Far = _far; }

    UINT GetLayerMask() const { return m_LayerMask; }
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

    const tRay& GetRay() const { return m_Ray; }

public:
    virtual void begin() override;
    virtual void finaltick() override;

    void SortObject();
    void SortShadowMapObject(UINT _MobilityType = 0);

public:
    void render_Deferred();
    void render_Forward();
    void render_DepthOnly(Ptr<CTexture> _DepthMapTex);

private:
    void render_Decal();
    void render_SSAO();
    void render_Light();
    void render_Merge();
    void render_Clear();

private:
    void render(vector<CGameObject*>& _vecObj);
    void render_OutLine(vector<CGameObject*>& _vecObj);
    void render_DepthOnly(vector<CGameObject*>& _vecObj);
    void render_IDMap(vector<CGameObject*>& _vecObj);
    void render_Postprocess();

    void CalculateRay();

public:
    virtual void SaveToLevelFile(FILE* _File) override;
    virtual void LoadFromLevelFile(FILE* _File) override;

    CLONE(CCamera);

public:
    CCamera();
    CCamera(const CCamera& origin);
    virtual ~CCamera();
};
