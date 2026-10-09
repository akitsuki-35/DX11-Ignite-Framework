/*============================================================
*	@file	 : Application.cpp
*	@brief	 : アプリケーション内部処理
*
* 　@author  : @akitsuki-35（https://github.com/akitsuki-35）
* 　@date	 : 2026/04/21
*	@updated : 2026/10/09
*============================================================*/
#include "Application.h"
#include "Scene.h"
#include "SystemTimer.h"
#include "Graphics.h"
#include "Transition.h"
#include "Input.h"
#include <cassert>

/*------------------------------------------------------------
	初期化
------------------------------------------------------------*/
void Application::Initialize(std::unique_ptr<Scene> scene)
{
	assert(scene);

	// 初期シーンをセットして初期化する
	_mCurrentScene = std::move(scene);
	_mCurrentScene->Initialize();
}

/*------------------------------------------------------------
	終了
------------------------------------------------------------*/
void Application::Finalize()
{
	// 遷移先シーンが存在する（シーン遷移予約済み）場合は
	// 現在シーン終了→遷移先シーン初期化
	// アプリケーション終了時はそのまま現在シーンを終了
	if (_mNextScene) {
		if (_mCurrentScene) {
			_mCurrentScene->Finalize();
		}

		_mCurrentScene = std::move(_mNextScene);
		_mCurrentScene->Initialize();
	}
	else {
		if (_mCurrentScene) {
			_mCurrentScene->Finalize();
		}
	}
}

/*------------------------------------------------------------
	更新
------------------------------------------------------------*/
void Application::Update(double deltaTime)
{
	Transition::getInstance().Update(deltaTime);
	Input::Update();

	// 現在シーン更新処理
	if (_mCurrentScene) {
		_mCurrentScene->Update(deltaTime);
	}

	// _mNextSceneが存在（シーン遷移予約済み）ならシーン終了→次シーン初期化
	if (_mNextScene) {
		if (_mCurrentScene) {
			_mCurrentScene->Finalize();
		}

		_mCurrentScene.reset();

		_mCurrentScene = std::move(_mNextScene);

		_mCurrentScene->Initialize();

		// ロード中の累積時間をリセット
		System::Timer::getInstance().Refresh();
	}
}

/*------------------------------------------------------------
	描画
------------------------------------------------------------*/
void Application::Draw()
{
	D3D11::Graphics::getInstance().Begin();

	// 現在シーン描画処理
	if (_mCurrentScene) {
		_mCurrentScene->Draw();
	}

	// トランジションテクスチャを最後に描画
	Transition::getInstance().Draw();

	D3D11::Graphics::getInstance().End();
}