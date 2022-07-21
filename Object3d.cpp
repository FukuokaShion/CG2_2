#include "Object3d.h"
#include <dinput.h>

Object3d::Object3d(ID3D12Device* device, XMMATRIX matProjection, XMMATRIX matView) {
	this->device = device;
	//�q�[�v�ݒ�
	D3D12_HEAP_PROPERTIES cbHeapProp{};
	//GPU�ւ̓]���p
	cbHeapProp.Type = D3D12_HEAP_TYPE_UPLOAD;
	//���\�[�X�ݒ�
	D3D12_RESOURCE_DESC cbResourceDesc{};
	cbResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	cbResourceDesc.Width = (sizeof(ConstBufferDataTransform) + 0xff) & ~0xff;
	cbResourceDesc.Height = 1;
	cbResourceDesc.DepthOrArraySize = 1;
	cbResourceDesc.MipLevels = 1;
	cbResourceDesc.SampleDesc.Count = 1;
	cbResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	//�萔�o�b�t�@�̐���
	this->result = this->device->CreateCommittedResource(&cbHeapProp, D3D12_HEAP_FLAG_NONE, &cbResourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&constBuffTransform));
	//�萔�o�b�t�@�̃}�b�s���O
	this->result = constBuffTransform->Map(0, nullptr, (void**)&constMapTransform);
	assert(SUCCEEDED(this->result));

	//�X�P�[�����O
	XMMATRIX matScale;
	matScale = XMMatrixIdentity();
	matScale = XMMatrixScaling(scale.x, scale.y, scale.z);
	//��]
	XMMATRIX matRota;
	matRota = XMMatrixIdentity();
	matRota *= XMMatrixRotationZ(XMConvertToRadians(rota.z));
	matRota *= XMMatrixRotationX(XMConvertToRadians(rota.x));
	matRota *= XMMatrixRotationY(XMConvertToRadians(rota.y));
	//���s�ړ�
	XMMATRIX matTrans;
	matTrans = XMMatrixTranslation(-50.0f, 0.0f, 0.0f);

	//���[���h�s��v�Z
	matWorld = XMMatrixIdentity(); //�P�ʍs�����
	matWorld *= matScale;
	matWorld *= matRota;
	matWorld *= matTrans;

	//�P�ʍs�����
	constMapTransform->mat = XMMatrixIdentity();
	//�萔�o�b�t�@�ɓ]��
	constMapTransform->mat = matWorld * matView * matProjection;
}

Object3d::~Object3d() {
}

void Object3d::Affinne(BYTE* keys, XMMATRIX matProjection, XMMATRIX matView) {
	//���[���h�ϊ�
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

	//�X�P�[�����O
	XMMATRIX matScale;
	matScale = XMMatrixIdentity();
	matScale = XMMatrixScaling(scale.x, scale.y, scale.z);
	//��]
	XMMATRIX matRota;
	matRota = XMMatrixIdentity();
	matRota *= XMMatrixRotationZ(XMConvertToRadians(rota.z));
	matRota *= XMMatrixRotationX(XMConvertToRadians(rota.x));
	matRota *= XMMatrixRotationY(XMConvertToRadians(rota.y));
	//���s�ړ�
	XMMATRIX matTrans;
	matTrans = XMMatrixIdentity();
	matTrans = XMMatrixTranslation(trans.x, trans.y, trans.z);
	//���[���h�s��v�Z
	matWorld = XMMatrixIdentity(); //�P�ʍs�����
	matWorld *= matScale;
	matWorld *= matRota;
	matWorld *= matTrans;

	if (parent != nullptr) {
		matWorld *= parent->matWorld;
	}

	//�萔�o�b�t�@�ɓ]��
	constMapTransform->mat = matWorld * matView * matProjection;
}

void Object3d::Draw(ID3D12GraphicsCommandList* commandList, D3D12_VERTEX_BUFFER_VIEW& vbView, D3D12_INDEX_BUFFER_VIEW& ibView,UINT numIndices) {
	// ���_�o�b�t�@�r���[�̐ݒ�R�}���h
	commandList->IASetVertexBuffers(0, 1, &vbView);
	// �C���f�b�N�X�o�b�t�@�r���[�̐ݒ�R�}���h
	commandList->IASetIndexBuffer(&ibView);
	//�萔�o�b�t�@�r���[(CBV)�̐ݒ�R�}���h
	commandList->SetGraphicsRootConstantBufferView(2, constBuffTransform->GetGPUVirtualAddress());
	// �`��R�}���h �i�C���f�b�N�X�o�b�t�@�����_�o�b�t�@�j
	commandList->DrawIndexedInstanced(numIndices, 1, 0, 0, 0);
}