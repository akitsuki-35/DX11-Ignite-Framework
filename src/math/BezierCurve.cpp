/*============================================================
*	@file	 : BezierCurve.cpp
*	@brief	 : ベジエ曲線
*
* 　@author  : @akitsuki-35（https://github.com/akitsuki-35）
* 　@date	 : 2026/07/09
*	@updated : 2026/09/30
*============================================================*/
#include "BezierCurve.h"

BezierCurve::BezierCurve()
{
	// 制御点初期化
	mControlPoints[0].Position = { -15.0f, 3.0f, -15.0f };
	mControlPoints[1].Position = { -30.0f, 3.0f, -30.0f };
	mControlPoints[2].Position = { 30.0f, 3.0f, 30.0f };
	mControlPoints[3].Position = { 15.0f, 3.0f, 15.0f };

	// フレーム0から開始
	mFrame = 0;

	// 最大フレーム定数で初期化(配列外参照防止)
	mFrameMax = DEFAULT_FRAMEMAX;
	mBezierPoint.resize(mFrameMax);

	// 曲線上座標初期化
	CalcBezier();
}

void BezierCurve::Update()
{
	// フレームをループさせる
	mFrame++;
	if (mFrame >= mFrameMax) {
		mFrame = 0;
	}
}

void BezierCurve::CalcBezier()
{
	double t{};

	// 全フレームのベジエ曲線上のポイント計算
	for (int k = 0; k < mFrameMax; k++) {
		t = static_cast<double>(k) / static_cast<double>(mFrameMax - 1);

		double b0 = (1.0 - t) * (1.0 - t) * (1.0 - t);
		double b1 = 3.0 * (1.0 - t) * (1.0 - t) * t;
		double b2 = 3.0 * (1.0 - t) * t * t;
		double b3 = t * t * t;

		// x座標算出
		mBezierPoint[k].Position.x = (static_cast<float>(b0) * mControlPoints[0].Position.x + 
			static_cast<float>(b1) * mControlPoints[1].Position.x + 
			static_cast<float>(b2) * mControlPoints[2].Position.x + 
			static_cast<float>(b3) * mControlPoints[3].Position.x);

		// y座標算出
		mBezierPoint[k].Position.y = (static_cast<float>(b0) * mControlPoints[0].Position.y + 
			static_cast<float>(b1) * mControlPoints[1].Position.y + 
			static_cast<float>(b2) * mControlPoints[2].Position.y + 
			static_cast<float>(b3) * mControlPoints[3].Position.y);

		// z座標算出
		mBezierPoint[k].Position.z = (static_cast<float>(b0) * mControlPoints[0].Position.z + 
			static_cast<float>(b1) * mControlPoints[1].Position.z + 
			static_cast<float>(b2) * mControlPoints[2].Position.z + 
			static_cast<float>(b3) * mControlPoints[3].Position.z);
	}
}
