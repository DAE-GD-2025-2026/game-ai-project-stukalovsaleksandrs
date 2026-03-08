// Fill out your copyright notice in the Description page of Project Settings.

#include "SteeringAgent.h"

#include <cassert>


// Sets default values
ASteeringAgent::ASteeringAgent()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	UpdateOldLocation();// Initializes the old location
}

FVector2D ASteeringAgent::GetLocation() const
{
	FVector3f Location{ GetActorLocation() };
	return {Location.X, Location.Y};
}

FVector2D ASteeringAgent::GetOldLocation() const
{
	return OldLocation;
}

void ASteeringAgent::UpdateOldLocation()
{
	OldLocation = GetLocation();
}

// Called when the game starts or when spawned
void ASteeringAgent::BeginPlay()
{
	Super::BeginPlay();
}

void ASteeringAgent::BeginDestroy()
{
	Super::BeginDestroy();
}

// Called every frame
void ASteeringAgent::Tick(float const DeltaSec)
{
	Super::Tick(DeltaSec);

	assert(SteeringBehavior);
	SteeringOutput const Output{ SteeringBehavior->CalculateSteering(DeltaSec, *this) };
	AddMovementInput(FVector{ Output.LinearVelocity, 0.f });
	AddAngularVelocity(DeltaSec, Output.DegreesPerSec);
}

// Called to bind functionality to input
void ASteeringAgent::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void ASteeringAgent::SetSteeringBehavior(ISteeringBehavior* NewSteeringBehavior)
{
	SteeringBehavior = NewSteeringBehavior;
}

void ASteeringAgent::AddAngularVelocity(float const DeltaSec, float const DegreesPerSec)
{
	FRotator Rotator{ GetActorRotation() };
	Rotator.Yaw += DegreesPerSec * DeltaSec;
	SetActorRotation(Rotator);
}

