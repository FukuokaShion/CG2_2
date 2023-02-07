#include<d3dcompiler.h>
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


	//設定

//----------------------ゲームループ-------------------------

	Object3d* obj1 = nullptr;
	Model* model1 = nullptr;
	
	Object3d::StaticInitialize(dxCommon->GetDevice(), WinApp::window_width, WinApp::window_heigit);
	obj1 = Object3d::Create();

	Model::SetDevice(dxCommon->GetDevice());
	model1 = Model::LoadFromOBJ("triangle_mat");

	obj1->SetModel(model1);
	obj1->SetPosition({ -30,0,0 });
	obj1->SetEye({0,0,-100});


	Object3d* obj2 = nullptr;
	Model* model2 = nullptr;

	Object3d::StaticInitialize(dxCommon->GetDevice(), WinApp::window_width, WinApp::window_heigit);
	obj2 = Object3d::Create();

	Model::SetDevice(dxCommon->GetDevice());
	model2 = Model::LoadFromOBJ("cube");

	obj2->SetModel(model2);

	obj2->SetPosition({ 30,0,0 });
	obj2->SetEye({0,0,-100});

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



#pragma endregion 最初のシーンの更新
		//描画前処理
		dxCommon->PreDraw();
		Object3d::PreDraw(dxCommon->GetCommandList());

#pragma region 最初のシーンの描画
		obj1->Draw();
		obj2->Draw();

#pragma endregion 最初のシーンの描画

		//描画後処理
		dxCommon->PostDraw();
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