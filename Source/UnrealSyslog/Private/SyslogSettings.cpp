// Copyright 2020 Roberto De Ioris.


#include "SyslogSettings.h"
#include "UnrealSyslog.h"

#if WITH_EDITOR

void USyslogSettings::PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent)
{
	FUnrealSyslogModule::Get().UpdateServerCache();

	Super::PostEditChangeChainProperty(PropertyChangedEvent);
}
#endif // WITH_EDITOR