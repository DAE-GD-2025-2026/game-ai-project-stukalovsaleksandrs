// Fill out your copyright notice in the Description page of Project Settings.

#include "FSMComponent.h"

#include "FSM.h"

// Sets default values for this component's properties
UFSMComponent::UFSMComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}

void UFSMComponent::AddState(std::unique_ptr<GameAI::FSM::IState>&& NewState)
{
	FSM.AddState(std::move(NewState));
}

void UFSMComponent::AddTransition(GameAI::FSM::FTransition const& Transition)
{
	FSM.AddTransition(Transition);
}

void UFSMComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UFSMComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	FSM.Tick(DeltaTime);
}

void UFSMComponent::StartLogic()
{
	Super::StartLogic();

	FSM.Start();
}

void UFSMComponent::StopLogic(const FString& Reason)
{
	FSM.Stop();
}

bool UFSMComponent::IsRunning() const
{
	return FSM.IsRunning();
}
