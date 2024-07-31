// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "OnlineSessionSettings.h"
#include "Interfaces/OnlineSessionInterface.h"

// generated MUST go last on the #include
#include "MGGameInstance.generated.h"


#define MGSESSION_NAME FName(TEXT("MGSESSION"))

/**
 * 
 */
UCLASS()
class MGD_TEMPLATE_API UMGGameInstance : public UGameInstance
{
	GENERATED_BODY()

	
	// Keyword any class can access
public:
	UMGGameInstance();
	
	virtual void Init() override;
	
	UFUNCTION(BlueprintCallable, Category="Login")
	void LoginEOS();

	UFUNCTION(BlueprintPure, Category="Login")
	bool IsLoggedIn() const;

	UFUNCTION(BlueprintPure, Category="User")
	FString GetDisplayName() const;

	UFUNCTION(BlueprintPure, Category ="Session")
	bool IsInSession() const; 

	//host a game using eos (only lan rn)
	UFUNCTION(BlueprintCallable, Category="Session")
	void HostGame(bool lan = true);

	UFUNCTION(BlueprintCallable, Category="Session")
	void FindAndJoinSession();
	
	UFUNCTION(BlueprintCallable, Category="Session")
	void StartLobbyGame();


	
	// Keyword Only this class
private:
	
	// runs when EOS Login completes
	void EOSloginComplete(int32 LocalUserNum, bool bWasSuccessful, const FUniqueNetId& UserId, const FString& Error);

	// runs when the session create function completes 
	void SessionCreateComplete(FName SessionName, bool bWasSuccessful);

	// when find sessions run this will fill with found sessions
	TSharedPtr<FOnlineSessionSearch> FoundSessions;

	// runs when found sessions complete
	void SessionsFindComplete(bool bWasSuccessful);

	// runs on session join complete
	void SessionJoinComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);


	
	// Keyword This class and child classes
protected:
	
	UFUNCTION(BlueprintImplementableEvent, Category="Login")
	void OnLoginComplete(bool success, const FString& error);

	UFUNCTION(BlueprintImplementableEvent, Category="Session")
	void OnSessionCreateComplete(bool success);

	UFUNCTION(BlueprintImplementableEvent, Category="Session")
	void OnSessionJoinComplete(bool success);
	
};
