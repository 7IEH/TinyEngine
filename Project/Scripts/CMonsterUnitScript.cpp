#include "pch.h"
#include "CMonsterUnitScript.h"

CMonsterUnitScript::CMonsterUnitScript(UINT _Type)
    : CUnitScript(_Type)
    , m_pTargetObj(nullptr)
    , m_RaycastDist(100.f)
{
    AddScriptParam(SCRIPT_PARAM::FLOAT, &m_CurInfo.HP, "HP current");
    AddScriptParam(SCRIPT_PARAM::FLOAT, &m_CurInfo.MAXHP, "HP max");
    AddScriptParam(SCRIPT_PARAM::FLOAT, &m_CurInfo.Speed, "Speed");
    AddScriptParam(SCRIPT_PARAM::FLOAT, &m_CurInfo.RotationSpeed, "Rotation Speed");
    AddScriptParam(SCRIPT_PARAM::FLOAT, &m_CurInfo.JumpPower, "Jump Power");
    AddScriptParam(SCRIPT_PARAM::OBJECT, m_pTargetObj, "Target Object");
    AddScriptParam(SCRIPT_PARAM::FLOAT, &m_RaycastDist, "Raycast Distance");
}

CMonsterUnitScript::CMonsterUnitScript(const CMonsterUnitScript& _Origin)
    : CUnitScript(_Origin)
    , m_pTargetObj(_Origin.m_pTargetObj)
    , m_RaycastDist(_Origin.m_RaycastDist)
{
    AddScriptParam(SCRIPT_PARAM::FLOAT, &m_CurInfo.HP, "HP current");
    AddScriptParam(SCRIPT_PARAM::FLOAT, &m_CurInfo.MAXHP, "HP max");
    AddScriptParam(SCRIPT_PARAM::FLOAT, &m_CurInfo.Speed, "Speed");
    AddScriptParam(SCRIPT_PARAM::FLOAT, &m_CurInfo.RotationSpeed, "Rotation Speed");
    AddScriptParam(SCRIPT_PARAM::FLOAT, &m_CurInfo.JumpPower, "Jump Power");
    AddScriptParam(SCRIPT_PARAM::OBJECT, m_pTargetObj, "Target Object");
    AddScriptParam(SCRIPT_PARAM::FLOAT, &m_RaycastDist, "Raycast Distance");
}

CMonsterUnitScript::~CMonsterUnitScript()
{
}

void CMonsterUnitScript::RigidbodyMove(CGameObject* _pTargetObj)
{
    // ���� �������� �̵�
    if (nullptr == _pTargetObj)
    {
        Vec3 Dir = Transform()->GetWorldDir(DIR_TYPE::FRONT);
        Rigidbody()->SetVelocity(Dir * m_CurInfo.Speed);
    }
    // Ÿ�� �������� �̵�
    else
    {
        Vec3 ToTargetDir = m_pTargetObj->Transform()->GetWorldPos() - Transform()->GetWorldPos();
        ToTargetDir.y = 0.f; // Y�� ����
        ToTargetDir.Normalize();
        Rigidbody()->SetVelocity(ToTargetDir * m_CurInfo.Speed);
    }
}

void CMonsterUnitScript::TransformRotate()
{
    if (nullptr == m_pTargetObj)
        return;

    Vec3 Dir = Transform()->GetWorldDir(DIR_TYPE::FRONT);
    Vec3 ToTargetDir = Transform()->GetWorldPos() - m_pTargetObj->Transform()->GetWorldPos();
    ToTargetDir.y = 0.f; // Y�� ����
    ToTargetDir.Normalize();

    // ���� �̼��� ���� ��� ���� X
    if (ToTargetDir.Dot(Dir) < cosf(0.f) - 0.0000001f)
    {
        // ����ó�� Dir �� Vec3(0.f, 0.f, -1.f)�ΰ�� Up���Ͱ� ������
        Vec3 up = Vec3(0.f, 1.f, 0.f);
        if (Dir == Vec3(0.f, 0.f, -1.f))
        {
            up = Vec3(0.f, -1.f, 0.f);
        }

        Quat ToWardQuaternion = Quat::LookRotation(ToTargetDir, up);
        Quat SlerpQuat = Quat::Slerp(Transform()->GetWorldQuaternion(), ToWardQuaternion, DT * m_CurInfo.RotationSpeed);
        Transform()->SetWorldRotation(SlerpQuat);
    }
}

bool CMonsterUnitScript::IsGround()
{
    static vector<wstring> vecCollision{L"World Static", L"World Dynamic"};
    RaycastHit Hit = CPhysicsMgr::GetInst()->RayCast(Transform()->GetWorldPos(), Vec3(0.f, -1.f, 0.f), m_RaycastDist, vecCollision);
    return nullptr != Hit.pCollisionObj;
}

void CMonsterUnitScript::SaveToLevelFile(FILE* _File)
{
    CUnitScript::SaveToLevelFile(_File);
    fwrite(&m_RaycastDist, 1, sizeof(float), _File);
}

void CMonsterUnitScript::LoadFromLevelFile(FILE* _File)
{
    CUnitScript::LoadFromLevelFile(_File);
    fread(&m_RaycastDist, 1, sizeof(float), _File);
}
