/*============================================================
*	@file	 : Renderer.h
*	@brief	 : 描画コンポーネント基底クラス
*
* 　@author  : @akitsuki-35（https://github.com/akitsuki-35）
* 　@date	 : 2026/07/27
*	@updated : 2026/11/05
*============================================================*/
#pragma once

#include "Component.h"
#include <string>
#include <DirectXMath.h>

/*------------------------------------------------------------
	前方宣言
------------------------------------------------------------*/
class Vector3;
class Shader;
class Instancing;

/*------------------------------------------------------------
	ブレンドステート
------------------------------------------------------------*/
enum class Blend : uint8_t
{
	Default,
	Add,
	Multiply
};

/*------------------------------------------------------------
	ソート用構造体
------------------------------------------------------------*/
// 描画レイヤー
enum class Layer : uint8_t
{
	Grid,
	World,
	Alpha,
	UI,

	Count
};

// 描画ソート用情報
struct SORTKEY
{
	// レイヤー
	Layer layer{};

	// Zバッファ
	float Zdepth{};

	// Zバッファによるソート
	bool operator<(const SORTKEY& key) const
	{
		if (layer != key.layer) {
			return layer < key.layer;
		}

		return Zdepth > key.Zdepth;
	}
};

/*============================================================
*	@class	: Renderer
*	@brief	: 描画コンポーネント基底クラス
*============================================================*/
class Renderer : public Component
{
protected:
	// シェーダー
	Shader* _mShader{ nullptr };

	// インスタンス情報の参照
	Instancing* _mInstancing{ nullptr };

	// カラー
	DirectX::XMFLOAT4 mColor{ 1.0f, 1.0f, 1.0f, 1.0f };

	// 汎用パラメータ
	// シェーダーで使用
	DirectX::XMFLOAT4 mParameter{ 0.0f, 0.0f, 0.0f, 0.0f };

	// ブレンドステート
	Blend mBlendState{};

	// ソート用情報
	mutable SORTKEY mSortKey{};

	virtual void Bind() const;

public:
	Renderer() {
		mBlendState = Blend::Default;
	}

	Renderer(GameObject* owner)
		: Component(owner){
		mBlendState = Blend::Default;
	}

	void Finalize() override;

	// 描画
	virtual void Draw() const = 0;

	// シェーダー読み込み
	Renderer* LoadShader(const std::string& keyName);

	// インスタンス描画設定
	Renderer* SetInstancing(Instancing* instancing);

	// インスタンス描画するか
	bool IsDrawInstanced() const;

	// インスタンス情報取得
	Instancing* GetInstancing() const { return _mInstancing; }

	// ブレンドステート指定
	Renderer* SetBlendState(const Blend& state);

	// レイヤー指定
	Renderer* SetLayer(const Layer& layer);

	// Zソート用
	virtual void CalcCameraZ(Vector3 cameraPosition, Vector3 cameraForward) const;
	void SetZdepth(float z) { mSortKey.Zdepth = z; }

	// ゲッター
	Shader* GetShader() const { return _mShader; }
	DirectX::XMFLOAT4 GetColor() const { return mColor; }
	SORTKEY& GetSortKey() const { return mSortKey; }

	// セッター
	Renderer* SetColor(const DirectX::XMFLOAT4 color);
	Renderer* SetParameter(const DirectX::XMFLOAT4 parameter);

private:
	// ワールド行列取得
	virtual DirectX::XMMATRIX getWorldMatrix() const = 0;

protected:
	void Begin() const;
	void End() const;
};