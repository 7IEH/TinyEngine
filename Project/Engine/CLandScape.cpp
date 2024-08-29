#include "pch.h"
#include "CLandScape.h"

#include "CKeyMgr.h"
#include "CTransform.h"
#include "CRenderMgr.h"

#include "CCamera.h"

CLandScape::CLandScape()
    : CRenderComponent(COMPONENT_TYPE::LANDSCAPE)
    , m_FaceX(64)
    , m_FaceZ(64)
    , m_CSHeightMap(nullptr)
    , m_HeightMapTex(nullptr)
    , m_CSRaycast(nullptr)
    , m_CrossBuffer(nullptr)
    , m_CSWeightMap(nullptr)
    , m_WeightMapBuffer(nullptr)
    , m_WeightWidth(0)
    , m_WeightHeight(0)
    , m_WeightIdx(0)
    , m_TileArrTex(nullptr)
    , m_Mode(LANDSCAPE_MODE::NONE)
    , m_BrushTex(nullptr)
    , m_BrushStrength(1.f)
    , m_BrushScale(Vec2(0.3f, 0.3f))

{
    Init();
    SetFrustumCheck(false);
}

CLandScape::~CLandScape()
{
    if (nullptr != m_CrossBuffer)
    {
        delete m_CrossBuffer;
        m_CrossBuffer = nullptr;
    }

    if (nullptr != m_WeightMapBuffer)
    {
        delete m_WeightMapBuffer;
        m_WeightMapBuffer = nullptr;
    }
}

void CLandScape::finaltick()
{
    if (LANDSCAPE_MODE::NONE == m_Mode)
    {
        return;
    }

    if (KEY_PRESSED(KEY::LBTN))
    {
        Raycasting();

        if (LANDSCAPE_MODE::HEIGHT_MAP == m_Mode)
        {
            // ���� ��ġ������ ���� ���̸� ���� ��
            m_CSHeightMap->SetInputBuffer(m_CrossBuffer); // ��ŷ ������ HeightMapShader �� ����

            m_CSHeightMap->SetBrushTex(m_BrushTex); // ����� �귯�� �ؽ��� ����
            m_CSHeightMap->SetBrushIndex(0);        // �귯�� �ε��� ����

            // Shift Ű�� ���� ���¸� ���� ����
            if (KEY_TAP(KEY::LSHIFT) || KEY_PRESSED(KEY::LSHIFT))
                m_CSHeightMap->SetBrushStrength(m_BrushStrength * -1.f);
            else
                m_CSHeightMap->SetBrushStrength(m_BrushStrength);

            m_CSHeightMap->SetBrushScale(m_BrushScale); // �귯�� ũ��
            m_CSHeightMap->SetHeightMap(m_HeightMapTex);
            m_CSHeightMap->Execute();
        }
        else if (LANDSCAPE_MODE::SPLAT == m_Mode)
        {
            // ��ŷ ��ġ������ ���� ����ġ�� ������
            m_CSWeightMap->SetInputBuffer(m_CrossBuffer); // ���� ĳ��Ʈ ��ġ
            m_CSWeightMap->SetBrushArrTex(m_BrushTex);
            m_CSWeightMap->SetBrushIndex(0);
            m_CSWeightMap->SetBrushScale(m_BrushScale);                                    // �귯�� ũ��
            m_CSWeightMap->SetWeightMap(m_WeightMapBuffer, m_WeightWidth, m_WeightHeight); // ����ġ��, ���� ���� ����
            m_CSWeightMap->SetWeightIdx(m_WeightIdx);
            m_CSWeightMap->Execute();
        }
    }
}

void CLandScape::render()
{
    UpdateData();

    GetMesh()->render(0);

    // ����ġ ���� Ŭ����(��ǻ�� ���̴������� ����ؼ�)
    m_WeightMapBuffer->Clear(17);
}

void CLandScape::render(Ptr<CMaterial> _mtrl)
{
    Transform()->UpdateData();
    _mtrl->UpdateData();

    GetMesh()->render(0);
}

void CLandScape::render(UINT _Subset)
{
    render();
}

UINT CLandScape::SaveToLevelFile(FILE* _File)
{
    UINT MemoryByte = 0;

    fwrite(&m_BrushStrength, 1, sizeof(float), _File);
    fwrite(&m_BrushScale, 1, sizeof(Vec2), _File);

    MemoryByte += sizeof(float);
    MemoryByte += sizeof(Vec2);

    return MemoryByte;
}

UINT CLandScape::LoadFromLevelFile(FILE* _File)
{
    UINT MemoryByte = 0;

    fread(&m_BrushStrength, 1, sizeof(float), _File);
    fread(&m_BrushScale, 1, sizeof(Vec2), _File);

    MemoryByte += sizeof(float);
    MemoryByte += sizeof(Vec2);

    return MemoryByte;
}

void CLandScape::UpdateData()
{
    Transform()->UpdateData();

    GetMaterial(0)->SetScalarParam(SCALAR_PARAM::INT_0, m_FaceX);
    GetMaterial(0)->SetScalarParam(SCALAR_PARAM::INT_1, m_FaceZ);
    GetMaterial(0)->SetTexParam(TEX_PARAM::TEX_3, m_HeightMapTex);

    // ����ġ ���� ����
    m_WeightMapBuffer->UpdateData(17);

    // ����ġ ���� �ػ� ����
    Vec2 vWeightMapResolution = Vec2((float)m_WeightWidth, (float)m_WeightHeight);
    GetMaterial(0)->SetScalarParam(SCALAR_PARAM::VEC2_0, vWeightMapResolution);

    // Ÿ�� �ؽ��� ����
    if (nullptr != m_TileArrTex)
    {
        GetMaterial(0)->SetTexParam(TEX_PARAM::TEXARR_0, m_TileArrTex);

        // Ÿ�� �迭 ���� ����
        float m_fTileCount = float(m_TileArrTex->GetArraySize() / 2); // ����, �븻 �������־ ������ 2 ����
        GetMaterial(0)->SetScalarParam(SCALAR_PARAM::FLOAT_0, m_fTileCount);
    }

    GetMaterial(0)->UpdateData();
}

void CLandScape::Raycasting()
{
    // ���� ī�޶� ������
    CCamera* pMainCam = CRenderMgr::GetInst()->GetMainCamera();
    if (nullptr == pMainCam)
        return;

    // ���� ���� ������ ������ ���÷� ����
    const Matrix& matWorldInv = Transform()->GetWorldInvMat();
    const tRay ray = pMainCam->GetRay();

    tRay CamRay = {};
    CamRay.vStart = XMVector3TransformCoord(ray.vStart, matWorldInv);
    CamRay.vDir = XMVector3TransformNormal(ray.vDir, matWorldInv);
    CamRay.vDir.Normalize();

    // ������ ī�޶� Ray �� ������ ����
    tRaycastOut out = {Vec2(0.f, 0.f), (float)0x7fffffff, 0};
    m_CrossBuffer->SetData(&out, 1);

    m_CSRaycast->SetHeightMap(m_HeightMapTex);
    m_CSRaycast->SetFaceCount(m_FaceX, m_FaceZ);
    m_CSRaycast->SetCameraRay(CamRay);
    m_CSRaycast->SetOuputBuffer(m_CrossBuffer);

    m_CSRaycast->Execute();

    // ������ �ڵ�
    // m_CrossBuffer->GetData(&out);
}