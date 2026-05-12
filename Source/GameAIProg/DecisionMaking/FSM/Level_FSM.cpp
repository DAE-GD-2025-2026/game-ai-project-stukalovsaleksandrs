// Fill out your copyright notice in the Description page of Project Settings.

#include "Level_FSM.h"
#include "FSMComponent.h"
#include "DecisionMaking/GameAIController.h"
#include "Kismet/GameplayStatics.h"
#include "States/PatrolState.h"
#include "Algo/Transform.h"
#include "BehaviorTree/BlackboardData.h"
#include "States/ChaseState.h"

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
			// 0. Initializing guard blackboard component
			GuardBlackboardComponent = FSM->GetBlackboardComponent();
			
			// 1. Creating states
			auto PatrolState{
				std::make_unique<GameAI::FSM::FPatrolState>(*Guard, PatrolPoints)
			};

			auto ChaseState{
				std::make_unique<GameAI::FSM::FChaseState>(*Guard)
			};

			// 2. Adding transitions
			FSM->AddTransition(
				{PatrolState.get(), ChaseState.get(), 
				std::bind(&ALevel_FSM::DoesGuardSeePlayerCharacter, this)}
			);
			
			// 3. Adding states
			FSM->AddState(std::move(PatrolState));
			FSM->AddState(std::move(ChaseState));
			
			// 4. Running FSM
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

bool ALevel_FSM::DoesGuardSeePlayerCharacter() const
{
	FVector const PlayerLocation{ GuardBlackboardComponent->GetValueAsVector(Guard->TargetLocationKeyName) };
	float const DetectionRadius{ GuardBlackboardComponent->GetValueAsFloat(Guard->DetectionRadiusKeyName) };
	FVector const GuardLocation{ Guard->GetActorLocation() };
	
	double const DistanceSq{ (PlayerLocation - GuardLocation).SizeSquared() };
	double const TargetDistanceSq{ DetectionRadius * DetectionRadius };
	UE_LOG(LogTemp, Display, TEXT("Distance: %f/%f"), std::sqrt(DistanceSq), std::sqrt(TargetDistanceSq));
	return DistanceSq < TargetDistanceSq;
}

// Called every frame
void ALevel_FSM::Tick(float DeltaTime)
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

