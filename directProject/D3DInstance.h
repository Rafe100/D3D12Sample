#pragma once
#include "Common.h"
class D3DInstance
{
public:
	D3DInstance();
	void Draw();
	virtual void OnInit(int width, int height);
	virtual void OnUpdate();
	virtual void OnRender();
	virtual void OnDestroy();
private:
	static const UINT m_frameCount = 2;
	int m_width;
	int m_height;
	UINT m_frameIndex;
	void LoadPipeline();
	void LoadAssets();
	void FlushCmdQueue();
	void PopulateCommandList();
	void WaitForPreviousFrame();

	//pipeline object
	ComPtr<ID3D12Device> m_device;
	ComPtr<ID3D12CommandQueue> m_cmdQueue;
	ComPtr<IDXGISwapChain3> m_swapChain;
	ComPtr<ID3D12DescriptorHeap> m_rtvHeap;
	ComPtr<ID3D12DescriptorHeap> m_dsvHeap;
	ComPtr<ID3D12CommandAllocator> m_commandAllocator;
	ComPtr<ID3D12Resource> m_renderTargets[m_frameCount];
	ComPtr<ID3D12GraphicsCommandList> m_cmdList;
	ComPtr< ID3D12PipelineState> m_pipelineState;
	ComPtr<ID3D12Fence> m_fence;
	D3D12_VIEWPORT m_viewPort;
	D3D12_RECT m_scissorRect;
	HANDLE m_fenceEvent;
	UINT64 m_fenceValue;
	UINT m_rtvDescriptorSize;
	UINT m_currentBackBuffer = 0;
	float m_bColor[4];
};
