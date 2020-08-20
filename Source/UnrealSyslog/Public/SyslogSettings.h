// Copyright 2020 Roberto De Ioris.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "IPAddress.h"
#include "SyslogSettings.generated.h"

UENUM()
enum class ESyslogFormat: uint8
{
	RFC5424 UMETA(DisplayName="IETF (RFC 5424)"),
	RFC3164 UMETA(DisplayName = "BSD (RFC 3164)"),
};

USTRUCT()
struct FSyslogServer
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Syslog")
	FString Address;

	UPROPERTY(EditAnywhere, Category = "Syslog")
	int32 Port;

	UPROPERTY(EditAnywhere, Category = "Syslog")
	int32 Facility;

	UPROPERTY(EditAnywhere, Category = "Syslog")
	ESyslogFormat Format;

	UPROPERTY(EditAnywhere, Category = "Syslog")
	bool bUTF8;

	UPROPERTY(EditAnywhere, Category = "Syslog")
	bool bAddBOM;

	UPROPERTY(EditAnywhere, Category = "Syslog")
	FString Tag;

	UPROPERTY(EditAnywhere, Category = "Syslog")
	TArray<FName> CategoriesWhiteList;

	UPROPERTY(EditAnywhere, Category = "Syslog")
	TArray<FName> CategoriesBlackList;

	TSharedPtr<FInternetAddr> DestinationEndPoint;

	FSyslogServer()
	{
		Address = "127.0.0.1";
		Port = 514;
		Facility = 1;
		Format = ESyslogFormat::RFC5424;
		bUTF8 = true;
		bAddBOM = false;
	}
};

/**
 * 
 */
UCLASS(Config = Engine, DefaultConfig, NotPlaceable)
class UNREALSYSLOG_API USyslogSettings : public UObject
{
	GENERATED_BODY()
	
public:
	UPROPERTY(Config, EditAnywhere, Category = "Syslog")
	TArray<FSyslogServer> SyslogServers;

#if WITH_EDITOR
	virtual void PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent) override;
#endif

};
