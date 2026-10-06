/*============================================================
*	@file	 : AudioManager.cpp
*	@brief	 : オーディオ管理
*
* 　@author  : @akitsuki-35（https://github.com/akitsuki-35）
* 　@date	 : 2026/08/09
*	@updated : 2026/10/06
*============================================================*/
#include "AudioManager.h"
#include "Audio.h"
#include "FileUtility.h"
#include <cassert>

// minimp3
#define MINIMP3_IMPLEMENTATION
#include "minimp3/minimp3.h"
#define MINIMP3_EX_IMPLEMENTATION
#include "minimp3/minimp3_ex.h"

// stb_vorbis
#pragma warning(push, 0)
#define STB_VORBIS_HEADER_ONLY
#include "stb_vorbis/stb_vorbis.c"
#pragma warning(pop)

Audio* AudioManager::Load(const char* audioPath)
{
	// キャッシュ取得用にパスを正規化
	std::string key = Utility::File::normalizePath(audioPath);

	// キャッシュが存在すれば返す
	auto it = mAudios.find(key);

	if (it != mAudios.end()) {
		return it->second.get();
	}

	// オーディオリソース生成
	std::unique_ptr<Audio> audio = std::make_unique<Audio>();

	if (!generateAudio(*audio, key)) {
		return nullptr;
	}

	// オーディオを一時変数に格納
	Audio* a = audio.get();

	// コンテナへ登録
	mAudios.emplace(key, std::move(audio));

    return a;
}

void AudioManager::Clear()
{
	mAudios.clear();
}

bool AudioManager::generateAudio(Audio& audio, const std::string& path)
{
	// ファイル拡張子を取得
	std::string ext = Utility::File::getFileExtension(path);

	// ファイル拡張子からオーディオ形式を取得
	AudioType type = getType(ext);

	// オーディオ形式に応じたロード処理
	switch (type)
	{
	case AudioManager::AudioType::WAV:
		return loadWav(audio, path);
		break;
	
	case AudioManager::AudioType::MP3:
		return loadMp3(audio, path);
		break;

	case AudioManager::AudioType::OGG:
		return loadOgg(audio, path);
		break;

	default:
		break;
	}

	return false;
}

AudioManager::AudioType AudioManager::getType(const std::string& ext)
{
	// ファイル拡張子からオーディオ形式を取得
	if (ext == "wav") {
		return AudioType::WAV;
	}
	else if (ext == "mp3") {
		return AudioType::MP3;
	}
	else if (ext == "ogg") {
		return AudioType::OGG;
	}

	return AudioType();
}

bool AudioManager::loadWav(Audio& audio, const std::string& path)
{
	// wavファイル読み込み

	HMMIO hmmio{ nullptr };
	MMIOINFO mmioInfo = {};
	MMCKINFO riffChunk = {}; // RIFFチャンク用
	MMCKINFO dataChunk = {}; // 音声データチャンク用
	MMCKINFO fmtChunk = {}; // フォーマットチャンク用

	// ファイルを読み取り専用でオープン
	hmmio = mmioOpen((LPSTR)path.c_str(), &mmioInfo, MMIO_READ);
	if (!hmmio) { 
		return false;
	}

	// RIFFからWAVEを検索
	riffChunk.fccType = mmioFOURCC('W', 'A', 'V', 'E');
	mmioDescend(hmmio, &riffChunk, NULL, MMIO_FINDRIFF);

	// RIFFからフォーマット情報を検索
	fmtChunk.ckid = mmioFOURCC('f', 'm', 't', ' ');
	mmioDescend(hmmio, &fmtChunk, &riffChunk, MMIO_FINDCHUNK);

	// フォーマット情報読み込み
	if (fmtChunk.cksize >= sizeof(WAVEFORMATEX)) {
		mmioRead(hmmio, (HPSTR)&audio.mFormat, sizeof(audio.GetFormat()));
	}
	else {
		PCMWAVEFORMAT pcmwf = { 0 };
		mmioRead(hmmio, (HPSTR)&pcmwf, sizeof(pcmwf));
		memset(&audio.mFormat, 0, sizeof(audio.mFormat));
		memcpy(&audio.mFormat, &pcmwf, sizeof(pcmwf));
		audio.mFormat.cbSize = 0;
	}

	mmioAscend(hmmio, &fmtChunk, 0);

	// データチャンクを検索
	dataChunk.ckid = mmioFOURCC('d', 'a', 't', 'a');
	mmioDescend(hmmio, &dataChunk, &riffChunk, MMIO_FINDCHUNK);

	// PCMデータ読み込み
	audio.mBytes = dataChunk.cksize;
	audio.mPCM.resize(audio.mBytes);

	// 波形をバッファへ読み込み
	LONG readlen = mmioRead(hmmio, (HPSTR)audio.mPCM.data(), audio.mBytes);

	// サンプル数算出
	audio.mSamples = readlen / audio.mFormat.nBlockAlign;

	// ファイルを閉じる
	mmioClose(hmmio, 0);

	return true;
}

bool AudioManager::loadMp3(Audio& audio, const std::string& path)
{
	// wavファイル読み込み

	// mp3デコーダ初期化
	mp3dec_t mp3d{};
	mp3dec_init(&mp3d);

	// デコード
	mp3dec_file_info_t info{};
	int ret = mp3dec_load(&mp3d, path.c_str(), &info, nullptr, nullptr);

	if (ret != 0 || info.buffer == nullptr) {
		return false;
	}

	// PCMをコピー
	audio.mPCM.resize(info.samples * sizeof(short));
	memcpy(audio.mPCM.data(), info.buffer, audio.mPCM.size());

	// フォーマット情報設定
	audio.mFormat.wFormatTag = WAVE_FORMAT_PCM; // PCM固定
	audio.mFormat.nChannels = static_cast<WORD>(info.channels); // チャンネル数
	audio.mFormat.nSamplesPerSec = info.hz; // サンプリングレート
	audio.mFormat.wBitsPerSample = 16; // 16bitPCM
	audio.mFormat.nBlockAlign = (audio.mFormat.nChannels * audio.mFormat.wBitsPerSample) / 8;
	audio.mFormat.nAvgBytesPerSec = audio.mFormat.nSamplesPerSec * audio.mFormat.nBlockAlign;
	audio.mFormat.cbSize = 0; // 拡張なし

	// 総バイト数を取得
	audio.mBytes = (UINT)audio.mPCM.size();
	
	// 総サンプル数算出
	audio.mSamples = audio.mBytes / audio.mFormat.nBlockAlign;

	// PCMバッファを解放
	free(info.buffer);

	return true;
}

bool AudioManager::loadOgg(Audio& audio, const std::string& path)
{
	// oggファイル読み込み

	int error = 0;

	// ogg読み込み
	stb_vorbis* vorbis = stb_vorbis_open_filename(path.c_str(), &error, nullptr);
	if (!vorbis) {
		return false;
	}

	// OGG情報
	stb_vorbis_info info = stb_vorbis_get_info(vorbis);

	// メタデータ取得
	stb_vorbis_comment vc = stb_vorbis_get_comment(vorbis);

	int loopStart = -1;
	int loopEnd = -1;

	// oggループタグ取得
	for (int i = 0; i < vc.comment_list_length; i++) {
		std::string c = vc.comment_list[i];

		// ループタグスタート位置
		if (c.rfind("LOOPSTART=", 0) == 0) {
			loopStart = std::stoi(c.substr(10));
		}

		// ループタグ終了位置
		if (c.rfind("LOOPEND=", 0) == 0) {
			loopEnd = std::stoi(c.substr(8));
		}

		// ループ長(LOOPENDが存在しない場合)
		if (c.rfind("LOOPLENGTH=", 0) == 0) {
			int length = std::stoi(c.substr(11));
			if (loopStart >= 0) {
				loopEnd = loopStart + length;
			}
		}
	}

	// 一括デコード
	int total = stb_vorbis_stream_length_in_samples(vorbis) * info.channels;

	audio.mPCM.resize(total * sizeof(short));

	stb_vorbis_get_samples_short_interleaved(vorbis, info.channels, 
		reinterpret_cast<short*>(audio.mPCM.data()), total
	);

	stb_vorbis_close(vorbis);

	// フォーマット設定
	audio.mFormat.wFormatTag = WAVE_FORMAT_PCM; // PCM固定
	audio.mFormat.nChannels = static_cast<WORD>(info.channels); // チャンネル数
	audio.mFormat.nSamplesPerSec = info.sample_rate; // サンプリングレート
	audio.mFormat.wBitsPerSample = 16; // 16bitPCM
	audio.mFormat.nBlockAlign = static_cast<WORD>((info.channels * 16) / 8);
	audio.mFormat.nAvgBytesPerSec = audio.mFormat.nSamplesPerSec * audio.mFormat.nBlockAlign;
	audio.mFormat.cbSize = 0; // 拡張なし

	// 総バイト数を取得
	audio.mBytes = static_cast<UINT>(audio.mPCM.size());

	// サンプル数算出
	audio.mSamples = audio.mBytes / audio.mFormat.nBlockAlign;

	// ループタグ保存
	audio.mLoopStart = loopStart;
	audio.mLoopEnd = loopEnd;

	return true;
}
