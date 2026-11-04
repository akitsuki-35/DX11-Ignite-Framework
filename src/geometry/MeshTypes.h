/*============================================================
*	@file	 : MeshTypes.h
*	@brief	 : メッシュタイプ管理
*
* 　@author  : @akitsuki-35（https://github.com/akitsuki-35）
* 　@date	 : 2026/07/26
*	@updated : 2026/11/04
*============================================================*/
#pragma once

#include "Elements.h"
#include <array>
#include <cstdint>
#include <vector>

/*============================================================
*	@namespace	: MeshType
*	@brief		: メッシュタイプのユーティリティ定義
*============================================================*/
namespace MeshType {
/*--------------------------------------------------
	板ポリゴン
----------------------------------------------------*/
	namespace Plane {
		// メッシュピボット列挙体
		enum class Pivot : uint8_t
		{
			Center,
			CenterTop,
			CenterBottom,
			LeftTop,
			LeftBottom,
			RightTop,
			RightBottom,
			
			Count
		};

		// メッシュ方向列挙体
		enum class Axis  : uint8_t
		{
			XY,
			XZ,
			YZ,
			
			Count
		};

		// 初期化用構造体
		struct DESC
		{
			Pivot pivot = Pivot::Center;
			Axis axis = Axis::XZ;
		};

		// 板ポリゴン生成
		std::array<Element::VERTEX3D, 4> Create(const DESC& desc);
	}

/*--------------------------------------------------
	凹凸地形
----------------------------------------------------*/
	namespace Unevenness {
		// 初期化用構造体
		struct DESC
		{
			// 縦・横の頂点数
			uint32_t row{ 10 };
			uint32_t column{ 10 };

			// メッシュの縦幅・横幅
			float width{ 10.0f };
			float depth{ 10.0f };

			// 高低差
			float height{ 1.0f };

			// 凹凸の間隔
			float frequency{ 0.5f };
		};

		// 地形データ
		struct DATA
		{
			std::vector<Element::VERTEX3D> vertices{};
			std::vector<uint32_t> indices{};
		};

		// 凹凸地形生成
		DATA Create(const DESC& desc);

		// 指定したXZ座標のYを取得
		bool GetHeight(const DESC& desc, float x, float z, float& height);
	}
}