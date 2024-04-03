#include "pch.h"
#include "CGameManagerScript.h"

#include "CLevelChangeScript.h"

CGameManagerScript* CGameManagerScript::m_Inst = nullptr;

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
    Ptr<CPrefab> pPlayerPref = CAssetMgr::GetInst()->Load<CPrefab>(L"prefab\\Player.pref", L"prefab\\Player.pref");
    m_Player = pPlayerPref->Instantiate();
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
}

void CGameManagerScript::begin()
{
    // ���ӸŴ��� ������Ʈ�� 1���̻��� ���
    vector<CGameObject*> pGMObjs = CLevelMgr::GetInst()->GetCurrentLevel()->FindObjectsOfType<CGameManagerScript>();
    if (1 != pGMObjs.size())
    {
        GamePlayStatic::DestroyGameObject(GetOwner());
        return;
    }
}

void CGameManagerScript::tick()
{
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

            int GMLayerIdx = 0;
            CLevel* pCurLevel = CLevelMgr::GetInst()->GetCurrentLevel();

            for (int i = 0; i < LAYER_MAX; i++)
            {
                if (L"GameManager" == pCurLevel->GetLayer(i)->GetName())
                {
                    GMLayerIdx = i;
                    break;
                }
            }

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
