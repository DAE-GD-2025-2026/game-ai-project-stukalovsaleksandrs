// Fill out your copyright notice in the Description page of Project Settings.

// Game
#include "Level_FSM.h"
#include "FSM.h"
#include "FSMComponent.h"
#include "DecisionMaking/GameAIController.h"
#include "States/PatrolState.h"
#include "States/ChaseState.h"
#include "States/SearchState.h"
// Engine
#include "Kismet/GameplayStatics.h"
#include "Algo/Transform.h"
#include "States/AlertState.h"

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
	Guard = GetWorld()->SpawnActor<AGuard>(GuardClass, 
	FVector{0,0,90}, FRotator::ZeroRotator);
	
	// 3. Finding all the patrol points and assigning them to the guard
	auto PatrolPoints{ GetPatrolPoints() };
	
	// 2. Setting up the guard's state machine & patrolPoints
	if (AGameAIController* AIController = Cast<AGameAIController>(Guard->GetController()))
	{
		if (UFSMComponent* FSM = Cast<UFSMComponent>(AIController->GetBrainComponent()))
		{
			// -1. Setting guard's initial state text
			Guard->SetStateText("Patrol");
			
			// 0. Initializing guard blackboard component
			GuardBlackboardComponent = FSM->GetBlackboardComponent();
			
			// 1. Creating states
			auto PatrolState{
				std::make_unique<GameAI::FSM::FPatrolState>(*Guard, PatrolPoints)
			};

			auto AlertState{
				std::make_unique<GameAI::FSM::FAlertState>(*Guard)
			};

			auto SearchState{
				std::make_unique<GameAI::FSM::FSearchState>(*Guard)
			};

			// 2. Adding transitions
			FSM->AddTransition(
				{PatrolState.get(), AlertState.get(), 
				std::bind(&ALevel_FSM::DoesGuardSeeTarget, this)}
			);
			FSM->AddTransition(
				{AlertState.get(), SearchState.get(), 
				std::bind(&ALevel_FSM::DoesGuardNotSeeTarget, this)}
			);
			FSM->AddTransition(
				{SearchState.get(), AlertState.get(), 
				std::bind(&ALevel_FSM::DoesGuardSeeTarget, this)}
			);

			SearchState->OnTimerOut.AddLambda(
				[this, FSM, RawPatrolState = PatrolState.get()]
				{
					FSM->TryChangingFSMState(RawPatrolState);
				}
			);
			
			// 3. Adding states
			// NOTE: Underlying pointers stay the same, so transitions do not break
			FSM->AddState(std::move(PatrolState));
			FSM->AddState(std::move(AlertState));
			FSM->AddState(std::move(SearchState));
			
			// 4. Running FSM
			AIController->RunFSM();
		}
	}
}

void ALevel_FSM::Tick(float const DeltaTime)
{
	Super::Tick(DeltaTime);

	if (auto const* const PlayerCharacter{ UGameplayStatics::GetPlayerPawn(GetWorld(), 0) };
		PlayerCharacter)
	{
		GuardBlackboardComponent->SetValueAsVector(
			Guard->TargetLocationKeyName,
			PlayerCharacter->GetActorLocation()
		);
	}
	
	float const DetectionRadius{
		GuardBlackboardComponent->GetValueAsFloat(Guard->DetectionRadiusKeyName)
	};
	// Debug rendering
	DrawDebugCircle(
		GetWorld(),
		Guard->GetActorLocation(),
		DetectionRadius,
		64,
		FColor::Green,
		false,
		0,
		0,
		5,
		FVector(0, 1, 0),
		FVector(1, 0, 0),
		false
	);
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

bool ALevel_FSM::DoesGuardSeeTarget() const
{
	FVector const PlayerLocation{ GuardBlackboardComponent->GetValueAsVector(Guard->TargetLocationKeyName) };
	float const DetectionRadius{ GuardBlackboardComponent->GetValueAsFloat(Guard->DetectionRadiusKeyName) };
	FVector const GuardLocation{ Guard->GetActorLocation() };
	
	double const DistanceSq{ (PlayerLocation - GuardLocation).SizeSquared() };
	double const TargetDistanceSq{ DetectionRadius * DetectionRadius };
#ifdef ENABLE_CHASING_DEBUGGING
	UE_LOG(LogTemp, Display, TEXT("Distance: %f/%f"), std::sqrt(DistanceSq), std::sqrt(TargetDistanceSq));
#endif
	return DistanceSq < TargetDistanceSq;
}

bool ALevel_FSM::DoesGuardNotSeeTarget() const
{
	return !DoesGuardSeeTarget();
}
