/*============================================================
*	@file	 : Scene.cpp
*	@brief	 : シーン基底クラス
*
* 　@author  : @akitsuki-35（https://github.com/akitsuki-35）
* 　@date	 : 2026/08/15
*	@updated : 2026/10/09
*============================================================*/
#include "Scene.h"
#include "DeviceManager.h"
#include "D3D11Config.h"
#include "Renderer.h"
#include "GameObject.h"
#include "Camera.h"
#include <array>
#include <algorithm>

void Scene::Finalize()
{
	for (const auto& obj : _mGameObjects) {
		obj->Finalize();
	}

	_mGameObjects.clear();
}

void Scene::Update(double deltaTime)
{
	// ポーズ中は処理しない
	if (mPause) return;

	for (const auto& obj : _mGameObjects) {
		obj->Update(deltaTime);
	}

	// ヒットストップ中はオブジェクトを削除しない
	if (!mHitStop) {
		// ゲームオブジェクト削除
		_mGameObjects.remove_if([](const auto& object) {
			return object->Destroy();
			});
	}
}

void Scene::Draw() const
{
	D3D11::DeviceManager::getInstance().SetSamplerState(D3D11::RenderState::Sampler::Anisotropic);

	// カメラ取得
	Camera* camera = GetGameObject<Camera>();

	// レイヤー別の描画キューを作成
	constexpr size_t LAYER = static_cast<size_t>(Layer::Count);
	std::array<std::vector<GameObject*>, LAYER> layerQueue{};

	// レイヤー別にオブジェクトを詰める
	for (const auto& obj : _mGameObjects) {
		if (auto* renderer = obj->GetComponent<Renderer>()) {
			size_t layer = static_cast<size_t>(renderer->GetSortKey().layer);
			layerQueue[layer].push_back(obj.get());
		}
	}
	
	// Zソート処理
	// 2Dオブジェクトには適用しない
	if (camera) {
		Vector3 forward = camera->GetForward();
		Vector3 position = camera->GetTransform().GetPosition();
		for (size_t layer = 0; layer < LAYER; layer++) {
			if (layer == static_cast<size_t>(Layer::UI)) {
				continue;
			}

			for (auto* obj : layerQueue[layer]) {
				auto* renderer = obj->GetComponent<Renderer>();
				renderer->CalcCameraZ(position, forward);
			}

			std::stable_sort(layerQueue[layer].begin(), layerQueue[layer].end(),
				[](GameObject* a, GameObject* b) {
					return a->GetComponent<Renderer>()->GetSortKey().Zdepth >
						b->GetComponent<Renderer>()->GetSortKey().Zdepth;});
		}

		// カメラ行列セット
		camera->SetMatrix();
	}

	// レイヤー順に描画
	for (size_t layer = 0; layer < LAYER; layer++) {
		if (layer == static_cast<size_t>(Layer::World)) {
			D3D11::DeviceManager::getInstance().SetDepthStencilState(D3D11::RenderState::Depth::Enable);
		}
		else if(layer == static_cast<size_t>(Layer::Alpha)) {
			D3D11::DeviceManager::getInstance().SetDepthStencilState(D3D11::RenderState::Depth::TestOnly);
		}
		else if (layer == static_cast<size_t>(Layer::UI)) {
			D3D11::DeviceManager::getInstance().SetDepthStencilState(D3D11::RenderState::Depth::Disable);
		}

		for (auto* obj : layerQueue[layer]) {
			obj->Draw();
		}

		// 深度ステートを有効に戻す
		D3D11::DeviceManager::getInstance().SetDepthStencilState(D3D11::RenderState::Depth::Enable);
	}
}