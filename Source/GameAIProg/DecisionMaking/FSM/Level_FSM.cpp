// Fill out your copyright notice in the Description page of Project Settings.

#include "Level_FSM.h"
#include "FSMComponent.h"
#include "DecisionMaking/GameAIController.h"
#include "DecisionMaking/Agent.h"
#include "States/MoveForwardState.h"


// Sets default values
ALevel_FSM::ALevel_FSM()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ALevel_FSM::BeginPlay()
{
	Super::BeginPlay();

	// 1. Creating a guard actor
	Guard = GetWorld()->SpawnActor<AAgent>(GuardClass, 
	FVector{0,0,90}, FRotator::ZeroRotator);
	
	// 2. Setting up the guard's state machine
	if (AGameAIController* AIController = Cast<AGameAIController>(Guard->GetController()))
	{
		if (UFSMComponent* FSM = Cast<UFSMComponent>(AIController->GetBrainComponent()))
		{
			FSM->AddState(std::make_unique<GameAI::FSM::FMoveForwardState>(*Guard));
			AIController->RunFiniteStateMachine();
		}
	}
}

// Called every frame
void ALevel_FSM::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

