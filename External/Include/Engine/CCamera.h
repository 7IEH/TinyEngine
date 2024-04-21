#pragma once
#include "CComponent.h"

enum class PROJ_TYPE
{
    ORTHOGRAPHIC, // ���� ����
    PERSPECTIVE,  // ���� ����
};

class CCamera : public CComponent
{
private:
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

    bool m_bHDRI;

    // ��ȯ ���
    Matrix m_matView;
    Matrix m_matProj;

    // ��ü �з�
    vector<CGameObject*> m_vecOpaque;
    vector<CGameObject*> m_vecMaked;
    vector<CGameObject*> m_vecTransparent;
    vector<CGameObject*> m_vecPostProcess;

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
    const Matrix& GetProjMat() const { return m_matProj; }

    void Resize(Vec2 Resolution);

public:
    virtual void begin() override;
    virtual void finaltick() override;

    void SortObject();
    void render();
    void render_DepthOnly(Ptr<CTexture> _DepthMapTex);

private:
    void render(vector<CGameObject*>& _vecObj);
    void render_DepthOnly(vector<CGameObject*>& _vecObj);
    void render_IDMap(vector<CGameObject*>& _vecObj);
    void render_postprocess();

public:
    virtual void SaveToLevelFile(FILE* _File) override;
    virtual void LoadFromLevelFile(FILE* _File) override;

    CLONE(CCamera);

public:
    CCamera();
    virtual ~CCamera();
};
