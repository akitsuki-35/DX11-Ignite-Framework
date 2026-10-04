/*============================================================
*	@file	 : BufferManager.cpp
*	@brief	 : 定数バッファ管理
*
* 　@author  : @akitsuki-35（https://github.com/akitsuki-35）
* 　@date	 : 2026/07/14
*	@updated : 2026/10/01
*============================================================*/
#include "BufferManager.h"
#include "DeviceManager.h"
#include "Config.h"
#include "Skeleton.h"

using namespace DirectX;

void D3D11::BufferManager::Initialize()
{
	// 定数バッファ初期化
	_mWorld = generateBuffer(sizeof(DirectX::XMMATRIX));
	_mView = generateBuffer(sizeof(DirectX::XMMATRIX));
	_mProjection = generateBuffer(sizeof(DirectX::XMMATRIX));
	_mMaterial = generateBuffer(sizeof(Element::MATERIAL));
	_mLight = generateBuffer(sizeof(Element::LIGHT));
	_mCamera = generateBuffer(sizeof(XMFLOAT4));
	_mBones = generateBuffer(sizeof(Element::BONE));
	_mParameter = generateBuffer(sizeof(XMFLOAT4));

	// ライト初期化
	Element::LIGHT light{};
	light.Position = { 1.0f, -1.0f, 1.0f, 0.0f };
	light.Enable = true;
	light.Direction = XMFLOAT4(1.0f, -1.0f, 1.0f, 0.0f);
	light.Ambient = XMFLOAT4(0.9f, 0.8f, 1.0f, 1.0f);
	light.Diffuse = XMFLOAT4(0.7f, 0.7f, 1.0f, 1.0f);
	SetLight(light);

	// マテリアル初期化
	Element::MATERIAL material{};
	material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	material.Ambient = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	SetMaterial(material);

	// 汎用パラメータ初期化
	DirectX::XMFLOAT4 parameter{ 0.0f, 0.0f, 0.0f, 0.0f };
	SetParameter(parameter);
}

Microsoft::WRL::ComPtr<ID3D11Buffer> D3D11::BufferManager::generateBuffer(UINT size)
{
	// 定数バッファ生成
	D3D11_BUFFER_DESC bufferDesc{};
	bufferDesc.ByteWidth = size;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = sizeof(float);

	ComPtr<ID3D11Buffer> buffer{};
	D3D11::DeviceManager::getInstance().
		GetDevice()->CreateBuffer(&bufferDesc, nullptr, &buffer);

	return buffer;
}

void D3D11::BufferManager::Set2DMatrix()
{
	// 2D用マトリクス設定
	SetWorldMatrix(XMMatrixIdentity());
	SetViewMatrix(XMMatrixIdentity());

	XMMATRIX projection;
	projection = XMMatrixOrthographicOffCenterLH(0.0f, Screen::WIDTH, Screen::HEIGHT, 0.0f, 0.0f, 1.0f);
	SetProjectionMatrix(projection);
}

void D3D11::BufferManager::SetWorldMatrix(const DirectX::XMMATRIX& worldMatrix)
{
	// ワールド行列設定
	XMFLOAT4X4 worldf;
	XMStoreFloat4x4(&worldf, XMMatrixTranspose(worldMatrix));
	DeviceManager::getInstance().
		GetContext()->UpdateSubresource(_mWorld.Get(), 0, nullptr, &worldf, 0, 0);

	ID3D11Buffer* buf = _mWorld.Get();
	DeviceManager::getInstance().
		GetContext()->VSSetConstantBuffers(0, 1, &buf);
}

void D3D11::BufferManager::SetViewMatrix(const DirectX::XMMATRIX& viewMatrix)
{
	// ビュー行列設定
	XMFLOAT4X4 viewf;
	XMStoreFloat4x4(&viewf, XMMatrixTranspose(viewMatrix));
	DeviceManager::getInstance().
		GetContext()->UpdateSubresource(_mView.Get(), 0, nullptr, &viewf, 0, 0);

	ID3D11Buffer* buf = _mView.Get();
	DeviceManager::getInstance().
		GetContext()->VSSetConstantBuffers(1, 1, &buf);
}

void D3D11::BufferManager::SetProjectionMatrix(const DirectX::XMMATRIX& projectionMatrix)
{
	// プロジェクション行列設定
	XMFLOAT4X4 projectionf;
	XMStoreFloat4x4(&projectionf, XMMatrixTranspose(projectionMatrix));
	DeviceManager::getInstance().
		GetContext()->UpdateSubresource(_mProjection.Get(), 0, nullptr, &projectionf, 0, 0);

	ID3D11Buffer* buf = _mProjection.Get();
	DeviceManager::getInstance().
		GetContext()->VSSetConstantBuffers(2, 1, &buf);
}

void D3D11::BufferManager::SetMaterial(const Element::MATERIAL& material)
{
	// マテリアル設定
	DeviceManager::getInstance().
		GetContext()->UpdateSubresource(_mMaterial.Get(), 0, nullptr, &material, 0, 0);

	ID3D11Buffer* buf = _mMaterial.Get();
	DeviceManager::getInstance().
		GetContext()->VSSetConstantBuffers(3, 1, &buf);
	DeviceManager::getInstance().
		GetContext()->PSSetConstantBuffers(3, 1, &buf);
}

void D3D11::BufferManager::SetLight(const Element::LIGHT& light)
{
	// ライト設定
	DeviceManager::getInstance().
		GetContext()->UpdateSubresource(_mLight.Get(), 0, nullptr, &light, 0, 0);

	ID3D11Buffer* buf = _mLight.Get();
	DeviceManager::getInstance().
		GetContext()->VSSetConstantBuffers(4, 1, &buf);
	DeviceManager::getInstance().
		GetContext()->PSSetConstantBuffers(4, 1, &buf);
}

void D3D11::BufferManager::SetCamera(const DirectX::XMFLOAT4& camera)
{
	// カメラ座標設定
	DeviceManager::getInstance().
		GetContext()->UpdateSubresource(_mCamera.Get(), 0, nullptr, &camera, 0, 0);

	ID3D11Buffer* buf = _mCamera.Get();
	DeviceManager::getInstance().
		GetContext()->VSSetConstantBuffers(5, 1, &buf);
	DeviceManager::getInstance().
		GetContext()->PSSetConstantBuffers(5, 1, &buf);
}

void D3D11::BufferManager::SetBoneMatrices(const Skeleton& skeleton)
{
	// ボーン行列設定
	Element::BONE buffer{};

	const DirectX::XMMATRIX identity = DirectX::XMMatrixIdentity();

	// 単位行列で初期化
	for (auto& matrix : buffer.Matrices) {
		DirectX::XMStoreFloat4x4(&matrix, identity);
	}

	const auto& matrices = skeleton.GetSkinningMatrices();

	assert(matrices.size() <= Element::MAX_BONE);

	for (size_t i = 0; i < matrices.size(); i++) {
		buffer.Matrices[i] = matrices[i];
	}

	DeviceManager::getInstance().GetContext()->UpdateSubresource(_mBones.Get(),
		0, nullptr, &buffer, 0, 0);

	ID3D11Buffer* buf = _mBones.Get();

	DeviceManager::getInstance().GetContext()->VSSetConstantBuffers(6, 1, &buf);
}

void D3D11::BufferManager::SetParameter(const DirectX::XMFLOAT4& parameter)
{
	// 汎用パラメータ設定
	DeviceManager::getInstance().
		GetContext()->UpdateSubresource(_mParameter.Get(), 0, nullptr, &parameter, 0, 0);

	ID3D11Buffer* buf = _mParameter.Get();
	DeviceManager::getInstance().
		GetContext()->VSSetConstantBuffers(7, 1, &buf);
	DeviceManager::getInstance().
		GetContext()->PSSetConstantBuffers(7, 1, &buf);
}