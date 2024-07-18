#pragma once
#include <Engine/CScript.h>

enum class BulletState
{
    HOLD_AIR, // ������ ���� ����
    HOLD_WALL, // ���� �ε��� ����
    DRAG, // ������ �޴� ����(Kirby���� ���ư��� x)
    BACK, // Ŀ�� ���� ���ư��� ����
    RAGE, // ������ �����̴� ����(Kirby�� ������ ȸ��������, Kirby�� �Ѿư��� �ʴ´�)
};

class CKirbyCutterBullet : public CScript
{
private:
    BulletState         m_CurState;

    bool                m_IsBack;           // Ŀ��� �浹�� ȸ��

    Vec3                m_MoveDir;          //  �����̴� ����
    Vec3                m_PrevDir;
    float               m_Speed;            //  �ӵ�
    float               m_RageSpeed;        //  Rage���� ���ǵ�

    Vec3                m_Velocity;         //  ���� �ӵ�

    int                 m_BounceCount;      //  ���� ƨ��� Ƚ��

    float               m_Duration;         //  ���� �ִ�ð�
    float               m_Acc;              //  ���� ���ӽð�

    float               m_Frequency;        //  �¿�� ��鸮�� �ֱ�
    float               m_RotAcc;           
    float               m_Angle;            //  �¿�� ��鸮�� ũ��


public:
    virtual void begin() override;
    virtual void tick() override;

public:
    void SetState(BulletState _State);
    void SetDir(Vec3 _Dir) { m_MoveDir = _Dir; }

private:
    virtual void OnCollisionEnter(CCollider* _OtherCollider){}
    virtual void OnCollisionStay(CCollider* _OtherCollider){}
    virtual void OnCollisionExit(CCollider* _OtherCollider){}

    virtual void OnTriggerEnter(CCollider* _OtherCollider);
    virtual void OnTriggerExit(CCollider* _OtherCollider);


public:
    virtual void SaveToLevelFile(FILE* _File) override;
    virtual void LoadFromLevelFile(FILE* _File) override;

public:
    CLONE(CKirbyCutterBullet);
    CKirbyCutterBullet();
    virtual ~CKirbyCutterBullet();

};
