#pragma once

#include <Engine/CScript.h>

// ���� ���� Ÿ�� (enum ������ �켱����)
enum class ForceDirType
{
    DEFORM,
    HIT,
    END,
};

enum class AddForceType
{
    Acceleration,   // ������ �����ϰ� ��ü�� �ﰢ���� ���ӵ� �߰�
    VelocityChange, // ������ �����ϰ� ��ü�� �ﰢ���� �ӵ� ��ȭ �߰�
};

// ��ü�� ������ ������ �ٲ���ϴ� ���
struct ForceDirInfo
{
    ForceDirType Type;
    Vec3 Dir; // �ٶ������ ����(World ��ǥ��)
};

class CKirbyMoveController : public CScript
{
private:
    // �Է�
    Vec3                        m_Input;
    bool                        m_bJump;
    bool                        m_bActiveFriction;

    // Lock
    bool                        m_bMoveLock;
    bool                        m_bJumpLock;
    bool                        m_bDirLock;

    // ����
    Vec3                        m_CurDir;
    Vec3                        m_MoveDir;
    Vec3                        m_TowardDir;
    Vec3                        m_GroundNormal;
    vector<ForceDirInfo>        m_ForceDirInfos;

    // ����
    Vec3                        m_MoveVelocity;
    Vec3                        m_AddVelocity;
    Vec3                        m_Accel;
    float                       m_Speed;
    float                       m_MaxSpeed;
    float                       m_Friction; // ������ ���
    float                       m_JumpPower;
    float                       m_Gravity;

    float                       m_HoveringLimitHeight;
    float                       m_HoveringMinSpeed;

    float                       m_RayCastDist;
    float                       m_RotSpeed;

    float                       m_HoveringHeight;


private:
    virtual void OnControllerColliderHit(struct ControllerColliderHit Hit);

public:
    virtual void begin() override;
    virtual void tick() override;

public:
    void LockMove() { m_bMoveLock = true; }
    void UnlockMove() { m_bMoveLock = false; }
    void LockJump() { m_bJumpLock = true; }
    void UnlockJump() { m_bJumpLock = false; }
    void LockDirection() { m_bDirLock = true; }
    void UnlockDirection() { m_bDirLock = false; }

    void Jump() { m_bJump = true; }
    void VelocityCut(float _f) { _f == 0.f ? m_MoveVelocity.y = 0.f : m_MoveVelocity.y /= _f; }
  

    void SetGuard(bool _Guard) { m_bActiveFriction = _Guard; }
    void SetSpeed(float _Speed) { m_Speed = _Speed; }
    void SetFriction(float _Friction) { m_Friction = _Friction; }
    void ClearVelocityY() { m_MoveVelocity.y = 0.f; }
    void SetVelocity(Vec3 _VeloCity) { m_MoveVelocity = _VeloCity; }
    void AddVelocity(Vec3 _AddVel) { m_AddVelocity += _AddVel; }

    void ClearHoveringHeight() { m_HoveringHeight = 0.f; }
    void SetGravity(float _Gravity) { m_Gravity = _Gravity; }

    Vec3 GetInput() const { return m_Input; }
    Vec3 GetMoveDir() const { return m_MoveDir; }
    Vec3 GetVelocity() const { return m_MoveVelocity; }
    float GetSpeed() const { return m_Speed; }
    float GetGravity() const { return m_Gravity; }
    float GetGuard() const { return m_bActiveFriction; }

private:
    void Input();
    void SetDir();
    void Move();
    void SurfaceAlignment();

public:
    virtual void SaveToLevelFile(FILE* _File) override;
    virtual void LoadFromLevelFile(FILE* _File) override;

public:
    CLONE(CKirbyMoveController);
    CKirbyMoveController();
    CKirbyMoveController(const CKirbyMoveController& _Origin);
    virtual ~CKirbyMoveController();
};
