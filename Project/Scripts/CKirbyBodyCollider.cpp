#include "pch.h"
#include "CKirbyBodyCollider.h"
#include "CPlayerMgr.h"
#include "CKirbyFSM.h"
#include "CKirbyVacuumCollider.h"

CKirbyBodyCollider::CKirbyBodyCollider()
    : CScript(KIRBYBODYCOLLIDER)
{
}

CKirbyBodyCollider::~CKirbyBodyCollider()
{
}

void CKirbyBodyCollider::OnCollisionEnter(CCollider* _OtherCollider)
{
}

void CKirbyBodyCollider::OnCollisionStay(CCollider* _OtherCollider)
{
}

void CKirbyBodyCollider::OnCollisionExit(CCollider* _OtherCollider)
{
}

void CKirbyBodyCollider::OnTriggerEnter(CCollider* _OtherCollider)
{
    // ������ ������ ����(Ÿ���� ������ ���Ƶ��̱� ������ ����)���� �浹�� ���
    if (PLAYERFSM->IsDrawing())
    {
        PLAYERFSM->DrawingCollisionEnter(_OtherCollider->GetOwner());
    }
}

void CKirbyBodyCollider::OnTriggerStay(CCollider* _OtherCollider)
{
}

void CKirbyBodyCollider::OnTriggerExit(CCollider* _OtherCollider)
{
}

void CKirbyBodyCollider::SaveToLevelFile(FILE* _File)
{
}

void CKirbyBodyCollider::LoadFromLevelFile(FILE* _File)
{
}
