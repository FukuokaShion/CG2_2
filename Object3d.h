#pragma once
#include <d3d12.h>
#include <DirectXMath.h>
using namespace DirectX;
#include<wrl.h>
using namespace Microsoft::WRL;

//定数バッファ用データ構造体(3D変換行列)
struct ConstBufferDataTransform {
	XMMATRIX mat;//3D変換行列
};

class Object3d{
public:
	Object3d(ID3D12Device* device, XMMATRIX matProjection, XMMATRIX matView);

	~Object3d();

	void Affinne(BYTE* keys, XMMATRIX matProjection, XMMATRIX matView);

	void Draw(ID3D12GraphicsCommandList* commandList, D3D12_VERTEX_BUFFER_VIEW& vbView, D3D12_INDEX_BUFFER_VIEW& ibView, UINT numIndices);


private:
	HRESULT result;
	ComPtr<ID3D12Device> device;
public:
	//定数バッファ(行列用)
	ComPtr<ID3D12Resource> constBuffTransform;
	//定数バッファマップ(行列用)
	ConstBufferDataTransform* constMapTransform;
	//アフィン変換情報
	XMFLOAT3 scale = { 1,1,1 };
	XMFLOAT3 rota = { 0,0,0 };
	XMFLOAT3 trans = { 0,0,0 };
	//ワールド変換行列
	XMMATRIX matWorld;
	//親オブジェクトへのポインタ
	Object3d* parent = nullptr;
};