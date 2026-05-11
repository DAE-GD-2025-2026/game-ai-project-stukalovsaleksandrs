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
	// Adding only non-repeating states
	// NOTE: Not using std::unordered_set, because it does not work with the non-copyable types
	if (!std::ranges::binary_search(States.begin(), States.end(), NewState))
	{
		// 1. Adding the state
		States.push_back(std::move(NewState));
		// 2. Passing blackboard to the state
		States.back()->SetBlackboard(BlackboardComp);
	}
	
	// The first state added is the starting state
	if (!CurrentState) CurrentState = States.begin()->get();
}

void UFSMComponent::AddTransition(GameAI::FSM::FTransition const& Transition)
{
	Transitions.insert(Transition);
}

// Called when the game starts
void UFSMComponent::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void UFSMComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	if (!bRunning) return;
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (CurrentState) CurrentState->Update(DeltaTime);

	// TODO: Process transitions
	BlackBoardComp
}

void UFSMComponent::StartLogic()
{
	Super::StartLogic();

	CurrentState->OnEnter();
	bRunning = true;
}

void UFSMComponent::StopLogic(const FString& Reason)
{
	bRunning = false;
}

bool UFSMComponent::IsRunning() const
{
	return bRunning;
}

