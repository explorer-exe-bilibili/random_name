#pragma once
#include <windows.h>
#include <mmdeviceapi.h>
#include <endpointvolume.h>
#include <audioclient.h>
#include <audiopolicy.h>
#pragma comment(lib, "Strmiids.lib")
int changevolum(int persent);

float SetVolum(int volume)
{
	HRESULT hr;
	IMMDeviceEnumerator* pDeviceEnumerator = 0;
	IMMDevice* pDevice = 0;
	IAudioEndpointVolume* pAudioEndpointVolume = 0;
	IAudioClient* pAudioClient = 0;
	ISimpleAudioVolume *pISimpleAudioVolume = 0;

	try {
		hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), (void**)&pDeviceEnumerator);
		if (FAILED(hr)) throw "CoCreateInstance";
		hr = pDeviceEnumerator->GetDefaultAudioEndpoint(eRender, eMultimedia, &pDevice);
		if (FAILED(hr)) throw "GetDefaultAudioEndpoint";
		hr = pDevice->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_ALL, NULL, (void**)&pAudioEndpointVolume);
		if (FAILED(hr)) throw "pDevice->Active";
		hr = pDevice->Activate(__uuidof(IAudioClient), CLSCTX_ALL, NULL, (void**)&pAudioClient);
		if (FAILED(hr)) throw "pDevice->Active";/*
		
		hr = pAudioClient->GetService(__uuidof(ISimpleAudioVolume), (void**)&pISimpleAudioVolume);
		float old;
		pISimpleAudioVolume->GetMasterVolume(&old);*/

		float fVolume;
		fVolume = volume / 100.0f;
		hr = pAudioEndpointVolume->SetMasterVolumeLevelScalar(fVolume, &GUID_NULL);
		if (FAILED(hr)) throw "SetMasterVolumeLevelScalar";


		pAudioClient->Release();
		pAudioEndpointVolume->Release();
		pDevice->Release();
		pDeviceEnumerator->Release();
		return TRUE;
	}

	catch (...) {
		if (pAudioClient) pAudioClient->Release();
		if (pAudioEndpointVolume) pAudioEndpointVolume->Release();
		if (pDevice) pDevice->Release();
		if (pDeviceEnumerator) pDeviceEnumerator->Release();
		throw;
	}
	return -1;
}


int changevolum(int persent) {
	IMMDevice *immdevice{};
	LPWSTR wsid;
	IMMDeviceEnumerator *immde{};
	IMMNotificationClient *immnc{};
	IAudioClient *iac{};
	ISimpleAudioVolume* isav{};
	float old;
	CoInitialize(NULL);
	CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), (void**)&immde);
	immde->GetDefaultAudioEndpoint(eRender, eConsole, &immdevice);
	immdevice->Activate(__uuidof(IAudioClient), CLSCTX_ALL, NULL, (void**)&iac);
	iac->GetService(__uuidof(ISimpleAudioVolume), (void**)&isav);
	isav->GetMasterVolume(&old);
	isav->SetMasterVolume(persent / 100,NULL);
	return (int)old * 100;
}