#include "pch.h"
#include "CSoundManagerScript.h"

CSoundManagerScript* CSoundManagerScript::m_Inst = nullptr;

CSoundManagerScript::CSoundManagerScript()
    : CScript(SOUNDMANAGERSCRIPT)
{
}

CSoundManagerScript::~CSoundManagerScript()
{
    ClearAllSound();
    m_Inst = nullptr;
}

CSoundManagerScript* CSoundManagerScript::GetInset()
{
    if (nullptr == m_Inst)
    {
        CGameObject* pSMObj = CLevelMgr::GetInst()->GetCurrentLevel()->FindObjectOfType<CSoundManagerScript>();

        if (nullptr != pSMObj)
        {
            CSoundManagerScript* ScriptInst = pSMObj->GetScript<CSoundManagerScript>();

            // GM������Ʈ�� �����ϴµ� ��ũ��Ʈ�� ���������ʴ� ���
            if (nullptr == ScriptInst)
            {
                ScriptInst = new CSoundManagerScript;
            }

            m_Inst = ScriptInst;
        }
        else // GM������Ʈ�� ���������ʴ� ���
        {
            CGameObject* pNewSMObj = new CGameObject;
            pNewSMObj->SetName(L"SoundManager");
            pNewSMObj->AddComponent(new CTransform);

            CSoundManagerScript* ScriptInst = new CSoundManagerScript;
            pNewSMObj->AddComponent(ScriptInst);
            m_Inst = ScriptInst;

            CLevel* pCurLevel = CLevelMgr::GetInst()->GetCurrentLevel();
            int GMLayerIdx = pCurLevel->FindLayerIndexByName(L"GameManager");
            if (-1 == GMLayerIdx)
                GMLayerIdx = 0;

            GamePlayStatic::SpawnGameObject(pNewSMObj, GMLayerIdx);
        }
    }

    return m_Inst;
}

void CSoundManagerScript::PlayBackGround()
{
    CLevel* pCurLevel = CLevelMgr::GetInst()->GetCurrentLevel();
    wstring CurLevelName = pCurLevel->GetName();

    if (L"Home Level.tLevel" == CurLevelName)
    {
        GamePlayStatic::Play2DBGM(L"sound\\BackGround\\Music_HAND_MenuTheme_230223.wav", 0.2f);
    }
    else if (L"Hall of Eternity Floor 0.tLevel" == CurLevelName || L"Hall of Eternity Floor 1.tLevel" == CurLevelName)
    {
        GamePlayStatic::Play2DBGM(L"sound\\BackGround\\W1\\HAND World 1 Musique 091221.wav", 0.2f);
    }
    else if (L"W9_Boss.tLevel" == CurLevelName)
    {
        GamePlayStatic::Play2DBGM(L"sound\\BackGround\\W9\\HAND_Music_Natalite_P1_165PBM_070223.wav", 0.2f);
    }
}

void CSoundManagerScript::ClearAllSound()
{
    const map<wstring, Ptr<CAsset>>& mapSound = CAssetMgr::GetInst()->GetMapAsset(ASSET_TYPE::SOUND);

    for (const auto& iter : mapSound)
    {
        ((CSound*)iter.second.Get())->Stop();
    }
}

void CSoundManagerScript::begin()
{
    // ���� �Ŵ��� ������Ʈ�� 1���̻��� ���
    vector<CGameObject*> vecSoundManager = CLevelMgr::GetInst()->GetCurrentLevel()->FindObjectsOfType<CSoundManagerScript>();
    if (1 != vecSoundManager.size())
    {
        GamePlayStatic::DestroyGameObject(GetOwner());
        return;
    }
}
