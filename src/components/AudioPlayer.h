/*============================================================
*	@file	 : AudioPlayer.h
*	@brief	 : オーディオ再生コンポーネント
*
* 　@author  : @akitsuki-35（https://github.com/akitsuki-35）
* 　@date	 : 2026/08/09
*	@updated : 2026/09/16
*============================================================*/
#pragma once

#include "Component.h"

/*--------------------------------------------------
	前方宣言
----------------------------------------------------*/
class Audio;
struct IXAudio2;
struct IXAudio2MasteringVoice;
struct IXAudio2SourceVoice;

/*============================================================
*	@class	: AudioPlayer
*	@brief	: オーディオ再生コンポーネント
*============================================================*/
class AudioPlayer : public Component
{
private:
	static inline IXAudio2* mXaudio{};
	static inline IXAudio2MasteringVoice* mMasteringVoice{};

	IXAudio2SourceVoice* mSourceVoice{ nullptr };
	Audio* _mAudio{ nullptr };

	// ボリューム
	float mVolume = { 1.0f };

public:
	// マスタリングボイス初期化・解放
	static void InitializeMaster();
	static void FinalizeMaster();

	using Component::Component;

	void Finalize() override;

	// オーディオ読み込み
	AudioPlayer* LoadAudio(const char* fileName);

	// 再生
	void Play(bool isLoop = false);

	// 一時停止
	void Pause();

	// 再開
	void Resume();

	// 停止
	void Stop();

	// ボリューム変更
	AudioPlayer* SetVolume(float Volume);
};

