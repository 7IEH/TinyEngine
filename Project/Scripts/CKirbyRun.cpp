#include "pch.h"
#include "CKirbyRun.h"

#include "CKirbyFSM.h"
#include "CKirbyAbility.h"

CKirbyRun::CKirbyRun()
{
}

CKirbyRun::~CKirbyRun()
{
}

void CKirbyRun::tick()
{
    CKirbyFSM* KirbyFSM = nullptr;
    //CKirbyFSM* KirbyFSM = dynamic_cast<CKirbyFSM*>(GetOwnerFSM());
    //PlayerManager ������ �� -> manager�� player�� ��� �ְ�, �װ� �������°� ������ (level �Ǵ� ���� ���۽� 1ȸ ã��)

    if (KirbyFSM->GetCurObject())
    {
        // object�� �ִٸ� �̰� �켱
    }
    else
    {
        // �⺻������ �����ؾ� �ϴ� ����

        // ���� state�� �´� �߰�����
        KirbyFSM->GetCurAbility()->Run();
    }
}

void CKirbyRun::Enter()
{
}

void CKirbyRun::Exit()
{
}
