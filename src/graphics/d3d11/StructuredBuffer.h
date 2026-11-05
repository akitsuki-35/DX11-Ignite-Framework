/*============================================================
*	@file	 : StructuredBuffer.h
*	@brief	 : 汎用ストラクチャードバッファ
*
* 　@author  : @akitsuki-35（https://github.com/akitsuki-35）
* 　@date	 : 2026/11/05
*	@updated : 2026/11/05
*============================================================*/
#pragma once
#include "DeviceManager.h"
#include <d3d11.h>
#include <wrl/client.h>
#include <cstdint>
#include <limits>
#include <span>
#include <type_traits>
#include <cstring>

/*============================================================
*	@class	: D3D11::StructuredBuffer
*	@brief	: 汎用ストラクチャードバッファ
*============================================================*/
namespace D3D11{

    template<typename T>
    class StructuredBuffer
    {
        // GPUへmemcpyでコピーできない型を弾く
        static_assert(std::is_trivially_copyable_v<T>, "StructuredBuffer requires a trivially copyable type.");

    private:
        template<typename U>
        using ComPtr = Microsoft::WRL::ComPtr<U>;

        // データ本体を保持するGPUバッファ
        ComPtr<ID3D11Buffer> mBuffer{};

        // シェーダーからの参照用SRV
        ComPtr<ID3D11ShaderResourceView> mSRV{};

        // 最大要素数
        uint32_t mCapacity{};

        // バッファに格納されている要素数
        uint32_t mCount{};

    public:
        StructuredBuffer() = default;
        ~StructuredBuffer() = default;

        StructuredBuffer(const StructuredBuffer&) = delete;
        StructuredBuffer& operator=(const StructuredBuffer&) = delete;

        StructuredBuffer(StructuredBuffer&&) noexcept = default;
        StructuredBuffer& operator=(StructuredBuffer&&) noexcept = default;

        // StructuredBuffer生成
        bool Create(uint32_t capacity, std::span<const T> initialData = {}) {
            // 既存リソース解放（再生成に対応）
            Release();

            // 要素数が0または最大数を超えるなら弾く
            if (capacity == 0) { return false; }

            if (initialData.size() > capacity) { return false; }

            // StructuredBufferにおける1要素のサイズ
            constexpr size_t stride = sizeof(T);

            // D3D11_BUFFER_DESC::ByteWidthはUINTのため、オーバーフローを抑制する
            constexpr size_t UINT_MAX_SIZE = static_cast<size_t>(std::numeric_limits<UINT>::max());

            if (static_cast<size_t>(capacity) > UINT_MAX_SIZE / stride) { return false; }

            ID3D11Device* device = DeviceManager::getInstance().GetDevice();

            if (!device) { return false; }

            // GPUバッファ生成
            D3D11_BUFFER_DESC bufferDesc{};
            bufferDesc.Usage = D3D11_USAGE_DYNAMIC; // map・ummapで書き換え可能にする
            bufferDesc.ByteWidth = static_cast<UINT>(stride * capacity);
            bufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
            bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE; // CPUから書き込み可能にする
            bufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
            bufferDesc.StructureByteStride = static_cast<UINT>(stride);

            // 初期データ設定
            D3D11_SUBRESOURCE_DATA subresourceData{};
            D3D11_SUBRESOURCE_DATA* subresource = nullptr;

            // 初期データが指定されている場合のみCreateBufferと同時にGPUへ転送
            if (!initialData.empty()) {
                subresourceData.pSysMem = initialData.data();
                subresource = &subresourceData;
            }

            HRESULT result = device->CreateBuffer(&bufferDesc, subresource, mBuffer.GetAddressOf());

            if (FAILED(result)) {
                Release();
                return false;
            }

            // SRV生成
            D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc{};
            srvDesc.Format = DXGI_FORMAT_UNKNOWN; // StructuredBufferでは個々の要素形式を指定しない
            srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
            srvDesc.Buffer.FirstElement = 0;
            srvDesc.Buffer.NumElements = capacity;

            result = device->CreateShaderResourceView(mBuffer.Get(), &srvDesc, mSRV.GetAddressOf());

            if (FAILED(result)) {
                Release();
                return false;
            }

            mCapacity = capacity;

            // 初期データが存在する場合のみ有効要素として数える
            mCount = static_cast<uint32_t>(initialData.size());

            return true;
        }

        // バッファ更新
        bool Update(std::span<const T> data) {
            // まだ作られていない場合は更新不可
            if (!mBuffer) { 
                return false;
            }

            // 作成時に確保した領域を超える書き込みはしない
            if (data.size() > mCapacity) {
                return false;
            }

            // データが0件の場合はGPUに転送せず、有効要素数のみ0にする
            if (data.empty()) {
                mCount = 0;
                return true;
            }

            ID3D11DeviceContext* context = DeviceManager::getInstance().GetContext();

            if (!context) { return false; }

            D3D11_MAPPED_SUBRESOURCE mapped{};

            // 既存内容を破棄し、CPUから新しい内容を書き込む
            const HRESULT result = context->Map(mBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);

            if (FAILED(result)) { return false; }

            const size_t byteSize = sizeof(T) * data.size();

            // CPU側配列をGPUバッファへコピー
            std::memcpy(mapped.pData, data.data(), byteSize);

            context->Unmap(mBuffer.Get(), 0);

            // 今回転送されたデータ数を有効要素数として保持
            mCount = static_cast<uint32_t>(data.size());

            return true;
        }

        // VSへバインド
        void BindVS(uint32_t slot) const {
            ID3D11DeviceContext* context = DeviceManager::getInstance().GetContext();

            if (!context || !mSRV) { return; }

            ID3D11ShaderResourceView* srv = mSRV.Get();
            context->VSSetShaderResources(slot, 1, &srv);
        }

        // VSからアンバインド
        void UnbindVS(uint32_t slot) const {
            ID3D11DeviceContext* context = DeviceManager::getInstance().GetContext();

            if (!context) { return; }

            ID3D11ShaderResourceView* nullSRV = nullptr;
            context->VSSetShaderResources(slot, 1, &nullSRV);
        }

        // リソース解放
        void Release() {
            mSRV.Reset();
            mBuffer.Reset();

            mCapacity = 0;
            mCount = 0;
        }

        // バッファが正常に作成されているか
        bool IsValid() const { return mBuffer != nullptr && mSRV != nullptr; }

        // 現在有効な要素数取得
        uint32_t GetCount() const { return mCount; }

        // 最大要素数取得
        uint32_t GetCapacity() const { return mCapacity; }
    };
}