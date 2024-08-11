// Fill out your copyright notice in the Description page of Project Settings.


#include "MGGameInstance.h"
#include "OnlineSubsystem.h"
#include "OnlineSubsystemUtils.h"
#include "GameFramework/GameModeBase.h"
#include "Online/OnlineSessionNames.h"

UMGGameInstance::UMGGameInstance()
{
	FoundSessions = MakeShareable(new FOnlineSessionSearch);
	FoundSessions->MaxSearchResults = 10;
	FoundSessions->bIsLanQuery = true;
	FoundSessions->QuerySettings.Set(SEARCH_LOBBIES, true, EOnlineComparisonOp::Equals);
	
}

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

	const IOnlineSessionPtr sessionRef = ossRef->GetSessionInterface();

	if(!sessionRef)
		return;

	sessionRef->OnCreateSessionCompleteDelegates.AddUObject(this, &UMGGameInstance::SessionCreateComplete);

	// when find session function has been called, wait for complete and run the bound function
	sessionRef->OnFindSessionsCompleteDelegates.AddUObject(this, &UMGGameInstance::SessionsFindComplete);

	// run when join session has completed
	sessionRef->OnJoinSessionCompleteDelegates.AddUObject(this, &UMGGameInstance::SessionJoinComplete);
}

void UMGGameInstance::LoginEOS()
{
	// This is the online subsystem
	// This gives us access online subsystem functions
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
	// the account portal is a login portal that opens when you try to Log in
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

bool UMGGameInstance::IsInSession() const
{
	if(!IsLoggedIn())
		return false;

	// get the interface that interacts with sessions
	const IOnlineSessionPtr sessionref = Online::GetSubsystem(GetWorld())->GetSessionInterface();

	if(!sessionref)
		return false;

	const EOnlineSessionState::Type state = sessionref->GetSessionState(MGSESSION_NAME);
	
	return state != EOnlineSessionState::NoSession;
	
}

void UMGGameInstance::HostGame(bool lan)
{
	// are we logged in already?
	if(!IsLoggedIn())
		return;

	// get the session interface
	const IOnlineSessionPtr sessionRef = Online::GetSubsystem(GetWorld())->GetSessionInterface();

	if(!sessionRef)
		return;

	// define settings of the session
	FOnlineSessionSettings settings;
	settings.NumPublicConnections = 4;
	settings.bIsLANMatch = lan;
	settings.bIsDedicated = false;
	settings.bAllowInvites = true;
	settings.bShouldAdvertise = true;
	settings.bUsesPresence = true;
	settings.bUseLobbiesIfAvailable = true;
	settings.Set(SEARCH_KEYWORDS, MGSESSION_NAME.ToString(), EOnlineDataAdvertisementType::ViaOnlineService);
	settings.Set(SEARCH_LOBBIES, true, EOnlineDataAdvertisementType::ViaOnlineService);

	// create the session using the settings and name we created
	sessionRef->CreateSession(0, MGSESSION_NAME, settings);
	
}

void UMGGameInstance::FindAndJoinSession()
{
	// make sure we're logged in
	if(!IsLoggedIn())
		return;

	const IOnlineSessionPtr sessionRef = Online::GetSubsystem(GetWorld())->GetSessionInterface();

	// finding a session based on found sessions query settings
	sessionRef->FindSessions(0, FoundSessions.ToSharedRef());
	
}

void UMGGameInstance::StartLobbyGame()
{
	GetWorld()->GetAuthGameMode()->bUseSeamlessTravel = true;
	GetWorld()->ServerTravel("/Game/MyContent/Maps/Lvl_Test", false);
}

void UMGGameInstance::EndLobbyGame()
{
	GetWorld()->GetAuthGameMode()->bUseSeamlessTravel = true;
	GetWorld()->ServerTravel("/Game/MyContent/Maps/Lvl_Lobby", false);
}


// Function EOSLoginComplete
void UMGGameInstance::EOSloginComplete(int32 LocalUserNum, bool bWasSuccessful, const FUniqueNetId& UserId, const FString& Error)
{
	OnLoginComplete(bWasSuccessful, Error);
}

// Function SessionCreateComplete
void UMGGameInstance::SessionCreateComplete(FName SessionName, bool bWasSuccessful)
{
	EnableListenServer(true);
	OnSessionCreateComplete(bWasSuccessful);
}
// Function SessionsFindComplete
void UMGGameInstance::SessionsFindComplete(bool bWasSuccessful)
{
	if(!bWasSuccessful)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to find sessions"))
		OnSessionJoinComplete(false);
		return;
	}

	const IOnlineSubsystem* ossRef = Online::GetSubsystem(GetWorld());

	if(!ossRef)
		return;

	const IOnlineSessionPtr sessionRef = ossRef->GetSessionInterface();

	if(!sessionRef)
		return;

	if (FoundSessions->SearchResults.IsEmpty())
	{
		UE_LOG(LogTemp, Error, TEXT("No sessions found"))
		OnSessionJoinComplete(false);
		return;
	}

	// Join the session that is the first session found
	UE_LOG(LogTemp, Warning, TEXT("Found Session attempting to join"))
	sessionRef->JoinSession(0, MGSESSION_NAME, FoundSessions->SearchResults[0]);
	
}

void UMGGameInstance::SessionJoinComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	OnSessionJoinComplete(Result == EOnJoinSessionCompleteResult::Success);
	
	if(Result != EOnJoinSessionCompleteResult::Success)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to join session"))
		return;
	}

	// Automatically get the IP address and remote location and travel client to the game
	ClientTravelToSession(0, SessionName);
}

