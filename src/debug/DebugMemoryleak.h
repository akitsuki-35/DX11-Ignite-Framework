/*============================================================
*	@file	 : DebugMemoryleak.h
*	@brief	 : メモリリークの検出
*
* 　@author  : @akitsuki-35（https://github.com/akitsuki-35）
* 　@date	 : 2026/04/11
*	@updated : 2026/11/05
*============================================================*/
#pragma once

// メモリリーク検出箇所を出力するマクロ
// 他ファイルにインクルードのみで導入可

// debugのみで動作
#if defined(DEBUG) || defined(_DEBUG)

#include <memory>

#define new ::new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif // defined(DEBUG) || defined(_DEBUG)