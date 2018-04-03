
#include "Common.h"


////////////////////////////////////////////////////////////////////////////// Main //////////////////////////////////////////////////////////////////////////
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, INT nCmdShow) {

	HWND hWnd;

	ID3D11RenderTargetView *pD3DRenderTargetView = nullptr;
	ID3D11Device *pD3DDevice = nullptr;
	ID3D11DeviceContext *pD3DImmediateContext = nullptr;
	IDXGISwapChain *pD3DSwapChain = nullptr;
	Font *fonts = nullptr;
	Vertex *vertices = nullptr;
	ID3D11VertexShader *pVertexShader = nullptr;
	ID3D11PixelShader *pPixelShader = nullptr;
	ID3D11InputLayout *pInputLayout = nullptr;
	HRESULT hr;

	hr = InitWindowAndD3D(hInstance, "DirectSound", &hWnd, &pD3DSwapChain, &pD3DRenderTargetView, &pD3DDevice, &pD3DImmediateContext);
	if (FAILED(hr)) {
		return hr;
	}
	hr = SetDepth(pD3DDevice, &pD3DImmediateContext, &pD3DRenderTargetView);
	if (FAILED(hr)) {
		return hr;
	}
	hr = InitLetter(&fonts);
	if (FAILED(hr)) {
		return hr;
	}

	hr = InitConstant(pD3DDevice, &pD3DImmediateContext);
	if (FAILED(hr)) {
		return hr;
	}
	hr = InitShader(pD3DDevice, &pD3DImmediateContext, &pVertexShader, &pPixelShader, &pInputLayout);
	if (FAILED(hr)) {
		return hr;
	}

	bool canSample = true;
	HQUERY hQuery = nullptr;
	HCOUNTER hCounter = nullptr;
	InitQueryAndCounter(&hQuery, &hCounter, &canSample);

	const string fpsText = "FPS : ";
	int tCount = 0;
	int fps = 0;
	unsigned long startTime = timeGetTime();

	const string cpuText = "CPU : ";
	unsigned long lastSampleTime = timeGetTime();
	int cpuUsage = 0;

	IDirectSound8 * pSoundDevice = nullptr;
	IDirectSoundBuffer *pSoundPrimaryBuffer = nullptr;

	hr = DirectSoundCreate8(nullptr, &pSoundDevice, nullptr);
	if (FAILED(hr)) {
		MessageBox(nullptr, "ERROR::CreateSoundDevice", "ERROR", MB_OK);
		return hr;
	}

	hr = pSoundDevice->SetCooperativeLevel(hWnd, DSSCL_PRIORITY);
	if (FAILED(hr)) {
		MessageBox(nullptr, "ERROR::SetCooperativeLevel", "ERROR", MB_OK);
		return hr;
	}

	DSBUFFERDESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.dwSize = sizeof(DSBUFFERDESC);
	bd.dwFlags = DSBCAPS_PRIMARYBUFFER | DSBCAPS_CTRLVOLUME;
	bd.guid3DAlgorithm = GUID_NULL;
	hr = pSoundDevice->CreateSoundBuffer(&bd, &pSoundPrimaryBuffer, nullptr);
	if (FAILED(hr)) {
		MessageBox(nullptr, "ERROR::CreatePrimaryBuffer", "ERROR", MB_OK);
		return hr;
	}

	WAVEFORMATEX wf;
	ZeroMemory(&wf, sizeof(wf));
	wf.wFormatTag = WAVE_FORMAT_PCM;
	wf.nSamplesPerSec = 44100;
	wf.wBitsPerSample = 16;
	wf.nChannels = 2;
	wf.nBlockAlign = wf.wBitsPerSample / 8 * wf.nChannels;
	wf.nAvgBytesPerSec = wf.nSamplesPerSec * wf.nBlockAlign;
	wf.cbSize = 0;
	hr = pSoundPrimaryBuffer->SetFormat(&wf);
	if (FAILED(hr)) {
		MessageBox(nullptr, "ERROR::SoundPrimaryBuffer::SetFormat", "ERROR", MB_OK);
		return hr;
	}

	INT error;
	FILE *filePtr = nullptr;
	UINT count;
	WaveHeader waveHeader;
	
	error = fopen_s(&filePtr, "./sound.wav", "rb");
	if (error) {
		MessageBox(nullptr, "ERROR::OpenWavFile", "ERROR", MB_OK);
		return false;
	}
	count = fread(&waveHeader, sizeof(waveHeader), 1, filePtr);
	if (count != 1) {
		MessageBox(nullptr, "ERROR::ReadWavFileHeader", "ERROR", MB_OK);
		return false;
	}
	if (waveHeader.chunkId[0] != 'R' || waveHeader.chunkId[1] != 'I' || waveHeader.chunkId[2] != 'F' || waveHeader.chunkId[3] != 'F') {
		MessageBox(nullptr, "ERROR::chunkId != RIFF", "ERROR", MB_OK);
		return false;
	}
	if (waveHeader.format[0] != 'W' || waveHeader.format[1] != 'A' || waveHeader.format[2] != 'V' || waveHeader.format[3] != 'E') {
		MessageBox(nullptr, "ERROR::format != WAVE", "ERROR", MB_OK);
		return false;
	}
	if (waveHeader.subChunkId[0] != 'f' || waveHeader.subChunkId[1] != 'm' || waveHeader.subChunkId[2] != 't' || waveHeader.subChunkId[3] != ' ') {
		MessageBox(nullptr, "ERROR::subChunkId != FMT", "ERROR", MB_OK);
		return false;
	}
	if (waveHeader.dataChunkId[0] != 'd' || waveHeader.dataChunkId[1] != 'a' || waveHeader.dataChunkId[2] != 't' || waveHeader.dataChunkId[3] != 'a') {
		MessageBox(nullptr, "ERROR::dataChunkId != data", "ERROR", MB_OK);
		return false;
	}
	if (waveHeader.audioFormat != WAVE_FORMAT_PCM) {
		MessageBox(nullptr, "ERROR::audioFormat != PCM", "ERROR", MB_OK);
		return false;
	}
	if (waveHeader.numChannels != 2) {
		MessageBox(nullptr, "ERROR::numChannels != 2", "ERROR", MB_OK);
		return false;
	}
	if (waveHeader.sampleRate != 44100) {
		MessageBox(nullptr, "ERROR::sampleRate != 44100", "ERROR", MB_OK);
		return false;
	}
	if (waveHeader.bitsPerSample != 16) {
		MessageBox(nullptr, "ERROR::bitsPerSample != 16", "ERROR", MB_OK);
		return false;
	}
	fseek(filePtr, sizeof(waveHeader), SEEK_SET);


	UCHAR *waveData = new UCHAR[waveHeader.dataSize];
	count = fread(waveData, 1, waveHeader.dataSize, filePtr);
	if (count != waveHeader.dataSize) {
		MessageBox(nullptr, "ERROR::ReadSize != waveHeader.dataSize", "ERROR", MB_OK);
		return false;
	}

	error = fclose(filePtr);
	if (error != 0) {
		MessageBox(nullptr, "ERROR::CloseFile", "ERROR", MB_OK);
		return false;
	}

	WAVEFORMATEX waveFormatEx;
	ZeroMemory(&waveFormatEx, sizeof(waveFormatEx));
	waveFormatEx.wFormatTag = WAVE_FORMAT_PCM;
	waveFormatEx.nSamplesPerSec = 44100;
	waveFormatEx.wBitsPerSample = 16;
	waveFormatEx.nChannels = 2;
	waveFormatEx.nBlockAlign = waveFormatEx.wBitsPerSample / 8 * waveFormatEx.nChannels;
	waveFormatEx.nAvgBytesPerSec = waveFormatEx.nSamplesPerSec * waveFormatEx.nBlockAlign;
	waveFormatEx.cbSize = 0;

	DSBUFFERDESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(bufferDesc));
	bufferDesc.dwSize = sizeof(DSBUFFERDESC);
	bufferDesc.dwFlags = DSBCAPS_CTRLVOLUME;
	bufferDesc.lpwfxFormat = &waveFormatEx;
	bufferDesc.dwBufferBytes = waveHeader.dataSize;
	bufferDesc.guid3DAlgorithm = GUID_NULL;

	IDirectSoundBuffer *pSoundTmpBuffer = nullptr;
	hr = pSoundDevice->CreateSoundBuffer(&bufferDesc, &pSoundTmpBuffer, nullptr);
	if (FAILED(hr)) {
		MessageBox(nullptr, "ERROR::CreateSoundTmpBuffer", "ERROR", MB_OK);
		return hr;
	}

	IDirectSoundBuffer8 *pSoundSecondaryBuffer = nullptr;
	hr = pSoundTmpBuffer->QueryInterface(IID_IDirectSoundBuffer8, reinterpret_cast<LPVOID*>(&pSoundSecondaryBuffer));
	if (FAILED(hr)) {
		MessageBox(nullptr, "ERROR::SoundTmpBuffer::QueryInterface", "ERROR", MB_OK);
		return hr;
	}
	pSoundTmpBuffer->Release();

	UCHAR *bufferPtr = nullptr;
	ULONG bufferSize;
	hr = pSoundSecondaryBuffer->Lock(0, waveHeader.dataSize, reinterpret_cast<LPVOID*>(&bufferPtr), &bufferSize, nullptr, 0, 0);
	if (FAILED(hr)) {
		MessageBox(nullptr, "ERROR::pSoundSecondaryBuffer::Lock", "ERROR", MB_OK);
		return hr;
	}
	memcpy(bufferPtr, waveData, waveHeader.dataSize);
	hr = pSoundSecondaryBuffer->Unlock(bufferPtr, bufferSize, nullptr, 0);
	if (FAILED(hr)) {
		MessageBox(nullptr, "ERROR::pSoundSecondaryBuffer::UNLock", "ERROR", MB_OK);
		return hr;
	}
	delete[] waveData;
	waveData = 0;

	hr = pSoundSecondaryBuffer->SetCurrentPosition(0);
	if (FAILED(hr)) {
		MessageBox(nullptr, "ERROR::pSoundSecondaryBuffer::SetCurrentPosition", "ERROR", MB_OK);
		return hr;
	}

	hr = pSoundSecondaryBuffer->SetVolume(DSBVOLUME_MAX);
	if (FAILED(hr)) {
		MessageBox(nullptr, "ERROR::pSoundSecondaryBuffer::SetVolume", "ERROR", MB_OK);
		return hr;
	}

	hr = pSoundSecondaryBuffer->Play(0, 0, 0);
	if (FAILED(hr)) {
		MessageBox(nullptr, "ERROR::pSoundSecondaryBuffer::Play", "ERROR", MB_OK);
		return hr;
	}

	MSG msg;
	ZeroMemory(&msg, sizeof(MSG));
	while (msg.message != WM_QUIT) {
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		float color[] = { 0.0f , 0.0f , 0.0f , 1.0f };
		pD3DImmediateContext->ClearRenderTargetView(pD3DRenderTargetView, color);
		PrintText(fpsText + to_string(GetFPS(startTime, fps, tCount)), -620, 300, fonts, pVertexShader, pPixelShader, pInputLayout, pD3DDevice, &vertices, &pD3DImmediateContext);
		PrintText(cpuText + to_string(GetCPUUsage(canSample, cpuUsage, lastSampleTime, hQuery, hCounter)), -620, 240, fonts, pVertexShader, pPixelShader, pInputLayout, pD3DDevice, &vertices, &pD3DImmediateContext);
		pD3DSwapChain->Present(0, 0);
	}

	///////////////////////////////////////////////////////////////////////////// Release ///////////////////////////////////////////////////////////////////////////

	pSoundTmpBuffer->Release();
	pSoundDevice->Release();
	pSoundSecondaryBuffer->Release();
	pSoundPrimaryBuffer->Release();
	pD3DImmediateContext->Release();
	pD3DRenderTargetView->Release();
	pD3DDevice->Release();
	pVertexShader->Release();
	pPixelShader->Release();
	pInputLayout->Release();
	delete[] fonts;
	delete[] vertices;
	fonts = 0;
	vertices = 0;
	PdhCloseQuery(hQuery);

	return 0;
}


