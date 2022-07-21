#include "Object3d.h"
#include <dinput.h>

Object3d::Object3d(ID3D12Device* device, XMMATRIX matProjection, XMMATRIX matView) {
	this->device = device;
	//ヒープ設定
	D3D12_HEAP_PROPERTIES cbHeapProp{};
	//GPUへの転送用
	cbHeapProp.Type = D3D12_HEAP_TYPE_UPLOAD;
	//リソース設定
	D3D12_RESOURCE_DESC cbResourceDesc{};
	cbResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	cbResourceDesc.Width = (sizeof(ConstBufferDataTransform) + 0xff) & ~0xff;
	cbResourceDesc.Height = 1;
	cbResourceDesc.DepthOrArraySize = 1;
	cbResourceDesc.MipLevels = 1;
	cbResourceDesc.SampleDesc.Count = 1;
	cbResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	//定数バッファの生成
	this->result = this->device->CreateCommittedResource(&cbHeapProp, D3D12_HEAP_FLAG_NONE, &cbResourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&constBuffTransform));
	//定数バッファのマッピング
	this->result = constBuffTransform->Map(0, nullptr, (void**)&constMapTransform);
	assert(SUCCEEDED(this->result));

	//スケーリング
	XMMATRIX matScale;
	matScale = XMMatrixIdentity();
	matScale = XMMatrixScaling(scale.x, scale.y, scale.z);
	//回転
	XMMATRIX matRota;
	matRota = XMMatrixIdentity();
	matRota *= XMMatrixRotationZ(XMConvertToRadians(rota.z));
	matRota *= XMMatrixRotationX(XMConvertToRadians(rota.x));
	matRota *= XMMatrixRotationY(XMConvertToRadians(rota.y));
	//平行移動
	XMMATRIX matTrans;
	matTrans = XMMatrixTranslation(-50.0f, 0.0f, 0.0f);

	//ワールド行列計算
	matWorld = XMMatrixIdentity(); //単位行列を代入
	matWorld *= matScale;
	matWorld *= matRota;
	matWorld *= matTrans;

	//単位行列を代入
	constMapTransform->mat = XMMatrixIdentity();
	//定数バッファに転送
	constMapTransform->mat = matWorld * matView * matProjection;
}

Object3d::~Object3d() {
}

void Object3d::Affinne(BYTE* keys, XMMATRIX matProjection, XMMATRIX matView) {
	//ワールド変換
	if (parent == nullptr) {
		if (keys[DIK_UP] || keys[DIK_DOWN] || keys[DIK_RIGHT] || keys[DIK_LEFT]) {
			if (keys[DIK_UP]) {
				trans.z += 1.0f;
			}
			else if (keys[DIK_DOWN]) {
				trans.z -= 1.0f;
			}
			if (keys[DIK_RIGHT]) {
				trans.x += 1.0f;
			}
			else if (keys[DIK_LEFT]) {
				trans.x -= 1.0f;
			}
		}
	}

	//スケーリング
	XMMATRIX matScale;
	matScale = XMMatrixIdentity();
	matScale = XMMatrixScaling(scale.x, scale.y, scale.z);
	//回転
	XMMATRIX matRota;
	matRota = XMMatrixIdentity();
	matRota *= XMMatrixRotationZ(XMConvertToRadians(rota.z));
	matRota *= XMMatrixRotationX(XMConvertToRadians(rota.x));
	matRota *= XMMatrixRotationY(XMConvertToRadians(rota.y));
	//平行移動
	XMMATRIX matTrans;
	matTrans = XMMatrixIdentity();
	matTrans = XMMatrixTranslation(trans.x, trans.y, trans.z);
	//ワールド行列計算
	matWorld = XMMatrixIdentity(); //単位行列を代入
	matWorld *= matScale;
	matWorld *= matRota;
	matWorld *= matTrans;

	if (parent != nullptr) {
		matWorld *= parent->matWorld;
	}

	//定数バッファに転送
	constMapTransform->mat = matWorld * matView * matProjection;
}

void Object3d::Draw(ID3D12GraphicsCommandList* commandList, D3D12_VERTEX_BUFFER_VIEW& vbView, D3D12_INDEX_BUFFER_VIEW& ibView,UINT numIndices) {
	// 頂点バッファビューの設定コマンド
	commandList->IASetVertexBuffers(0, 1, &vbView);
	// インデックスバッファビューの設定コマンド
	commandList->IASetIndexBuffer(&ibView);
	//定数バッファビュー(CBV)の設定コマンド
	commandList->SetGraphicsRootConstantBufferView(2, constBuffTransform->GetGPUVirtualAddress());
	// 描画コマンド （インデックスバッファ＆頂点バッファ）
	commandList->DrawIndexedInstanced(numIndices, 1, 0, 0, 0);
}