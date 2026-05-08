// Fill out your copyright notice in the Description page of Project Settings.

// Game
#include "FSM/Thief.h"

// Sets default values
AThief::AThief()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AThief::SetTargetLocation(FVector const& Location)
{
	TargetLocation = Location;
	bMoving = true;
}

void AThief::Tick(float const DeltaTime)
{
	Super::Tick(DeltaTime);
	if (!bMoving) return;

	FVector const ToTarget{ TargetLocation - GetActorLocation() };

	if (ToTarget.SizeSquared() < FMath::Square(AcceptanceRadius))
	{
		bMoving = false;
		return;
	}

	AddMovementInput(ToTarget.GetSafeNormal(), 1.f);
}
