#include "pch.h"
#include "CGameManagerScript.h"

#include "CLevelChangeScript.h"

CGameManagerScript* CGameManagerScript::m_Inst = nullptr;
bool CGameManagerScript::m_bPlayerPrefabLoaded = false;

CGameManagerScript::CGameManagerScript()
    : CScript(GAMEMANAGERSCRIPT)
    , m_Player(nullptr)
    , m_ExitElevator(nullptr)
{
    CGameObject* pPlayer = CLevelMgr::GetInst()->GetCurrentLevel()->FindObjectByName(L"Player");

    // ������ �÷��̾ �����ϴ� ��� ����
    // �÷��̾�� GameManager������ ����
    if (nullptr != pPlayer)
        GamePlayStatic::DestroyGameObject(pPlayer);

    // ���������� ���� �÷��̾� �ε�
    Ptr<CPrefab> pPlayerPref = nullptr;
    if (m_bPlayerPrefabLoaded)
        pPlayerPref = CAssetMgr::GetInst()->Load<CPrefab>(L"prefab\\Player.pref", L"prefab\\Player.pref");
    else
    {
        pPlayerPref = CAssetMgr::GetInst()->Load<CPrefab>(L"prefab\\Player_Backup.pref", L"prefab\\Player_Backup.pref");
        m_bPlayerPrefabLoaded = true;
    }

    m_Player = pPlayerPref->Instantiate();
    m_Player->SetName(L"Player");
}

CGameManagerScript::~CGameManagerScript()
{
    // ���� ����� ���� �������� �����ϴ� ������Ʈ ����
    Release();
}

CGameObject* CGameManagerScript::GetExitElevator()
{
    if (nullptr == m_ExitElevator)
    {
        m_ExitElevator = CLevelMgr::GetInst()->GetCurrentLevel()->FindObjectByName(L"ExitElevator");
    }

    return m_ExitElevator;
}

void CGameManagerScript::ChangeLevel(const std::string& _LevelName)
{
    CLevelChangeScript* LevelChangeScript = GetOwner()->GetScript<CLevelChangeScript>();
    if (nullptr == LevelChangeScript)
    {
        LevelChangeScript = new CLevelChangeScript;
        GetOwner()->AddComponent(LevelChangeScript);
    }

    LevelChangeScript->ChangeLevel(_LevelName);

    // Player Prefab Save
    if (nullptr != m_Player)
    {
        Ptr<CPrefab> pPlyaerPrefab = CAssetMgr::GetInst()->Load<CPrefab>(L"prefab\\Player.pref", L"prefab\\Player.pref");
        if (nullptr != pPlyaerPrefab)
            GamePlayStatic::DeleteAsset(ASSET_TYPE::PREFAB, pPlyaerPrefab.Get());

        pPlyaerPrefab = new CPrefab(m_Player->Clone());
        pPlyaerPrefab->Save(L"prefab\\Player.pref");
    }
}

void CGameManagerScript::begin()
{
    // ���ӸŴ��� ������Ʈ�� 1���̻��� ���
    vector<CGameObject*> vecGameManager = CLevelMgr::GetInst()->GetCurrentLevel()->FindObjectsOfType<CGameManagerScript>();
    if (1 != vecGameManager.size())
    {
        GamePlayStatic::DestroyGameObject(GetOwner());
        return;
    }

    // Transition Animation
    CLevel* pCurLevel = CLevelMgr::GetInst()->GetCurrentLevel();
    int PostProcessIdx = pCurLevel->FindLayerIndexByName(L"PostProcess");
    if (-1 == PostProcessIdx)
        PostProcessIdx = LAYER_MAX - 1;

    CGameObject* TransitionFilterObj =
        CAssetMgr::GetInst()->Load<CPrefab>(L"prefab\\TransitionFilter.pref", L"prefab\\TransitionFilter.pref")->Instantiate();
    TransitionFilterObj->Animator2D()->Play(L"Transition01_Reverse", false);
    GamePlayStatic::SpawnGameObject(TransitionFilterObj, PostProcessIdx);

    // BackGround Sound Play
    CSoundManagerScript::GetInset()->PlayBackGround();

    // ���� ����� �� �ʱ�ȭ
    if (L"Home Level.tLevel" == pCurLevel->GetName())
        m_bPlayerPrefabLoaded = false;
}

CGameManagerScript* CGameManagerScript::GetInset()
{
    if (nullptr == m_Inst)
    {
        CGameObject* pGMObj = CLevelMgr::GetInst()->GetCurrentLevel()->FindObjectOfType<CGameManagerScript>();

        if (nullptr != pGMObj)
        {
            CGameManagerScript* ScriptInst = pGMObj->GetScript<CGameManagerScript>();

            // GM������Ʈ�� �����ϴµ� ��ũ��Ʈ�� ���������ʴ� ���
            if (nullptr == ScriptInst)
            {
                ScriptInst = new CGameManagerScript;
            }

            m_Inst = ScriptInst;
        }
        else // GM������Ʈ�� ���������ʴ� ���
        {
            CGameObject* pNewGMObj = new CGameObject;
            pNewGMObj->SetName(L"GameManager");
            pNewGMObj->AddComponent(new CTransform);

            CGameManagerScript* ScriptInst = new CGameManagerScript;
            pNewGMObj->AddComponent(ScriptInst);
            m_Inst = ScriptInst;

            CLevel* pCurLevel = CLevelMgr::GetInst()->GetCurrentLevel();
            int GMLayerIdx = pCurLevel->FindLayerIndexByName(L"GameManager");
            if (-1 == GMLayerIdx)
                GMLayerIdx = 0;

            GamePlayStatic::SpawnGameObject(pNewGMObj, GMLayerIdx);
        }
    }

    return m_Inst;
}

void CGameManagerScript::Release()
{
    m_Inst = nullptr;

    // Player �� ������ �����ϰ����� ���� ��쿡�� GameManager���� Player �޸� ����
    CGameObject* pObj = CLevelMgr::GetInst()->GetCurrentLevel()->FindObjectByName(L"Player");
    if (nullptr == pObj)
    {
        if (nullptr != m_Player)
        {
            delete m_Player;
        }
    }

    m_Player = nullptr;
}
