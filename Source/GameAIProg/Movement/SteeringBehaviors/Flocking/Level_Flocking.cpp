// Fill out your copyright notice in the Description page of Project Settings.


#include "Level_Flocking.h"


// Sets default values
ALevel_Flocking::ALevel_Flocking()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ALevel_Flocking::BeginPlay()
{
	Super::BeginPlay();

	TrimWorld->SetTrimWorldSize(3000.f);
	TrimWorld->bShouldTrimWorld = true;

	// Spawning an agent to evade
	AgentToEvade = GetWorld()->SpawnActor<ASteeringAgent>(
		SteeringAgentClass,
		FVector{0, 0, 90},
		FRotator::ZeroRotator
	);
	assert(AgentToEvade);
	SeekBehavior = std::make_unique<Seek>();
	assert(SeekBehavior);
	AgentToEvade->SetSteeringBehavior(SeekBehavior.get());

	// Initializing flock
	Flock = TUniquePtr<FFlock>(
		new FFlock(
			GetWorld(),
			SteeringAgentClass,
			FlockSize,
			TrimWorld->GetTrimWorldSize(),
			AgentToEvade)
	);
	assert(Flock);

	// Setting the initial world trim size
	TrimWorld->SetTrimWorldSize(1000.f);
}

// Called every frame
void ALevel_Flocking::Tick(float const DeltaTime)
{
	Super::Tick(DeltaTime);
	// Agent to evade
    SeekBehavior->SetTarget(MouseTarget);
	// Flock
	Flock->ImGuiRender(WindowPos, WindowSize, TrimWorld);
	Flock->Tick(DeltaTime);
	Flock->RenderDebug();
	if (bUseMouseTarget)
		Flock->SetTarget_Seek(MouseTarget);
	
}
