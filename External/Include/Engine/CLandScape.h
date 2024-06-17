#pragma once
#include "CRenderComponent.h"

#include "CHeightMapShader.h"
#include "CRaycastShader.h"
#include "CWeightMapShader.h"

enum class LANDSCAPE_MODE
{
    HEIGHT_MAP,
    SPLAT,
    NONE,
};

struct tWeight_4
{
    float arrWeight[4];
};

class CLandScape : public CRenderComponent
{
private:
    UINT m_FaceX;
    UINT m_FaceZ;

    Ptr<CHeightMapShader> m_CSHeightMap; // ���̸� ���̴�
    Ptr<CTexture> m_HeightMapTex;        // ���̸�

    Ptr<CRaycastShader> m_CSRaycast;  // ��ŷ ���̴�
    CStructuredBuffer* m_CrossBuffer; // ���콺 ��ŷ�Ǵ� ���� ���� �޴� ����

    Ptr<CWeightMapShader> m_CSWeightMap;  // ����ġ ���̴�
    CStructuredBuffer* m_WeightMapBuffer; // ����ġ ���� ����
    UINT m_WeightWidth;                   // ����ġ ���� ���μ��� ��� ��
    UINT m_WeightHeight;                  // ����ġ ���� ���μ��� ��� ��
    UINT m_WeightIdx;                     // ���� ��ų ����ġ ����

    Ptr<CTexture> m_TileArrTex; // Ÿ�� �迭 �ؽ���

    LANDSCAPE_MODE m_Mode;

    Ptr<CTexture> m_BrushTex; // �귯���� �ؽ���
    float m_BrushStrength;    // �귯�� ����
    Vec2 m_BrushScale;        // �귯�� ũ��(��ü ������� ũ�� ������)

public:
    void SetHeightMap(Ptr<CTexture> _HeightMap) { m_HeightMapTex = _HeightMap; }

    LANDSCAPE_MODE GetLandScapeMode() const { return m_Mode; }
    void SetLandScapeMode(LANDSCAPE_MODE _Mode) { m_Mode = _Mode; }

    Ptr<CTexture> GetBrushTex() const { return m_BrushTex; }
    void SetBrushTex(Ptr<CTexture> _Tex) { m_BrushTex = _Tex; }

    float GetBrushStrength() const { return m_BrushStrength; }
    void SetBrushStrength(float _Strength) { m_BrushStrength = _Strength; }

    Vec2 GetBrushScale() const { return m_BrushScale; }
    void SetBrushScale(Vec2 _Scale) { m_BrushScale = _Scale; }

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
    virtual void render(UINT _Subset) override;

public:
    CLONE(CLandScape);

    virtual void SaveToLevelFile(FILE* _File) override;
    virtual void LoadFromLevelFile(FILE* _File) override;

public:
    CLandScape();
    virtual ~CLandScape();
};
