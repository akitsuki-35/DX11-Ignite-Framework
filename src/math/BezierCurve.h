/*============================================================
*	@file	 : BezierCurve.h
*	@brief	 : ベジエ曲線
*
* 　@author  : @akitsuki-35（https://github.com/akitsuki-35）
* 　@date	 : 2026/07/09
*	@updated : 2026/09/30
*============================================================*/
#pragma once

#include "Vector3.h"
#include <array>
#include <vector>

/*============================================================
*	@class	: BezierCurve
*	@brief	: ベジエ曲線
*============================================================*/
class BezierCurve
{
private:
	// 曲線上の座標
	struct POINT {
		Vector3 Position{};
	};

	// 指定可能な最大フレーム(初期化・参照用)
	static constexpr int DEFAULT_FRAMEMAX{ 120 };

	int mFrameMax{}; // 全体フレーム
	int mFrame{}; // 現在フレーム

	// 制御点(三次ベジエ曲線)
	std::array<POINT, 4> mControlPoints{};

	// フレームごとのベジエ曲線上座標
	std::vector<POINT> mBezierPoint{};

public:
	BezierCurve();

	// 更新
	void Update();
	
	// ベジエ曲線上座標計算
	void CalcBezier();

	// 現在フレーム取得
	int GetFrame() const { return mFrame; }

	// 制御点座標取得
	Vector3& GetControlPoint(int index) { return mControlPoints[index].Position; }

	// フレーム指定でベジエ曲線上座標取得
	Vector3& GetBezierPoint(int index) { return mBezierPoint[index].Position; }

	// 制御点座標更新
	void SetControlPoint(int index, Vector3 position) {
		if (index > 4) return;
		mControlPoints[index].Position = position;
	}

	// 最大フレーム変更
	void SetFrameMax(const int& frameMax) {
		mFrameMax = frameMax;
	}
};