#include "pch.h"
#include "CMeshRender.h"

#include "CMesh.h"
#include "CGraphicsShader.h"
#include "CMaterial.h"

#include "CGameObject.h"
#include "CTransform.h"

#include "CConstBuffer.h"
#include "CDevice.h"

#include "CLevelMgr.h"

#include "CAssetMgr.h"
#include "CTimeMgr.h"

CMeshRender::CMeshRender()
    : CRenderComponent(COMPONENT_TYPE::MESHRENDER)
    , m_bDrawNormalLine(false)
    , m_NormalLineScale(1.0f)
    , m_bUseTexture(false)
    , m_bUseRim(false)
    , m_RimColor(Vec3())
    , m_RimPower(1.f)

{
}

CMeshRender::~CMeshRender()
{
}

void CMeshRender::UpdateData()
{
    if (nullptr != GetMaterial())
    {
        GetMaterial()->UpdateData();
    }

    GetOwner()->Transform()->UpdateData();

    // Global Data 바인딩
    if (m_bDrawNormalLine)
        g_Global.NormalLineScale = m_NormalLineScale;

    g_Global.UseTexture = m_bUseTexture;
    g_Global.UseRim = m_bUseRim;
    g_Global.rimColor = m_RimColor;
    g_Global.rimPower = m_RimPower;
    g_Global.resolution = CDevice::GetInst()->GetRenderResolution();
    g_Global.CurTime += CTimeMgr::GetInst()->GetDeltaTime();

    // 오브젝트 이름으로 HashID 설정
    std::hash<std::wstring> hasher;
    int HashID = (int)hasher(GetOwner()->GetName());
    g_Global.pickingColor = HashIDToColor(HashID);

    static CConstBuffer* pCB = CDevice::GetInst()->GetConstBuffer(CB_TYPE::GLOBAL_DATA);
    pCB->SetData(&g_Global);
    pCB->UpdateData();
}

void CMeshRender::render()
{
    if (nullptr == GetMesh() || nullptr == GetMaterial())
        return;

    UpdateData();

    GetMesh()->render();
}
