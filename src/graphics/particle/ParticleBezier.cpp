/*============================================================
*	@file	 : ParticleBezier.cpp
*	@brief	 : ベジエ曲線上を移動するパーティクル
*
* 　@author  : @akitsuki-35（https://github.com/akitsuki-35）
* 　@date	 : 2026/08/28
*	@updated : 2026/09/30
*============================================================*/
#include "ParticleBezier.h"
#include "ParticleBox.h"
#include "ParticleEmitter.h"
#include "ParticleRenderer.h"
#include "CSVHandler.h"

using namespace DirectX;

void ParticleType::Bezier::Update(double deltaTime)
{
	// 現フレームのベジエ曲線上の座標を取得
	mBezier.Update();
	_mEmitter->SetPosition(mBezier.GetBezierPoint(mBezier.GetFrame()));

	Base::Update(deltaTime);
}

void ParticleType::Bezier::Emission(PARTICLE_DESC& desc)
{
	// エミッタ本体から発射数とパーティクル配列を取得
	int count = _mEmitter->GetCount();
	auto& particles = _mEmitter->GetParticles();

	// 全体フレームをセット
	mBezier.SetFrameMax(_mEmitter->GetLife());

	// ベジエ曲線再計算
	mBezier.CalcBezier();

	// 現フレームのベジエ曲線上の座標を取得
	mBezier.Update();
	_mEmitter->SetPosition(mBezier.GetBezierPoint(mBezier.GetFrame()));

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

std::unique_ptr<ParticleType::Base> ParticleType::Bezier::LoadCSV(const char* filePath)
{
	CSVHandler::Data data{};

	// ロード失敗時はreturn
	if (!CSVHandler::Load(filePath, data)) {
		return nullptr;
	}

	std::unique_ptr<ParticleType::Base> newType = nullptr;

	for (const auto& row : data) {
		std::string tag = CSVHandler::GetString(row, 0);

		if (tag == "TYPE") {
			std::string type = CSVHandler::GetString(row, 1);
			if (type == "Box") {
				newType = std::make_unique<ParticleType::Box>(_mEmitter);
			}
			if (type == "Bezier") {
				newType = std::make_unique<ParticleType::Bezier>(_mEmitter);
			}
		}
		if (tag == "LIFE") {
			// 全体フレーム取得
			int life = CSVHandler::GetInt(row, 1, 60);
			_mEmitter->SetLife(life);
		}
		else if (tag == "INTERVAL") {
			// インターバル取得
			double interval = CSVHandler::GetDouble(row, 1, 0.1);
			_mEmitter->SetInterval(interval);
		}
		else if (tag == "COUNT") {
			// 発射数取得
			int count = CSVHandler::GetInt(row, 1, 100);
			_mEmitter->SetCount(count);
		}
		else if (tag == "MAINCOLOR") {
			// メインカラー取得
			XMFLOAT4 color{};

			color.x = CSVHandler::GetFloat(row, 1, 1.0f);
			color.y = CSVHandler::GetFloat(row, 2, 1.0f);
			color.z = CSVHandler::GetFloat(row, 3, 1.0f);
			color.w = CSVHandler::GetFloat(row, 4, 1.0f);

			_mEmitter->GetComponent<ParticleRenderer>()->SetColor(color);
		}
		else if (tag == "SUBCOLOR") {
			// サブカラー取得
			XMFLOAT4 color{};

			color.x = CSVHandler::GetFloat(row, 1, 1.0f);
			color.y = CSVHandler::GetFloat(row, 2, 1.0f);
			color.z = CSVHandler::GetFloat(row, 3, 1.0f);
			color.w = CSVHandler::GetFloat(row, 4, 1.0f);

			_mEmitter->GetComponent<ParticleRenderer>()->SetSubColor(color);
		}
		// ベジエ曲線用パラメータ取得
		else if (tag == "CONTROLPOINT0") {
			// 制御点0取得
			Vector3 position{};

			position.x = CSVHandler::GetFloat(row, 1, 0.0f);
			position.y = CSVHandler::GetFloat(row, 2, 0.0f);
			position.z = CSVHandler::GetFloat(row, 3, 0.0f);

			// エミッタ座標をオフセットとして制御点座標を設定
			mBezier.SetControlPoint(0, _mEmitter->GetPosition() + position);
		}
		else if (tag == "CONTROLPOINT1") {
			// 制御点1取得
			Vector3 position{};

			position.x = CSVHandler::GetFloat(row, 1, 0.0f);
			position.y = CSVHandler::GetFloat(row, 2, 0.0f);
			position.z = CSVHandler::GetFloat(row, 3, 0.0f);

			// エミッタ座標をオフセットとして制御点座標を設定
			mBezier.SetControlPoint(1, _mEmitter->GetPosition() + position);
		}
		else if (tag == "CONTROLPOINT2") {
			// 制御点2取得
			Vector3 position{};

			position.x = CSVHandler::GetFloat(row, 1, 0.0f);
			position.y = CSVHandler::GetFloat(row, 2, 0.0f);
			position.z = CSVHandler::GetFloat(row, 3, 0.0f);

			// エミッタ座標をオフセットとして制御点座標を設定
			mBezier.SetControlPoint(2, _mEmitter->GetPosition() + position);
		}
		else if (tag == "CONTROLPOINT3") {
			// 制御点3取得
			Vector3 position{};

			position.x = CSVHandler::GetFloat(row, 1, 0.0f);
			position.y = CSVHandler::GetFloat(row, 2, 0.0f);
			position.z = CSVHandler::GetFloat(row, 3, 0.0f);

			// エミッタ座標をオフセットとして制御点座標を設定
			mBezier.SetControlPoint(3, _mEmitter->GetPosition() + position);
		}
	}

	return newType;
}
