/*============================================================
*	@file	 : ParticleRenderer.h
*	@brief	 : パーティクル描画コンポーネント
*
* 　@author  : @akitsuki-35（https://github.com/akitsuki-35）
* 　@date	 : 2026/08/15
*	@updated : 2026/09/16
*============================================================*/
#pragma once

#include "Renderer.h"
#include "Mesh.h"

/*------------------------------------------------------------
	前方宣言
------------------------------------------------------------*/
class Texture;

/*============================================================
*	@class	: ParticleRenderer
*	@brief	: パーティクル描画コンポーネント
*============================================================*/
class ParticleRenderer : public Renderer
{
	friend class ParticleEmitter;

private:
	// エミッタ
	ParticleEmitter* _mEmitter{ nullptr };

	// メッシュ
	Mesh mMesh{};

	// テクスチャ
	Texture* _mTexture{ nullptr };

	// サブカラー
	DirectX::XMFLOAT4 mSubColor{ 1.0f, 1.0f, 1.0f, 1.0f };

public:
	ParticleRenderer(GameObject* owner);

	~ParticleRenderer() override = default;

	// エミッタのセット
	// コンポーネントのアタッチと同時に呼び出す
	ParticleRenderer* SetEmitter(ParticleEmitter* emitter) {
		_mEmitter = emitter;
		return this;
	}

	// 描画
	void Draw() const override;

	void Finalize() override {
		_mTexture = nullptr;
		Renderer::Finalize();
	}

private:
	// ワールド行列取得
	DirectX::XMMATRIX getWorldMatrix() const override;

	// メインカラー描画
	void mainColorDraw(const DirectX::XMMATRIX& rotation) const;

	// サブカラー描画
	void subColorDraw(const DirectX::XMMATRIX& rotation) const;

public:
	// テクスチャ読み込み
	ParticleRenderer* LoadTexture(const char* fileName, bool isMip = false);

	// ゲッター
	Mesh& GetMesh() { return mMesh; }
	Texture* GetTexture() const { return _mTexture; }

	// サブカラーのセット
	void SetSubColor(const DirectX::XMFLOAT4& color) { mSubColor = color; }
};