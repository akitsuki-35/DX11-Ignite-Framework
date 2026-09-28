/*============================================================
*	@file	 : ShaderManager.cpp
*	@brief	 : シェーダーリソース管理
*
* 　@author  : @akitsuki-35（https://github.com/akitsuki-35）
* 　@date	 : 2026/07/14
*	@updated : 2026/09/28
*============================================================*/
#include "ShaderManager.h"
#include "DeviceManager.h"
#include "FileUtility.h"
#include <d3d11.h>

using namespace Microsoft::WRL;

Shader* ShaderManager::Get(const std::string& keyName)
{
	// キャッシュが存在すれば返す
	auto it = mShaders.find(keyName);

	if (it != mShaders.end()) {
		return it->second.get();
	}

	return nullptr;
}

Shader* ShaderManager::Register(const std::string& keyName, const char* vsPath, const char* psPath)
{
	// 登録済みならreturn
	if (mShaders.contains(keyName)) {
		return mShaders[keyName].get();
	}

	// キャッシュ取得用にパスを正規化
	std::string vsKey = Utility::File::normalizePath(vsPath);
	std::string psKey = Utility::File::normalizePath(psPath);

	std::unique_ptr<Shader> shader = std::make_unique<Shader>();
	
	// 頂点シェーダー作成
	if (mVSCache.contains(vsKey)) {
		shader->_mVertexShader = mVSCache[vsKey];
		shader->_mLayout = mLayoutCache[vsKey];
	}
	else {
		auto vsBuffer = Utility::File::load(vsPath);

		D3D11::DeviceManager::getInstance().
			GetDevice()->CreateVertexShader(vsBuffer.data(), vsBuffer.size(), nullptr, &shader->_mVertexShader);

		// 頂点レイアウト作成
		D3D11_INPUT_ELEMENT_DESC layout[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 4 * 3, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 4 * 6, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 4 * 10, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "BLENDINDICES", 0, DXGI_FORMAT_R32G32B32A32_UINT, 0, 4 * 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "BLENDWEIGHT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 4 * 16, D3D11_INPUT_PER_VERTEX_DATA, 0 }
		};

		UINT numElements = ARRAYSIZE(layout);

		D3D11::DeviceManager::getInstance().
			GetDevice()->CreateInputLayout(layout, numElements,
				vsBuffer.data(), vsBuffer.size(), &shader->_mLayout);

		// キャッシュ登録
		mVSCache[vsKey] = shader->_mVertexShader;
		mLayoutCache[vsKey] = shader->_mLayout;

		// キャッシュからコンテナに登録
		shader->_mVertexShader = mVSCache[vsKey];
		shader->_mLayout = mLayoutCache[vsKey];
	}

	// ピクセルシェーダー作成
	if (mPSCache.contains(psKey)) {
		shader->_mPixelShader = mPSCache[psKey];
	}
	else {
		auto psBuffer = Utility::File::load(psPath);

		D3D11::DeviceManager::getInstance().
			GetDevice()->CreatePixelShader(psBuffer.data(), psBuffer.size(), nullptr, &shader->_mPixelShader);
	
		// キャッシュ登録
		mPSCache[psKey] = shader->_mPixelShader;

		// キャッシュからコンテナに登録
		shader->_mPixelShader = mPSCache[psKey];
	}

	// コンテナに登録
	mShaders[keyName] = std::move(shader);

	return mShaders[keyName].get();
}

void ShaderManager::Clear()
{
	mShaders.clear();
	mVSCache.clear();
	mPSCache.clear();
	mLayoutCache.clear();
}