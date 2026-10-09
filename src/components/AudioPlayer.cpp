/*============================================================
*	@file	 : AudioPlayer.cpp
*	@brief	 : オーディオ再生コンポーネント
*
* 　@author  : @akitsuki-35（https://github.com/akitsuki-35）
* 　@date	 : 2026/08/09
*	@updated : 2026/09/16
*============================================================*/
#include "AudioPlayer.h"
#include "AudioManager.h"
#include "Audio.h"
#include <cassert>
#include <algorithm>
#include <xaudio2.h>
#pragma comment (lib, "winmm.lib")

void AudioPlayer::InitializeMaster()
{
	// COM初期化
	HRESULT hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
	assert(SUCCEEDED(hr));

	// XAudio生成
	hr = XAudio2Create(&mXaudio, 0);
	assert(SUCCEEDED(hr));

	// マスタリングボイス生成
	hr = mXaudio->CreateMasteringVoice(&mMasteringVoice);
	assert(SUCCEEDED(hr));
}


void AudioPlayer::FinalizeMaster()
{
	// マスタリングボイス解放
	if (mMasteringVoice) {
		mMasteringVoice->DestroyVoice();
		mMasteringVoice = nullptr;
	}

	// XAudio解放
	if (mXaudio) {
		mXaudio->Release();
		mXaudio = nullptr;
	}

	// COM終了処理
	CoUninitialize();
}

void AudioPlayer::Finalize()
{
	if (mSourceVoice) {
		// 再生停止
		mSourceVoice->Stop();

		// バッファ解放
		mSourceVoice->FlushSourceBuffers();
		
		// ボイス破棄
		mSourceVoice->DestroyVoice();
		
		// nullptrで上書き
		mSourceVoice = nullptr;
	}

	_mAudio = nullptr;
}

AudioPlayer* AudioPlayer::LoadAudio(const char* fileName)
{
	// オーディオファイルのロード
	_mAudio = AudioManager::getInstance().Load(fileName);
	assert(_mAudio);

	// サウンドソース作成
	HRESULT hr = mXaudio->CreateSourceVoice(&mSourceVoice, _mAudio->GetFormat());

	if (FAILED(hr)) {
		return nullptr;
	}

	return this;
}

void AudioPlayer::Play(bool isLoop)
{
	if (!_mAudio || !mSourceVoice) {
		return;
	}

	// バッファのクリア
	mSourceVoice->Stop();
	mSourceVoice->FlushSourceBuffers();

	// バッファ設定
	XAUDIO2_BUFFER buf{};
	buf.AudioBytes = _mAudio->GetAudioBytes(); // PCM総バイト数
	buf.pAudioData = reinterpret_cast<const BYTE*>(_mAudio->GetPCM()); // PCMデータ先頭
	buf.PlayBegin = 0;
	buf.PlayLength = _mAudio->GetSamples(); // 先頭サンプル数

	// ループタグが存在する場合は優先
	if (isLoop && _mAudio->IsLoopTag()) {
		buf.LoopBegin = _mAudio->GetLoopStart();
		buf.LoopLength = _mAudio->GetLoopEnd() - _mAudio->GetLoopStart();
		buf.LoopCount = XAUDIO2_LOOP_INFINITE;
	}
	else if (isLoop) {
		// 通常ループ
		buf.LoopBegin = 0;
		buf.LoopLength = _mAudio->GetSamples();
		buf.LoopCount = XAUDIO2_LOOP_INFINITE;
	}

	// バッファ登録
	mSourceVoice->SubmitSourceBuffer(&buf);
	
	// 再生
	mSourceVoice->Start();
}

void AudioPlayer::Pause()
{
	// 一時停止
	if (mSourceVoice) {
		mSourceVoice->Stop();
	}
}

void AudioPlayer::Resume()
{
	// 再開
	if (mSourceVoice) {
		mSourceVoice->Start();
	}
}

void AudioPlayer::Stop()
{
	// 再生停止
	if (mSourceVoice) {
		mSourceVoice->Stop();
		mSourceVoice->FlushSourceBuffers();
	}
}

AudioPlayer* AudioPlayer::SetVolume(float Volume)
{
	// 0.0f～1.0f間で補間
	mVolume = std::clamp(Volume, 0.0f, 1.0f);
	
	// ボリューム変更
	if (mSourceVoice) {
		mSourceVoice->SetVolume(mVolume);
	}

	return this;
}