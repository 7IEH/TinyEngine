#include "pch.h"
#include "CElevatorScript.h"

CElevatorScript::CElevatorScript()
    : CScript(ELEVATORSCRIPT)
    , m_vDest{}
    , m_eState(ElevatorState::Stop)
    , m_fSpeed(0.f)
    , m_fOffset(0.f)
    , m_bFlag(false)
{
    AddScriptParam(SCRIPT_PARAM::VEC3, &m_vDest, "Destination");
    AddScriptParam(SCRIPT_PARAM::FLOAT, &m_fSpeed, "Speed");
}

CElevatorScript::CElevatorScript(const CElevatorScript& Origin)
    : CScript(Origin)
    , m_vDest{}
    , m_eState(ElevatorState::Stop)
    , m_fSpeed(Origin.m_fSpeed)
    , m_fOffset(Origin.m_fOffset)
    , m_bFlag(false)
{
    AddScriptParam(SCRIPT_PARAM::VEC3, &m_vDest, "Destination");
    AddScriptParam(SCRIPT_PARAM::FLOAT, &m_fSpeed, "Speed");
}

CElevatorScript::~CElevatorScript()
{
}

void CElevatorScript::tick()
{
    switch (m_eState)
    {
    case ElevatorState::Move: {
        Move();
    }
    break;
    case ElevatorState::Stop: {
    }
    break;
    case ElevatorState::End:
        break;
    default:
        break;
    }
}

void CElevatorScript::Move()
{
    Vec3 vPos = Transform()->GetLocalPos();
    if (vPos.y >= m_vDest.y)
    {
        m_eState = ElevatorState::Stop;
        Transform()->SetLocalPos(m_vDest);
    }
    else
    {
        Transform()->SetWorldPos(Transform()->GetWorldPos() + Vec3(0.f, 1.f, 0.f) * m_fSpeed * DT);
    }
}

UINT CElevatorScript::SaveToLevelFile(FILE* _File)
{
    UINT MemoryByte = 0;

    fwrite(&m_vDest, sizeof(Vec3), 1, _File);
    fwrite(&m_fSpeed, sizeof(float), 1, _File);

    MemoryByte += sizeof(Vec3);
    MemoryByte += sizeof(float);

    return MemoryByte;
}

UINT CElevatorScript::LoadFromLevelFile(FILE* _File)
{
    UINT MemoryByte = 0;

    fread(&m_vDest, sizeof(Vec3), 1, _File);
    fread(&m_fSpeed, sizeof(float), 1, _File);

    MemoryByte += sizeof(Vec3);
    MemoryByte += sizeof(float);

    return MemoryByte;
}