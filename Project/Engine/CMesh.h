#pragma once
#include "CAsset.h"

#include "CFBXLoader.h"

class CStructuredBuffer;

struct tIndexInfo
{
    ComPtr<ID3D11Buffer> pIB;
    D3D11_BUFFER_DESC tIBDesc;
    UINT iIdxCount;
    void* pIdxSysMem;
};

class CMesh : public CAsset
{
private:
    ComPtr<ID3D11Buffer> m_VB;
    D3D11_BUFFER_DESC m_VBDesc;
    UINT m_VtxCount;
    void* m_VtxSysMem;

    // �ϳ��� ���ؽ����ۿ� �������� �ε������۰� ����
    vector<tIndexInfo> m_vecIdxInfo;

    // Animator ����
    vector<tMTAnimClip> m_vecAnimClip;
    vector<tMTBone> m_vecBones;

    CStructuredBuffer* m_pBoneFrameData; // ��ü �� ������ ����(ũ��, �̵�, ȸ��) (������ ������ŭ)
    CStructuredBuffer* m_pBoneOffset;    // �� ���� offset ���(�� ���� ��ġ�� �ǵ����� ���) (1�� ¥��)

private:
    void UpdateData(UINT _iSubset);
    void UpdateData_Inst(UINT _iSubset);

public:
    UINT GetVtxCount() const { return m_VtxCount; }
    Vtx* GetVtxSysMem() const { return (Vtx*)m_VtxSysMem; }
    UINT GetSubsetCount() const { return (UINT)m_vecIdxInfo.size(); }

public:
    const vector<tMTBone>* GetBones() const { return &m_vecBones; }
    UINT GetBoneCount() const { return (UINT)m_vecBones.size(); }
    const vector<tMTAnimClip>* GetAnimClip() const { return &m_vecAnimClip; }
    bool IsAnimMesh() const { return !m_vecAnimClip.empty(); }

    CStructuredBuffer* GetBoneFrameDataBuffer() const { return m_pBoneFrameData; } // ��ü �� ������ ����
    CStructuredBuffer* GetBoneOffsetBuffer() const { return m_pBoneOffset; }       // �� ���� offset ���

public:
    static CMesh* CreateFromContainer(CFBXLoader& _loader);
    int Create(void* _Vtx, UINT _VtxCount, void* _Idx, UINT _IdxCount);

    void render(UINT _iSubset);
    void render_draw(UINT _iSubset);
    void render_IndexedInstanced(UINT _InstanceCount);
    void render_instancing(UINT _iSubset);

public:
    virtual int Save(const wstring& _strRelativePath) override;
    virtual int Load(const wstring& _strFilePath) override;

    CLONE_DISABLE(CMesh);

public:
    CMesh(bool _bEngineAsset = false);
    virtual ~CMesh();
};
