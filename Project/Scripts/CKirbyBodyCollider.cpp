#include "pch.h"
#include "CKirbyBodyCollider.h"
#include "CPlayerMgr.h"
#include "CKirbyFSM.h"
#include "CKirbyVacuumCollider.h"
#include "CKirbyUnitScript.h"

CKirbyBodyCollider::CKirbyBodyCollider()
    : CScript(KIRBYBODYCOLLIDER)
{
}

CKirbyBodyCollider::~CKirbyBodyCollider()
{
}
void CKirbyBodyCollider::OnTriggerEnter(CCollider* _OtherCollider)
{
    int Layeridx = _OtherCollider->GetOwner()->GetLayerIdx();

    // ������ ������ ����(Ÿ���� ������ ���Ƶ��̱� ������ ����)���� �浹�� ���
    if (PLAYERFSM->IsDrawing())
    {
        PLAYERFSM->DrawingCollisionEnter(_OtherCollider->GetOwner());
    }

    // ���� �浹�� ���
    else if (Layeridx == LAYER_MONSTER || Layeridx == LAYER_MONSTERATK)
    {
        UnitHit HitInfo = {DAMAGE_TYPE::NORMAL, 10.f, 0.f, 0.f};
        PLAYERUNIT->GetDamage(HitInfo);

        // ���������� ���
        if (PLAYERFSM->IsInvincible())
        {
            
        }
        // ���� ���°� �ƴѰ��
        else
        {
            // ���󰡴� ���� ����
            Vec3 MonsterPos = _OtherCollider->Transform()->GetWorldPos();
            Vec3 KirbyPos = Transform()->GetWorldPos();

            Vec3 KnockBackDir = KirbyPos - MonsterPos;
            KnockBackDir.y = 0.f;
            KnockBackDir.Normalize();

            PLAYERFSM->SetKnockBackDir(KnockBackDir);

            // ���� ����
            PLAYERFSM->ChangeState(L"DAMAGE");
        }
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
