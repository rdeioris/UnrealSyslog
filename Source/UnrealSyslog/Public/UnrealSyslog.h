// Copyright 2020 Roberto De Ioris.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "SyslogOutputDevice.h"

class FUnrealSyslogModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	virtual void UpdateServerCache();

	static FUnrealSyslogModule& Get();

protected:
	TSharedPtr<FSyslogOutputDevice> SyslogOutputDevice;
};
