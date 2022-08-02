#pragma once
#include <d3d12.h>
#include <DirectXMath.h>
using namespace DirectX;
#include<wrl.h>
using namespace Microsoft::WRL;

//�萔�o�b�t�@�p�f�[�^�\����(3D�ϊ��s��)
struct ConstBufferDataTransform {
	XMMATRIX mat;//3D�ϊ��s��
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
	//�萔�o�b�t�@(�s��p)
	ComPtr<ID3D12Resource> constBuffTransform;
	//�萔�o�b�t�@�}�b�v(�s��p)
	ConstBufferDataTransform* constMapTransform;
	//�A�t�B���ϊ����
	XMFLOAT3 scale = { 1,1,1 };
	XMFLOAT3 rota = { 0,0,0 };
	XMFLOAT3 trans = { 0,0,0 };
	//���[���h�ϊ��s��
	XMMATRIX matWorld;
	//�e�I�u�W�F�N�g�ւ̃|�C���^
	Object3d* parent = nullptr;
};