/*============================================================
*	@file	 : EasingUtility.h
*	@brief	 : イージング関連ユーティリティ
*
* 　@author  : @akitsuki-35（https://github.com/akitsuki-35）
* 　@date	 : 2026/09/28
*	@updated : 2026/11/18
*============================================================*/
#pragma once

#include "Easing.h"

/*============================================================
*	@namespace	: Utility::Easing
*	@brief		: イージング関連ユーティリティ関数群
*============================================================*/
namespace Utility {
	namespace Easing {
		// ratio算出
		double CalculateRatio(double current, double duration);

		// ease算出
		float CalculateEase(double current, double duration, easing_functions easeType);
	}
}