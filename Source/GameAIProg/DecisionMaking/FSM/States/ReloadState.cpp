#pragma once

// Game
#include "ReloadState.h"
// Engine
#include "BrainComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "DecisionMaking/Guard.h"
#include "DecisionMaking/FSM/FSM.h"
#include "DecisionMaking/GameAIController.h"

GameAI::FSM::FReloadState::FReloadState(AGuard& ControlledGuard)
	: Guard{ ControlledGuard }
	, BlackboardComponent{ 
		Cast<AGameAIController>(Guard.GetController())->GetBrainComponent()->GetBlackboardComponent()
	}
{}

void GameAI::FSM::FReloadState::OnEnter()
{
	Guard.SetSubstateText("Reload");
	CurrentReloadSec = 0.f;
}

void GameAI::FSM::FReloadState::OnExit()
{
	Guard.SetSubstateText("");
}

void GameAI::FSM::FReloadState::Tick(float const DeltaTime)
{
	CurrentReloadSec += DeltaTime;
	if (CurrentReloadSec > ReloadSec)
	{
		DoneReloading.Broadcast();
	}

	// Moving away from the player
	FVector const TargetLocation{ BlackboardComponent->GetValueAsVector(Guard.TargetLocationKeyName) };
	Guard.SetTargetLocation(Guard.GetActorLocation() + (Guard.GetActorLocation() - TargetLocation));
}
