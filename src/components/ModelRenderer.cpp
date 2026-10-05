/*============================================================
*	@file	 : ModelRenderer.cpp
*	@brief	 : モデル描画コンポーネント
*
* 　@author  : @akitsuki-35（https://github.com/akitsuki-35）
* 　@date	 : 2026/08/01
*	@updated : 2026/10/05
*============================================================*/
#include "ModelRenderer.h"
#include "ModelManager.h"
#include "Model.h"
#include "TextureManager.h"
#include "Texture.h"
#include "BufferManager.h"
#include "GameObject.h"
#include "FileUtility.h"

void ModelRenderer::Draw() const
{
	if (!_mModel) return;

    Renderer::Begin();

    Bind();

    // ワールド行列セット
	D3D11::BufferManager::getInstance().SetWorldMatrix(getWorldMatrix());

    // ボーン行列セット
    if (_mModel->GetSkeleton().GetBoneCount() > 0) {
        D3D11::BufferManager::getInstance().SetBoneMatrices(_mModel->GetSkeleton());
    }

    for (auto& mesh : _mModel->mMeshes)
    {
        mesh.Bind();

        // マテリアル設定
        for (auto& subset : mesh.subsets)
        {
            Element::MATERIAL material{};
            material.Diffuse = _mModel->mMaterials[subset.MaterialIndex].Material.Diffuse;

            if (mTextures.Albedo || _mModel->mMaterials[subset.MaterialIndex].Material.TextureEnable) {
                // 埋め込みテクスチャまたは外部テクスチャが存在
                material.TextureEnable = true;
            }
            else {
                // 埋め込みテクスチャまたは外部テクスチャが存在しない
                material.TextureEnable = false;
            }
            
            // マテリアルのセット
            D3D11::BufferManager::getInstance().SetMaterial(material);

            // パラメータ設定
            D3D11::BufferManager::getInstance().SetParameter(mParameter);

            if (mTextures.Albedo) {
                // 外部テクスチャを使用して描画
                mTextures.Albedo->Bind();
            }
            else if (material.TextureEnable) {
                // 埋め込みテクスチャを使用して描画
                _mModel->mMaterials[subset.MaterialIndex]._Texture->Bind();
            }

            // マップテクスチャをセット
            setMapTextures();

            // メッシュ描画
            mesh.Draw(subset);
        }
    }

    Renderer::End();
}

DirectX::XMMATRIX ModelRenderer::getWorldMatrix() const
{
    // ワールド行列取得
    return _mOwner->GetTransform().GetWorldMatrix();
}

ModelRenderer* ModelRenderer::LoadModel(const char* fileName, const bool& isAnimLoad)
{
    _mModel = ModelManager::getInstance().Load(fileName, isAnimLoad);

    // モデルディレクトリ取得
    mDirectory = Utility::File::getDirectoryPath(fileName);

    return this;
}

ModelRenderer* ModelRenderer::LoadTexture(std::string textureName, TextureType type, bool isMip)
{
    // 指定タイプにテクスチャをロード
    switch (type)
    {
    case TextureType::Albedo:
        mTextures.Albedo = TextureManager::getInstance().Load(
            converttoTexturePath(textureName).c_str(), isMip);
        break;

    case TextureType::Normal:
        mTextures.Normal = TextureManager::getInstance().Load(
            converttoTexturePath(textureName).c_str(), isMip);
        break;

    case TextureType::Roughness:
        mTextures.Roughness = TextureManager::getInstance().Load(
            converttoTexturePath(textureName).c_str(), isMip);
        break;

    case TextureType::Metalness:
        mTextures.Metalness = TextureManager::getInstance().Load(
            converttoTexturePath(textureName).c_str(), isMip);
        break;

    case TextureType::Ramp:
        mTextures.Rump = TextureManager::getInstance().Load(
            converttoTexturePath(textureName).c_str(), isMip);
        break;

    default:
        break;
    }

    return this;
}

void ModelRenderer::setMapTextures() const
{
    // 法線マップ
    if (mTextures.Normal) {
        mTextures.Normal->Bind(1);
    }

    // ざらつき
    if (mTextures.Roughness) {
        mTextures.Roughness->Bind(2);
    }

    // 金属感
    if (mTextures.Metalness) {
        mTextures.Metalness->Bind(3);
    }

    // セルシェーディング明るさ値
    if (mTextures.Rump) {
        mTextures.Rump->Bind(4);
    }
}

std::string ModelRenderer::converttoTexturePath(const std::string& textureName)
{
    // テクスチャファイル名からパスを生成

    // モデルと同ディレクトリから参照
    // モデル用テクスチャを1ディレクトリに集結するため、複数ディレクトリの参照は原則不可
    std::string texturePath = mDirectory.string() + textureName;

    return texturePath;
}