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
    , m_bDrawLandScape(false)
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
}

void CLandScape::finaltick()
{
    if (m_bDrawLandScape && KEY_PRESSED(KEY::LBTN))
    {
        Raycasting();

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
}

void CLandScape::render()
{
    UpdateData();

    GetMesh()->render();
}

void CLandScape::render(Ptr<CMaterial> _mtrl)
{
    Transform()->UpdateData();
    _mtrl->UpdateData();

    GetMesh()->render();
}

void CLandScape::SaveToLevelFile(FILE* _File)
{
    fwrite(&m_bDrawLandScape, 1, sizeof(bool), _File);
    fwrite(&m_BrushStrength, 1, sizeof(float), _File);
    fwrite(&m_BrushScale, 1, sizeof(Vec2), _File);
}

void CLandScape::LoadFromLevelFile(FILE* _File)
{
    fread(&m_bDrawLandScape, 1, sizeof(bool), _File);
    fread(&m_BrushStrength, 1, sizeof(float), _File);
    fread(&m_BrushScale, 1, sizeof(Vec2), _File);
}

void CLandScape::UpdateData()
{
    Transform()->UpdateData();

    GetMaterial()->SetScalarParam(SCALAR_PARAM::INT_0, m_FaceX);
    GetMaterial()->SetScalarParam(SCALAR_PARAM::INT_1, m_FaceZ);
    GetMaterial()->SetTexParam(TEX_PARAM::TEX_3, m_HeightMapTex);

    GetMaterial()->UpdateData();
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