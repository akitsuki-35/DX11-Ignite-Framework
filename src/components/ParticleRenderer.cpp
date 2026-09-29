/*============================================================
*	@file	 : ParticleRenderer.cpp
*	@brief	 : パーティクル描画コンポーネント
*
* 　@author  : @akitsuki-35（https://github.com/akitsuki-35）
* 　@date	 : 2026/08/15
*	@updated : 2026/09/16
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

using namespace DirectX;
using namespace MeshType;

ParticleRenderer::ParticleRenderer(GameObject* owner)
	: Renderer(owner) 
{
	// 不透明レイヤーに描画
	mSortKey.layer = Layer::Alpha;
	mMesh.CreatePlane(Plane::Pivot::Center, Plane::Axis::XY);
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

	// パラメータ設定
	D3D11::BufferManager::getInstance().SetParameter(mParameter);

	mainColorDraw(r);
	subColorDraw(r);

	Renderer::End();
}

DirectX::XMMATRIX ParticleRenderer::getWorldMatrix() const
{
	// ビルボード行列取得
	Camera* camera = Scene::GetGameObject<Camera>();
	return _mOwner->GetTransform().GetBillboardMatrix(camera->GetViewMatrix());
}

void ParticleRenderer::mainColorDraw(const DirectX::XMMATRIX& rotation) const
{
	// マテリアル設定
	Element::MATERIAL material{};
	material.Diffuse = mColor;
	material.TextureEnable = static_cast<bool>(_mTexture != nullptr);
	D3D11::BufferManager::getInstance().SetMaterial(material);

	mMesh.Bind();

	// 有効パーティクル個数
	for (int i = 0; i < _mEmitter->PARTICLE_MAX; i++) {
		if (_mEmitter->mParticles[i].mEnable) {

			// マトリクス設定
			XMMATRIX w{}, s{}, t{};

			// 拡大縮小	
			s = XMMatrixScaling(_mEmitter->mParticles[i].mScale.x, _mEmitter->mParticles[i].mScale.y,
				_mEmitter->mParticles[i].mScale.z);

			// 平行移動
			t = XMMatrixTranslation(_mEmitter->mParticles[i].mPosition.x,
				_mEmitter->mParticles[i].mPosition.y, _mEmitter->mParticles[i].mPosition.z);

			w = s * rotation * t;

			D3D11::BufferManager::getInstance().SetWorldMatrix(w);

			// 描画
			mMesh.Draw();
		}
	}
}

void ParticleRenderer::subColorDraw(const DirectX::XMMATRIX& rotation) const
{
	// マテリアル設定
	Element::MATERIAL material{};
	material.Diffuse = mSubColor;
	material.TextureEnable = static_cast<bool>(_mTexture != nullptr);
	D3D11::BufferManager::getInstance().SetMaterial(material);

	// 有効パーティクル個数
	for (int i = 0; i < _mEmitter->PARTICLE_MAX; i++) {
		if (_mEmitter->mParticles[i].mEnable) {

			// マトリクス設定
			XMMATRIX w{}, s{}, t{};

			// 拡大縮小	
			s = XMMatrixScaling(_mEmitter->mParticles[i].mScale.x - 0.5f, _mEmitter->mParticles[i].mScale.y - 0.5f,
				_mEmitter->mParticles[i].mScale.z - 0.5f);

			// 平行移動
			t = XMMatrixTranslation(_mEmitter->mParticles[i].mPosition.x,
				_mEmitter->mParticles[i].mPosition.y, _mEmitter->mParticles[i].mPosition.z);

			w = s * rotation * t;

			D3D11::BufferManager::getInstance().SetWorldMatrix(w);

			// 描画
			mMesh.Draw();
		}
	}
}

ParticleRenderer* ParticleRenderer::LoadTexture(const char* fileName, bool isMip)
{
	// テクスチャ読み込み
	_mTexture = TextureManager::getInstance().Load(fileName, isMip);
	return this;
}
