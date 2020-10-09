
#include "D3DInstance.h"
#include "Common.h"
#include "WinApplication.h"
D3DInstance::D3DInstance() : m_bColor{0,1.0,1.0,1.0} {

}

void D3DInstance::OnInit(int width, int height) {
	m_width = width;
	m_height = height;
	LoadPipeline();
	LoadAssets();
}

void D3DInstance::OnRender() {

}

void D3DInstance::OnUpdate() {

}

void D3DInstance::OnDestroy() {

}

void D3DInstance::LoadPipeline() {
	//factory
	UINT dxgiFactoryFlags = 0;
	ComPtr<IDXGIFactory4> factory;
	ThrowIfFailed(CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&factory)));

	//adapter
	UINT adapterIndex = 0;
	ComPtr<IDXGIAdapter1> adapter;
	std::vector<DXGI_ADAPTER_DESC1> descVector;
	for (adapterIndex = 0; DXGI_ERROR_NOT_FOUND != factory->EnumAdapters1(adapterIndex, &adapter); adapterIndex++) {
		DXGI_ADAPTER_DESC1 desc;
		adapter->GetDesc1(&desc);
		descVector.push_back(desc);
		// Check to see whether the adapter supports Direct3D 12, but don't create the actual device yet.
		bool supportD3D12 = SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), nullptr));
		if (supportD3D12) {
			break;
		}
	}
	ThrowIfFailed(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_device)));

	//command queue
	D3D12_COMMAND_QUEUE_DESC commandQueueDesc = {};
	commandQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	commandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	ThrowIfFailed(m_device->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(&m_cmdQueue)));

	//swapChain
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
	swapChainDesc.BufferCount = m_frameCount;
	swapChainDesc.Width = m_width;
	swapChainDesc.Height = m_height;
	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapChainDesc.SampleDesc.Count = 1;
	ComPtr<IDXGISwapChain1> swapChain;
	ThrowIfFailed(factory->CreateSwapChainForHwnd(
		m_cmdQueue.Get(), 
		WinApplication::GetHwnd(),
		&swapChainDesc,
		nullptr,
		nullptr,
		&swapChain
	));

	// This sample does not support fullscreen transitions.
	ThrowIfFailed(factory->MakeWindowAssociation(WinApplication::GetHwnd(), DXGI_MWA_NO_ALT_ENTER));

	ThrowIfFailed(swapChain.As(&m_swapChain));
	m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();

	// Describe and create a render target view (RTV) descriptor heap.
	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
	rtvHeapDesc.NumDescriptors = m_frameCount;
	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	ThrowIfFailed(m_device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&m_rtvHeap)));
	m_rtvDescriptorSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	//frame resources
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_rtvHeap->GetCPUDescriptorHandleForHeapStart());
	for (UINT n = 0; n < m_frameCount; n++) {
		ThrowIfFailed(m_swapChain->GetBuffer(n, IID_PPV_ARGS(&m_renderTargets[n])));
		m_device->CreateRenderTargetView(m_renderTargets[n].Get(), nullptr, rtvHandle);
		rtvHandle.Offset(1, m_rtvDescriptorSize);
	}

	ThrowIfFailed(m_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_commandAllocator)));
}

void D3DInstance::LoadAssets() {

	ThrowIfFailed(m_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_commandAllocator.Get(), nullptr, IID_PPV_ARGS(&m_cmdList)));
	ThrowIfFailed(m_cmdList->Close());

	// sync object fence
	ThrowIfFailed(m_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence)));
	m_fenceValue = 1;
	m_fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
	if (m_fenceEvent == nullptr) {
		ThrowIfFailed(HRESULT_FROM_WIN32(GetLastError()));
	}
}

void D3DInstance::Draw() {
	ThrowIfFailed(m_commandAllocator->Reset());
	ThrowIfFailed(m_cmdList->Reset(m_commandAllocator.Get(), nullptr));
	//m_cmdList->RSSetViewports(1, &m_viewPort);
	//m_cmdList->RSSetScissorRects(1, &m_scissorRect);
	//将后台缓冲资源从呈现状态转换到渲染目标状态（即准备接收图像渲染）。
	UINT& ref_mCurrentBackBuffer = m_currentBackBuffer;
	m_cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_renderTargets[ref_mCurrentBackBuffer].Get(),//转换资源为后台缓冲区资源
		D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));//从呈现到渲染目标转换
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = m_rtvHeap->GetCPUDescriptorHandleForHeapStart();
	
	m_cmdList->ClearRenderTargetView(rtvHandle, m_bColor, 0, nullptr);//清除RT背景色为m_bColor，并且不设置裁剪矩形
	//D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = m_dsvHeap->GetCPUDescriptorHandleForHeapStart();
	//m_cmdList->ClearDepthStencilView(dsvHandle,	//DSV描述符句柄
	//	D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL,	//FLAG
	//	1.0f,	//默认深度值
	//	0,	//默认模板值
	//	0,	//裁剪矩形数量
	//	nullptr);	//裁剪矩形指针
	//然后我们指定将要渲染的缓冲区，即指定RTV和DSV。
	m_cmdList->OMSetRenderTargets(1,//待绑定的RTV数量
		&rtvHandle,	//指向RTV数组的指针
		true,	//RTV对象在堆内存中是连续存放的
		//&dsvHandle);	//指向DSV的指针
		nullptr);	//指向DSV的指针
	//等到渲染完成，我们要将后台缓冲区的状态改成呈现状态，使其之后推到前台缓冲区显示。完了，关闭命令列表，等待传入命令队列。
	m_cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_renderTargets[ref_mCurrentBackBuffer].Get(),
		D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));//从渲染目标到呈现
	//完成命令的记录关闭命令列表
	ThrowIfFailed(m_cmdList->Close());
	//等CPU将命令都准备好后，需要将待执行的命令列表加入GPU的命令队列。使用的是ExecuteCommandLists函数。
	ID3D12CommandList* commandLists[] = { m_cmdList.Get() };//声明并定义命令列表数组
	m_cmdQueue->ExecuteCommandLists(_countof(commandLists), commandLists);//将命令从命令列表传至命令队列
	//然后交换前后台缓冲区索引（这里的算法是1变0，0变1，为了让后台缓冲区索引永远为0）。
	ThrowIfFailed(m_swapChain->Present(0, 0));
	ref_mCurrentBackBuffer = (ref_mCurrentBackBuffer + 1) % 2;
	FlushCmdQueue();
}

void D3DInstance::FlushCmdQueue()
{
	// Advance the fence value to mark commands up to this fence point.
	m_fenceValue++;

	// Add an instruction to the command queue to set a new fence point.
	// Because we are on the GPU timeline, the new fence point won’t be
	// set until the GPU finishes processing all the commands prior to
	// this Signal().
	ThrowIfFailed(m_cmdQueue->Signal(m_fence.Get(), m_fenceValue));

	// Wait until the GPU has completed commands up to this fence point.
	if (m_fence->GetCompletedValue() < m_fenceValue)
	{
		HANDLE eventHandle = CreateEventEx(nullptr, false, false, EVENT_ALL_ACCESS);

		// Fire event when GPU hits current fence.
		ThrowIfFailed(m_fence->SetEventOnCompletion(m_fenceValue, eventHandle));

		// Wait until the GPU hits current fence event is fired.
		WaitForSingleObject(eventHandle, INFINITE);

		CloseHandle(eventHandle);
	}
}