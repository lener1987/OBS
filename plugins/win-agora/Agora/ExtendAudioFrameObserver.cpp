#include "ExtendAudioFrameObserver.hpp"
#include "agorartcengine.hpp"
#include <tchar.h>
extern AgoraRtcEngine*	pAgoraManager;


CExtendAudioFrameObserver::CExtendAudioFrameObserver()
{
	int rate = AgoraRtcEngine::GetInstance()->sampleRate;
	int channel = AgoraRtcEngine::GetInstance()->audioChannel;
	this->pCircleBuffer = new CicleBuffer(rate* channel * 2, 0);//(44100 * 2 * 2, 0);
	pPlayerData = new BYTE[0x800000];
}

CExtendAudioFrameObserver::~CExtendAudioFrameObserver()
{
	delete[] pPlayerData;
	pPlayerData = NULL; 
	delete pCircleBuffer;
	pCircleBuffer = NULL;
}

static inline int16_t MixerAddS16(int16_t var1, int16_t var2) {
	static const int32_t kMaxInt16 = 32767;
	static const int32_t kMinInt16 = -32768;
	int32_t tmp = (int32_t)var1 + (int32_t)var2;
	int16_t out16;

	if (tmp > kMaxInt16) {
		out16 = kMaxInt16;
	}
	else if (tmp < kMinInt16) {
		out16 = kMinInt16;
	}
	else {
		out16 = (int16_t)tmp;
	}

	return out16;
}

void MixerAddS16(int16_t* src1, const int16_t* src2, size_t size) {
	for (size_t i = 0; i < size; ++i) {
		src1[i] = MixerAddS16(src1[i], src2[i]);
	}
}

BOOL mixAudioData(char* psrc, char* pdst, int datalen)
{
	if (!psrc || !pdst || datalen <= 0)
	{
		return FALSE;
	}

	for (int i = 0; i < datalen; i++)
	{
		pdst[i] += psrc[i];
	}
	return TRUE;
}

bool CExtendAudioFrameObserver::onRecordAudioFrame(AudioFrame& audioFrame)
{
	if (!pPlayerData || !pCircleBuffer)
		return false;
	
	SIZE_T nSize = audioFrame.channels*audioFrame.samples * audioFrame.bytesPerSample;
	unsigned int datalen = 0;
	pCircleBuffer->readBuffer(this->pPlayerData, nSize, &datalen);

/*	TCHAR szBuf[MAX_PATH] = { 0 };
	if (nSize > datalen){
		_stprintf_s(szBuf, MAX_PATH, _T("*************** nSize=%d datalen=%d \n"), nSize, datalen);
		OutputDebugString(szBuf);
	}	
	else{
		_stprintf_s(szBuf, MAX_PATH, _T("############## nSize=%d datalen=%d \n"), nSize, datalen);
		OutputDebugString(szBuf);
	}
	*/
	if (nSize > 0 && datalen == nSize)
	{
		//int nMixLen = datalen > nSize ? nSize : datalen;
		int len = datalen / sizeof(int16_t);
		for (int i = 0; i < len; i++){
			int16_t* buffer = (int16_t*)(audioFrame.buffer) + i*sizeof(int16_t);
			int16_t* obsbuffer = (int16_t*)(pPlayerData)+i*sizeof(int16_t);
			int mix = *buffer + *obsbuffer;
			if (mix > 32767)
				*obsbuffer = 32767;
			else if (mix < -32768)
				*obsbuffer = -32768;
			else
				*obsbuffer = mix;
		}
		memcpy(audioFrame.buffer, pPlayerData, datalen);
	}
	
	return true;
}

bool CExtendAudioFrameObserver::onPlaybackAudioFrame(AudioFrame& audioFrame)
{
	SIZE_T nSize = audioFrame.channels*audioFrame.samples * 2;
	
	return true;
}

bool CExtendAudioFrameObserver::onMixedAudioFrame(AudioFrame& audioFrame)
{
	return true;
}

bool CExtendAudioFrameObserver::onPlaybackAudioFrameBeforeMixing(unsigned int uid, AudioFrame& audioFrame)
{
	return true;
}
