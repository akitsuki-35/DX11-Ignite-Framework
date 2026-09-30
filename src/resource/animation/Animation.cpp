/*============================================================
*	@file	 : Animation.cpp
*	@brief	 : アニメーションクリップ
*
* 　@author  : @akitsuki-35（https://github.com/akitsuki-35）
* 　@date	 : 2026/08/13
*	@updated : 2026/09/30
*============================================================*/
#include "Animation.h"

void Animation::AddChannel(const CHANNEL& channel)
{
	// 新規チャンネル追加
	mChannels.push_back(channel);
}