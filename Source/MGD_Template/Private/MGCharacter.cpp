// Fill out your copyright notice in the Description page of Project Settings.


#include "MGCharacter.h"


// Sets default values
AMGCharacter::AMGCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	pRepPitch = 0.0f;
}

void AMGCharacter::Pure_MoveCharacter(const FVector2D Axis)
{
	// update the member axis
	pMoveAxis = Axis;

	// move character based on Y Axis
	AddMovementInput(GetActorRotation().Vector(), Axis.Y);

	// move character based on X Axis
	AddMovementInput(FRotationMatrix(GetActorRotation()).GetScaledAxis(EAxis::Y), Axis.X);
	
}

void AMGCharacter::Pure_RotateController(const FVector2D Axis)
{
	// rotate controller Yaw based on X Axis
	AddControllerYawInput(Axis.X);

	// rotate controller Pitch based on Y Axis
	AddControllerPitchInput(-Axis.Y);

	// the math for pitch diff
	if (FMath::Abs(pRepPitch - GetControlRotation().Pitch) > 10.f)
	{
		// are we the server? O_O
		if (HasAuthority())
			Multi_RepPitch(GetControlRotation().Pitch);
		else
			Server_RepPitch(GetControlRotation().Pitch);
	}
}

void AMGCharacter::Server_RepPitch_Implementation(const float& Pitch)
{
	// Run the multicast to update all the clients
	Multi_RepPitch(Pitch);
}

void AMGCharacter::Multi_RepPitch_Implementation(const float& Pitch)
{
	// update the pitch to match the value
	pRepPitch = Pitch; 
}

