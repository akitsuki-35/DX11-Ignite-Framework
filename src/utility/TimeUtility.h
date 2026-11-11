/*============================================================
*	@file	 : TimeUtility.h
*	@brief	 : タイム関連ユーティリティ
*
* 　@author  : @akitsuki-35（https://github.com/akitsuki-35）
* 　@date	 : 2026/11/09
*	@updated : 2026/11/10
*============================================================*/
#pragma once

/*============================================================
*	@namespace	: Utility::Time
*	@brief		: タイム関連ユーティリティ関数群
*============================================================*/
namespace Utility {
	namespace Time {
		// フレーム→秒に変換
		double toSeconds(int frame);

		// 秒→フレームに変換
		int toFrame(double seconds);
	}
}