﻿#include<d3dcompiler.h>
#include<Windows.h>
#include <tchar.h>
#include<iostream>
#include<d3d12.h>
#include<dxgi1_6.h>
#include<cassert>
#include<vector>
#include<string>
#include<DirectXMath.h>
#include<DirectXTex.h>
#pragma comment(lib,"d3dcompiler.lib")
#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")
#include"Triangle.h"
#include"object3d.h"
#include"input.h"
#include"WinApp.h"
#include"DirectXCommon.h"

using namespace DirectX;
using namespace std;


//定数バッファ用データ構造体（マテリアル）
struct ConstBufferDataMaterial {
	XMFLOAT4 color;//色（RGBA）
};

//ウィンドウプロージャー
LRESULT WindowProc(HWND hwnd, UINT msg, WPARAM wapram, LPARAM lparam) {
	//メッセージに応じてゲーム固有の処理を行う
	switch (msg)
	{
		//ウインドウが破棄された
	case WM_DESTROY:
		//OSに対して、アプリの終了を伝える
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, msg, wapram, lparam);
}


//windowsアプリのエントリーポイント（main関数）
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
#pragma region 基盤システムの初期化
	WinApp* winApp = nullptr;
	//WindowsAPIの初期化
	winApp = new WinApp;
	winApp->Initialize();

	DirectXCommon* dxCommon = nullptr;
	//DirectX初期化処理
	dxCommon = new DirectXCommon();
	dxCommon->Initialize(winApp);

	Input* input = nullptr;
	//DirectInputの初期化
	input = new Input;
	input->Initialize(winApp);
#pragma endregion 基盤システムの初期化
#pragma region 最初のシーンの初期化


#pragma endregion 最初のシーンの初期化

	MSG msg{};

	HRESULT result;

	//描画初期化処理
	//頂点データ構造体
	struct Vertex
	{
		XMFLOAT3 pos;    //xyz座標
		XMFLOAT3 normal; //法線ベクトル
		XMFLOAT2 uv;     //uv座標
	};
	//頂点データ
	Vertex vertices[] = {
		//x      y	   z	 u    v
		//前
		{{-5.0f,-5.0f,-5.0f},{}, {0.0f,1.0f} },//左下
		{{-5.0f, 5.0f,-5.0f},{},{0.0f,0.0f} },//左上
		{{ 5.0f,-5.0f,-5.0f},{},{1.0f,1.0f} },//右下
		{{ 5.0f, 5.0f,-5.0f},{},{1.0f,0.0f} },//右上
		//後
		{{-5.0f,-5.0f, 5.0f},{},{0.0f,1.0f} },//左下
		{{-5.0f, 5.0f, 5.0f},{},{0.0f,0.0f} },//左上
		{{ 5.0f,-5.0f, 5.0f},{},{1.0f,1.0f} },//右下
		{{ 5.0f, 5.0f, 5.0f},{},{1.0f,0.0f} },//右上
		//左
		{{-5.0f,-5.0f,-5.0f},{},{0.0f,1.0f} },//左下
		{{-5.0f,-5.0f, 5.0f},{},{0.0f,0.0f} },//左上
		{{-5.0f, 5.0f,-5.0f},{},{1.0f,1.0f} },//右下
		{{-5.0f, 5.0f, 5.0f},{},{1.0f,0.0f} },//右上
		//右
		{{ 5.0f,-5.0f,-5.0f},{},{0.0f,1.0f} },//左下
		{{ 5.0f,-5.0f, 5.0f},{},{0.0f,0.0f} },//左上
		{{ 5.0f, 5.0f,-5.0f},{},{1.0f,1.0f} },//右下
		{{ 5.0f, 5.0f, 5.0f},{},{1.0f,0.0f} },//右上
		//下
		{{-5.0f,-5.0f,-5.0f},{},{0.0f,1.0f} },//左下
		{{-5.0f,-5.0f, 5.0f},{},{0.0f,0.0f} },//左上
		{{ 5.0f,-5.0f,-5.0f},{},{1.0f,1.0f} },//右下
		{{ 5.0f,-5.0f, 5.0f},{},{1.0f,0.0f} },//右上
		//上
		{{-5.0f, 5.0f,-5.0f},{},{0.0f,1.0f} },//左下
		{{-5.0f, 5.0f, 5.0f},{},{0.0f,0.0f} },//左上
		{{ 5.0f, 5.0f,-5.0f},{},{1.0f,1.0f} },//右下
		{{ 5.0f, 5.0f, 5.0f},{},{1.0f,0.0f} },//右上
	};
	//インデックスデータ
	unsigned short indices[] =
	{
		//前
		0,1,2,//三角形1
		1,3,2,//三角形2
		//後ろ
		6,5,4,//三角形1
		6,7,5,//三角形2
		//左
		8,9,10,//三角形1
		9,11,10,//三角形2
		//右
		14,13,12,//三角形1
		14,15,13,//三角形2
		//下
		18,17,16,//三角形1
		18,19,17,//三角形2
		//上
		20,21,22,//三角形1
		21,23,22,//三角形2
	};
	//頂点データ全体のサイズ=頂点データ一つ分のサイズ*頂点データの要素数
	UINT sizeVB = static_cast<UINT>(sizeof(vertices[0]) * _countof(vertices));
	//頂点バッファの設定
	D3D12_HEAP_PROPERTIES heapProp{};  //ヒープ設定
	heapProp.Type = D3D12_HEAP_TYPE_UPLOAD; //GPUへの転送用
	//リソース設定
	D3D12_RESOURCE_DESC resDesc{};
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resDesc.Width = sizeVB; //頂点データ全体のサイズ
	resDesc.Height = 1;
	resDesc.DepthOrArraySize = 1;
	resDesc.MipLevels = 1;
	resDesc.SampleDesc.Count = 1;
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	//頂点バッファの生成
	ComPtr<ID3D12Resource> vertBuff = nullptr;
	result = dxCommon->GetDevice()->CreateCommittedResource(
		&heapProp, //ヒープ設定
		D3D12_HEAP_FLAG_NONE,
		&resDesc, //リソース設定
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&vertBuff));
	assert(SUCCEEDED(result));
	//インデックスデータ全体のサイズ
	UINT sizeIB = static_cast<UINT>(sizeof(uint16_t) * _countof(indices));

	//リソース設定
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resDesc.Width = sizeIB;//インデックス情報が入る分のサイズ
	resDesc.Height = 1;
	resDesc.DepthOrArraySize = 1;
	resDesc.MipLevels = 1;
	resDesc.SampleDesc.Count = 1;
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	//インデックスバッファの生成
	ComPtr<ID3D12Resource> indexBuff = nullptr;
	result = dxCommon->GetDevice()->CreateCommittedResource(
		&heapProp,//ヒープ設定
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&indexBuff));
	//インデックスバッファをマッピング
	uint16_t* indexMap = nullptr;
	result = indexBuff->Map(0, nullptr, (void**)&indexMap);
	//全インデックスに対して
	for (int i = 0; i < _countof(indices); i++)
	{
		indexMap[i] = indices[i];//インデックスをコピー
	}
	//マッピング解除
	indexBuff->Unmap(0, nullptr);

	//法線の計算
	for (int i = 0; i < _countof(indices) / 3; i++)
	{//三角形1つごとの計算していく
		//三角形のインデックスを取り出して、一時的な変数に入れる
		unsigned short indices0 = indices[i * 3 + 0];
		unsigned short indices1 = indices[i * 3 + 1];
		unsigned short indices2 = indices[i * 3 + 2];
		//三角形を構成する頂点座標をベクトルに代入
		XMVECTOR p0 = XMLoadFloat3(&vertices[indices0].pos);
		XMVECTOR p1 = XMLoadFloat3(&vertices[indices1].pos);
		XMVECTOR p2 = XMLoadFloat3(&vertices[indices2].pos);
		//p0→p1ベクトル、p0→p2ベクトルを計算(ベクトル減算)
		XMVECTOR v1 = XMVectorSubtract(p1, p0);
		XMVECTOR v2 = XMVectorSubtract(p2, p0);
		//外積は両方から垂直なベクトル
		XMVECTOR normal = XMVector3Cross(v1, v2);
		//正規化（長さを1にする）
		normal = XMVector3Normalize(normal);
		//求めた法線を頂点データに代入
		XMStoreFloat3(&vertices[indices0].normal, normal);
		XMStoreFloat3(&vertices[indices1].normal, normal);
		XMStoreFloat3(&vertices[indices2].normal, normal);
	}

	//GPU上のバッファに対応した仮想メモリ（メインメモリ上）を取得
	Vertex* vertMap = nullptr;
	result = vertBuff->Map(0, nullptr, (void**)&vertMap);
	assert(SUCCEEDED(result));
	//前頂点に対して
	for (int i = 0; i < _countof(vertices); i++) {
		vertMap[i] = vertices[i]; //座標コピー
	}
	//繋がり解除
	vertBuff->Unmap(0, nullptr);

	//頂点バッファビューの作成
	D3D12_VERTEX_BUFFER_VIEW vbView{};
	//GPU仮想アドレス
	vbView.BufferLocation = vertBuff->GetGPUVirtualAddress();
	//頂点バッファのサイズ
	vbView.SizeInBytes = sizeVB;
	//頂点一つ分のデータサイズ
	vbView.StrideInBytes = sizeof(vertices[0]);

	//インデックスバッファビューの作成
	D3D12_INDEX_BUFFER_VIEW ibView{};
	ibView.BufferLocation = indexBuff->GetGPUVirtualAddress();
	ibView.Format = DXGI_FORMAT_R16_UINT;
	ibView.SizeInBytes = sizeIB;

	ID3DBlob* vsBlob = nullptr;//頂点シェーダーオブジェクト
	ID3DBlob* psBlob = nullptr;//ピクセルシェーダーオブジェクト
	ID3DBlob* errorBlob = nullptr;//エラーオブジェクト

	//頂点シェーダーの読み込みとコンパイル
	result = D3DCompileFromFile(
		L"Resources/shaders/BasicVS.hlsl", //シェーダーファイル名
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,//インクルード可能にする
		"main", "vs_5_0",//エントリーポイント名、シェーダーモデル指定
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, //デバッグ用設定
		0,
		&vsBlob, &errorBlob);

	//エラーなら
	if (FAILED(result)) {
		//errorBlobからエラー内容をstring型にコピー
		std::string error;
		error.resize(errorBlob->GetBufferSize());

		std::copy_n((char*)errorBlob->GetBufferPointer(),
			errorBlob->GetBufferSize(),
			error.begin());
		error += "\n";
		//エラー内容を出力ウインドウに表示
		OutputDebugStringA(error.c_str());
		assert(0);
	}
	//ピクセルシェーダーの読み込みとコンパイル
	result = D3DCompileFromFile(
		L"Resources/shaders/BasicPS.hlsl",
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"Graph", "ps_5_0",
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
		0,
		&psBlob, &errorBlob);
	//エラーなら
	if (FAILED(result)) {
		//errorBlobから内容をstring型にコピー
		std::string error;
		error.resize(errorBlob->GetBufferSize());

		std::copy_n((char*)errorBlob->GetBufferPointer(),
			errorBlob->GetBufferSize(),
			error.begin());
		error += "\n";
		//エラー内容を出力ウインドウに表示
		OutputDebugStringA(error.c_str());
		assert(0);
	}
	//頂点レイアウト
	D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
		{
		"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,
		D3D12_APPEND_ALIGNED_ELEMENT,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0,
		},//vyz座標(一行で書いたほうが見やすい)
		{
			"NORMAL",0,DXGI_FORMAT_R32G32B32_FLOAT,0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0
		},//法線ベクトル(一行で書いたほうが見やすい)
		{
		"TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0,
		D3D12_APPEND_ALIGNED_ELEMENT,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0,
		},//uv(一行で書いたほうが見やすい)
	};

	//設定
	D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineDesc{};
	//シェーダーの設定
	pipelineDesc.VS.pShaderBytecode = vsBlob->GetBufferPointer();
	pipelineDesc.VS.BytecodeLength = vsBlob->GetBufferSize();
	pipelineDesc.PS.pShaderBytecode = psBlob->GetBufferPointer();
	pipelineDesc.PS.BytecodeLength = psBlob->GetBufferSize();

	//サンプルマスクの設定
	pipelineDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
	//ラスタライザの設定
	pipelineDesc.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;//カリングしない
	pipelineDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;//ポリゴン内塗りつぶし
	pipelineDesc.RasterizerState.DepthClipEnable = true;//深度グリッピングを有効に
	//ブレンドステート
	//pipelineDesc.BlendState.RenderTarget[0].RenderTargetWriteMask
	//	= D3D12_COLOR_WRITE_ENABLE_ALL;//RBGAすべてのチャンネルを描画
	//レンダーターゲットのブレンド設定
	D3D12_RENDER_TARGET_BLEND_DESC& blenddesc = pipelineDesc.BlendState.RenderTarget[0];
	blenddesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;//RGBA全てのチャンネルを描画
	blenddesc.BlendEnable = false;//ブレンドを有効にする
	blenddesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;//加算
	blenddesc.SrcBlendAlpha = D3D12_BLEND_ONE;//ソースの値を100%使う
	blenddesc.DestBlendAlpha = D3D12_BLEND_ZERO;//デストの値を0%使う
	//加算合成
	blenddesc.BlendOp = D3D12_BLEND_OP_ADD;//加算
	blenddesc.SrcBlend = D3D12_BLEND_ONE;//ソースの値を100%使う
	blenddesc.DestBlend = D3D12_BLEND_ONE;//デストの値を100%使う
	////減算合成
	//blenddesc.BlendOp = D3D12_BLEND_OP_REV_SUBTRACT;//デストからソースを減算
	//blenddesc.SrcBlend = D3D12_BLEND_ONE;//ソースの値を100%使う
	//blenddesc.DestBlend = D3D12_BLEND_ONE;//デストの値を100%使う
	////色反転
	//blenddesc.BlendOp = D3D12_BLEND_OP_ADD;//加算
	//blenddesc.SrcBlend = D3D12_BLEND_INV_DEST_COLOR;//1.0f-デストカラーの値
	//blenddesc.DestBlend = D3D12_BLEND_ZERO;//使わない
	////半透明合成
	//blenddesc.BlendOp = D3D12_BLEND_OP_ADD;//加算
	//blenddesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;//ソースのアルファ値
	//blenddesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;//1.0f-ソースのアルファ値
	//頂点レイアウトの設定
	pipelineDesc.InputLayout.pInputElementDescs = inputLayout;
	pipelineDesc.InputLayout.NumElements = _countof(inputLayout);
	//図形の形状設定
	pipelineDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	//その他の設定
	pipelineDesc.NumRenderTargets = 1;//描画対象は一つ
	pipelineDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;//0～255指定のRGBA
	pipelineDesc.SampleDesc.Count = 1; //1ピクセルにつき一回サンプリング

	//デスクリプタレンジの設定
	D3D12_DESCRIPTOR_RANGE descriptorRange{};
	descriptorRange.NumDescriptors = 1;//一度の描画に使うテクスチャが一枚なので１
	descriptorRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorRange.BaseShaderRegister = 0;//テクスチャレジスタ0番
	descriptorRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	//ルートパラメーターの設定
	D3D12_ROOT_PARAMETER rootParams[3] = {};
	rootParams[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;//定数バッファ
	rootParams[0].Descriptor.ShaderRegister = 0;//定数バッファ番号
	rootParams[0].Descriptor.RegisterSpace = 0;//デフォルト値
	rootParams[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;//全てのシェーダーから見える
	//テクスチャレジスタ0番
	rootParams[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;//種類
	rootParams[1].DescriptorTable.pDescriptorRanges = &descriptorRange;//デスクリプタレンジ
	rootParams[1].DescriptorTable.NumDescriptorRanges = 1;//デスクリプタレンジ数
	rootParams[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;//全てのシェーダーから見える
	//定数バッファ1番目
	rootParams[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;//種類
	rootParams[2].Descriptor.ShaderRegister = 1;				//定数バッファ番号
	rootParams[2].Descriptor.RegisterSpace = 0;					//デフォルト値
	rootParams[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;//すべてのシェーダーから見える
	//テクスチャサンプラー設定
	D3D12_STATIC_SAMPLER_DESC samplerDesc{};
	samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;//横繰り返し（タイリング）
	samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;//縦繰り返し（タイリング）
	samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;//奥行繰り返し（タイリング）
	samplerDesc.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;//ボーダーの時は黒
	samplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;//全てリニア補完
	samplerDesc.MaxLOD = D3D12_FLOAT32_MAX;//ミップマップ最大値
	samplerDesc.MinLOD = 0.0f;//ミップマップ最小値
	samplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	samplerDesc.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//ピクセルシェーダーからのみ使用可能
	//ルートシグネチャ
	ComPtr<ID3D12RootSignature> rootSignature;
	//ルートシグネチャの設定
	D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc{};
	rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	rootSignatureDesc.pParameters = rootParams;//ルートパラメーターの先頭アドレス
	rootSignatureDesc.NumParameters = _countof(rootParams);//ルートパラメータ数
	rootSignatureDesc.pStaticSamplers = &samplerDesc;
	rootSignatureDesc.NumStaticSamplers = 1;
	//ルートシグネチャのシリアライズ
	ComPtr<ID3DBlob> rootSigBlob;
	result = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0,
		&rootSigBlob, &errorBlob);
	assert(SUCCEEDED(result));
	result = dxCommon->GetDevice()->CreateRootSignature(0, rootSigBlob->GetBufferPointer(), rootSigBlob->GetBufferSize(),
		IID_PPV_ARGS(&rootSignature));
	assert(SUCCEEDED(result));
	//パイプラインにルートシグネチャをセット
	pipelineDesc.pRootSignature = rootSignature.Get();

	//デプスステンシルステートの設定
	pipelineDesc.DepthStencilState.DepthEnable = true;//深度テストを行う
	pipelineDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;//書き込み許可
	pipelineDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;//小さければ合格
	pipelineDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;//深度値フォーマット

	//パイプランスステートの生成
	ComPtr<ID3D12PipelineState> pipelineState = nullptr;
	result = dxCommon->GetDevice()->CreateGraphicsPipelineState(&pipelineDesc, IID_PPV_ARGS(&pipelineState));
	assert(SUCCEEDED(result));

	ComPtr<ID3D12Resource> constBuffTransform0 = nullptr;
	//ConstBufferDataTransform* constMapTransform0 = nullptr;
	////ヒープ設定
	//D3D12_HEAP_PROPERTIES cbHeapProp{};
	//cbHeapProp.Type = D3D12_HEAP_TYPE_UPLOAD;
	////リソース設定
	//D3D12_RESOURCE_DESC cbResourceDesc{};
	//cbResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	//cbResourceDesc.Width = (sizeof(ConstBufferDataTransform) + 0xff) & ~0xff;//256バイトアラインメント
	//cbResourceDesc.Height = 1;
	//cbResourceDesc.DepthOrArraySize = 1;
	//cbResourceDesc.MipLevels = 1;
	//cbResourceDesc.SampleDesc.Count = 1;
	//cbResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	////3Dオブジェクトの数
	//const size_t kObjectConst = 50;
	////3Dオブジェクトの配列
	//Object3D object3ds[kObjectConst];

	////配列内の全オブジェクトに対して
	//for (int i = 0; i < _countof(object3ds); i++) {
	//	//初期化
	//	object3ds[i].Initialize(dxCommon->GetDevice());

	//	//ここからは親子構造のサンプル
	//	//先頭以外なら
	//	if (i > 0) {
	//		//一つ前のオブジェクトを親オブジェクトとする
	//		object3ds[i].parent = &object3ds[i - 1];
	//		//親オブジェクトの9割の大きさ
	//		object3ds[i].scale = { 0.9f,0.9f,0.9f };
	//		//親オブジェクトに対してZ軸まわりに30度回転
	//		object3ds[i].rotation = { 0.0f,0.0f,XMConvertToRadians(30.0f) };

	//		//親オブジェクトに対してZ方向-8.0ずらす
	//		object3ds[i].position = { 0.0f,0.0f,-8.0f };
	//	}

	//}
	ComPtr<ID3D12Resource> constBuffTransform1 = nullptr;
	//ConstBufferDataTransform* constMapTransform1 = nullptr;
	//{
	//	//ヒープ設定
	//	D3D12_HEAP_PROPERTIES cbHeapProp{};
	//	cbHeapProp.Type = D3D12_HEAP_TYPE_UPLOAD;
	//	//リソース設定
	//	D3D12_RESOURCE_DESC cbResourceDesc{};
	//	cbResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	//	cbResourceDesc.Width = (sizeof(ConstBufferDataTransform) + 0xff) & ~0xff;//256バイトアラインメント
	//	cbResourceDesc.Height = 1;
	//	cbResourceDesc.DepthOrArraySize = 1;
	//	cbResourceDesc.MipLevels = 1;
	//	cbResourceDesc.SampleDesc.Count = 1;
	//	cbResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	//}
	////並行投影行列の計算
	//constMapTransform->mat = XMMatrixOrthographicOffCenterLH(
	//	0.0f, window_width,
	//	window_heigit, 0.0f,
	//	0.0f, 1.0f);
	XMMATRIX matProjection = XMMatrixPerspectiveFovLH(
		XMConvertToRadians(45.0f),		   //上下が書く45度
		(float)WinApp::window_width / WinApp::window_heigit,//アスペクト比（画面横幅/画面縦幅）
		0.1f, 1000.0f					   //前端、奥端
	);

	//ビュー変換行列
	XMMATRIX matView;
	XMFLOAT3 eye(0, 0, -100);//視点ベクトル
	XMFLOAT3 target(0, 0, 0);//注視点座標
	XMFLOAT3 up(0, 1, 0);	 //上方向ベクトル
	matView = XMMatrixLookAtLH(XMLoadFloat3(&eye), XMLoadFloat3(&target), XMLoadFloat3(&up));

	////行列の計算
	//constMapTransform0->mat = matView * matProjection;

	////行列の計算
	//constMapTransform1->mat = matView * matProjection;

	//ヒープ設定
	D3D12_HEAP_PROPERTIES cbHeapProp{};
	cbHeapProp.Type = D3D12_HEAP_TYPE_UPLOAD;//GPUへの転送用
	//リソース設定
	D3D12_RESOURCE_DESC cbResourceDesc{};
	cbResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	cbResourceDesc.Width = (sizeof(ConstBufferDataMaterial) + 0xff) & ~0xff;//256バイトアラインメント
	cbResourceDesc.Height = 1;
	cbResourceDesc.DepthOrArraySize = 1;
	cbResourceDesc.MipLevels = 1;
	cbResourceDesc.SampleDesc.Count = 1;
	cbResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	ComPtr<ID3D12Resource> constBuffMaterial = nullptr;
	//定数バッファの生成
	result = dxCommon->GetDevice()->CreateCommittedResource(
		&cbHeapProp,//ヒープ設定
		D3D12_HEAP_FLAG_NONE,
		&cbResourceDesc,//リソース設定
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuffMaterial));
	assert(SUCCEEDED(result));
	//定数バッファのマッピング
	ConstBufferDataMaterial* constMapMaterial = nullptr;
	result = constBuffMaterial->Map(0, nullptr, (void**)&constMapMaterial);//マッピング
	//値を書き込むと自動的に転送される
	constMapMaterial->color = XMFLOAT4(1, 1, 1, 0.5f);//RGBAで半透明の赤
	assert(SUCCEEDED(result));

	//横方向ピクセル数
	const size_t textureWidth = 256;
	//縦方向ピクセル数
	const size_t textureHeight = 256;
	//配列の要素数
	const size_t imageDataCount = textureWidth * textureHeight;
	//画像イメージデータ配列
	XMFLOAT4* imageData = new XMFLOAT4[imageDataCount];//必ず後で解放する

	//全ピクセルの色を初期化
	for (size_t i = 0; i < imageDataCount; i++) {
		imageData[i].x = 1.0f;//R
		imageData[i].y = 0.0f;//G
		imageData[i].z = 0.0f;//B
		imageData[i].w = 1.0f;//A
	}
	TexMetadata metadata{};
	ScratchImage scratchImg{};
	//WICテクスチャのロード
	result = LoadFromWICFile(
		L"Resources/mario.jpg",//Resourcesフォルダのtexture.png
		WIC_FLAGS_NONE,
		&metadata, scratchImg);
	ScratchImage mipChain{};
	//ミニマップ生成
	result = GenerateMipMaps(
		scratchImg.GetImages(), scratchImg.GetImageCount(), scratchImg.GetMetadata(),
		TEX_FILTER_DEFAULT, 0, mipChain);
	if (SUCCEEDED(result)) {
		scratchImg = std::move(mipChain);
		metadata = scratchImg.GetMetadata();
	}
	//読み込んだディフューズテクスチャをSRGBとして扱う
	metadata.format = MakeSRGB(metadata.format);
	//ヒープ設定
	D3D12_HEAP_PROPERTIES textureHeapProp{};
	textureHeapProp.Type = D3D12_HEAP_TYPE_CUSTOM;
	textureHeapProp.CPUPageProperty =
		D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
	textureHeapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;
	//リソース設定
	D3D12_RESOURCE_DESC textureResourceDesc{};
	textureResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	textureResourceDesc.Format = metadata.format;
	textureResourceDesc.Width = metadata.width;//幅
	textureResourceDesc.Height = (UINT)metadata.height;//高さ
	textureResourceDesc.DepthOrArraySize = (UINT16)metadata.arraySize;
	textureResourceDesc.MipLevels = (UINT16)metadata.mipLevels;
	textureResourceDesc.SampleDesc.Count = 1;
	//テクスチャバッファを生成
	ComPtr<ID3D12Resource> texBuff = nullptr;
	result = dxCommon->GetDevice()->CreateCommittedResource(
		&textureHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&textureResourceDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&texBuff));
	//全ミップマップについて
	for (size_t i = 0; i < metadata.mipLevels; i++)
	{
		//ミップマップレベルを指定してイメージを取得
		const Image* img = scratchImg.GetImage(i, 0, 0);
		//テクスチャバッファにデータを転送
		result = texBuff->WriteToSubresource(
			(UINT)i,
			nullptr,//全領域へコピー
			img->pixels,//元データアドレス
			(UINT)img->rowPitch,//1ラインサイズ
			(UINT)img->slicePitch//一枚サイズ
		);
		assert(SUCCEEDED(result));
	}
	//SRVの最大個数
	const size_t kMaxSRVCount = 2056;

	//デスクリプタヒープの設定
	D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
	srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;//シェーダーから見えるように
	srvHeapDesc.NumDescriptors = kMaxSRVCount;

	//設定を元にSRV用デスクリプタヒープを生成
	ComPtr <ID3D12DescriptorHeap> srvHeap = nullptr;
	result = dxCommon->GetDevice()->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&srvHeap));
	assert(SUCCEEDED(result));
	//SRVヒープの先頭ハンドルを取得
	D3D12_CPU_DESCRIPTOR_HANDLE srvHandle = srvHeap->GetCPUDescriptorHandleForHeapStart();

	//シェーダリソースビュー設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};//設定構造体
	srvDesc.Format = resDesc.Format;//RGBA float
	srvDesc.Shader4ComponentMapping =
		D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;//2Dテクスチャ
	srvDesc.Texture2D.MipLevels = resDesc.MipLevels;

	//ハンドルの指す位置にシェーダーリソースビュー作成
	dxCommon->GetDevice()->CreateShaderResourceView(texBuff.Get(), &srvDesc, srvHandle);

	TexMetadata metadata2{};
	ScratchImage scratchImg2{};
	//WICテクスチャのロード
	result = LoadFromWICFile(
		L"Resources/reimu.png",//Resourcesフォルダのtexture.png
		WIC_FLAGS_NONE,
		&metadata2, scratchImg2);

	//リソース設定
	D3D12_RESOURCE_DESC textureResourceDesc2{};
	textureResourceDesc2.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	textureResourceDesc2.Format = metadata2.format;
	textureResourceDesc2.Width = metadata2.width;//幅
	textureResourceDesc2.Height = (UINT)metadata2.height;//高さ
	textureResourceDesc2.DepthOrArraySize = (UINT16)metadata2.arraySize;
	textureResourceDesc2.MipLevels = (UINT16)metadata2.mipLevels;
	textureResourceDesc2.SampleDesc.Count = 1;

	//テクスチャバッファを生成
	ComPtr<ID3D12Resource> texBuff2 = nullptr;
	result = dxCommon->GetDevice()->CreateCommittedResource(
		&textureHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&textureResourceDesc2,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&texBuff2));

	//全ミップマップについて
	for (size_t i = 0; i < metadata2.mipLevels; i++)
	{
		//ミップマップレベルを指定してイメージを取得
		const Image* img = scratchImg2.GetImage(i, 0, 0);
		//テクスチャバッファにデータを転送
		result = texBuff2->WriteToSubresource(
			(UINT)i,
			nullptr,//全領域へコピー
			img->pixels,//元データアドレス
			(UINT)img->rowPitch,//1ラインサイズ
			(UINT)img->slicePitch//一枚サイズ
		);
		assert(SUCCEEDED(result));
	}

	//一つハンドルを進める
	UINT incrementSize = dxCommon->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	srvHandle.ptr += incrementSize;

	//シェーダリソースビュー設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc2{};//設定構造体
	srvDesc2.Format = textureResourceDesc2.Format;//RGBA float
	srvDesc2.Shader4ComponentMapping =
		D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc2.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;//2Dテクスチャ
	srvDesc2.Texture2D.MipLevels = textureResourceDesc2.MipLevels;

	//ハンドルの指す位置にシェーダーリソースビュー作成
	dxCommon->GetDevice()->CreateShaderResourceView(texBuff2.Get(), &srvDesc2, srvHandle);

	float angle = 0.0f;//カメラの回転角

	//XMFLOAT3 position = { 0.0f,-30.0f,0.0f };

	//XMFLOAT3 scale = { 1.0f,1.0f,1.0f };

	//XMFLOAT3 rotation = { 0.0f,0.0f,0.0f };

	//XMFLOAT3 position1 = { -20.0f,0,0 };

	//XMFLOAT3 scale1 = { 1.0f,1.0f,1.0f };

	//XMFLOAT3 rotation1 = { 0.0f,45.0f,0.0f };

	////ワールド変換行列
	//XMMATRIX matWorld = XMMatrixIdentity();

	//XMMATRIX matScale = XMMatrixIdentity();
	//matScale *= XMMatrixScaling(scale.x, scale.y, scale.z);

	//XMMATRIX matRot = XMMatrixIdentity();//回転行列
	//matRot *= XMMatrixRotationZ(XMConvertToRadians(XM_PI / 180 * rotation.z));//Z軸回りに0度回転
	//matRot *= XMMatrixRotationX(XMConvertToRadians(XM_PI / 180 * rotation.x));//X軸回りに15度回転
	//matRot *= XMMatrixRotationY(XMConvertToRadians(XM_PI / 180 * rotation.y));//Y軸回りに度回転

	//XMMATRIX matTrans = XMMatrixIdentity();//平行移動行列
	//matTrans *= XMMatrixTranslation(position.x, position.y, position.z);//(-50,0,0)平行移動

	////ワールド変換行列
	//XMMATRIX matWorld1=XMMatrixIdentity();

	//XMMATRIX matScale1=XMMatrixScaling(scale1.x, scale1.y, scale1.z);//スケーリング行列

	//XMMATRIX matRot1=XMMatrixRotationY(XM_PI / 180 * rotation1.y);//回転行列

	//XMMATRIX matTrans1=XMMatrixTranslation(position1.x, position1.y, position1.z);//平行移動行列

	////ワールド行列を合成
	//matWorld1 = matScale1 * matRot1 * matTrans1;
	////ワールド、ビュー、射影行列を合成してシェーダーに転送
	//constMapTransform1->mat = matWorld1 * matView * matProjection;;

//----------------------ゲームループ-------------------------

	Object3d* obj1 = nullptr;
	Model* model1 = nullptr;
	
	Object3d::StaticInitialize(dxCommon->GetDevice(), WinApp::window_width, WinApp::window_heigit);
	obj1 = Object3d::Create();

	Model::SetDevice(dxCommon->GetDevice());
	model1 = Model::LoadFromOBJ("triangle_mat");

	obj1->SetModel(model1);
	obj1->SetPosition({ -30,0,0 });
	obj1->SetEye(eye);


	Object3d* obj2 = nullptr;
	Model* model2 = nullptr;

	Object3d::StaticInitialize(dxCommon->GetDevice(), WinApp::window_width, WinApp::window_heigit);
	obj2 = Object3d::Create();

	Model::SetDevice(dxCommon->GetDevice());
	model2 = Model::LoadFromOBJ("cube");

	obj2->SetModel(model2);

	obj2->SetPosition({ 30,0,0 });
	obj2->SetEye(eye);

	while (true) {
#pragma region 基盤システムの更新
		//×ボタンで終了メッセージがきたらゲームループを抜ける
		if (winApp->ProcessMessage()) {
			break;
		}
		//入力の更新
		input->Update();

#pragma endregion 基盤システムの更新

#pragma region 最初のシーンの更新


		obj1->Update();
		obj2->Update();

		if (input->PushKey(DIK_SPACE))
		{
			obj1->SetModel(model2);
			obj2->SetModel(model1);
		}
		else {
			obj1->SetModel(model1);
			obj2->SetModel(model2);
		}


		if (input->PushKey(DIK_Z) || input->PushKey(DIK_X))
		{
			if (input->PushKey(DIK_Z)) { angle += XMConvertToRadians(1.0f); }
			else if (input->PushKey(DIK_X)) { angle -= XMConvertToRadians(1.0f); }

			//angleラジアンだけY軸まわりに回転。半径は-100
			eye.x = -100 * sinf(angle);
			eye.z = -100 * cosf(angle);

			obj1->SetEye(eye);
			obj2->SetEye(eye);

			matView = XMMatrixIdentity();
			matView = XMMatrixLookAtLH(XMLoadFloat3(&eye), XMLoadFloat3(&target), XMLoadFloat3(&up));
		}

#pragma endregion 最初のシーンの更新
		//描画前処理
		dxCommon->PreDraw();
		Object3d::PreDraw(dxCommon->GetCommandList());

#pragma region 最初のシーンの描画
		obj1->Draw();
		obj2->Draw();

		//パイプラインステートとルートシグネチャの設定コマンド
		dxCommon->GetCommandList()->SetPipelineState(pipelineState.Get());
		dxCommon->GetCommandList()->SetGraphicsRootSignature(rootSignature.Get());
		//プリミティブ形状の設定コマンド
		dxCommon->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		//頂点バッファビューの設定コマンド
		dxCommon->GetCommandList()->IASetVertexBuffers(0, 1, &vbView);
		//定数バッファビュー（CBV）の設定コマンド
		dxCommon->GetCommandList()->SetGraphicsRootConstantBufferView(0, constBuffMaterial->GetGPUVirtualAddress());
		//SRVヒープの設定コマンド
		dxCommon->GetCommandList()->SetDescriptorHeaps(1, srvHeap.GetAddressOf());
		//SRVヒープの先頭ハンドルを取得（SRVを指しているはず）
		D3D12_GPU_DESCRIPTOR_HANDLE srvGpuHandle = srvHeap->GetGPUDescriptorHandleForHeapStart();
		//2枚目を指し示すようにしたSRVのハンドルをルトパラメーター1番に設定
		/*srvGpuHandle.ptr += incrementSize;*/
		//SRVヒープの先頭にあるSRVをルートパラメータ1番に設定
		dxCommon->GetCommandList()->SetGraphicsRootDescriptorTable(1, srvGpuHandle);
		////インデックスバッファビューの設定コマンド
		//dxCommon->GetCommandList()->IASetIndexBuffer(&ibView);
		////定数バッファビュー(CBV)の設定コマンド
		//dxCommon->GetCommandList()->SetGraphicsRootConstantBufferView(2, constBuffTransform0->GetGPUVirtualAddress());
		////描画コマンド
		//dxCommon->GetCommandList()->DrawIndexedInstanced(_countof(indices), 1, 0, 0, 0);//すべての頂点を使って描画

		////定数バッファビュー(CBV)の設定コマンド
		//dxCommon->GetCommandList()->SetGraphicsRootConstantBufferView(2, constBuffTransform1->GetGPUVirtualAddress());
		////描画コマンド
		//dxCommon->GetCommandList()->DrawIndexedInstanced(_countof(indices), 1, 0, 0, 0);//すべての頂点を使って描画




#pragma endregion 最初のシーンの描画

		//描画後処理
		dxCommon->PostDrow();
		Object3d::PostDraw();
	}

#pragma region 最初のシーンの終了


#pragma endregion 最初のシーンの終了

#pragma region 基盤システムの終了
	delete input;

	delete dxCommon;

	winApp->Finalize();

	delete winApp;
#pragma endregion 基盤システムの終了
	return 0;
}