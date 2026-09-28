/*============================================================
*	@file	 : EasingUtility.cpp
*	@brief	 : イージング関連ユーティリティ
*
* 　@author  : @akitsuki-35（https://github.com/akitsuki-35）
* 　@date	 : 2026/09/28
*	@updated : 2026/09/28
*============================================================*/
#include "EasingUtility.h"
#include "Easing.h"

double Utility::Easing::CalculateRatio(double current, double duration)
{
	// イージング用ratio算出

	if (duration <= 0.0) return 1.0;

	double elapsed = duration - current;

	double ratio = elapsed / duration;

	return (ratio > 1.0) ? 1.0 : (ratio < 0.0) ? 0.0 : ratio;
}

float Utility::Easing::CalculateEase(double current, double duration, easing_functions easeType)
{
	// イージング用ease算出

	double ratio = CalculateRatio(current, duration);

	float ease = static_cast<float>(getEasingFunction(easeType)(ratio));

	return ease;
}
