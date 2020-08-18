// Copyright 2020, Roberto De Ioris

#pragma once

#include "CoreMinimal.h"
#include "Misc/OutputDevice.h"
#include "Common/UdpSocketBuilder.h"
#include "SyslogSettings.h"

/**
 * 
 */
class UNREALSYSLOG_API FSyslogOutputDevice : public FOutputDevice
{
public:
	FSyslogOutputDevice();
	~FSyslogOutputDevice();

	bool IsInitialized() const { return bInitialized; }

	void UpdateServerCache();
protected:
	virtual void Serialize(const TCHAR* V, ELogVerbosity::Type Verbosity, const FName& Category) override;

	FSocket* Socket;
	bool bInitialized;
	uint64 MessageCounter;

	TArray<FSyslogServer> ServersCache;

};
