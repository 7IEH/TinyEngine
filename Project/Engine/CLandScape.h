#pragma once
#include "CRenderComponent.h"

#include "CHeightMapShader.h"
#include "CRaycastShader.h"

class CLandScape : public CRenderComponent
{
private:
    UINT m_FaceX;
    UINT m_FaceZ;

    Vec2 m_BrushScale;        // �귯�� ũ��(��ü ������� ũ�� ������)
    Ptr<CTexture> m_BrushTex; // �귯���� �ؽ���

    Ptr<CHeightMapShader> m_CSHeightMap; // ���̸� ���̴�
    Ptr<CTexture> m_HeightMapTex;        // ���̸�

    Ptr<CRaycastShader> m_CSRaycast;  // ��ŷ ���̴�
    CStructuredBuffer* m_CrossBuffer; // ���콺 ��ŷ�Ǵ� ���� ���� �޴� ����

public:
    void SetHeightMap(Ptr<CTexture> _HeightMap) { m_HeightMapTex = _HeightMap; }

private:
    void Init();
    void CreateMesh();
    void CreateComputeShader();
    void CreateTexture();

public:
    void Raycasting();

public:
    virtual void finaltick() override;
    virtual void UpdateData() override;
    virtual void render() override;
    virtual void render(Ptr<CMaterial> _mtrl) override;

public:
    CLONE(CLandScape);

    virtual void SaveToLevelFile(FILE* _File) override;
    virtual void LoadFromLevelFile(FILE* _File) override;

public:
    CLandScape();
    virtual ~CLandScape();
};
