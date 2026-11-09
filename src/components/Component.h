/*============================================================
*	@file	 : Component.h
*	@brief	 : コンポーネント基底クラス
*
* 　@author  : @akitsuki-35（https://github.com/akitsuki-35）
* 　@date	 : 2026/05/19
*	@updated : 2026/11/09
*============================================================*/
#pragma once

/*============================================================
*	@class	: Component
*	@brief	: コンポーネント基底クラス
*============================================================*/
class Component
{
public:
	// コンポーネント処理優先度（高い順にUpdate, Draw処理）
	// 特定コンポーネント処理後に処理が必要なコンポーネントに使用
	enum class Priority : int
	{
		None, // 並べ替えない
		Low,
		Medium,
		High
	};

protected:
	class GameObject* _mOwner{ nullptr };
	Priority mPriority{ Priority::None };

public:
	Component() {}
	Component(GameObject* owner)
		: _mOwner(owner){}
	virtual ~Component() = default;

	virtual void Initialize() {};
	virtual void Finalize() {};
	virtual void Update(double /*deltaTime*/) {};
	virtual void Draw() const {};

	Priority GetPriority() { return mPriority; }
};