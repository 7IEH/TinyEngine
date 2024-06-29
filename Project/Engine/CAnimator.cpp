#include "pch.h"

#include "CAnimator.h"

#include "CTimeMgr.h"
#include "CMeshRender.h"
#include "CStructuredBuffer.h"
#include "CAssetMgr.h"
#include "CAnimationUpdateShader.h"
#include "CKeyMgr.h"

CAnimator::CAnimator()
    : CComponent(COMPONENT_TYPE::ANIMATOR)
    , m_SkeletalMesh(nullptr)
    , m_mapClip{}
    , m_CurClipIdx(0)
    , m_vecClipUpdateTime{}
    , m_bPlay(true)
    , m_bRepeat(true)
    , m_PlaySpeed(1.f)
    , m_FrameRate(30)
    , m_CurTime(0.)
    , m_FrameIdx(0)
    , m_NextFrameIdx(0)
    , m_Ratio(0.f)
    , m_BoneFinalMatBuffer(nullptr)
    , m_bFinalMatUpdate(false)
    , m_bChanging(false)
    , m_CurChangeTime(0.)
    , m_ChangeDuration(0.)
    , m_NextClipIdx(-1)
    , m_bNextRepeat(true)
    , m_NextPlaySpeed(1.f)
{
    m_BoneFinalMatBuffer = new CStructuredBuffer;
}

CAnimator::CAnimator(const CAnimator& _origin)
    : CComponent(_origin)
    , m_SkeletalMesh(_origin.m_SkeletalMesh)
    , m_mapClip(_origin.m_mapClip)
    , m_CurClipIdx(_origin.m_CurClipIdx)
    , m_vecClipUpdateTime(_origin.m_vecClipUpdateTime)
    , m_bPlay(_origin.m_bPlay)
    , m_bRepeat(_origin.m_bRepeat)
    , m_PlaySpeed(_origin.m_PlaySpeed)
    , m_FrameRate(_origin.m_FrameRate)
    , m_CurTime(_origin.m_CurTime)
    , m_FrameIdx(_origin.m_FrameIdx)
    , m_NextFrameIdx(_origin.m_NextFrameIdx)
    , m_Ratio(_origin.m_Ratio)
    , m_BoneFinalMatBuffer(nullptr)
    , m_bFinalMatUpdate(false)
    , m_bChanging(_origin.m_bChanging)
    , m_CurChangeTime(_origin.m_CurChangeTime)
    , m_ChangeDuration(_origin.m_ChangeDuration)
    , m_NextClipIdx(_origin.m_NextClipIdx)
    , m_bNextRepeat(_origin.m_bNextRepeat)
    , m_NextPlaySpeed(_origin.m_NextPlaySpeed)
{
    m_BoneFinalMatBuffer = new CStructuredBuffer;
}

CAnimator::~CAnimator()
{
    if (nullptr != m_BoneFinalMatBuffer)
    {
        delete m_BoneFinalMatBuffer;
        m_BoneFinalMatBuffer = nullptr;
    }
}

void CAnimator::finaltick()
{
    if (!IsVaild())
        return;

    // �ٸ� Clip ���� ��ȯ ���� ���
    if (m_bChanging)
    {
        if (m_bPlay)
        {
            m_CurChangeTime += DT * m_PlaySpeed;
        }

        m_Ratio = float(m_CurChangeTime / m_ChangeDuration);

        if (m_CurChangeTime >= m_ChangeDuration)
        {
            SetCurClipIdx(m_NextClipIdx);
            m_vecClipUpdateTime[m_CurClipIdx] = 0.f; // �ִϸ��̼��� ó������ ����

            // �ִϸ��̼� ��� ����
            m_bRepeat = m_bNextRepeat;
            m_PlaySpeed = m_NextPlaySpeed;

            // �ʱ�ȭ
            m_bChanging = false;
            m_CurChangeTime = 0.;
            m_ChangeDuration = 0.;
            m_NextClipIdx = -1;
            m_bNextRepeat = true;
            m_NextPlaySpeed = 1.f;

            // �̹� ������ �ִϸ��̼� ����
            m_FrameIdx = m_SkeletalMesh->GetAnimClip()->at(m_CurClipIdx).iStartFrame;
            m_NextFrameIdx = m_FrameIdx + 1;
            m_Ratio = 0.;
        }
    }
    else
    {
        // ���� ������� Clip �� �ð��� �����Ѵ�.
        if (m_bPlay)
        {
            m_vecClipUpdateTime[m_CurClipIdx] += DT * m_PlaySpeed;
        }

        if (m_vecClipUpdateTime[m_CurClipIdx] >= m_SkeletalMesh->GetAnimClip()->at(m_CurClipIdx).dTimeLength)
        {
            // �ݺ� ���
            if (m_bRepeat)
            {
                m_vecClipUpdateTime[m_CurClipIdx] = 0.f;
            }
            else
            {
                m_vecClipUpdateTime[m_CurClipIdx] = m_SkeletalMesh->GetAnimClip()->at(m_CurClipIdx).dTimeLength;
            }
        }

        m_CurTime = m_SkeletalMesh->GetAnimClip()->at(m_CurClipIdx).dStartTime + m_vecClipUpdateTime[m_CurClipIdx];

        // ���� ������ �ε��� ���ϱ�
        double dFrameIdx = m_CurTime * m_FrameRate;
        m_FrameIdx = (int)dFrameIdx;

        if (m_FrameIdx >= m_SkeletalMesh->GetAnimClip()->at(m_CurClipIdx).iEndFrame)
            m_NextFrameIdx = m_FrameIdx; // ���̸� ���� �ε����� ����
        else
            m_NextFrameIdx = m_FrameIdx + 1;

        // �����Ӱ��� �ð��� ���� ������ �����ش�.
        m_Ratio = (float)(dFrameIdx - (double)m_FrameIdx);
    }

    // ��ǻƮ ���̴� ���꿩��
    m_bFinalMatUpdate = false;
}

void CAnimator::UpdateData()
{
    if (!IsVaild())
        return;

    if (!m_bFinalMatUpdate)
    {
        // Animation Update Compute Shader
        static CAnimationUpdateShader* pUpdateShader =
            (CAnimationUpdateShader*)CAssetMgr::GetInst()->FindAsset<CComputeShader>(L"AnimationUpdateCS").Get();

        // Bone Data
        CheckBoneFinalMatBuffer();

        pUpdateShader->SetFrameDataBuffer(m_SkeletalMesh->GetBoneFrameDataBuffer());
        pUpdateShader->SetOffsetMatBuffer(m_SkeletalMesh->GetBoneOffsetBuffer());
        pUpdateShader->SetOutputBuffer(m_BoneFinalMatBuffer);

        pUpdateShader->SetBoneCount(m_SkeletalMesh->GetBoneCount());
        pUpdateShader->SetFrameIndex(m_FrameIdx);
        pUpdateShader->SetNextFrameIdx(m_NextFrameIdx);
        pUpdateShader->SetFrameRatio(m_Ratio);

        // ������Ʈ ���̴� ����
        pUpdateShader->Execute();

        m_bFinalMatUpdate = true;
    }

    // t31 �������Ϳ� ������� ������(��������) ���ε�
    m_BoneFinalMatBuffer->UpdateData(31);
}

void CAnimator::finaltick_ModelEditor()
{
    if (!IsVaild())
        return;

    // ���� ������� Clip �� �ð��� �����Ѵ�.
    if (m_bPlay)
    {
        m_vecClipUpdateTime[m_CurClipIdx] += DT_ENGINE * m_PlaySpeed;
    }

    if (m_vecClipUpdateTime[m_CurClipIdx] >= m_SkeletalMesh->GetAnimClip()->at(m_CurClipIdx).dTimeLength)
    {
        // �ݺ� ���
        if (m_bRepeat)
        {
            m_vecClipUpdateTime[m_CurClipIdx] = 0.f;
        }
        else
        {
            m_vecClipUpdateTime[m_CurClipIdx] = m_SkeletalMesh->GetAnimClip()->at(m_CurClipIdx).dTimeLength;
        }
    }

    m_CurTime = m_SkeletalMesh->GetAnimClip()->at(m_CurClipIdx).dStartTime + m_vecClipUpdateTime[m_CurClipIdx];

    // ���� ������ �ε��� ���ϱ�
    double dFrameIdx = m_CurTime * m_FrameRate;
    m_FrameIdx = (int)(dFrameIdx);

    // ���� ������ �ε���
    if (m_FrameIdx >= m_SkeletalMesh->GetAnimClip()->at(m_CurClipIdx).iEndFrame)
        m_NextFrameIdx = m_FrameIdx; // ���̸� ���� �ε����� ����
    else
        m_NextFrameIdx = m_FrameIdx + 1;

    // �����Ӱ��� �ð��� ���� ������ �����ش�.
    m_Ratio = (float)(dFrameIdx - (double)m_FrameIdx);

    // Animation Update Compute Shader
    static CAnimationUpdateShader* pUpdateShader =
        (CAnimationUpdateShader*)CAssetMgr::GetInst()->FindAsset<CComputeShader>(L"AnimationUpdateCS").Get();

    // Bone Data
    CheckBoneFinalMatBuffer();

    pUpdateShader->SetFrameDataBuffer(m_SkeletalMesh->GetBoneFrameDataBuffer());
    pUpdateShader->SetOffsetMatBuffer(m_SkeletalMesh->GetBoneOffsetBuffer());
    pUpdateShader->SetOutputBuffer(m_BoneFinalMatBuffer);

    pUpdateShader->SetBoneCount(m_SkeletalMesh->GetBoneCount());
    pUpdateShader->SetFrameIndex(m_FrameIdx);
    pUpdateShader->SetNextFrameIdx(m_NextFrameIdx);
    pUpdateShader->SetFrameRatio(m_Ratio);

    // ������Ʈ ���̴� ����
    pUpdateShader->Execute();
}

void CAnimator::SetSkeletalMesh(Ptr<CMesh> _SkeletalMesh)
{
    if (nullptr == _SkeletalMesh)
    {
        // Reset
        m_SkeletalMesh = nullptr;
        m_mapClip.clear();
        m_CurClipIdx = 0;
        m_vecClipUpdateTime.clear();
        m_bPlay = true;
        m_bRepeat = true;
        m_PlaySpeed = 1.f;
        m_FrameRate = 30;
        m_CurTime = 0.;
        m_NextFrameIdx = 0;
        m_Ratio = 0.;
        m_bFinalMatUpdate = false;

        if (nullptr != m_BoneFinalMatBuffer)
        {
            delete m_BoneFinalMatBuffer;
            m_BoneFinalMatBuffer = new CStructuredBuffer;
        }

        return;
    }

    m_SkeletalMesh = _SkeletalMesh;

    const vector<tMTAnimClip>* vecAnimClip = m_SkeletalMesh->GetAnimClip();
    for (int i = 0; i < (int)vecAnimClip->size(); ++i)
    {
        const tMTAnimClip& Clip = vecAnimClip->at(i);
        const auto& iter = m_mapClip.insert(make_pair(Clip.strAnimName, i));
        assert(!(iter.first == m_mapClip.end())); // ������ Key�� Clip�� �����Ͽ���.
    }

    m_vecClipUpdateTime.resize(vecAnimClip->size());

    // Frame Rate ����
    if (!vecAnimClip->empty())
    {
        m_FrameRate = FbxTime::GetFrameRate(vecAnimClip->back().eMode);
    }

    CheckBoneFinalMatBuffer();
}

void CAnimator::SetFrameIdx(int _FrameIdx)
{
    double CurFrameTime = (_FrameIdx / m_FrameRate) - m_SkeletalMesh->GetAnimClip()->at(m_CurClipIdx).dStartTime;
    m_vecClipUpdateTime[m_CurClipIdx] = std::clamp(CurFrameTime, 0., m_SkeletalMesh->GetAnimClip()->at(m_CurClipIdx).dTimeLength);
}

UINT CAnimator::GetBoneCount() const
{
    if (nullptr == m_SkeletalMesh)
        return 0;

    return m_SkeletalMesh->GetBoneCount();
}

void CAnimator::ClearData()
{
    m_BoneFinalMatBuffer->Clear(31);

    UINT iMtrlCount = MeshRender()->GetMtrlCount();
    Ptr<CMaterial> pMtrl = nullptr;
    for (UINT i = 0; i < iMtrlCount; ++i)
    {
        pMtrl = MeshRender()->GetSharedMaterial(i);
        if (nullptr == pMtrl)
            continue;

        pMtrl->SetAnim3D(false);
        pMtrl->SetBoneCount(0);
    }
}

int CAnimator::FindClipIndex(const wstring& _strClipName)
{
    map<wstring, int>::iterator iter = m_mapClip.find(_strClipName);
    if (iter == m_mapClip.end())
        return -1; // Key�� �ش��ϴ� Clip �� ã�� ���Ͽ���.

    return iter->second;
}

void CAnimator::Play(const wstring& _strClipName, bool _bRepeat, float _PlaySpeed, double _ChangeDuration)
{
    int ClipIndex = FindClipIndex(_strClipName);
    if (-1 == ClipIndex)
        return;

    m_bChanging = true;
    m_CurChangeTime = 0.;
    m_ChangeDuration = _ChangeDuration;
    m_NextClipIdx = ClipIndex;
    m_bNextRepeat = _bRepeat;
    m_NextPlaySpeed = _PlaySpeed;

    m_NextFrameIdx = m_SkeletalMesh->GetAnimClip()->at(m_NextClipIdx).iStartFrame;
}

bool CAnimator::IsFinish() const
{
    // ���� ClipUpdateTime�� TimeLength�� ���̰� ���� ��� Finish
    return 1e-3 > abs(m_vecClipUpdateTime[m_CurClipIdx] - m_SkeletalMesh->GetAnimClip()->at(m_CurClipIdx).dTimeLength);
}

bool CAnimator::IsVaild()
{
    if (nullptr != m_SkeletalMesh && m_SkeletalMesh->IsSkeletalMesh())
    {
        return true;
    }

    return false;
}

void CAnimator::CheckBoneFinalMatBuffer()
{
    if (!IsVaild())
        return;

    UINT iBoneCount = m_SkeletalMesh->GetBoneCount();
    if (m_BoneFinalMatBuffer->GetElementCount() != iBoneCount)
    {
        m_BoneFinalMatBuffer->Create(sizeof(Matrix), iBoneCount, SB_TYPE::READ_WRITE, true, nullptr);
    }
}

void CAnimator::SaveToLevelFile(FILE* _File)
{
    SaveAssetRef(m_SkeletalMesh, _File);

    fwrite(&m_bPlay, 1, sizeof(bool), _File);
    fwrite(&m_bRepeat, 1, sizeof(bool), _File);
    fwrite(&m_PlaySpeed, 1, sizeof(float), _File);
}

void CAnimator::LoadFromLevelFile(FILE* _File)
{
    LoadAssetRef(m_SkeletalMesh, _File);
    SetSkeletalMesh(m_SkeletalMesh);

    fread(&m_bPlay, 1, sizeof(bool), _File);
    fread(&m_bRepeat, 1, sizeof(bool), _File);
    fread(&m_PlaySpeed, 1, sizeof(float), _File);
}
