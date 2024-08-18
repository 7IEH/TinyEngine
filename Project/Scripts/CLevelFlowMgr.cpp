#include "pch.h"
#include "CLevelFlowMgr.h"

#include <Engine/CLevelSaveLoad.h>
#include <Engine/CLevelMgr.h>
#include <Engine/CRenderMgr.h>

#include "CPlayerMgr.h"
#include "CCameraController.h"
#include "CFadeEffectScript.h"

#include "CUIBossHPScript.h"
#include "CUIHPScript.h"

#include "CEnterUIScript.h"
#include "CUIFlowScript.h"

#include "CUIHPScript.h"

CLevelFlowMgr::CLevelFlowMgr(UINT _Type)
    : CScript(_Type)
    , m_CurLevelPath{}
    , m_NextLevelPath{}
    , m_DimensionFadeEffect(nullptr)
    , m_FadeEffectScript(nullptr)
    , m_UIFlowScript(nullptr)
    , m_ToneMappingMtrl(nullptr)
    , m_pLoadingUI(nullptr)
    , m_pPlayerHP(nullptr)
    , m_pBossHP(nullptr)
    , m_pEnterUIScript(nullptr)
    , m_pClearUI(nullptr)
    , m_bStartLevel(false)
    , m_bStartLevelDurationValue(true)
    , m_bEnterLevel(true)
    , m_fFadeInAccTime(0.f)
    , m_fFadeInWaitTime(2.f)
{
    AddScriptParam(SCRIPT_PARAM::STRING, &m_NextLevelPath, "Next Level Name");
}

CLevelFlowMgr::CLevelFlowMgr(const CLevelFlowMgr& _Origin)
    : CScript(_Origin)
    , m_CurLevelPath{}
    , m_NextLevelPath(_Origin.m_NextLevelPath)
    , m_DimensionFadeEffect(nullptr)
    , m_FadeEffectScript(nullptr)
    , m_UIFlowScript(nullptr)
    , m_ToneMappingMtrl(nullptr)
    , m_pLoadingUI(nullptr)
    , m_pEnterUIScript(nullptr)
    , m_pPlayerHP(nullptr)
    , m_pClearUI(nullptr)
    , m_pBossHP(nullptr)
    , m_bStartLevel(false)
    , m_bStartLevelDurationValue(true)
    , m_bEnterLevel(true)
    , m_fFadeInAccTime(0.f)
    , m_fFadeInWaitTime(2.f)
{
    AddScriptParam(SCRIPT_PARAM::STRING, &m_NextLevelPath, "Next Level Name");
}

CLevelFlowMgr::~CLevelFlowMgr()
{
}

void CLevelFlowMgr::begin()
{
    m_bIsChangedLevel = false;

    m_CurLevelPath = CLevelMgr::GetInst()->GetCurrentLevel()->GetName();

    // Effect �����
    m_DimensionFadeEffect =
        CAssetMgr::GetInst()->Load<CPrefab>(L"prefab\\DimensionFadeEffect.pref", L"prefab\\DimensionFadeEffect.pref")->Instantiate();

    m_DimensionFadeEffect->SetActive(false);
    GamePlayStatic::AddChildObject(GetOwner(), m_DimensionFadeEffect);

    Ptr<CPrefab> pFadeEffectPref = CAssetMgr::GetInst()->Load<CPrefab>(L"prefab\\FadeEffect.pref", L"prefab\\FadeEffect.pref");
    if (nullptr != pFadeEffectPref)
    {
        CGameObject* pFadeEffectObj = pFadeEffectPref->Instantiate();
        m_FadeEffectScript = pFadeEffectObj->GetScript<CFadeEffectScript>();
        GamePlayStatic::AddChildObject(GetOwner(), pFadeEffectObj);
    }

    m_ToneMappingMtrl = CAssetMgr::GetInst()->FindAsset<CMaterial>(L"ToneMappingMtrl");

    // UI �ʱ�ȭ
    {
        // Loading UI �ʱ�ȭ
        m_pLoadingUI = CLevelMgr::GetInst()->GetCurrentLevel()->FindObjectByName(L"UI_Loading");
        if (nullptr != m_pLoadingUI)
            m_pLoadingUI->SetActive(false);

        // Player �ʱ�ȭ
        m_pPlayerHP = CLevelMgr::GetInst()->GetCurrentLevel()->FindObjectByName(L"UI_PlayerHP");
        if (nullptr != m_pPlayerHP)
            m_pPlayerHP->SetActive(false);

        // Boss �ʱ�ȭ
        m_pBossHP = CLevelMgr::GetInst()->GetCurrentLevel()->FindObjectByName(L"UI_BossHP");
        if (nullptr != m_pBossHP)
            m_pBossHP->SetActive(false);

        // Start Level Duration Value
        m_bStartLevelDurationValue = true;

        // Enter UI Script �ʱ�ȭ
        CGameObject* pObj = CLevelMgr::GetInst()->GetCurrentLevel()->FindObjectByName(L"UI_EnterScene");
        if (nullptr != pObj)
        {
            m_pEnterUIScript = pObj->GetScript<CEnterUIScript>();
        }

        m_pClearUI = CLevelMgr::GetInst()->GetCurrentLevel()->FindObjectByName(L"UI_LevelClear");
        if (nullptr != m_pClearUI)
            TrunOffStageClearUI();
    }
}

void CLevelFlowMgr::tick()
{
    if (KEY_TAP(KEY::N))
    {
        LevelEnd();
    }

    // ��ŸƮ ���� UI ����!
    if (m_bStartLevel)
    {
        if (nullptr != m_pEnterUIScript)
        {
            if (!m_pEnterUIScript->IsFinish())
            {
                SetReverseFadeEffect();
            }
            else
            {
                m_bStartLevel = false;
            }
        }
    }

    // Player HP UI On
    if (m_bEnterLevel)
    {
        m_fFadeInAccTime += DT;

        // UI�� ������
        if (m_fFadeInAccTime > m_fFadeInWaitTime)
        {
            m_bEnterLevel = false;
            if (nullptr != m_pPlayerHP)
                m_pPlayerHP->SetActive(true);
        }
    }

    if (m_bFadeEffect)
    {
        m_FadeEffectAcc += DT;

        // UI�� ������
        if (m_FadeEffectAcc > m_FadeEffectDuration)
        {
            // Level ��ȯ
            m_bFadeEffect = false;
            LevelExit();
        }
    }

    // tick���� �־���� �ϴ� Param setting
    MtrlParamUpdate();
}

void CLevelFlowMgr::LevelStart()
{
    // Post Process Enable
    CRenderMgr::GetInst()->SetEnableDOF(true);
    CRenderMgr::GetInst()->SetEnableDepthMasking(true);
    // g_Global.g_EnableSSAO = true; // Option

    // FadeEffect Timer �ʱ�ȭ
    m_bFadeEffect = false;
    m_FadeEffectAcc = 0.f;
    m_FadeEffectDuration = 2.5f;

    // FadeIn �ʱ�ȭ
    m_bEnterLevel = true;
    m_fFadeInAccTime = 0.f;
    m_fFadeInWaitTime = 2.f;

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
    MainPlayer = CLevelMgr::GetInst()->GetCurrentLevel()->FindObjectByName(L"Main Player");

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

    // SetPlayer
    if (nullptr != m_pPlayerHP)
        m_pPlayerHP->GetScript<CUIHPScript>()->SetPlayer();

    // UI (Fade In)
    if (m_bStartLevel)
    {
        SetFadeEffect(Vec3(255.f, 0.f, 255.f), true, 0.25f, 1.25f, false);
        if (nullptr != m_pEnterUIScript)
        {
            m_pEnterUIScript->ChangeState(EnterUIState::Enter);
        }
    }
    else
    {
        SetFadeEffect(Vec3(255.f, 0.f, 255.f), true, 0.25f, 1.25f, false);
    }

    // @TODO BGM ���
}

void CLevelFlowMgr::LevelEnd()
{
    // �̹� ���� ��ȯ���� ��� ó������ �ʴ´�.
    if (m_bIsChangedLevel == true)
        return;

    // UI (Fade Out)
    SetFadeEffect(Vec3(255.f, 0.f, 255.f), false, 1.f, 1.25f, false);

    // Player UI
    TurnOffPlayerHP();

    m_bIsChangedLevel = true;
    m_bFadeEffect = true;
    m_FadeEffectAcc = 0.f;

    // FadeIn �ʱ�ȭ

    // @TODO BGM ����
}

void CLevelFlowMgr::LevelExit()
{
    // Loding UI ����
    if (nullptr != m_pLoadingUI)
        m_pLoadingUI->SetActive(true);

    //// Kirby ������ ����
    Ptr<CPrefab> MainPlayerPref = new CPrefab(PLAYER->Clone());
    MainPlayerPref->Save(L"prefab\\Main Player.pref");

    m_pPlayerHP->SetActive(false);

    // Level Change
    GamePlayStatic::ChangeLevelAsync(ToWstring(m_NextLevelPath), LEVEL_STATE::PLAY);
}

void CLevelFlowMgr::LevelRestart()
{
    // �̹� ���� ��ȯ���� ��� ó������ �ʴ´�.
    if (m_bIsChangedLevel == true)
        return;

    // UI (Fade Out)
    SetFadeEffect(Vec3(255.f, 0.f, 255.f), false, 1.f, 1.25f, false);

    m_bIsChangedLevel = true;
    m_bFadeEffect = true;
    m_FadeEffectAcc = 0.f;

    // Fade In UI �ʱ�ȭ
    m_bEnterLevel = true;
    m_fFadeInAccTime = 0.f;
    m_fFadeInWaitTime = 2.f;

    // ���� ������ �ٽ� �����ϱ� ���� NextLevelPath �� ���緹���� Path�� �ٲ��ش�.
    m_NextLevelPath = ToString(m_CurLevelPath);
    // @TODO BGM ����
}

void CLevelFlowMgr::MtrlParamUpdate()
{
    // DOF Focus Player ��ġ ����
    if (nullptr != PLAYER)
    {
        static Ptr<CMaterial> pDOFMtrl = CAssetMgr::GetInst()->Load<CMaterial>(L"DOFMtrl");
        Vec3 NDCPos = PositionToNDC(PLAYER->Transform()->GetWorldPos());

        // PLAYER�� ȭ�� ���ΰ��
        if (NDCPos.x < -1.f || NDCPos.y < -1.f || NDCPos.z < -1.f || NDCPos.x > 1.f || NDCPos.y > 1.f || NDCPos.z > 1.f)
        {
            pDOFMtrl->SetScalarParam(VEC2_0, Vec2(-100.f, -100.f));
        }
        else
        {
            pDOFMtrl->SetScalarParam(VEC2_0, NDCToUV(NDCPos)); // Focus UV
        }
    }
}

void CLevelFlowMgr::TurnOnBossHP()
{
    if (nullptr != m_pBossHP)
    {
        CUIBossHPScript* pScript = m_pBossHP->GetScript<CUIBossHPScript>();
        if (nullptr != pScript)
            pScript->ChangeState(HPState::Enter);
    }
}

void CLevelFlowMgr::TurnOffBossHP()
{
    if (nullptr != m_pBossHP)
    {
        m_pBossHP->SetActive(false);
    }
}

void CLevelFlowMgr::TurnOffPlayerHP()
{
    if (nullptr != m_pPlayerHP)
    {
        m_pPlayerHP->SetActive(false);
    }
}

void CLevelFlowMgr::TrunOffStageClearUI()
{
    if (nullptr != m_pClearUI)
    {
        m_pClearUI->SetActive(false);
    }
}

void CLevelFlowMgr::TurnOnStageclearUI()
{
    if (nullptr != m_pClearUI)
    {
        m_pClearUI->SetActive(true);
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

void CLevelFlowMgr::SetFadeEffectColor(Vec3 _Color)
{
    if (!m_FadeEffectScript)
        return;

    Vec4 Color = Vec4(_Color.x, _Color.y, _Color.z, 255.f) / 255.f;
    m_FadeEffectScript->SetBackGroundColor(Color);
}

void CLevelFlowMgr::SetFadeEffect(Vec3 _Color, bool _bReverse, float _Duration, float _Speed, bool _CenterMode)
{
    if (!m_FadeEffectScript)
        return;

    Vec4 Color = Vec4(_Color.x, _Color.y, _Color.z, 255.f) / 255.f;
    m_FadeEffectScript->SetBackGroundColor(Color);
    m_FadeEffectScript->SetReverse(_bReverse);
    m_FadeEffectScript->SetDuration(_Duration);
    m_FadeEffectScript->SetRotateSpeed(_Speed);
    m_FadeEffectScript->SetCenterMode(_CenterMode);
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

void CLevelFlowMgr::SetReverseFadeEffect(bool _bReverse)
{
    if (!m_FadeEffectScript)
        return;

    m_FadeEffectScript->SetReverse(_bReverse);
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
