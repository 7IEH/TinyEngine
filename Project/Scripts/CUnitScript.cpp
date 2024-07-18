#include "pch.h"
#include "CUnitScript.h"

CUnitScript::CUnitScript(UINT _Type)
    : CScript(_Type)
    , m_InitInfo{}
    , m_PrevInfo{}
    , m_CurInfo{}
{
    AddScriptParam(SCRIPT_PARAM::FLOAT, &m_InitInfo.HP, "[Init] HP");
    AddScriptParam(SCRIPT_PARAM::FLOAT, &m_InitInfo.MAXHP, "[Init] HP max");
    AddScriptParam(SCRIPT_PARAM::FLOAT, &m_InitInfo.Speed, "[Init] Speed");
    AddScriptParam(SCRIPT_PARAM::FLOAT, &m_InitInfo.JumpPower, "[Init] Jump Power");

    AddScriptParam(SCRIPT_PARAM::FLOAT, &m_CurInfo.HP, "[Current] HP");
    AddScriptParam(SCRIPT_PARAM::FLOAT, &m_CurInfo.MAXHP, "[Current] HP max");
    AddScriptParam(SCRIPT_PARAM::FLOAT, &m_CurInfo.Speed, "[Current] Speed");
    AddScriptParam(SCRIPT_PARAM::FLOAT, &m_CurInfo.JumpPower, "[Current] Jump Power");
}

CUnitScript::CUnitScript(const CUnitScript& _Origin)
    : CScript(_Origin)
    , m_InitInfo{}
    , m_PrevInfo{}
    , m_CurInfo(_Origin.m_CurInfo)
{
}

CUnitScript::~CUnitScript()
{
}

void CUnitScript::begin()
{
    m_CurInfo = m_InitInfo;
}

void CUnitScript::tick()
{
    m_PrevInfo = m_CurInfo;

    // ���� �����ӿ� �޴� ���������� ������Ʈ
    DamageProc();

    if (m_CurInfo.HP < 0.f)
    {
        m_CurInfo.HP = 0.f;
    }
    
    if (m_CurInfo.HP > m_CurInfo.MAXHP)
    {
        m_CurInfo.HP = m_CurInfo.MAXHP;
    }
}

void CUnitScript::GetDamage(UnitHit _Damage)
{
    m_HitHistory.push_back(_Damage);
}

void CUnitScript::DamageProc()
{
    float CurDamage = 0.f;

    std::list<UnitHit>::iterator iter = m_HitHistory.begin();

    while (iter != m_HitHistory.end())
    {
        switch (iter->Type)
        {
        case DAMAGE_TYPE::NORMAL:
            CurDamage += iter->Damage;
            iter = m_HitHistory.erase(iter);
            break;

        case DAMAGE_TYPE::DOT: {
            if (iter->Acc >= DOT_TERM)
            {
                CurDamage += iter->Damage;
                iter->Acc -= DOT_TERM;
                iter->Duration -= DOT_TERM;
            }

            if (iter->Duration <= 0.f)
            {
                iter = m_HitHistory.erase(iter);
            }
        }
        break;

        default:
            break;
        }
    }

    m_CurInfo.HP -= CurDamage;
}

void CUnitScript::SaveToLevelFile(FILE* _File)
{
    fwrite(&m_InitInfo, sizeof(UnitInfo), 1, _File);
}

void CUnitScript::LoadFromLevelFile(FILE* _File)
{
    fread(&m_InitInfo, sizeof(UnitInfo), 1, _File);
    m_CurInfo = m_InitInfo;
}
