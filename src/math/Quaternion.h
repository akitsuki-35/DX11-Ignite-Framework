/*============================================================
*	@file	 : Quaternion.h
*	@brief	 : クォータニオン
*
* 　@author  : @akitsuki-35（https://github.com/akitsuki-35）
* 　@date	 : 2026/08/07
*	@updated : 2026/10/01
*============================================================*/
#pragma once

#include <DirectXMath.h>

/*============================================================
*	@class	: Quaternion
*	@brief	: クォータニオン
*============================================================*/
class Quaternion
{
public:
    float x{};
    float y{};
    float z{};
    float w{ 1.0f };

public:
    constexpr Quaternion() = default;
    constexpr Quaternion(const Quaternion& quaternion)
        : x(quaternion.x), y(quaternion.y), z(quaternion.z), w(quaternion.w) {}
    constexpr Quaternion(const float& x, const float& y, const float& z, const float& w)
        : x(x), y(y), z(z), w(w) {}
    constexpr Quaternion(const DirectX::XMFLOAT4& float4)
        : x(float4.x), y(float4.y), z(float4.z), w(float4.w) {}

    // XMMATRIXへ変換
    DirectX::XMMATRIX ToMatrix() const {
        DirectX::XMVECTOR q = DirectX::XMVectorSet(x, y, z, w);
        return DirectX::XMMatrixRotationQuaternion(q);
    }

    // 回転を補間
    static Quaternion Slerp(const Quaternion& a, const Quaternion& b, float t) {
        t = (t < 0.0f) ? 0.0f : ((t > 1.0f) ? 1.0f : t);

        // 球面補間
        DirectX::XMVECTOR qa = DirectX::XMVectorSet(a.x, a.y, a.z, a.w);
        DirectX::XMVECTOR qb = DirectX::XMVectorSet(b.x, b.y, b.z, b.w);

        // 正規化
        qa = DirectX::XMQuaternionNormalize(qa);
        qb = DirectX::XMQuaternionNormalize(qb);

        // 内積を計算
        DirectX::XMVECTOR dotV = DirectX::XMQuaternionDot(qa, qb);
        float dot;
        DirectX::XMStoreFloat(&dot, dotV);

        if (dot < 0.0f) {
            qb = DirectX::XMVectorNegate(qb);
        }

        DirectX::XMVECTOR result = DirectX::XMQuaternionSlerp(qa, qb, t);
        result = DirectX::XMQuaternionNormalize(result);

        DirectX::XMFLOAT4 value{};
        DirectX::XMStoreFloat4(&value, result);

        return Quaternion(value.x, value.y, value.z, value.w);
    }
};