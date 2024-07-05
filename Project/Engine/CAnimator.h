#pragma once
#include "CComponent.h"

#include "Ptr.h"
#include "CTexture.h"
#include "CMaterial.h"
#include "CMesh.h"

class CMesh;
class CStructuredBuffer;

class CAnimator : public CComponent
{
private:
    Ptr<CMesh> m_SkeletalMesh;

    map<wstring, int> m_mapClip; // Key, Clip Index

    int m_CurClipIdx; // Ŭ�� �ε���
    vector<double> m_vecClipUpdateTime;

    bool m_bPlay;      // �ִϸ��̼� ��� ����
    bool m_bRepeat;    // �ݺ� ��� ����
    float m_PlaySpeed; // ��� �ӵ�

    double m_FrameRate; // Animation FPS
    double m_CurTime;

    int m_FrameIdx;     // Ŭ���� ���� ������
    int m_NextFrameIdx; // Ŭ���� ���� ������
    float m_Ratio;      // ������ ���� ����

    vector<Matrix> m_BoneTransformMat;           // �� ��ȯ ��� ������
    CStructuredBuffer* m_BoneTransformMatBuffer; // Ư�� �������� ���� ���

    CStructuredBuffer* m_BoneFinalMatBuffer; // Ư�� �������� ���� ���
    bool m_bFinalMatUpdate;                  // ������� ���� ���࿩��

    // Next Animation Parameter
    bool m_bChanging;
    double m_CurChangeTime;
    double m_ChangeDuration;
    int m_NextClipIdx;
    bool m_bNextRepeat;
    float m_NextPlaySpeed;

public:
    virtual void finaltick() override;
    virtual void UpdateData() override;
    void finaltick_ModelEditor();

    void ClearData();

public:
    int FindClipIndex(const wstring& _strClipName);
    void Play(const wstring& _strClipName, bool _bRepeat = true, float _PlaySpeed = 1.f);
    bool IsFinish() const;

    bool IsValid();

public:
    Ptr<CMesh> GetSkeletalMesh() const { return m_SkeletalMesh; }
    void SetSkeletalMesh(Ptr<CMesh> _SkeletalMesh);

    int GetCurClipIdx() const { return m_CurClipIdx; }
    void SetCurClipIdx(int _Idx) { m_CurClipIdx = _Idx; }

    const vector<double>& GetClipTime() const { return m_vecClipUpdateTime; }

    bool IsPlaying() const { return m_bPlay; }
    void SetPlay(bool _bPlay) { m_bPlay = _bPlay; }

    bool IsRepeat() const { return m_bRepeat; }
    void SetRepeat(bool _bRepeat) { m_bRepeat = _bRepeat; }

    float GetPlaySpeed() const { return m_PlaySpeed; }
    void SetPlaySpeed(float _Speed) { m_PlaySpeed = _Speed; }

    double GetFrameRate() const { return m_FrameRate; }

    // ��ü ������ �ε���
    int GetCurFrameIdx() const { return m_FrameIdx; }
    void SetFrameIdx(int _FrameIdx);

    // Clip������ ������ �ε���
    int GetClipFrameIndex();
    void SetClipFrameIndex(int _FrameIdx);

    const vector<Matrix>& GetBoneTransformMat() const { return m_BoneTransformMat; }
    const Matrix& GetBoneTransformMat(UINT _Idx) const { return m_BoneTransformMat[_Idx]; }

    CStructuredBuffer* GetFinalBoneMatBuffer() const { return m_BoneFinalMatBuffer; }
    UINT GetBoneCount() const;

private:
    void CheckBoneMatBuffer();

public:
    virtual void SaveToLevelFile(FILE* _File) override;
    virtual void LoadFromLevelFile(FILE* _File) override;

    CLONE(CAnimator);

public:
    CAnimator();
    CAnimator(const CAnimator& _origin);
    virtual ~CAnimator();
};