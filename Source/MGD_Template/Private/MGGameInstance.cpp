// Fill out your copyright notice in the Description page of Project Settings.


#include "MGGameInstance.h"
#include "OnlineSubsystem.h"
#include "OnlineSubsystemUtils.h"

void UMGGameInstance::Init()
{
	Super::Init();
	
	const IOnlineSubsystem* ossRef = Online::GetSubsystem(GetWorld());
	
	if (!ossRef)
		return;
	
	const IOnlineIdentityPtr identityRef = ossRef->GetIdentityInterface();
	
	if (!identityRef)
		return;

	// Bind to EOS Login complete function in the identity interface
	identityRef->OnLoginCompleteDelegates->AddUObject(this, &UMGGameInstance::EOSloginComplete);
}

void UMGGameInstance::LoginEOS()
{
	// This is the online subsystem
	// This gives us access to all of the online subsystem functions
	const IOnlineSubsystem* ossRef = Online::GetSubsystem(GetWorld());

	// If ossRef returns null
	// exit function
	if (!ossRef)
		return;

	// this is the identity interface for EOS
	// We can get info about the players acc from this interface
	// it also allows us to access auth (login/logout)
	const IOnlineIdentityPtr identityRef = ossRef->GetIdentityInterface();

	// if IdentityRef returns null
	// exit function
	if (!identityRef)
		return;

	// this sets up the type of account credentials that the login expects
	// the account portal is a login portal that opens when you try to login
	FOnlineAccountCredentials accCreds;
	accCreds.Id = FString();
	accCreds.Token = FString();
	accCreds.Type = FString(TEXT("accountportal"));

	// run the login portal
	identityRef->Login(0,accCreds);
	
}

bool UMGGameInstance::IsLoggedIn() const
{
	const IOnlineSubsystem* ossRef = Online::GetSubsystem(GetWorld());
	
	if (!ossRef)
		return false;
	
	const IOnlineIdentityPtr identityRef = ossRef->GetIdentityInterface();
	
	if (!identityRef)
		return false;

	return identityRef->GetLoginStatus(0) == ELoginStatus::LoggedIn;
	
}

// Get the display name from the identity
FString UMGGameInstance::GetDisplayName() const
{
	if(!IsLoggedIn())
		return FString(TEXT("Dispay Name"));

	const IOnlineIdentityPtr identityRef = Online::GetSubsystem(GetWorld())->GetIdentityInterface();

	return identityRef->GetPlayerNickname(0);
	
}

// Fucntion EOSLoginComplete
void UMGGameInstance::EOSloginComplete(int32 LocalUserNum, bool bWasSuccessful, const FUniqueNetId& UserId,
                                       const FString& Error)
{
	OnLoginComplete(bWasSuccessful, Error);
}
