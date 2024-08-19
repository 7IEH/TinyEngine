#include "pch.h"
#include "CLevelFlowMgr.h"

#include <Engine/CLevelSaveLoad.h>
#include <Engine/CLevelMgr.h>
#include <Engine/CRenderMgr.h>

#include "CPlayerMgr.h"
#include "CCameraController.h"
#include "CFadeEffectScript.h"

#include "CUIFlowScript.h"

CLevelFlowMgr::CLevelFlowMgr(UINT _Type)
    : CScript(_Type)
    , m_CurLevelPath{}
    , m_NextLevelPath{}
    , m_DimensionFadeEffect(nullptr)
    , m_FadeEffectScript(nullptr)
    , m_UIFlowScript(nullptr)
    , m_ToneMappingMtrl(nullptr)
    , m_RadialBlurEffect(nullptr)
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
    , m_RadialBlurEffect(nullptr)
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

    // ==================
    //    Effect Init
    // ==================
    m_DimensionFadeEffect =
        CAssetMgr::GetInst()->Load<CPrefab>(L"prefab\\DimensionFadeEffect.pref", L"prefab\\DimensionFadeEffect.pref")->Instantiate();

    m_DimensionFadeEffect->SetActive(false);
    GamePlayStatic::AddChildObject(GetOwner(), m_DimensionFadeEffect);

    m_RadialBlurEffect = CAssetMgr::GetInst()->Load<CPrefab>(L"prefab\\RadialBlurEffect.pref")->Instantiate();

    m_RadialBlurEffect->SetActive(false);
    GamePlayStatic::AddChildObject(GetOwner(), m_RadialBlurEffect);

    Ptr<CPrefab> pFadeEffectPref = CAssetMgr::GetInst()->Load<CPrefab>(L"prefab\\FadeEffect.pref", L"prefab\\FadeEffect.pref");
    if (nullptr != pFadeEffectPref)
    {
        CGameObject* pFadeEffectObj = pFadeEffectPref->Instantiate();
        m_FadeEffectScript = pFadeEffectObj->GetScript<CFadeEffectScript>();
        GamePlayStatic::AddChildObject(GetOwner(), pFadeEffectObj);
    }

    m_ToneMappingMtrl = CAssetMgr::GetInst()->FindAsset<CMaterial>(L"ToneMappingMtrl");

    // Level Start
    LevelStart();
}

void CLevelFlowMgr::tick()
{
    if (KEY_TAP(KEY::N))
    {
        LevelEnd();
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

    if (m_bRadialBlurEffect)
    {
        m_RadialBlurAcc += DT_ENGINE;

        if (m_RadialBlurAcc > m_RadialBlurDuration)
        {
            OffRadialBlurEffect();
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

    // Radial Blur Effect Timer �ʱ�ȭ
    m_bRadialBlurEffect = false;
    m_RadialBlurAcc = 0.f;
    m_RadialBlurDuration = 0.f;

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

    // UI (Fade In)
    SetFadeEffect(Vec3(255.f, 0.f, 255.f), true, 1.f, 1.25f, false);

    // @TODO BGM ���
}

void CLevelFlowMgr::LevelEnd()
{
    // �̹� ���� ��ȯ���� ��� ó������ �ʴ´�.
    if (m_bIsChangedLevel == true)
        return;

    // UI (Fade Out)
    SetFadeEffect(Vec3(255.f, 0.f, 255.f), false, 1.f, 1.25f, false);

    m_bIsChangedLevel = true;
    m_bFadeEffect = true;
    m_FadeEffectAcc = 0.f;

    // @TODO BGM ����
}

void CLevelFlowMgr::LevelExit()
{
    // Kirby ������ ����
    Ptr<CPrefab> MainPlayerPref = new CPrefab(PLAYER->Clone());
    MainPlayerPref->Save(L"prefab\\Main Player.pref");

    // Loding UI ����

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

    if (m_RadialBlurEffect->IsActive())
    {
        static Ptr<CMaterial> pRadialBlurMtrl = CAssetMgr::GetInst()->Load<CMaterial>(L"RadialBlurMtrl");
        Vec3 NDCPos = PositionToNDC(PLAYER->Transform()->GetWorldPos());
        Vec2 UV = NDCToUV(NDCPos);

        pRadialBlurMtrl->SetScalarParam(VEC2_0, UV);
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

void CLevelFlowMgr::OnRadialBlurEffect(float _Duration, float _Radius, float _BlurPower)
{
    if (nullptr != m_RadialBlurEffect)
    {
        m_RadialBlurAcc = 0.f;
        m_RadialBlurDuration = _Duration;
        m_bRadialBlurEffect = true;

        static Ptr<CMaterial> pRadialBlurMtrl = CAssetMgr::GetInst()->Load<CMaterial>(L"RadialBlurMtrl");
        pRadialBlurMtrl->SetScalarParam(FLOAT_0, _Radius);
        pRadialBlurMtrl->SetScalarParam(FLOAT_1, _BlurPower);

        m_RadialBlurEffect->SetActive(true);
    }
}

void CLevelFlowMgr::OffRadialBlurEffect()
{
    if (nullptr != m_RadialBlurEffect)
    {
        m_RadialBlurAcc = 0.f;
        m_RadialBlurDuration = 0.f;
        m_bRadialBlurEffect = false;

        m_RadialBlurEffect->SetActive(false);
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
