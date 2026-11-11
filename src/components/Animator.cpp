/*============================================================
*	@file	 : Animator.cpp
*	@brief	 : アニメーションコンポーネント
*
* 　@author  : @akitsuki-35（https://github.com/akitsuki-35）
* 　@date	 : 2026/08/07
*	@updated : 2026/11/11
*============================================================*/
#include "Animator.h"
#include "Model.h"
#include "AnimationManager.h"
#include "ModelRenderer.h"
#include "Scene.h"
#include "GameObject.h"
#include <cmath>
#include <algorithm>

using namespace DirectX;

Animator::Animator(GameObject* owner)
    : Component(owner)
{
    // ソケットがアニメーション後の行列を使うため先に処理する
    mPriority = Priority::High;

    // モデルのスケルトン取得
    setSkeleton();

    // スプリングボーン初期化
    initSpring();
}

void Animator::Finalize()
{
    mNext._Animation = nullptr;
    mCurrent._Animation = nullptr;
}

void Animator::Update(double deltaTime)
{
    if (!mCurrent._Animation || !mNext._Animation) {
        return;
    }

    // Current != Nextの場合はブレンド処理
    if (mCurrent.Name != mNext.Name) {
        updateBlend(deltaTime);
    }
    else {
        updateCurrent(deltaTime);
    }
}

Animator* Animator::Load(std::string keyName, const char* fileName, const bool& isSet)
{
    Animation* animation = AnimationManager::getInstance().Load(keyName, fileName);
    
    // ノードテーブル作成
    generateNodeTable(keyName, animation);

    // ロードしたアニメーションをセット
    if (isSet) {
        Set(keyName);
    }

    return this;
}

void Animator::Set(const std::string& keyName, const bool& isLoop, const double& duration)
{
    if (mNext.Name == keyName) {
        return;
    }

    Animation* animation = AnimationManager::getInstance().Get(keyName);

    if (!animation) {
        return;
    }

    if (!mNodeTable.contains(keyName)) {
        generateNodeTable(keyName, animation);
    }

    mNext._Animation = animation;
    mNext.Name = keyName;
    mNext.ElapsedTime = 0.0;
    mNext.IsLoop = isLoop;

    // Currentが存在する場合はアニメーションブレンド処理
    if (!mCurrent._Animation) {
        mCurrent = mNext;
    }
    else {
        animBlend(duration);
    }
}

bool Animator::IsPlaying(std::string keyName) const
{
    if (mCurrent.Name == keyName || mNext.Name == keyName) {
        if (!IsFinished(keyName)) {
            return true;
        }
    }

    return false;
}

bool Animator::IsFinished(std::string keyName) const
{
    if (mCurrent.Name != keyName && mNext.Name != keyName) {
        return false;
    }

    return mCurrent.ElapsedTime >= mCurrent._Animation->GetDuration();
}

bool Animator::IsBlending(std::string keyName) const
{
    if (mNext.Name != keyName) {
        return false;
    }

    return mCurrent.Name != mNext.Name;
}

void Animator::updateCurrent(double deltaTime)
{
    // 全ノードをバインドポーズに戻す
    mSkeleton.ToBindPose();

    // ノードテーブル参照でアニメーション更新
    const auto& channels = mCurrent._Animation->GetChannels();
    const auto& table = mNodeTable.at(mCurrent.Name);

    for (size_t i = 0; i < channels.size(); i++) {
        updateBoneTransform(channels[i], table[i], mCurrent.ElapsedTime);
    }

    updateSpring(static_cast<float>(deltaTime));

    mSkeleton.Update();

    // 経過時間更新
    mCurrent.ElapsedTime += deltaTime * mCurrent._Animation->GetTicksPerSecond();

    looping(mCurrent);
}

void Animator::updateBlend(double deltaTime)
{
    // 全ノードをバインドポーズに戻す
    mSkeleton.ToBindPose();

    // 現在アニメーションのチャンネルとテーブル取得
    const auto& currentChannels = mCurrent._Animation->GetChannels();
    const auto& currentTable = mNodeTable.at(mCurrent.Name);

    // 次アニメーションのチャンネルとテーブル取得
    const auto& nextChannels = mNext._Animation->GetChannels();
    const auto& nextTable = mNodeTable.at(mNext.Name);

    // Next用の逆引きノードテーブル作成
    std::vector<int> nextNodes(mSkeleton.GetNodeCount(), -1);

    for (size_t i = 0; i < nextTable.size(); i++) {
        int nodeIndex = nextTable[i];

        if (nodeIndex < 0 || static_cast<size_t>(nodeIndex) >= nextNodes.size()) {
            continue;
        }

        nextNodes[nodeIndex] = static_cast<int>(i);
    }

    // ウェイト算出
    float weight = 1.0f;
    if (mBlendDuration > 0.0) {
        weight = std::clamp(static_cast<float>(mBlendTime / mBlendDuration), 0.0f, 1.0f);

        // エルミート補間
        weight = weight * weight * (3.0f - 2.0f * weight);
    }

    // チャンネル更新
    // 逆引きテーブルを元にNextを更新する
    for (size_t i = 0; i < currentChannels.size(); i++) {
        const int nodeIndex = currentTable[i];

        if (nodeIndex < 0 || static_cast<size_t>(nodeIndex) >= mSkeleton.GetNodeCount()) {
            continue;
        }

        const int nextIndex = nextNodes[nodeIndex];

        if (nextIndex < 0 || static_cast<size_t>(nextIndex) >= nextChannels.size()) {
            updateBoneTransform(currentChannels[i], nodeIndex, mCurrent.ElapsedTime);
            continue;
        }

        // 現在アニメーションのトランスフォーム更新
        BoneTransform current = calculateBoneTransform(currentChannels[i], mCurrent.ElapsedTime);

        // 次アニメーションのトランスフォーム更新
        BoneTransform next = calculateBoneTransform(nextChannels[nextIndex], mNext.ElapsedTime);

        // ブレンド処理
        BoneTransform blend{};
        blend.mPosition = Vector3::Lerp(current.mPosition, next.mPosition, weight);
        blend.mRotation = Quaternion::Slerp(current.mRotation, next.mRotation, weight);
        blend.mScale = Vector3::Lerp(current.mScale, next.mScale, weight);

        auto& node = mSkeleton.GetNode(nodeIndex);
        XMStoreFloat4x4(&node.Local, blend.ToMatrix());
    }

    updateSpring(static_cast<float>(deltaTime));

    mSkeleton.Update();

    // 現在アニメーションと次アニメーションの経過時間を更新
    mCurrent.ElapsedTime += deltaTime * mCurrent._Animation->GetTicksPerSecond();
    mNext.ElapsedTime += deltaTime * mNext._Animation->GetTicksPerSecond();

    // 各アニメーションのループ
    looping(mCurrent);
    looping(mNext);

    // ブレンド進行
    mBlendTime += deltaTime;

    // ブレンド完了処理
    if (mBlendTime >= mBlendDuration) {
        // Current == Next
        mCurrent = mNext;

        mBlendTime = 0.0f;
        mBlendDuration = 0.0f;
    }
}

void Animator::looping(ANIMATION& animation)
{
    // ループフラグに準じたループ処理
    double duration = animation._Animation->GetDuration();
    if (duration > 0.0) {
        if (animation.IsLoop) {
            animation.ElapsedTime = std::fmod(animation.ElapsedTime, duration);
        }
        else {
            animation.ElapsedTime = std::min(animation.ElapsedTime, duration);
        }
    }
}

bool Animator::setSkeleton()
{
    Model* model{};

    // オブジェクトのModelRendererコンポーネント取得
    ModelRenderer* renderer = _mOwner->GetComponent<ModelRenderer>();

    if (!renderer) {
        return false;
    }

    // ModelRendererのモデル取得
    model = renderer->GetModel();

    if (!model) {
        return false;
    }

    // モデルからスケルトンを取得
    mSkeleton = model->GetSkeleton();

    return true;
}

void Animator::generateNodeTable(const std::string& keyName, const Animation* animation)
{
    mNodeTable[keyName].clear();

    if (!animation) return;

    const auto& channels = animation->GetChannels();

    // アニメーションチャンネルからノードテーブルを作成
    for (size_t i = 0; i < channels.size(); i++) {
        mNodeTable[keyName].push_back(mSkeleton.FindNode(channels[i].NodeName));
    }
}

void Animator::updateBoneTransform(const Animation::CHANNEL& channel, int nodeIndex, double time)
{
    if (nodeIndex < 0 || static_cast<size_t>(nodeIndex) >= mSkeleton.GetNodeCount()) {
        return;
    }

    // ノード取得
    auto& node = mSkeleton.GetNode(nodeIndex);

    BoneTransform transform{};

    // トランスフォーム計算
    transform = calculateBoneTransform(channel, time);

    // BindLocalを作らない
    XMStoreFloat4x4(&node.Local, transform.ToMatrix());
}

BoneTransform Animator::calculateBoneTransform(const Animation::CHANNEL& channel, double time)
{
    BoneTransform transform{};

    // 座標計算
    transform.mPosition = calculatePosition(channel.Positions, time);

    // 回転計算
    transform.mRotation = calculateRotation(channel.Rotations, time);

    // スケール計算
    transform.mScale = calculateScale(channel.Scales, time);

    return transform;
}

Vector3 Animator::calculatePosition(const std::vector<Animation::KEY_POSITION>& keys, double time)
{
    if (keys.empty()) {
        return Vector3{};
    }

    // キーが1つのみの場合は補間しない
    if (keys.size() == 1 || time <= keys.front().Time) {
        return keys.front().Position;
    }

    if (time >= keys.back().Time) {
        return keys.back().Position;
    }

    int index = 0;

    for (int i = 0; i < static_cast<int>(keys.size()) - 1; i++) {
        if (time < keys[i + 1].Time) {
            index = i;
            break;
        }
    }

    for (size_t i = 0; i + 1 < keys.size(); i++) {
        const auto& current = keys[i];
        const auto& next = keys[i + 1];

        if (time <= next.Time) {
            const double interval = next.Time - current.Time;

            if (interval <= 0.0) {
                return current.Position;
            }

            const float factor = std::clamp(static_cast<float>((time - current.Time) / interval), 0.0f, 1.0f);

            return Vector3::Lerp(current.Position, next.Position, factor);
        }
    }

    return keys.back().Position;
}

Quaternion Animator::calculateRotation(const std::vector<Animation::KEY_ROTATION>& keys, double time)
{
    if (keys.empty()) {
        return Quaternion{};
    }

    // キーが1つのみの場合は補間しない
    if (keys.size() == 1 || time <= keys.front().Time) {
        return keys.front().Rotation;
    }

    if (time >= keys.back().Time) {
        return keys.back().Rotation;
    }

    for (size_t i = 0; i + 1 < keys.size(); i++) {
        const auto& current = keys[i];
        const auto& next = keys[i + 1];

        if (time <= next.Time) {
            const double interval = next.Time - current.Time;

            if (interval <= 0.0) {
                return current.Rotation;
            }

            const float factor = std::clamp(static_cast<float>((time - current.Time) / interval), 0.0f, 1.0f);

            return Quaternion::Slerp(current.Rotation, next.Rotation, factor);
        }
    }

    return keys.back().Rotation;
}

Vector3 Animator::calculateScale(const std::vector<Animation::KEY_SCALE>& keys, double time)
{
    if (keys.empty()) {
        return Vector3{ 1.0f, 1.0f, 1.0f };
    }

    // キーが1つのみの場合は補間しない
    if (keys.size() == 1 || time <= keys.front().Time) {
        return keys.front().Scale;
    }

    if (time >= keys.back().Time) {
        return keys.back().Scale;
    }

    for (size_t i = 0; i + 1 < keys.size(); i++) {
        const auto& current = keys[i];
        const auto& next = keys[i + 1];

        if (time <= next.Time) {
            const double interval = next.Time - current.Time;

            if (interval <= 0.0) {
                return current.Scale;
            }

            const float factor = std::clamp(static_cast<float>((time - current.Time) / interval), 0.0f, 1.0f);

            return Vector3::Lerp(current.Scale, next.Scale, factor);
        }
    }

    return keys.back().Scale;
}

void Animator::animBlend(const double& duration)
{
    mNext.ElapsedTime = 0.0;

    mBlendTime = 0.0;
    mBlendDuration = duration;
}

void Animator::initSpring()
{
    // 各チェーンの実行時状態初期化
    mSprings.assign(mSkeleton.GetSpringChains().size(), SPRING{});
}

void Animator::updateSpring(const float& dt)
{
    // スプリングチェーンが存在しない
    if (!mSkeleton.SpringChainEnable()) {
        return;
    }

    if (dt <= 0.0f) {
        return;
    }

    // スプリングチェーン取得
    const auto& chains = mSkeleton.GetSpringChains();
    
    // チェーン数が変更されていた場合は、各チェーンの実行時状態を初期化
    if (mSprings.size() != chains.size()) {
        initSpring();
    }

    // ゼロ除算防止のための下限
    const float motionDt = std::max(dt, 0.0001f);

    // バネシミュレーションへ渡す時間
    // フレーム落ちで角速度が急増しないよう制限
    const float springDt = std::min(dt, 1.0f / 15.0f);

    // 1回のバネ積分で処理する最大時間
    constexpr float maxSpringStep = 1.0f / 120.0f;

    // 1サブステップあたりの更新時間を算出
    // springDtをmaxSpringStep以下へ分割する
    const int springStepCount = std::max(1, static_cast<int>(std::ceil(springDt / maxSpringStep)));
    const float springStepDt = springDt / static_cast<float>(springStepCount);

    // 現在座標取得
    const Vector3 currentPosition = _mOwner->GetPosition();

    // 初回フレームでは移動速度を0として扱う
    DirectX::XMVECTOR worldVelocityVec = DirectX::XMVectorZero();
    
    if (mPositionInitialized) {
        // 現在座標と前フレーム座標をXMFLOAT3に変換
        const DirectX::XMFLOAT3 currentPositionValue = currentPosition.ConvertToXMFLOAT3();
        const DirectX::XMFLOAT3 previousPositionValue = mPrevPosition.ConvertToXMFLOAT3();

        // ワールド空間におけるオブジェクトの移動速度を計算
        worldVelocityVec = DirectX::XMVectorSet(
                (currentPositionValue.x - previousPositionValue.x) / motionDt,
                (currentPositionValue.y - previousPositionValue.y) / motionDt,
                (currentPositionValue.z - previousPositionValue.z) / motionDt,
                0.0f);
    }

    // オブジェクト回転行列を取得
    const DirectX::XMFLOAT3 objRot =_mOwner->GetRotation().ConvertToXMFLOAT3();
    const DirectX::XMMATRIX objRotMtx = DirectX::XMMatrixRotationRollPitchYaw(objRot.x, objRot.y, objRot.z);

    // オブジェクト回転行列の逆回転行列を作成
    const DirectX::XMMATRIX invRot = DirectX::XMMatrixTranspose(objRotMtx);

    // ワールド空間の移動速度をオブジェクトのローカル空間に変換
    const DirectX::XMVECTOR localVelocityVec = DirectX::XMVector3TransformNormal(worldVelocityVec, invRot);

    // ローカル移動速度を保存
    DirectX::XMFLOAT3 localVelocity{};
    DirectX::XMStoreFloat3(&localVelocity, localVelocityVec);

    // 風パラメータ取得
    const Element::WIND wind = Scene::GetWind();

    const float windStrength = std::clamp(wind.Strength, 0.0f, 1.0f);
    constexpr float windAngleRatio = 0.6f;

    // 各チェーン更新
    for (size_t i = 0; i < chains.size(); i++) {
        const auto& chain = chains[i];
        auto& spring = mSprings[i];

        // チェーン内のスプリング対象ノード数取得
        const size_t nodeCount = chain.NodeIndices.size();
        if (nodeCount == 0) {
            continue;
        }

        // NodeIndicesの先頭をスプリングチェーンの根元とする
        const int rootNodeIndex = chain.NodeIndices.front();

        // チェーン根元のノードインデックスが有効か確認
        if (rootNodeIndex < 0 || static_cast<size_t>(rootNodeIndex) >= mSkeleton.GetNodeCount()) {
            continue;
        }

        // スプリングチェーンの根元ノードを取得
        const auto& rootNode = mSkeleton.GetNode(static_cast<size_t>(rootNodeIndex));

        // チェーン根元の親ノードを駆動ノードとする
        // 親ノードの回転変化に応じてチェーンを揺らす
        const int parentNodeIndex = rootNode.ParentIndex;

        // 親ノードが存在しない場合は回転変化を取得できないため処理しない
        if (parentNodeIndex < 0 || static_cast<size_t>(parentNodeIndex) >= mSkeleton.GetNodeCount()) {
            continue;
        }

        // スプリングチェーンを駆動する親ノードを取得
        const auto& parentNode = mSkeleton.GetNode(static_cast<size_t>(parentNodeIndex));

        // 親ノードのローカル行列を取得
        const DirectX::XMMATRIX parentLocalMtx = DirectX::XMLoadFloat4x4(&parentNode.Local);

        DirectX::XMVECTOR parentScale{}, parentRotationVec{}, parentTranslation{};

        // 親ノードのローカル行列を分解
        // 行列を正常に分解できない場合は以降の処理を行わない
        if (!DirectX::XMMatrixDecompose(&parentScale, &parentRotationVec, &parentTranslation, parentLocalMtx)) {
            continue;
        }

        // 回転を正規化
        parentRotationVec = DirectX::XMQuaternionNormalize(parentRotationVec);

        // 前フレームの親回転が保存済みか
        const bool hasPreviousRotation = spring.RotationInitialized;

        // 前フレームから現在フレームまでの差分回転
        // 初回更新時は回転差がないため単位Quaternionで初期化
        DirectX::XMVECTOR deltaRotationVec = DirectX::XMQuaternionIdentity();

        if (hasPreviousRotation) {
            // 前フレームの親回転をXMFLOAT4に変換
            const DirectX::XMFLOAT4 prevRotationValue = spring.PrevRotation.ToXMFLOAT4();

            DirectX::XMVECTOR prevRotationVec = DirectX::XMLoadFloat4(&prevRotationValue);

            // 前回回転を正規化
            prevRotationVec = DirectX::XMQuaternionNormalize(prevRotationVec);

            // 前回回転と現在回転の内積が負の場合、現在回転の符号を反転
            const float rotationDot = DirectX::XMVectorGetX(DirectX::XMVector4Dot(
                prevRotationVec, parentRotationVec));
            if (rotationDot < 0.0f) {
                parentRotationVec = DirectX::XMVectorNegate(parentRotationVec);
            }

            // 前フレームから現在フレームまでの差分回転を計算
            deltaRotationVec = DirectX::XMQuaternionMultiply(DirectX::XMQuaternionInverse(
                prevRotationVec), parentRotationVec);

            // 後続の軸角変換で数値誤差が影響しないように正規化
            deltaRotationVec = DirectX::XMQuaternionNormalize(deltaRotationVec);
        }

        // 符号補正後の現在回転をXMFLOAT4に保持
        DirectX::XMFLOAT4 storedRotationValue{};
        DirectX::XMStoreFloat4(&storedRotationValue, parentRotationVec);

        // 現在フレームの親回転を次フレームの比較用として保存
        spring.PrevRotation = Quaternion{ storedRotationValue.x, storedRotationValue.y,
            storedRotationValue.z, storedRotationValue.w };

        // 初期化フラグをオン
        spring.RotationInitialized = true;

        // 親ノードの回転変化によるPitchとRoll
        // 初回フレームではどちらも0.0f
        float motionPitch{}, motionRoll{};

        if (hasPreviousRotation) {
            // 差分QuaternionのXZ成分を取得
            const float deltaX = DirectX::XMVectorGetX(deltaRotationVec);
            const float deltaZ = DirectX::XMVectorGetZ(deltaRotationVec);

            // Quaternionのw成分を-1.0から1.0に制限
            const float deltaW = std::clamp(DirectX::XMVectorGetW(deltaRotationVec), -1.0f, 1.0f);

            // 差分Quaternionから回転角度を取得
            const float rotationAngle = 2.0f * std::acos(deltaW);

            // 回転角度の半分に対するsin値を取得する
            const float sinHalfAngle = std::sqrt(std::max(1.0f - deltaW * deltaW, 0.0f));

            // 回転量が十分に存在する場合だけ回転軸と角速度を計算する
            // 回転量がほぼ0の場合にsinHalfAngleで除算すると非常に大きな値やNaNが発生するため処理しない
            constexpr float minRotation = 0.00001f;

            if (sinHalfAngle > minRotation && rotationAngle > minRotation) {
                // 差分Quaternionから回転軸を取得
                const float axisX = deltaX / sinHalfAngle;
                const float axisZ = deltaZ / sinHalfAngle;

                // フレーム間の回転角度から角速度を計算
                const float angularSpeed = rotationAngle / motionDt;

                // 親ノードのX軸回転をPitchに反映
                motionPitch = -axisX * angularSpeed * chain.MotionInfluence;

                // 親ノードのZ軸回転をRollに反映
                motionRoll = -axisZ * angularSpeed * chain.MotionInfluence;
            }
        }

        // 風によって発生させる最大角度を計算
        // MaxAngleを基準に、シーン全体の風の強さとチェーン固有の風影響度を反映
        const float windAngle = chain.MaxAngle * windAngleRatio * windStrength * chain.WindInfluence;

        // ワールド空間の風向きをベクトルとして取得
        const DirectX::XMVECTOR worldWindDir = DirectX::XMVectorSet(wind.Direction.x,
                wind.Direction.y, wind.Direction.z, 0.0f);

        // ワールド空間の風向きをオブジェクトのローカル空間に変換
        const DirectX::XMVECTOR localWindVec = DirectX::XMVector3TransformNormal(worldWindDir, invRot);

        // 風向きを保存
        DirectX::XMFLOAT3 localWindDir{};
        DirectX::XMStoreFloat3(&localWindDir, localWindVec);

        // ローカルの風をPitchとRollに反映
        const float windPitch = localWindDir.z * windAngle;
        const float windRoll = localWindDir.x * windAngle;

        // 親ボーン回転・風による角度を合成
        spring.TargetPitch = motionPitch + windPitch;
        spring.TargetRoll = motionRoll + windRoll;

        // 目標PitchとRollがチェーンの最大角度を超えないようクランプ
        spring.TargetPitch = std::clamp(spring.TargetPitch, -chain.MaxAngle, chain.MaxAngle);
        spring.TargetRoll = std::clamp(spring.TargetRoll, -chain.MaxAngle, chain.MaxAngle);

        // 外力を作成
        const float translationPitchForce = localVelocity.z * chain.TranslationInfluence;
        const float translationRollForce = localVelocity.x * chain.TranslationInfluence;

        // バネ計算
        for (int step = 0; step < springStepCount; step++) {
            // Pitch方向の復元力を計算
            const float pitchSpringForce = (spring.TargetPitch - spring.CurrentPitch) * chain.Stiffness;

            // 現在のPitch角速度に対する減衰力を計算
            const float pitchDampingForce = -spring.PitchVelocity * chain.Damping;

            // 復元力・減衰力・オブジェクト移動による外力からPitch方向の角加速度を計算
            const float pitchAcceleration = pitchSpringForce + pitchDampingForce + translationPitchForce;

            // Pitch角速度を更新
            spring.PitchVelocity += pitchAcceleration * springStepDt;

            // 更新後の角速度を使ってPitch角度を更新
            spring.CurrentPitch += spring.PitchVelocity * springStepDt;

            // Roll方向の復元力を計算する
            const float rollSpringForce = (spring.TargetRoll - spring.CurrentRoll) * chain.Stiffness;

            // 現在のRoll角速度に対する減衰力を計算
            const float rollDampingForce = -spring.RollVelocity * chain.Damping;

            // Roll方向の角加速度を計算
            const float rollAcceleration = rollSpringForce + rollDampingForce + translationRollForce;

            // Roll角速度を更新
            spring.RollVelocity += rollAcceleration * springStepDt;

            // 更新後の角速度を使ってRoll角度を更新
            spring.CurrentRoll += spring.RollVelocity * springStepDt;

            // 最大角度を超えた場合のクランプ処理
            // Pitch
            if (spring.CurrentPitch > chain.MaxAngle) {
                // 正方向の最大角度まで戻す
                spring.CurrentPitch = chain.MaxAngle;

                // さらに正方向へ進もうとしている角速度だけ停止
                if (spring.PitchVelocity > 0.0f) {
                    spring.PitchVelocity = 0.0f;
                }
            }
            else if (spring.CurrentPitch < -chain.MaxAngle) {
                // 負方向の最大角度まで戻す
                spring.CurrentPitch = -chain.MaxAngle;

                // さらに負方向へ進もうとしている角速度だけ停止する
                if (spring.PitchVelocity < 0.0f) {
                    spring.PitchVelocity = 0.0f;
                }
            }

            // Roll
            if (spring.CurrentRoll > chain.MaxAngle) {
                // 正方向の最大角度まで戻す
                spring.CurrentRoll = chain.MaxAngle;

                // さらに正方向へ進もうとしている角速度だけ停止する
                if (spring.RollVelocity > 0.0f) {
                    spring.RollVelocity = 0.0f;
                }
            }
            else if (spring.CurrentRoll < -chain.MaxAngle) {
                // 負方向の最大角度まで戻す
                spring.CurrentRoll = -chain.MaxAngle;

                // さらに負方向へ進もうとしている角速度だけ停止する
                if (spring.RollVelocity < 0.0f) {
                    spring.RollVelocity = 0.0f;
                }
            }
        }

        // チェーン内の各ノードにスプリング回転を適用
        for (size_t j = 0; j < nodeCount; j++) {
            // ノードインデックスが無効の場合は処理しない
            const int nodeIndex = chain.NodeIndices[j];
            if (nodeIndex < 0 || static_cast<size_t>(nodeIndex) >= mSkeleton.GetNodeCount()) {
                continue;
            }

            // ノード取得
            auto& node = mSkeleton.GetNode(static_cast<size_t>(nodeIndex));

            // 根元を弱く、先端を強くする
            const float weight = chain.UniformWeight ? 1.0f : 
                static_cast<float>(j + 1) / static_cast<float>(nodeCount);
            const float pitch = spring.CurrentPitch * weight;
            const float roll = spring.CurrentRoll * weight;

            // マトリクス作成
            const DirectX::XMMATRIX pitchMtx = DirectX::XMMatrixRotationX(pitch);
            const DirectX::XMMATRIX rollMtx = DirectX::XMMatrixRotationZ(roll);
            const DirectX::XMMATRIX localMtx = DirectX::XMLoadFloat4x4(&node.Local);
            const DirectX::XMMATRIX springMtx = pitchMtx * rollMtx;

            const DirectX::XMMATRIX result = springMtx * localMtx;
            
            // スプリング補正後のローカル行列をノードに保存
            DirectX::XMStoreFloat4x4(&node.Local, result);
        }
    }

    // 現在座標を次フレームの移動速度計算用に保存
    mPrevPosition = currentPosition;
    mPositionInitialized = true;
}