//************************************************************
// Changes from base MS code:
//  ParticleCount = 1056;
//  GPUxThreads = 32; Determines # of rings and threads in GPU
//************************************************************

#pragma once

#include "DXSample.h"
#include "SimpleCamera.h"
#include "StepTimer.h"

using namespace DirectX;

// Note that while ComPtr is used to manage the lifetime of resources on the CPU,
// it has no understanding of the lifetime of resources on the GPU. Apps must account
// for the GPU lifetime of resources to avoid destroying objects that may still be
// referenced by the GPU.
// An example of this can be found in the class method: OnDestroy().
using Microsoft::WRL::ComPtr;

class D3D12nChristmasTree : public DXSample
{
public:
    D3D12nChristmasTree(UINT width, UINT height, std::wstring name);

    virtual void OnInit();
    virtual void OnUpdate();
    virtual void OnRender();
    virtual void OnDestroy();
    virtual void OnKeyDown(UINT8 key);
    virtual void OnKeyUp(UINT8 key);

private:
    static const UINT FrameCount = 2;
    static const UINT ThreadCount = 1; 
    static const float ParticleSpread;
    static const UINT ParticleCount = 1056; // 1024 = tree, 32 top of tree
    static const UINT GPUxThreads = 32; // The number of GPU threads in the x blocksize. 

    // "Vertex" definition for particles. Triangle vertices are generated 
    // by the geometry shader. Color data will be assigned to those 
    // vertices via this struct.
    struct ParticleVertex
    {
        XMFLOAT4 color;
        
    };

    // Position and velocity data for the particles in the system.
    // Two buffers full of Particle data are utilized in this sample.
    // The compute thread alternates writing to each of them.
    // The render thread renders using the buffer that is not currently
    // in use by the compute shader.
    struct Particle
    {
        XMFLOAT4 position;
        XMFLOAT4 velocity;
    };
    // added Particle2
    struct Particle2
    {
        XMFLOAT4 position;
        XMFLOAT4 velocity;
        bool onTree;
    };

    struct ConstantBufferGS
    {
        XMFLOAT4X4 worldViewProjection;
        XMFLOAT4X4 inverseView;
        
        
        // Constant buffers are 256-byte aligned in GPU memory. Padding is added
        // for convenience when computing the struct's size.
        float padding[32];
    };

    struct ConstantBufferCS
    {
        UINT param[4];
        float paramf[4];
    };

    // Pipeline objects.
    CD3DX12_VIEWPORT m_viewport;
    CD3DX12_RECT m_scissorRect;
    ComPtr<IDXGISwapChain3> m_swapChain;
    ComPtr<ID3D12Device> m_device;
    ComPtr<ID3D12Resource> m_renderTargets[FrameCount];
    UINT m_frameIndex;
    ComPtr<ID3D12CommandAllocator> m_commandAllocators[FrameCount];
    ComPtr<ID3D12CommandQueue> m_commandQueue;
    ComPtr<ID3D12RootSignature> m_rootSignature;

    ComPtr<ID3D12RootSignature> m_computeRootSignature;

    ComPtr<ID3D12DescriptorHeap> m_rtvHeap;
    ComPtr<ID3D12DescriptorHeap> m_srvUavHeap;
    UINT m_rtvDescriptorSize;
    UINT m_srvUavDescriptorSize;

    // Asset objects.
    ComPtr<ID3D12PipelineState> m_pipelineState;

    ComPtr<ID3D12PipelineState> m_computeState;

    //ComPtr<ID3D12CommandList> m_commandList;
    ComPtr<ID3D12GraphicsCommandList> m_commandList;

    ComPtr<ID3D12Resource> m_vertexBuffer;
    ComPtr<ID3D12Resource> m_vertexBufferUpload;
    D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView;
    ComPtr<ID3D12Resource> m_particleBuffer0[ThreadCount];
    ComPtr<ID3D12Resource> m_particleBuffer1[ThreadCount];
    // added m_particleBuffer2
    ComPtr<ID3D12Resource> m_particleBuffer2[ThreadCount];

    ComPtr<ID3D12Resource> m_particleBuffer0Upload[ThreadCount];
    ComPtr<ID3D12Resource> m_particleBuffer1Upload[ThreadCount];
    // added m_particleBuffer2Upload
    ComPtr<ID3D12Resource> m_particleBuffer2Upload[ThreadCount];
    // added mReadBackBuffer
    ComPtr<ID3D12Resource> m_ReadBackBuffer[ThreadCount];
    ComPtr<ID3D12Resource> m_constantBufferGS;
    UINT8* m_pConstantBufferGSData;
    ComPtr<ID3D12Resource> m_constantBufferCS;
    UINT8* m_pConstantBufferCSData;

    bool TreeIsUp = false;
    
    UINT m_srvIndex[ThreadCount]; // Denotes which of the particle buffer resource views is the SRV (0 or 1). The UAV is 1 - srvIndex.
    UINT m_heightInstances;
    UINT m_widthInstances;
    SimpleCamera m_camera;

    StepTimer m_timer;
    StepTimer m_movetimer;

    // Compute objects.
    ComPtr<ID3D12CommandAllocator> m_computeAllocator[ThreadCount];
    ComPtr<ID3D12CommandQueue> m_computeCommandQueue[ThreadCount];
    ComPtr<ID3D12GraphicsCommandList> m_computeCommandList[ThreadCount];

    // Synchronization objects.
    HANDLE m_swapChainEvent;
    ComPtr<ID3D12Fence> m_renderContextFence;
    UINT64 m_renderContextFenceValue;
    HANDLE m_renderContextFenceEvent;
    UINT64 m_frameFenceValues[FrameCount];

    ComPtr<ID3D12Fence> m_threadFences[ThreadCount];
    volatile HANDLE m_threadFenceEvents[ThreadCount];

    // Thread state.
    LONG volatile m_terminating;
    UINT64 volatile m_renderContextFenceValues[ThreadCount];
    UINT64 volatile m_threadFenceValues[ThreadCount];

    struct ThreadData
    {
        D3D12nChristmasTree* pContext;
        UINT threadIndex;
    };
    ThreadData m_threadData[ThreadCount];
    HANDLE m_threadHandles[ThreadCount];

    // Indices of the root signature parameters.
    enum GraphicsRootParameters : UINT32
    {
        GraphicsRootCBV = 0,
        GraphicsRootSRVTable,
        GraphicsRootParametersCount
    };

    enum CameraState
    {
        CameraInit,        
        CameraMoving,
        CameraStopped
    };

    CameraState cs = CameraInit;

    // added ComputeRootUAVTable2,
    enum ComputeRootParameters : UINT32
    {
        ComputeRootConstant = 0,
        ComputeRootCBV,
        ComputeRootSRVTable,
        ComputeRootUAVTable,
        ComputeRootUAVTable2,
        ComputeRootParametersCount
    };

    // Indices of shader resources in the descriptor heap.
    // added UavParticlePosVelo2
    enum DescriptorHeapIndex : UINT32
    {
        UavParticlePosVelo0 = 0,
        UavParticlePosVelo1 = UavParticlePosVelo0 + ThreadCount,
        SrvParticlePosVelo0 = UavParticlePosVelo1 + ThreadCount,
        SrvParticlePosVelo1 = SrvParticlePosVelo0 + ThreadCount,
        UavParticlePosVelo2 = SrvParticlePosVelo1 + ThreadCount,
        DescriptorCount = UavParticlePosVelo2 + ThreadCount
    };
    
    void LoadPipeline();
    void LoadAssets();
    void RestoreD3DResources();
    void ReleaseD3DResources();
    void WaitForGpu();

    void CreateAsyncContexts();

    float RandomPercent();
    void LoadParticlesSpace(_Out_writes_(numParticles) Particle* pParticles, const XMFLOAT3 &center, const XMFLOAT4 &velocity, float spread, UINT numParticles);
    void LoadParticlesTree(_Out_writes_(numParticles) Particle2* pParticles, const XMFLOAT4& velocity, const bool onTree, UINT numParticles);
    void CalcVel(_Out_writes_(numParticles) Particle* pParticles, Particle2* pParticles2, UINT numParticles);

    void CreateVertexBuffer();
    void CreateParticleBuffers();
    void PopulateCommandList();

    static DWORD WINAPI ThreadProc(ThreadData* pData)
    {
        return pData->pContext->AsyncComputeThreadProc(pData->threadIndex);
    }
    DWORD AsyncComputeThreadProc(int threadIndex);
    void Simulate(UINT threadIndex);

    void WaitForRenderContext();
    void MoveToNextFrame();
};
