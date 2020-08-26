// Copyright 2020 Roberto De Ioris.


#include "SyslogOutputDevice.h"
#include "SyslogSettings.h"
#include "Misc/CString.h"
#include "Interfaces/IPv4/IPv4Endpoint.h"
#include "Misc/App.h"


FSyslogOutputDevice::FSyslogOutputDevice()
{
	bInitialized = false;
	Socket = FUdpSocketBuilder("UDPSyslogSender").AsNonBlocking();
	if (!Socket)
	{
		return;
	}

	MessageCounter = 0;

	UpdateServerCache();

	bInitialized = true;
}

void FSyslogOutputDevice::UpdateServerCache()
{
	ServersCache.Empty();
	TArray<FSyslogServer> Servers = GetDefault<USyslogSettings>()->SyslogServers;
	// Cache only valid servers
	for (FSyslogServer& Server : Servers)
	{
		ISocketSubsystem* SocketSubsystem = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM);

		FAddressInfoResult AddressInfoResult = SocketSubsystem->GetAddressInfo(*Server.Address, nullptr, EAddressInfoFlags::Default, NAME_None);
		if (AddressInfoResult.Results.Num() < 1)
		{
			continue;
		}

		Server.DestinationEndPoint = AddressInfoResult.Results[0].Address;
		Server.DestinationEndPoint->SetPort(Server.Port);

		ServersCache.Add(Server);
	}
}

FSyslogOutputDevice::~FSyslogOutputDevice()
{
}

void FSyslogOutputDevice::Serialize(const TCHAR* V, ELogVerbosity::Type Verbosity, const FName& Category)
{

	int32 Severity = 6;
	switch (Verbosity)
	{
	case ELogVerbosity::Log:
		Severity = 6;
		break;
	case ELogVerbosity::Warning:
		Severity = 4;
		break;
	case ELogVerbosity::Error:
		Severity = 3;
		break;
	case ELogVerbosity::Fatal:
		Severity = 2;
		break;
	case ELogVerbosity::Display:
		Severity = 5;
		break;
	case ELogVerbosity::Verbose:
	case ELogVerbosity::VeryVerbose:
		Severity = 7;
		break;
	default:
		Severity = 6;
		break;
	}

	uint32 Pid = FPlatformProcess::GetCurrentProcessId();

	FDateTime Now = FDateTime::Now();
	FDateTime UtcNow = FDateTime::UtcNow();

	for (FSyslogServer& Server : ServersCache)
	{
		if (Server.CategoriesWhiteList.Num() > 0)
		{
			if (!Server.CategoriesWhiteList.Contains(Category))
			{
				continue;
			}
		}

		if (Server.CategoriesBlackList.Num() > 0)
		{
			if (Server.CategoriesBlackList.Contains(Category))
			{
				continue;
			}
		}

		FDateTime Timestamp = Server.bUTC ? UtcNow : Now;
		TArray<uint8> Data;

		FString LogHeader;
		if (Server.Format == ESyslogFormat::RFC5424)
		{
			LogHeader = FString::Printf(TEXT("<%u>1 %s %s %s %u %llu - "),
				Server.Facility * 8 + Severity,
				*Timestamp.ToIso8601(),
				FPlatformProcess::ComputerName(),
				Server.Tag.IsEmpty() ? FApp::GetProjectName() : *Server.Tag,
				Pid,
				MessageCounter);

			Data.Append((const uint8*)TCHAR_TO_ANSI(*LogHeader), FCStringAnsi::Strlen(TCHAR_TO_ANSI(*LogHeader)));

			if (Server.bAddBOM)
			{
				Data.Add(0xEF);
				Data.Add(0xBB);
				Data.Add(0xBF);
			}
		}
		else if (Server.Format == ESyslogFormat::RFC3164)
		{
			TArray<FString> MonthsNames;
			MonthsNames.Add("Jan");
			MonthsNames.Add("Feb");
			MonthsNames.Add("Mar");
			MonthsNames.Add("Apr");
			MonthsNames.Add("May");
			MonthsNames.Add("Jun");
			MonthsNames.Add("Jul");
			MonthsNames.Add("Aug");
			MonthsNames.Add("Sep");
			MonthsNames.Add("Oct");
			MonthsNames.Add("Nov");
			MonthsNames.Add("Dec");

			LogHeader = FString::Printf(TEXT("<%u>%s %02d %02d:%02d:%02d %s %s[%u]: "),
				Server.Facility * 8 + Severity,
				*MonthsNames[Timestamp.GetMonth() - 1],
				Timestamp.GetDay(),
				Timestamp.GetHour(),
				Timestamp.GetMinute(),
				Timestamp.GetSecond(),
				FPlatformProcess::ComputerName(),
				Server.Tag.IsEmpty() ? FApp::GetProjectName() : *Server.Tag,
				Pid);

			Data.Append((const uint8*)TCHAR_TO_ANSI(*LogHeader), FCStringAnsi::Strlen(TCHAR_TO_ANSI(*LogHeader)));
		}
		else
		{
			continue;
		}

		Data.Append((const uint8*)(Server.bUTF8 ? TCHAR_TO_UTF8(V) : TCHAR_TO_ANSI(V)),
			FCStringAnsi::Strlen(Server.bUTF8 ? TCHAR_TO_UTF8(V) : TCHAR_TO_ANSI(V)));

		int32 Count = 0;
		Socket->SendTo(Data.GetData(), Data.Num(), Count, *Server.DestinationEndPoint.Get());
	}

	MessageCounter++;
}
