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

    // ��ȯ ���
    Matrix m_matView;
    Matrix m_matProj;

    UINT m_LayerCheck;

    // ��ü �з�
    vector<CGameObject*> m_vecOpaque;
    vector<CGameObject*> m_vecMaked;
    vector<CGameObject*> m_vecTransparent;

private:
    float m_CamSpeed;

    int m_iCamPriority; // ī�޶� �켱����

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

    const Matrix& GetViewMat() const { return m_matView; }
    const Matrix& GetProjMat() const { return m_matProj; }

    void SetCameraPriority(int _Priority);
    void SetUICamera();
    void LayerCheck(UINT _LayerIdx, bool _bCheck);
    void LayerCheck(const wstring& _strLayerName, bool _bCheck);
    void LayerCheckAll() { m_LayerCheck = 0xffffffff; }

public:
    float GetCameraSpeed() const { return m_CamSpeed; }
    void SetCameraSpeed(float speed) { m_CamSpeed = speed; }
    void Resize(Vec2 Resolution);

public:
    virtual void begin() override;
    virtual void finaltick() override;

    void SortObject();
    void render();

    // Render pass
    void render_DepthMap();
    void render_NormalLine();
    void render_IDMap();

    void clear();

private:
    void render(vector<CGameObject*>& _vecObj);
    void render_DepthMap(vector<CGameObject*>& _vecObj);
    void render_NormalLine(vector<CGameObject*>& _vecObj);
    void render_IDMap(vector<CGameObject*>& _vecObj);

public:
    virtual void SaveToLevelFile(FILE* _File) override;
    virtual void LoadFromLevelFile(FILE* _File) override;

public:
    CCamera();
    virtual ~CCamera();
};
