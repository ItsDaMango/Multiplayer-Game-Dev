// Fill out your copyright notice in the Description page of Project Settings.


#include "MGGamemode_Lobby.h"

#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"

void AMGGamemode_Lobby::OnPostLogin(AController* NewPlayer)
{
	// PARENT FUNCTION - Do not delete this on default events, use only on personal events
	Super::OnPostLogin(NewPlayer);

	// Initialise an empty local array of player starts
	TArray<AActor*> PlayerStartArray;

	// get all of the player starts and add them to the "PlayerStartArray" variable
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(),PlayerStartArray);

	// If there are no player starts just stop and return nothing changed
	if (PlayerStartArray.IsEmpty())
		return;

	for (AActor* actorRef : PlayerStartArray)
	{
		if (APlayerStart* psRef = Cast<APlayerStart>(actorRef))
		{
			int playerStartIndex = 1;

			if (psRef->PlayerStartTag == FName("Player2"))
				playerStartIndex = 2;
			else if (psRef->PlayerStartTag == FName("Player3"))
				playerStartIndex = 3;
			else if (psRef->PlayerStartTag == FName("Player4"))
				playerStartIndex = 4;
			else if (psRef->PlayerStartTag == FName("Player5"))
				playerStartIndex = 5;

			// getting the number of players in the game
			const int32 numPlayers = UGameplayStatics::GetNumPlayerStates(GetWorld());

			if (playerStartIndex == numPlayers)
			{
				RestartPlayerAtPlayerStart(NewPlayer, psRef);
				break;
			}
		}
		
		
	}
}
