#include <D3D11.h>
#include <minwinbase.h>
#include <iostream>
#include <Windows.h>
#include <D3DX10math.h>
#include <D3DX11async.h>
#include <DirectXMath.h>
#include <D3Dcompiler.h>
#include <Assimp/Importer.hpp>
#include <Assimp/postprocess.h>
#include <Assimp/scene.h>
#include <vector>


const int width = 1920, height = 1080;

HRESULT CALLBACK WndProc(const HWND hWnd, const UINT uMsg, const WPARAM wParam, const LPARAM lParam);

using namespace std;
using namespace DirectX;
using namespace Assimp;

struct Vertex
{
	XMFLOAT3 pos;
	XMFLOAT2 texcoord;
	XMFLOAT3 normal;
	XMFLOAT4 Kd;  //材质漫反射系数 
	XMFLOAT4 Ks;  //材质的高光系数 
	XMFLOAT3 tangent;	// 切线
	XMFLOAT3 bitangent; // 副切线
};

struct Constant {
	XMMATRIX world;
	XMMATRIX view;
	XMMATRIX projection;
};

class Mesh {

	ID3D11Buffer *pIndexBufferObject;
	ID3D11Buffer *pVertexBufferObject;
	ID3D11VertexShader *pVertexShaderObject;
	ID3D11PixelShader *pPixelShaderObject;
	ID3D11InputLayout *pInputLayout;
	ID3D11SamplerState *pSamplerState;
	ID3D11ShaderResourceView *pShaderResourceView;

public:

	Vertex * vertices;
	UINT *indices;
	vector<std::string> texturePaths;

	UINT vertexCount;
	UINT indexCount;

	Mesh(): pIndexBufferObject(nullptr), pVertexBufferObject(nullptr), pVertexShaderObject(nullptr),
	        pPixelShaderObject(nullptr), pInputLayout(nullptr), pSamplerState(nullptr), pShaderResourceView(nullptr),
	        vertices(nullptr), indices(nullptr),  vertexCount(0), indexCount(0) {
	}

	Mesh(const UINT vertexCount, const UINT indexCount): pIndexBufferObject(nullptr),
	                                                                               pVertexBufferObject(nullptr),
	                                                                               pVertexShaderObject(nullptr),
	                                                                               pPixelShaderObject(nullptr),
	                                                                               pInputLayout(nullptr),
	                                                                               pSamplerState(nullptr),
	                                                                               pShaderResourceView(nullptr) {
		this->vertexCount = vertexCount;
		this->indexCount = indexCount;
		this->vertices = new Vertex[vertexCount];
		this->indices = new UINT[indexCount];
	}

	HRESULT Init(ID3D11Device **pDevice , const char* vertexShaderPath , const char* pixelShaderPath) {
		HRESULT hr;
		D3D11_BUFFER_DESC vertexBufferDesc;
		ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));
		vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		vertexBufferDesc.ByteWidth = sizeof(Vertex) * vertexCount;
		vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		D3D11_SUBRESOURCE_DATA verticesSourceData;
		ZeroMemory(&verticesSourceData, sizeof(D3D11_SUBRESOURCE_DATA));
		verticesSourceData.pSysMem = vertices;

		D3D11_BUFFER_DESC indexDesc;
		ZeroMemory(&indexDesc, sizeof(indexDesc));
		indexDesc.Usage = D3D11_USAGE_IMMUTABLE;
		indexDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		indexDesc.ByteWidth = sizeof(UINT) * indexCount;
		D3D11_SUBRESOURCE_DATA indexData;
		ZeroMemory(&indexData, sizeof(indexData));
		indexData.pSysMem = indices;
		hr = (*pDevice)->CreateBuffer(&vertexBufferDesc, &verticesSourceData, &pVertexBufferObject);
		if (FAILED(hr)) { return hr; }
		hr = (*pDevice)->CreateBuffer(&indexDesc, &indexData, &pIndexBufferObject);
		if (FAILED(hr)) { return hr; }

		ID3D10Blob* pErrorMessage = nullptr;
		ID3D10Blob* pVertexShaderBlob = nullptr;
		ID3D10Blob* pPixelShaderBlob = nullptr;

		hr = D3DX11CompileFromFile(vertexShaderPath, nullptr, nullptr, "main", "vs_5_0", D3DCOMPILER_STRIP_DEBUG_INFO, 0, nullptr, &pVertexShaderBlob, &pErrorMessage, nullptr);
		if (FAILED(hr)) {
			if (pErrorMessage) MessageBox(NULL, static_cast<CHAR*>(pErrorMessage->GetBufferPointer()), "Error", MB_OK);
			else MessageBox(NULL, "vertexShader File Not Found", "Error", MB_OK);
			return hr;
		}
		hr = D3DX11CompileFromFile(pixelShaderPath, nullptr, nullptr, "main", "ps_5_0", D3DCOMPILER_STRIP_DEBUG_INFO, 0, nullptr, &pPixelShaderBlob, &pErrorMessage, nullptr);
		if (FAILED(hr)) {
			if (pErrorMessage) MessageBox(NULL, static_cast<CHAR*>(pErrorMessage->GetBufferPointer()), "Error", MB_OK);
			else MessageBox(NULL, "pixelShader File Not Found", "Error", MB_OK);
			return hr;
		}

		hr = (*pDevice)->CreateVertexShader(pVertexShaderBlob->GetBufferPointer(), pVertexShaderBlob->GetBufferSize(), nullptr, &pVertexShaderObject);
		if (FAILED(hr)) {
			MessageBox(NULL, "ERROR::CreateVertexShader", "Error", MB_OK);
			return hr;
		}

		hr = (*pDevice)->CreatePixelShader(pPixelShaderBlob->GetBufferPointer(), pPixelShaderBlob->GetBufferSize(), nullptr, &pPixelShaderObject);
		if (FAILED(hr)) {
			MessageBox(NULL, "ERROR::CreatePixelShader", "Error", MB_OK);
			return hr;
		}

		D3D11_SAMPLER_DESC samplerDesc;
		samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.MipLODBias = 0.0f;
		samplerDesc.MaxAnisotropy = 1;
		samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
		samplerDesc.BorderColor[0] = 0;
		samplerDesc.BorderColor[1] = 0;
		samplerDesc.BorderColor[2] = 0;
		samplerDesc.BorderColor[3] = 0;
		samplerDesc.MinLOD = 0;
		samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

		hr = (*pDevice)->CreateSamplerState(&samplerDesc, &pSamplerState);
		if (FAILED(hr)) {
			MessageBox(nullptr, "ERROR::CreateSampler", "Error", MB_OK);
			return hr;
		}

		hr = D3DX11CreateShaderResourceViewFromFile(*pDevice, texturePaths[0].c_str() , nullptr, nullptr, &pShaderResourceView, nullptr);
		if (FAILED(hr)) {
			MessageBox(nullptr, "ERROR::CreateShaderResourceView", "Error", MB_OK);
			return hr;
		}

		D3D11_INPUT_ELEMENT_DESC layout[] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 } ,
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 } ,
			{ "NORMAL", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
		};
		hr = (*pDevice)->CreateInputLayout(layout, ARRAYSIZE(layout), pVertexShaderBlob->GetBufferPointer(), pVertexShaderBlob->GetBufferSize(), &pInputLayout);
		if (FAILED(hr)) {
			MessageBox(NULL, "ERROR::CreateInputLayout", "Error", MB_OK);
			return hr;
		}

		pVertexShaderBlob->Release();
		pPixelShaderBlob->Release();
		if (pErrorMessage) {
			pErrorMessage->Release();
		}
	}

	void Draw(ID3D11DeviceContext **pImmediateContext) {
		UINT stride = sizeof(Vertex);
		UINT offset = 0;
		(*pImmediateContext)->IASetVertexBuffers(0, 1, &pVertexBufferObject, &stride, &offset);
		(*pImmediateContext)->IASetIndexBuffer(pIndexBufferObject, DXGI_FORMAT_R32_UINT, 0);	
		(*pImmediateContext)->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		(*pImmediateContext)->PSSetShaderResources(0, 1, &pShaderResourceView);
		(*pImmediateContext)->PSSetSamplers(0, 1, &pSamplerState);
		(*pImmediateContext)->VSSetShader(pVertexShaderObject, nullptr, 0);
		(*pImmediateContext)->PSSetShader(pPixelShaderObject, nullptr, 0);
		(*pImmediateContext)->IASetInputLayout(pInputLayout);
		(*pImmediateContext)->DrawIndexed(indexCount, 0, 0);
	}

	void Release() {
		delete[] vertices;
		vertices = 0;
		delete[] indices;
		indices = 0;
		pIndexBufferObject->Release();
		pVertexBufferObject->Release();
	}
};

vector<Mesh> LoadModelUsingAssimp(const char* path) {
	Importer imp;
	const aiScene *scene = imp.ReadFile(path,
		aiProcess_GenNormals | aiProcess_Triangulate |
		aiProcess_FixInfacingNormals | aiProcess_FlipWindingOrder |
		aiProcess_GenUVCoords | aiProcess_FlipUVs);
	if (!scene) {
		MessageBox(nullptr, "ERROR::ReadOBJ", "ERROR", MB_OK);
		return vector<Mesh>();
	}

	vector<Mesh> meshes(scene->mNumMeshes);

	for (unsigned i = 0; i < scene->mNumMeshes ; ++i) {
		auto &mesh = meshes[i];
		const auto aiMesh = scene->mMeshes[i];
		mesh = Mesh(aiMesh->mNumVertices, aiMesh->mNumFaces * 3);

		for (size_t j = 0; j < aiMesh->mNumVertices; ++j) {
			mesh.vertices[j].pos = XMFLOAT3(aiMesh->mVertices[j].x, aiMesh->mVertices[j].y, aiMesh->mVertices[j].z);
			if (aiMesh->HasTextureCoords(0)) {
				mesh.vertices[j].texcoord = XMFLOAT2(aiMesh->mTextureCoords[0][j].x, aiMesh->mTextureCoords[0][j].y);
			}
			else {
				mesh.vertices[j].texcoord = XMFLOAT2(0.0f, 0.0f);
			}
			if (aiMesh->HasNormals()) {
				mesh.vertices[j].normal = XMFLOAT3(aiMesh->mNormals[j].x, aiMesh->mNormals[j].y, aiMesh->mNormals[j].z);
			}
			else {
				mesh.vertices[j].normal = XMFLOAT3(0.0f, 0.0f, -1.0f);
			}
		}

		for (size_t j = 0; j < aiMesh->mNumFaces; ++j) {
			for (size_t l = 0; l < aiMesh->mFaces[j].mNumIndices; ++l) {
				mesh.indices[j * aiMesh->mFaces[j].mNumIndices + l] = aiMesh->mFaces[j].mIndices[l];
			}
		}


		const auto material = scene->mMaterials[aiMesh->mMaterialIndex];
		mesh.texturePaths = vector<std::string>(material->GetTextureCount(aiTextureType_DIFFUSE) , std::string("./Resources/Low-Poly\ Spider/textures/"));
		for (auto& str : mesh.texturePaths) {
			aiString aiStr;
			material->GetTexture(aiTextureType_DIFFUSE, 0, &aiStr);
			str += aiStr.C_Str();
		}
	}

	return meshes;
}


/**
 * \brief 启动主方法
 * \param hInstance 程序实例
 * \param hPrevInstance 上一个程序实例
 * \param lpCmdLine 命令行参数
 * \param nCmdShow 显示方式
 * \return 退出码
 */
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, INT nCmdShow) {

	///////////////////////////////////////////////////////////////// INIT WIN32 ///////////////////////////////////////////////////////

	WNDCLASS wc;
	ZeroMemory(&wc, sizeof(WNDCLASS));
	wc.hInstance = hInstance;
	wc.lpfnWndProc = static_cast<WNDPROC>(WndProc);
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wc.hIcon = LoadIcon(nullptr, IDC_ICON);
	wc.hbrBackground = static_cast<HBRUSH>(GetStockObject(WHITE_BRUSH));
	wc.lpszClassName = "mWndClass";
	wc.lpszMenuName = nullptr;
	wc.style = CS_VREDRAW | CS_HREDRAW;
	if (!RegisterClass(&wc)) {
		MessageBox(nullptr, "ERROR::RegisterClass_Error", "Error", MB_OK);
		return -1;
	}
	const auto hWnd = CreateWindow("mWndClass", "DX11Tutorial-Mesh Render", WS_EX_TOPMOST | WS_OVERLAPPEDWINDOW, 0, 0, width, height, nullptr, nullptr, hInstance, nullptr);
	if (!hWnd) {
		MessageBox(nullptr, "ERROR::CreateWindow_Error", "Error", MB_OK);
		return -1;
	}

	ShowWindow(hWnd, SW_NORMAL);
	UpdateWindow(hWnd);

	////////////////////////////////////////////////////////////////////////////////// INIT DX //////////////////////////////////////////////////////////////

	DXGI_SWAP_CHAIN_DESC dc;
	ZeroMemory(&dc, sizeof(DXGI_SWAP_CHAIN_DESC));
	dc.BufferDesc.Width = width;
	dc.BufferDesc.Height = height;
	dc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	dc.BufferDesc.RefreshRate.Numerator = 60;
	dc.BufferDesc.RefreshRate.Denominator = 1;
	dc.BufferCount = 1;
	dc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	dc.SampleDesc.Count = 1;
	dc.SampleDesc.Quality = 0;
	dc.OutputWindow = hWnd;
	dc.Windowed = true;

	UINT createDeviceFlags = 0;
#ifdef _DEBUG
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	IDXGISwapChain *pSwapChain = nullptr;
	ID3D11RenderTargetView *pRenderTargetView = nullptr;
	ID3D11Device *pDevice = nullptr;
	ID3D11DeviceContext *pImmediateContext = nullptr;
	D3D_FEATURE_LEVEL eFeatureLevel = D3D_FEATURE_LEVEL_11_0;

	D3D_FEATURE_LEVEL aFeatureLevels[] = {
		D3D_FEATURE_LEVEL_11_0 ,
		D3D_FEATURE_LEVEL_10_1 ,
		D3D_FEATURE_LEVEL_10_0
	};
	const auto nFeatureLevel = ARRAYSIZE(aFeatureLevels);

	HRESULT hr = S_OK;
	hr = D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		createDeviceFlags,
		aFeatureLevels,
		nFeatureLevel,
		D3D11_SDK_VERSION,
		&dc,
		&pSwapChain,
		&pDevice,
		&eFeatureLevel,
		&pImmediateContext);
	if (FAILED(hr)) {
		MessageBox(nullptr, "ERROR::CreateDeviceAndSwapChain_Error", "Error", MB_OK);
		return hr;
	}

	ID3D11Texture2D *pTexture2D = nullptr;
	hr = pSwapChain->GetBuffer(0, __uuidof(pTexture2D), reinterpret_cast<LPVOID*>(&pTexture2D));
	if (FAILED(hr)) {
		MessageBox(nullptr, "ERROR::SetTextureBuffer_Error", "Error", MB_OK);
		return hr;
	}
	hr = pDevice->CreateRenderTargetView(pTexture2D, nullptr, &pRenderTargetView);
	if (FAILED(hr)) {
		MessageBox(nullptr, "ERROR::CreateRenderTargetView_Error", "Error", MB_OK);
		return hr;
	}
	


	pImmediateContext->OMSetRenderTargets(1, &pRenderTargetView, nullptr);
	D3D11_VIEWPORT dv{ 0 , 0 , width , height , 0 , 1 };
	pImmediateContext->RSSetViewports(1, &dv);

	///////////////////////////////////////////////////////////// Model ///////////////////////////////////////////////////////

	vector<Mesh> meshes = LoadModelUsingAssimp("./Resources/Low-Poly\ Spider/spider.x");
	for (auto& mesh : meshes) mesh.Init(&pDevice , "./vertexShader.hlsl" , "./pixelShader.hlsl");

	///////////////////////////////////////////////////////////// Constant ////////////////////////////////////////////////////

	Constant cb = {
		XMMatrixIdentity() * XMMatrixRotationX(45.0f),
		XMMatrixLookAtLH(
			XMVectorSet(0.0f, 5.0f, -10.0f, 0.0f),
			XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f),
			XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f)
		),
		XMMatrixPerspectiveFovLH(90, static_cast<FLOAT>(width) / height, 0.1f, 1000.0f)
	};

	D3D11_BUFFER_DESC constantBufferDesc;
	ZeroMemory(&constantBufferDesc, sizeof(constantBufferDesc));
	constantBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	constantBufferDesc.BindFlags = D3D10_BIND_CONSTANT_BUFFER;
	constantBufferDesc.ByteWidth = sizeof(Constant);

	D3D11_SUBRESOURCE_DATA constantSourceData;
	ZeroMemory(&constantSourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	constantSourceData.pSysMem = &cb;

	ID3D11Buffer *pConstantBuffer = nullptr;
	hr = pDevice->CreateBuffer(&constantBufferDesc, &constantSourceData, &pConstantBuffer);
	if (FAILED(hr)) {
		MessageBox(nullptr, "ERROR::CreateConstantBuffer", "ERROR", MB_OK);
		return hr;
	}

	////////////////////////////////////////////////////////////////////// MSG ///////////////////////////////////////////////////////////
	MSG msg;
	ZeroMemory(&msg, sizeof(MSG));
	while (msg.message != WM_QUIT) {
		float color[] = { 0.1f , 0.2f , 0.3f , 1.0f };
		pImmediateContext->ClearRenderTargetView(pRenderTargetView, color);
		cb.world = cb.world * XMMatrixTranspose(XMMatrixRotationY(0.0001f));
		pImmediateContext->UpdateSubresource(pConstantBuffer, 0, nullptr, &cb, 0, 0);
		pImmediateContext->VSSetConstantBuffers(0, 1, &pConstantBuffer);

		for (auto& mesh : meshes) mesh.Draw(&pImmediateContext);

		pSwapChain->Present(0, 0);
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	pConstantBuffer->Release();
	pSwapChain->Release();
	pImmediateContext->Release();
	pRenderTargetView->Release();
	pDevice->Release();
	pTexture2D->Release();



	return 0;
}



/**
 * \brief 消息处理回调函数
 * \param hWnd 窗口句柄
 * \param uMsg 消息
 * \param wParam 消息附加参数
 * \param lParam 消息附加参数
 * \return 返回默认消息函数
 */
HRESULT CALLBACK WndProc(const HWND hWnd, const UINT uMsg, const WPARAM wParam, const LPARAM lParam) {
	switch (uMsg) {
	case WM_DESTROY: PostQuitMessage(0); break;
	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE) {
			DestroyWindow(hWnd);
		}
		break;
	default: break;
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}