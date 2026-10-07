/*============================================================
*	@file	 : Application.cpp
*	@brief	 : アプリケーション内部処理
*
* 　@author  : @akitsuki-35（https://github.com/akitsuki-35）
* 　@date	 : 2026/04/21
*	@updated : 2026/10/06
*============================================================*/
#include "Application.h"
#include "Scene.h"
#include "SystemTimer.h"
#include "Graphics.h"
#include "Transition.h"
#include "Keyboard.h"
#include "GamePad.h"
#include "AudioPlayer.h"
#include <cassert>

/*------------------------------------------------------------
	初期化
------------------------------------------------------------*/
void Application::Initialize(std::unique_ptr<Scene> scene)
{
	assert(scene);

	_mNextScene = std::move(scene);

	_mCurrentScene = std::move(_mNextScene);
	_mCurrentScene->Initialize();
}

/*------------------------------------------------------------
	終了
------------------------------------------------------------*/
void Application::Finalize()
{
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
	Keyboard::Update();
	GamePad::Update();

	// 現在シーン更新
	if (_mCurrentScene) {
		_mCurrentScene->Update(deltaTime);
	}

	// シーン遷移
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

	// 現在シーン描画
	if (_mCurrentScene) {
		_mCurrentScene->Draw();
	}

	// トランジションテクスチャを最後に描画
	Transition::getInstance().Draw();

	D3D11::Graphics::getInstance().End();
}