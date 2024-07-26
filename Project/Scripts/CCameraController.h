#pragma once

#include <Engine/CScript.h>

enum class CameraMode
{
    NORMAL,

};

class CCameraController : public CScript
{
private:
    CGameObject*                m_Target;               // ī�޶� �ٶ�����ϴ� Ÿ��

    // ���� ��
    Vec3                        m_LookEyePos;           // ī�޶� �־�� �ϴ� ��ġ
    Vec3                        m_LookDir;              // ī�޶� �ٶ󺸴� ����
    float                       m_LookDist;             // ī�޶�� Ÿ�� ������ �Ÿ�

    float                       m_MinSpeed;             // ī�޶��� �ּ� ���ǵ�
    float                       m_MaxSpeed;             // ī�޶��� �ִ� ���ǵ�
    float                       m_ThresholdDistance;    // ī�޶� �ִ� ���ǵ�� �̵��ϱ� ���� �Ӱ� �Ÿ�

    float                       m_RotationSpeed;        // ȸ�� �ӵ�

    // ���� ������
    Vec3                        m_CurLookDir;           // ���� �����ӿ��� ī�޶��� ����
    Vec3                        m_CurLookAtPos;         // ���� �����ӿ��� ī�޶� �ٶ�����ϴ� ��ġ
    Vec3                        m_TargetPos;            // ���� �����ӿ��� Ÿ���� ��ġ

    // ���� ������
    Vec3                        m_PrevLookDir;          // ���� �����ӿ��� ī�޶��� ����
    Vec3                        m_PrevLookAtPos;        // ���� �����ӿ��� ī�޶� �ٶ�����ϴ� ��ġ

    //Edit
    bool                        m_EditMode;             // EditMode ����ġ
    float                       m_EditRotSpeed;         // EditMode������ ī�޶� ȸ�� ���� �ӵ�
    float                       m_EditZoomSpeed;        // EditMode������ ī�޶� �Ÿ� ���� �ӵ�

public:
    virtual void begin() override;
    virtual void tick() override;

private:
    void UpdateTargetPos(); 
    void UpdateLookAtPos();
    void UpdateLookDir();
    
    Vec3 CalCamPos(Vec3 _TargetWorldPos, Vec3 _LookDir, float _CamDist);
    
    // EditMode
    void EditMode();


public:
    virtual UINT SaveToLevelFile(FILE* _File) override;
    virtual UINT LoadFromLevelFile(FILE* _File) override;

public:
    CLONE(CCameraController);
    CCameraController();
    virtual ~CCameraController();
};
