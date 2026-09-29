/*============================================================
*	@file	 : ParticleEmitter.cpp
*	@brief	 : パーティクルエミッタ
*
* 　@author  : @akitsuki-35（https://github.com/akitsuki-35）
* 　@date	 : 2026/06/18
*	@updated : 2026/09/16
*============================================================*/
#include "ParticleEmitter.h"
#include "ParticleRenderer.h"
#include "ParticleBox.h"
#include "MeshTypes.h"
#include "Timer.h"
#include "Scene.h"

using namespace MeshType;
using namespace DirectX;

void ParticleEmitter::Initialize()
{
	// レンダラーにエミッタ本体をセット
	ParticleRenderer* renderer = AddComponent<ParticleRenderer>(this)->SetEmitter(this);

	// パーティクル用テクスチャ読み込み
	renderer->LoadTexture("assets\\textures\\particle.png")->LoadShader("Unlit")
		->SetBlendState(Blend::Add)->SetLayer(Layer::Alpha);

	// 構造体初期化
	mParticles.resize(PARTICLE_MAX);
	for (int i = 0; i < PARTICLE_MAX; i++) {
		mParticles[i].mEnable = false;
	}

	// 基本的に外部からCSVファイルで指定するため、仮にボックスで初期化
	_mType = std::make_unique<ParticleType::Box>(this);
}

void ParticleEmitter::Finalize()
{
	GameObject::Finalize();
}

void ParticleEmitter::Update(double deltaTime)
{
	if (Scene::IsHitStop() || Scene::IsPause()) return;

	_mType->Update(deltaTime);

	// インターバル毎にパーティクル発射
	mCurrentInterval -= deltaTime;

	if (mCurrentInterval <= 0.0) {
		_mType->Emission(mDesc);
		mCurrentInterval = mMaxInterval;
	}

	// ループが無効の場合は徐々に透明にする
	if (!mLoop) {
		alphaUpdate();

		if (_mEmitterLife->IsTimeUp()) {
			SetDestroy();
		}
	}

	GameObject::Update(deltaTime);
}

void ParticleEmitter::Draw() const
{
	GameObject::Draw();
}

ParticleEmitter* ParticleEmitter::LoadCSV(const char* filePath)
{
	// CSVファイル読み込み
	auto newType =_mType->LoadCSV(filePath);

	// タイプを最後にセットする
	if (newType) {
		this->SetType(std::move(newType));
	}
	return this;
}

ParticleEmitter* ParticleEmitter::SetEmitterLife(double lifeTime)
{
	// エミッタの寿命（再生時間）をセット
	_mEmitterLife = AddComponent<Timer>(this);
	_mEmitterLife->Start(lifeTime);

	// ループしない設定にする
	mLoop = false;

	return this;
}

void ParticleEmitter::alphaUpdate()
{
	// エミッタ寿命に応じて透明度を変更
	auto renderer = GetComponent<ParticleRenderer>();
	float alpha = _mEmitterLife->GetProgress();

	// 値をクランプ
	if (alpha > 1.0f) alpha = 1.0f;
	if (alpha < 0.0f) alpha = 0.0f;

	// メインカラー透明度更新
	DirectX::XMFLOAT4 color = renderer->mColor;
	color.w = alpha;
	renderer->mColor = color;

	// サブカラー透明度更新
	color = renderer->mSubColor;
	color.w = alpha;
	renderer->mSubColor = color;
}