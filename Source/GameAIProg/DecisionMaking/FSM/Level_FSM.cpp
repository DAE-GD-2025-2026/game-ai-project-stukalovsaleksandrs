// Fill out your copyright notice in the Description page of Project Settings.

#include "Level_FSM.h"
#include "FSMComponent.h"
#include "DecisionMaking/GameAIController.h"
#include "DecisionMaking/Agent.h"
#include "Kismet/GameplayStatics.h"
#include "States/PatrolState.h"
#include "Algo/Transform.h"

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
	
	// 3. Finding all the patrol points and assigning them to the guard
	auto PatrolPoints{ GetPatrolPoints() };
	
	// 2. Setting up the guard's state machine & patrolPoints
	if (AGameAIController* AIController = Cast<AGameAIController>(Guard->GetController()))
	{
		if (UFSMComponent* FSM = Cast<UFSMComponent>(AIController->GetBrainComponent()))
		{
			FSM->AddState(std::make_unique<GameAI::FSM::FPatrolState>(*Guard, PatrolPoints));
			AIController->RunFiniteStateMachine();
		}
	}

}

TArray<FVector> ALevel_FSM::GetPatrolPoints() const
{
	// Getting the patrol point actors
	TArray<AActor*> PatrolPointActors;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName(TEXT("PatrolPoint")), PatrolPointActors);
	
	// Extracting locations	
	TArray<FVector> PatrolPoints;
	PatrolPoints.Reserve(PatrolPointActors.Num());
	Algo::Transform(PatrolPointActors, PatrolPoints,
		[](AActor const* Actor){ return Actor->GetActorLocation(); }
	);

	return PatrolPoints;
}

// Called every frame
void ALevel_FSM::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

