// Fill out your copyright notice in the Description page of Project Settings.

// Game
#include "FSM/Thief.h"

#include "GameFramework/CharacterMovementComponent.h"

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

void AThief::BeginPlay()
{
	Super::BeginPlay();
	EnableRotationTowardsMovementDirection();
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

void AThief::EnableRotationTowardsMovementDirection()
{
	auto* const MovementComponent{ GetCharacterMovement() };
	check(MovementComponent);
	// 1. Setting the feature
	MovementComponent->bOrientRotationToMovement = true;
	// 2. Setting angular velocity
	// TODO: Update when angular velocity is changed
	MovementComponent->RotationRate = FRotator{ 0.f, DegreesPerSec, 0.f };
	// 3. Disabling snapping to controller's forward direction
	bUseControllerRotationYaw = false;
	MovementComponent->bUseControllerDesiredRotation = false;
}
