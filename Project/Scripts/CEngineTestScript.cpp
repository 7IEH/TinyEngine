#include "pch.h"
#include "CEngineTestScript.h"
#include <Engine\\CTransform.h>

CEngineTestScript::CEngineTestScript()
    : CScript(ENGINETESTSCRIPT)
{
}

CEngineTestScript::~CEngineTestScript()
{
}

void CEngineTestScript::begin()
{
}

void CEngineTestScript::tick()
{
    GamePlayStatic::DrawDebugLine(GetOwner()->Transform()->GetWorldPos(), GetOwner()->Transform()->GetWorldDir(DIR_TYPE::FRONT), 100.f,
                                  Vec3(1.f, 1.f, 0.f), true);

    //// =============================
    //// 1. Ÿ�� �������� ȸ�� ���� ����
    //// =============================
    //{
    //    CGameObject* pTarget = CLevelMgr::GetInst()->GetCurrentLevel()->FindObjectByName(L"Target");
    //    if (pTarget)
    //    {
    //        Vec3 Dir = Transform()->GetWorldPos() - pTarget->Transform()->GetWorldPos();
    //        Dir.Normalize();
    //        Quat Quaternion = Quat::LookRotation(Dir, Vec3(0.f, 1.f, 0.f));
    //        Transform()->SetWorldRotation(Quaternion);
    //    }
    //}

    //// =============================
    //// 2. Ÿ�ٰ��� ����
    //// =============================
    //{
    //    CGameObject* pTarget = CLevelMgr::GetInst()->GetCurrentLevel()->FindObjectByName(L"Target");
    //    if (pTarget)
    //    {
    //        Quat Quaternion = Transform()->GetWorldQuaternion();
    //        Quat TargetQuaternion = pTarget->Transform()->GetWorldQuaternion();

    //        float Angle = Quat::Angle(Quaternion, TargetQuaternion);
    //        Angle = XMConvertToDegrees(Angle);

    //        LOG(Log, "%f", Angle);
    //    }
    //}

    //// =============================
    //// 3. Ư�� �� ���� ȸ��
    //// =============================
    //{
    //    CGameObject* pTarget = CLevelMgr::GetInst()->GetCurrentLevel()->FindObjectByName(L"Target");
    //    if (pTarget)
    //    {
    //        static float Angle = 0.f;
    //        Angle += DT;
    //        Quat Quaternion = Quat::CreateFromAxisAngle(Vec3(0.f, 1.f, 0.f), Angle);
    //        Transform()->SetWorldRotation(Quaternion);
    //    }
    //}

    //// =============================
    //// 4. Ư�� ������ �����ϴ� ����
    //// =============================
    //{
    //    if (KEY_TAP(KEY::R))
    //    {
    //        CGameObject* pTarget = CLevelMgr::GetInst()->GetCurrentLevel()->FindObjectByName(L"Target");
    //        if (pTarget)
    //        {
    //            Quat Quaternion = Quat::FromToRotation(Vec3(0.f, 1.f, 0.f), Transform()->GetWorldDir(DIR_TYPE::FRONT));
    //            Transform()->SetWorldRotation(Quaternion);
    //        }
    //    }
    //}

    // =============================
    // 5. Slerp ����
    // =============================
    {
        CGameObject* pTarget = CLevelMgr::GetInst()->GetCurrentLevel()->FindObjectByName(L"Target");
        if (pTarget)
        {
            Quat Quaternion = Transform()->GetWorldQuaternion();

            Vec3 Dir = Transform()->GetWorldPos() - pTarget->Transform()->GetWorldPos();
            Dir.Normalize();
            Quat ToTargetQuaternion = Quat::LookRotation(Dir, Vec3(0.f, 1.f, 0.f));

            float RotSpeed = 2.f;

            Quat SlerpQuat = Quat::Slerp(Quaternion, ToTargetQuaternion, DT * RotSpeed);

            // ���� �̱� ������ ������ ����������� ������ ��������������
            // ������� ������� ��� ����ó�� �ʿ�
            Transform()->SetWorldRotation(SlerpQuat);
        }
    }

    //// =============================
    //// 6. RotateToWard ����
    //// Ÿ�� ������Ʈ�� ȸ�� ���� ���������� �����ϴ� ����
    //// =============================
    //{
    //    CGameObject* pTarget = CLevelMgr::GetInst()->GetCurrentLevel()->FindObjectByName(L"Target");
    //    if (pTarget)
    //    {
    //        Quat Quaternion = Transform()->GetWorldQuaternion();
    //        Quat TargetQuaternion = pTarget->Transform()->GetWorldQuaternion();

    //        float RotSpeed = 1.f;

    //        Quaternion.RotateTowards(TargetQuaternion, DT * RotSpeed);

    //        Transform()->SetWorldRotation(Quaternion);
    //    }
    //}
}

void CEngineTestScript::OnCollisionEnter(CCollider* _OtherCollider)
{
}

void CEngineTestScript::OnCollisionStay(CCollider* _OtherCollider)
{
}

void CEngineTestScript::OnCollisionExit(CCollider* _OtherCollider)
{
}

void CEngineTestScript::OnTriggerEnter(CCollider* _OtherCollider)
{
}

void CEngineTestScript::OnTriggerStay(CCollider* _OtherCollider)
{
}

void CEngineTestScript::OnTriggerExit(CCollider* _OtherCollider)
{
}

void CEngineTestScript::SaveToLevelFile(FILE* _File)
{
}

void CEngineTestScript::LoadFromLevelFile(FILE* _File)
{
}
