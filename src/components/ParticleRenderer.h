/*============================================================
*	@file	 : ParticleRenderer.h
*	@brief	 : パーティクル描画コンポーネント
*
* 　@author  : @akitsuki-35（https://github.com/akitsuki-35）
* 　@date	 : 2026/08/15
*	@updated : 2026/11/05
*============================================================*/
#pragma once

#include "Renderer.h"
#include "Mesh.h"
#include "Elements.h"
#include "StructuredBuffer.h"
#include <vector>

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

	// メインカラー描画用インスタンス
	mutable std::vector<Element::InstanceData> mMainInstances{};

	// サブカラー描画用インスタンス
	mutable std::vector<Element::InstanceData> mSubInstances{};

	// メインカラー描画用ストラクチャードバッファ
	mutable D3D11::StructuredBuffer<Element::InstanceData> mMainInstanceBuffer{};

	// サブカラー描画用ストラクチャードバッファ
	mutable D3D11::StructuredBuffer<Element::InstanceData> mSubInstanceBuffer{};

public:
	ParticleRenderer(GameObject* owner);

	~ParticleRenderer() override = default;

	// エミッタのセット
	// コンポーネントのアタッチと同時に呼び出す
	ParticleRenderer* SetEmitter(ParticleEmitter* emitter);

	// 描画
	void Draw() const override;

	void Finalize() override;

private:
	// ワールド行列取得
	DirectX::XMMATRIX getWorldMatrix() const override;

	// インスタンスデータ生成
	void updateInstanceData() const;

	// メインカラー描画
	void mainColorDraw() const;

	// サブカラー描画
	void subColorDraw() const;

public:
	// テクスチャ読み込み
	ParticleRenderer* LoadTexture(const char* fileName, bool isMip = false);

	// ゲッター
	Mesh& GetMesh() { return mMesh; }
	Texture* GetTexture() const { return _mTexture; }

	// サブカラーのセット
	void SetSubColor(const DirectX::XMFLOAT4& color) { mSubColor = color; }
};