// Copyright 2020 Roberto De Ioris.

#include "UnrealSyslog.h"
#if WITH_EDITOR
#include "ISettingsModule.h"
#include "SyslogSettings.h"
#endif

#define LOCTEXT_NAMESPACE "FUnrealSyslogModule"

void FUnrealSyslogModule::StartupModule()
{
#if WITH_EDITOR
	ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings");
	if (SettingsModule)
	{
		SettingsModule->RegisterSettings("Project", "Plugins", "Syslog Plugin",
			LOCTEXT("ProjectSettings_Label", "Syslog Plugin"),
			LOCTEXT("ProjectSettings_Description", "Configure Syslog plugin global settings"),
			GetMutableDefault<USyslogSettings>()
		);
	}
#endif // WITH_EDITOR

	SyslogOutputDevice = MakeShared<FSyslogOutputDevice>();

	if (SyslogOutputDevice.IsValid() && SyslogOutputDevice->IsInitialized())
	{
		GLog->AddOutputDevice(SyslogOutputDevice.Get());
		GLog->SerializeBacklog(SyslogOutputDevice.Get());
	}

}

void FUnrealSyslogModule::ShutdownModule()
{
	if (SyslogOutputDevice.IsValid() && SyslogOutputDevice->IsInitialized())
	{
		GLog->RemoveOutputDevice(SyslogOutputDevice.Get());
	}
#if WITH_EDITOR
	ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings");
	if (SettingsModule)
	{
		SettingsModule->UnregisterSettings("Project", "Plugins", "Syslog Plugin");
	}
#endif // WITH_EDITOR
}

FUnrealSyslogModule& FUnrealSyslogModule::Get()
{
	static FUnrealSyslogModule* Singleton = nullptr;
	if (Singleton == nullptr)
	{
		Singleton = &FModuleManager::LoadModuleChecked<FUnrealSyslogModule>("UnrealSyslog");
	}
	return *Singleton;
}

void FUnrealSyslogModule::UpdateServerCache()
{
	if (SyslogOutputDevice.IsValid() && SyslogOutputDevice->IsInitialized())
	{
		SyslogOutputDevice->UpdateServerCache();
	}
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FUnrealSyslogModule, UnrealSyslog)