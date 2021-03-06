#pragma once

#include <string>

class OBSBasic;

struct AgoraOutputHandler {
	OBSOutput              agoraOutput;
	bool                   agoraActive = false;
	OBSBasic               *main;

	std::string            outputType;

	OBSSignal              startAgora;
	OBSSignal              stopAgora;

	inline AgoraOutputHandler(OBSBasic *main_) : main(main_) {}

	virtual ~AgoraOutputHandler() {};

	virtual bool StartAgora(obs_service_t *service) = 0;
	virtual void StopAgora(bool force = false) = 0;
	virtual bool AgoraActive() const = 0;

	virtual void Update() = 0;

	inline bool Active() const
	{
		return agoraActive;
	}
};

AgoraOutputHandler *CreateAgoraOutputHandler(OBSBasic *main);