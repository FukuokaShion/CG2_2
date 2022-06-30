#pragma once
#include <Windows.h>
#include <string>
#include "Struct.h"
#include <DirectXMath.h>
using namespace DirectX;
#include <d3d12.h>
#pragma comment(lib, "d3d12.lib")
#include <d3dcompiler.h>
#pragma comment(lib, "d3dcompiler.lib")

class Triangle {
public:
	Triangle(float x1, float y1, float x2, float y2, float x3, float y3,ID3D12Device* Device, ID3D12GraphicsCommandList *CommandList);

	void draw();

public:
	ID3D12GraphicsCommandList* commandList = nullptr;
	HRESULT result;
	ID3D12Device* device = nullptr;
	D3D12_VERTEX_BUFFER_VIEW vbView{};
	ID3D12PipelineState* pipelineState = nullptr;
	ID3D12RootSignature* rootSignature;

};