#include "pch.h"
#include "CLevelFlowMgr.h"

#include <Engine/CLevelSaveLoad.h>
#include <Engine/CLevelMgr.h>
#include <Engine/CRenderMgr.h>

#include "CPlayerMgr.h"
#include "CCameraController.h"
#include "CFadeOutScript.h"

CLevelFlowMgr::CLevelFlowMgr(UINT _Type)
    : CScript(_Type)
    , m_CurLevelPath{}
    , m_NextLevelPath{}
    , m_DimensionFadeEffect(nullptr)
    , m_FadeOutObj(nullptr)
    , m_ToneMappingMtrl(nullptr)
{
    AddScriptParam(SCRIPT_PARAM::STRING, &m_NextLevelPath, "Next Level Name");
}

CLevelFlowMgr::CLevelFlowMgr(const CLevelFlowMgr& _Origin)
    : CScript(_Origin)
    , m_CurLevelPath{}
    , m_NextLevelPath(_Origin.m_NextLevelPath)
    , m_DimensionFadeEffect(nullptr)
    , m_FadeOutObj(nullptr)
    , m_ToneMappingMtrl(nullptr)
{
    AddScriptParam(SCRIPT_PARAM::STRING, &m_NextLevelPath, "Next Level Name");
}

CLevelFlowMgr::~CLevelFlowMgr()
{
}

void CLevelFlowMgr::begin()
{
    m_CurLevelPath = CLevelMgr::GetInst()->GetCurrentLevel()->GetName();

    // Effect �����
    m_DimensionFadeEffect =
        CAssetMgr::GetInst()->Load<CPrefab>(L"prefab\\DimensionFadeEffect.pref", L"prefab\\DimensionFadeEffect.pref")->Instantiate();

    m_DimensionFadeEffect->SetActive(false);
    GamePlayStatic::AddChildObject(GetOwner(), m_DimensionFadeEffect);

    Ptr<CPrefab> pFadeOutPref = CAssetMgr::GetInst()->Load<CPrefab>(L"prefab\\FadeOut.pref", L"prefab\\FadeOut.pref");
    if (nullptr != pFadeOutPref)
    {
        m_FadeOutObj = pFadeOutPref->Instantiate();
        GamePlayStatic::AddChildObject(GetOwner(), m_FadeOutObj);
    }

    m_ToneMappingMtrl = CAssetMgr::GetInst()->FindAsset<CMaterial>(L"ToneMappingMtrl");

    // Level Start
    LevelStart();
}

void CLevelFlowMgr::tick()
{
    // tick���� �־���� �ϴ� Param setting
    MtrlParamUpdate();
}

void CLevelFlowMgr::LevelStart()
{
    // Post Process Enable
    CRenderMgr::GetInst()->SetEnableDOF(true);
    CRenderMgr::GetInst()->SetEnableDepthMasking(true);
    g_Global.g_EnableSSAO = true;

    // Stating Point ��������
    CGameObject* StartingPoint = CLevelMgr::GetInst()->GetCurrentLevel()->FindObjectByName(L"Starting Point");

    Vec3 StartingPos = Vec3(0.f, 0.f, 0.f);
    Vec3 StartingRot = Vec3(0.f, 0.f, 0.f);

    if (StartingPoint != nullptr)
    {
        StartingPos = StartingPoint->Transform()->GetWorldPos();
        StartingRot = StartingPoint->Transform()->GetWorldRotation();
        GamePlayStatic::DestroyGameObject(StartingPoint);
    }

    // MainPlayer
    CGameObject* MainPlayer = nullptr;
    MainPlayer = CLevelMgr::GetInst()->GetCurrentLevel()->FindObjectByName(L"Main Player", LAYER_PLAYER);

    // Level�� Main Player�� ���ٸ� ���� ����
    if (MainPlayer == nullptr)
    {
        // ���� �������� ����� �÷��̾� �������� �ִٸ�
        Ptr<CPrefab> PlayerPrefab = CAssetMgr::GetInst()->Load<CPrefab>(L"prefab\\Main Player.pref", L"prefab\\Main Player.pref");
        if (PlayerPrefab.Get() != nullptr)
        {
            MainPlayer = PlayerPrefab->Instantiate();
        }
        else
        {
            PlayerPrefab = CAssetMgr::GetInst()->Load<CPrefab>(L"prefab\\Kirby.pref", L"prefab\\Kirby.pref");

            // Kirby Prefab�� ���ٸ� assert ���� ���� �Ұ���
            assert(PlayerPrefab.Get());

            MainPlayer = PlayerPrefab->Instantiate();
        }

        GamePlayStatic::SpawnGameObject(MainPlayer, LAYER_PLAYER);
    }

    // Player Spawn
    MainPlayer->Transform()->SetWorldPos(StartingPos);
    MainPlayer->Transform()->SetWorldRotation(StartingRot);

    // Player ����� �ʿ��� Manager�鿡�� Player�� ����Ѵ�.
    CPlayerMgr::SetPlayer(MainPlayer);
    CGameObject* MainCam = CLevelMgr::GetInst()->GetCurrentLevel()->FindObjectByName(L"Main Camera");
    if (MainCam)
    {
        CCameraController* CamCtrl = MainCam->GetScript<CCameraController>();
        if (CamCtrl != nullptr)
        {
            CamCtrl->SetPlayer(MainPlayer);
        }
    }

    // UI (Fade In)
    if (nullptr != m_FadeOutObj)
    {
        CFadeOutScript* pFadeOutScript = m_FadeOutObj->GetScript<CFadeOutScript>();
        pFadeOutScript->SetBackGroundColor(Vec4(255.f, 0.f, 255.f, 255.f));
        pFadeOutScript->SetReverse(true);
        pFadeOutScript->SetDuration(0.25f);
        pFadeOutScript->SetRotateSpeed(1.25f);
    }

    // @TODO BGM ���
}

void CLevelFlowMgr::LevelEnd()
{
    // UI (Fade Out)
    if (nullptr != m_FadeOutObj)
    {
        CFadeOutScript* pFadeOutScript = m_FadeOutObj->GetScript<CFadeOutScript>();
        pFadeOutScript->SetBackGroundColor(Vec4(0.f, 255.f, 0.f, 255.f));
        pFadeOutScript->SetReverse(false);
        pFadeOutScript->SetDuration(1.f);
        pFadeOutScript->SetRotateSpeed(1.25f);
    }

    // BGM ����
}

void CLevelFlowMgr::LevelExit()
{

    // Level Change
    GamePlayStatic::ChangeLevelAsync(ToWstring(m_NextLevelPath), LEVEL_STATE::PLAY);
}

void CLevelFlowMgr::LevelRestart()
{
    // UI (Fade Out)
    if (nullptr != m_FadeOutObj)
    {
        CFadeOutScript* pFadeOutScript = m_FadeOutObj->GetScript<CFadeOutScript>();
        pFadeOutScript->SetBackGroundColor(Vec4(0.f, 255.f, 0.f, 255.f));
        pFadeOutScript->SetReverse(false);
        pFadeOutScript->SetDuration(1.f);
        pFadeOutScript->SetRotateSpeed(1.25f);
    }

    // BGM ����

    // Level Restart
    GamePlayStatic::ChangeLevelAsync(m_CurLevelPath, LEVEL_STATE::PLAY);
}

void CLevelFlowMgr::MtrlParamUpdate()
{
    // // DOF Focus Player ��ġ ����
    if (nullptr != PLAYER)
    {
        static Ptr<CMaterial> pDOFMtrl = CAssetMgr::GetInst()->FindAsset<CMaterial>(L"DOFMtrl");
        Vec3 NDCPos = PositionToNDC(PLAYER->Transform()->GetWorldPos());
        Vec2 UVPos = NDCToUV(NDCPos);
        pDOFMtrl->SetScalarParam(VEC2_0, UVPos); // Focus UV
    }
}

void CLevelFlowMgr::OnDimensionFade()
{
    if (nullptr != m_DimensionFadeEffect)
    {
        m_DimensionFadeEffect->SetActive(true);
    }
}

void CLevelFlowMgr::OffDimensionFade()
{
    if (nullptr != m_DimensionFadeEffect)
    {
        m_DimensionFadeEffect->SetActive(false);
    }
}

void CLevelFlowMgr::SetToneMappingParam(bool _bBloomEnable, bool _bBlendMode, float _BloomStrength, float _Threshold, float _FilterRadius,
                                        float _Exposure, float _Gamma)
{
    CRenderMgr::GetInst()->SetEnableBloom(_bBloomEnable);
    m_ToneMappingMtrl->SetScalarParam(INT_1, (int)_bBlendMode);
    m_ToneMappingMtrl->SetScalarParam(FLOAT_0, _Exposure);
    m_ToneMappingMtrl->SetScalarParam(FLOAT_1, _Gamma);
    m_ToneMappingMtrl->SetScalarParam(FLOAT_2, _BloomStrength);
    m_ToneMappingMtrl->SetScalarParam(VEC2_0, Vec2(_FilterRadius, _Threshold));
}

UINT CLevelFlowMgr::SaveToLevelFile(FILE* _File)
{
    UINT MemoryByte = 0;

    MemoryByte += SaveWStringToFile(ToWstring(m_NextLevelPath), _File);

    return MemoryByte;
}

UINT CLevelFlowMgr::LoadFromLevelFile(FILE* _File)
{
    UINT MemoryByte = 0;

    wstring NextLvName;
    MemoryByte += LoadWStringFromFile(NextLvName, _File);

    m_NextLevelPath = ToString(NextLvName);

    return MemoryByte;
}
