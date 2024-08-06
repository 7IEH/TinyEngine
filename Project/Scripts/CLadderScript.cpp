#include "pch.h"
#include "CLadderScript.h"

#include "CPlayerMgr.h"
#include "CKirbyFSM.h"
#include "CKirbyMoveController.h"

CLadderScript::CLadderScript()
    : CScript(LADDERSCRIPT)
{
}

CLadderScript::CLadderScript(const CLadderScript& _Origin)
    : CScript(LADDERSCRIPT)
{
}

CLadderScript::~CLadderScript()
{
}

void CLadderScript::begin()
{
    Vec3 WorldPos = Transform()->GetWorldPos();
    Vec3 WorldScale = Transform()->GetWorldScale();
    WorldScale.x = 0.f;
    WorldScale.z = 0.f;

    m_UpDir = -Transform()->GetWorldDir(DIR_TYPE::FRONT).Normalize();
    m_DownDir = Transform()->GetWorldDir(DIR_TYPE::FRONT).Normalize();
    m_TopPos = WorldPos + WorldScale;
    m_BottomPos = WorldPos;
    m_Scale = WorldScale.y;

    m_PlayerCapsuleScale = 30.f;
    m_Pivot = 5.f;
}

UINT CLadderScript::SaveToLevelFile(FILE* _File)
{
    return 0;
}

UINT CLadderScript::LoadFromLevelFile(FILE* _File)
{
    return 0;
}

void CLadderScript::OnTriggerEnter(CCollider* _OtherCollider)
{
    if (_OtherCollider->GetOwner()->GetLayerIdx() == LAYER_PLAYER)
    {
        PLAYERFSM->SetLadderUpSightDir(m_UpDir);
        PLAYERFSM->SetLadderDownSightDir(m_DownDir);
        PLAYERFSM->SetLadderTop(m_TopPos);
        PLAYERFSM->SetLadderBottom(m_BottomPos);
        PLAYERFSM->SetEscapeLadder(false);

    }

}

void CLadderScript::OnTriggerStay(CCollider* _OtherCollider)
{
    // PLAYER�� �浹�� ���
    if (_OtherCollider->GetOwner()->GetLayerIdx() == LAYER_PLAYER)
    {
        Vec3 InputDir = PLAYERCTRL->GetInputWorld().Normalize();
        Vec3 PlayerPos = PLAYER->Transform()->GetWorldPos();

        // �Է��� ������ ��ٸ��� Ÿ�� �ʴ´�
        if (InputDir.Length() == 0.f)
            return;

        // �÷��̾��� ��ġ�� ��ٸ��� �ణ �Ʒ����� ���� ���
        if (PlayerPos.y < m_TopPos.y - m_PlayerCapsuleScale)
        {
            // �÷��̾��� �Է��� ����(�ö󰡴� ��)�̶�� �÷��̾ ��ٸ��� Ÿ�� �Ѵ�(���� �÷��̾��� ��ġ����).
            if (!PLAYERFSM->GetCollisionLadder() && InputDir.Dot(m_UpDir) > cosf(60.f * XM_PI / 180.f))
            {
                // �÷��̾��� ���� ���� (��ٸ��� Up����(Back)�� �ٶ󺸵��� �Ѵ�).
                PLAYERCTRL->ForceDir(ForceDirInfo{ForceDirType::STATE, m_UpDir, true});

                // �÷��̾��� ��ġ ����(��ٸ��� �߾ӿ��� Down����(Front) �ǹ���ŭ �������Ÿ�
                Vec3 NewPlayerPos = m_BottomPos + m_DownDir * m_Pivot;
                NewPlayerPos.y = PlayerPos.y;

                PLAYER->Transform()->SetWorldPos(NewPlayerPos);

                PLAYERCTRL->ClearVelocityY();

                // �÷��̾��� ���� ����
                PLAYERFSM->ChangeState(L"LADDER_WAIT");
            }

        }
        // �÷��̾ ��ٸ� ���ʿ� �ִ� ���
        else
        {
            // �÷��̾��� �Է��� ���� ��(�������� ��)�̶�� �÷��̾ ��ٸ��� Ÿ�� �Ѵ�.
            if (!PLAYERFSM->GetCollisionLadder() && InputDir.Dot(m_DownDir) > cosf(60.f * XM_PI / 180.f))
            {
                // �÷��̾��� ���� ���� (��ٸ��� Up����(Back)�� �ٶ󺸵��� �Ѵ�).
                PLAYERCTRL->ForceDir(ForceDirInfo{ForceDirType::STATE, m_UpDir, true});

                // �÷��̾��� ��ġ ����(��ٸ��� �߾ӿ��� Down����(Front) �ǹ���ŭ �������Ÿ�
                Vec3 NewPlayerPos = m_TopPos + m_DownDir * m_Pivot;
                NewPlayerPos.y -= m_PlayerCapsuleScale;
                PLAYER->Transform()->SetWorldPos(NewPlayerPos);

                PLAYERCTRL->ClearVelocityY();

                // �÷��̾��� ���� ����
                PLAYERFSM->ChangeState(L"LADDER_WAIT");
            }

            // �÷��̾��� �Է��� ����(�ö󰡴� ��)�̶�� �÷��̾ ��ٸ����� ����� �Ѵ�.
            if (PLAYERFSM->GetCollisionLadder() && InputDir.Dot(m_UpDir) > 0.f && PLAYERFSM->GetEscapeLadder() == false)
            {
                // �÷��̾��� ��ġ ����
                Vec3 NewPlayerPos = m_TopPos;
                NewPlayerPos.y -= 10.f;
                PLAYER->Transform()->SetWorldPos(NewPlayerPos);

                PLAYERCTRL->ClearVelocityY();

                // �÷��̾��� ���� ����
                PLAYERFSM->ChangeState(L"LADDER_EXIT");

                // �ٽ� �浹���� �ʵ��� Player�� ��ٸ����� Ż�� ���� ���·� ����
                PLAYERFSM->SetEscapeLadder(true);
            }
        }
    }

}

void CLadderScript::OnTriggerExit(CCollider* _OtherCollider)
{
    if (_OtherCollider->GetOwner()->GetLayerIdx() == LAYER_PLAYER)
    {
        // ��ٸ��� �ٽ� �浹�Ҽ� �ֵ��� escape�� ���ش�.
        PLAYERFSM->SetEscapeLadder(false);

        PLAYERFSM->SetCollisionLadder(false);
    }

}
