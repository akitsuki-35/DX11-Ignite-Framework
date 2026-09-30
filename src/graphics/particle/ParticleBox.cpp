/*============================================================
*	@file	 : ParticleBox.cpp
*	@brief	 : ボックス型散布パーティクル
*
* 　@author  : @akitsuki-35（https://github.com/akitsuki-35）
* 　@date	 : 2026/08/27
*	@updated : 2026/09/30
*============================================================*/
#include "ParticleBox.h"
#include "ParticleEmitter.h"

void ParticleType::Box::Emission(PARTICLE_DESC& desc)
{
	// エミッタ本体から発射数とパーティクル配列を取得
	int count = _mEmitter->GetCount();
	auto& particles = _mEmitter->GetParticles();

	// エミッタ本体の座標をセット
	Vector3 position = _mEmitter->GetTransform().GetPosition();

	// パーティクル発射
	for (int i = 0; i < _mEmitter->GetParticleMax(); i++) {
		if (!particles[i].IsEnable()) {
			Vector3 velocity = { 
				desc.Velocity.x + ((float)rand() / RAND_MAX - 0.5f) * desc.SpreadRate.x,
				desc.Velocity.y + ((float)rand() / RAND_MAX - 0.5f) * desc.SpreadRate.y,
				desc.Velocity.z + ((float)rand() / RAND_MAX - 0.5f) * desc.SpreadRate.z };
			float scale = ((float)rand() / RAND_MAX - 0.5f) * desc.Scale;

			particles[i].SetParameter(position, velocity,
				desc.Accel, { scale, scale, scale }, desc.Gravity, desc.Drag, desc.Life);
			particles[i].SetEnable(true);

			count--;
			if (count <= 0) {
				break;
			}
		}
	}
}