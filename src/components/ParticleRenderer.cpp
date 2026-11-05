/*============================================================
*	@file	 : ParticleRenderer.cpp
*	@brief	 : パーティクル描画コンポーネント
*
* 　@author  : @akitsuki-35（https://github.com/akitsuki-35）
* 　@date	 : 2026/08/15
*	@updated : 2026/11/05
*============================================================*/
#include "ParticleRenderer.h"
#include "ParticleEmitter.h"
#include "BufferManager.h"
#include "MeshTypes.h"
#include "Texture.h"
#include "TextureManager.h"
#include "Elements.h"
#include "Camera.h"
#include "Scene.h"
#include "Instancing.h"

using namespace DirectX;
using namespace MeshType;

ParticleRenderer::ParticleRenderer(GameObject* owner)
	: Renderer(owner) 
{
	// 不透明レイヤーに描画
	mSortKey.layer = Layer::Alpha;
	mMesh.CreatePlane(Plane::Pivot::Center, Plane::Axis::XY);
}

ParticleRenderer* ParticleRenderer::SetEmitter(ParticleEmitter* emitter)
{
	_mEmitter = emitter;

	int particleMax{};
	if (_mEmitter) {
		particleMax = _mEmitter->GetParticleMax();
	}

	const uint32_t capacity = static_cast<uint32_t>(particleMax);

	// CPU側配列の再確保を抑制
	mMainInstances.reserve(capacity);
	mSubInstances.reserve(capacity);

	// バッファ側にパーティクル分の領域を確保
	mMainInstanceBuffer.Create(capacity);
	mSubInstanceBuffer.Create(capacity);

	return this;
}

void ParticleRenderer::Draw() const
{
	if (!_mEmitter || !_mTexture) {
		return;
	}

	Camera* camera = Scene::GetGameObject<Camera>();

	if (!camera) {
		return;
	}

	// 有効パーティクルのインスタンスを作成してバッファに転送
	updateInstanceData();

	// 有効パーティクルが存在しない場合は描画しない
	if (mMainInstances.empty()) {
		return;
	}

	// カメラ距離計算
	CalcCameraZ(camera->GetPosition(), camera->GetForward());

	Renderer::Begin();

	Bind();
	_mTexture->Bind();

	D3D11::BufferManager::getInstance().SetWorldMatrix(_mEmitter->GetTransform().GetWorldMatrix());

	// ビュー行列取得
	XMMATRIX view = camera->GetViewMatrix();

	// ビルボード用回転行列取得
	XMMATRIX r = _mEmitter->GetTransform().createBillboardRotation(view);

	// ワールド行列設定
	D3D11::BufferManager::getInstance().SetWorldMatrix(r);

	// パラメータ設定
	D3D11::BufferManager::getInstance().SetParameter(mParameter);

	mainColorDraw();
	subColorDraw();

	Renderer::End();
}

void ParticleRenderer::Finalize()
{
	_mEmitter = nullptr;
	_mTexture = nullptr;

	mMainInstances.clear();
	mSubInstances.clear();

	mMainInstanceBuffer.Release();
	mSubInstanceBuffer.Release();

	Renderer::Finalize();
}

DirectX::XMMATRIX ParticleRenderer::getWorldMatrix() const
{
	// ビルボード行列取得
	Camera* camera = Scene::GetGameObject<Camera>();
	return _mOwner->GetTransform().GetBillboardMatrix(camera->GetViewMatrix());
}

void ParticleRenderer::updateInstanceData() const
{
	mMainInstances.clear();
	mSubInstances.clear();

	if (!_mEmitter) {
		return;
	}

	// 有効なParticleだけを描画対象とする
	for (const Particle& particle : _mEmitter->GetParticles()) {
		if (!particle.mEnable) {
			continue;
		}

		// メインカラーのインスタンス
		Element::InstanceData mainData{};
		mainData.Position = particle.mPosition.ConvertToXMFLOAT3();
		mainData.Scale = particle.mScale.ConvertToXMFLOAT3();
		mainData.RotationY = 0.0f;
		mMainInstances.push_back(mainData);

		// サブカラーのインスタンス
		Element::InstanceData subData{};
		subData.Position = particle.mPosition.ConvertToXMFLOAT3();
		subData.Scale = { particle.mScale.x - 0.5f, particle.mScale.y - 0.5f, particle.mScale.z - 0.5f };
		subData.RotationY = 0.0f;
		mSubInstances.push_back(subData);
	}

	// 有効Particleが存在しない場合はバッファの有効要素数を0にする
	if (mMainInstances.empty()) {
		mMainInstanceBuffer.Update(mMainInstances);
		mSubInstanceBuffer.Update(mSubInstances);
		return;
	}

	// インスタンスデータをGPUに転送
	mMainInstanceBuffer.Update(mMainInstances);
	mSubInstanceBuffer.Update(mSubInstances);
}

void ParticleRenderer::mainColorDraw() const
{
	if (mMainInstances.empty()) {
		return;
	}

	// マテリアル設定
	Element::MATERIAL material{};
	material.Diffuse = mColor;
	material.TextureEnable = static_cast<bool>(_mTexture != nullptr);
	D3D11::BufferManager::getInstance().SetMaterial(material);

	mMesh.Bind();

	// インスタンスデータを設定
	mMainInstanceBuffer.BindVS(Instancing::BUFFER_SLOT);

	// 有効パーティクル分描画
	mMesh.DrawInstanced(static_cast<uint32_t>(mMainInstances.size()));

	// SRVをクリア
	mMainInstanceBuffer.UnbindVS(Instancing::BUFFER_SLOT);
}

void ParticleRenderer::subColorDraw() const
{
	if (mSubInstances.empty()) {
		return;
	}

	// マテリアル設定
	Element::MATERIAL material{};
	material.Diffuse = mSubColor;
	material.TextureEnable = static_cast<bool>(_mTexture != nullptr);
	D3D11::BufferManager::getInstance().SetMaterial(material);

	// インスタンスデータを設定
	mSubInstanceBuffer.BindVS(Instancing::BUFFER_SLOT);

	// 有効パーティクル分描画
	mMesh.DrawInstanced(static_cast<uint32_t>(mSubInstances.size()));

	// SRVをクリア
	mSubInstanceBuffer.UnbindVS(Instancing::BUFFER_SLOT);
}

ParticleRenderer* ParticleRenderer::LoadTexture(const char* fileName, bool isMip)
{
	// テクスチャ読み込み
	_mTexture = TextureManager::getInstance().Load(fileName, isMip);
	return this;
}
