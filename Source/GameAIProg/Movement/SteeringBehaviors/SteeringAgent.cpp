// Fill out your copyright notice in the Description page of Project Settings.

#include "SteeringAgent.h"


// Sets default values
ASteeringAgent::ASteeringAgent()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
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
void ASteeringAgent::Tick(float DeltaSec)
{
	Super::Tick(DeltaSec);

	if (SteeringBehavior)
	{
		SteeringOutput const Output{ SteeringBehavior->CalculateSteering(DeltaSec, *this) };
		AddMovementInput(FVector{ Output.Direction, 0.f });
		AddAngularVelocity(DeltaSec, Output.DegreesPerSec);
	}
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

