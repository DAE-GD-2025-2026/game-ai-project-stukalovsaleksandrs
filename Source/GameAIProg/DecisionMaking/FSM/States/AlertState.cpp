#include "AlertState.h"

#include "AttackState.h"
#include "BrainComponent.h"
#include "ChaseState.h"
#include "ReloadState.h"
#include "DecisionMaking/GameAIController.h"

GameAI::FSM::FAlertState::FAlertState(AGuard& ControlledGuard)
	: Guard{ ControlledGuard }
	, GuardBlackboardComponent{ 
		Cast<AGameAIController>(ControlledGuard.GetController())->GetBrainComponent()->GetBlackboardComponent()
	}
{}

void GameAI::FSM::FAlertState::OnEnter()
{
	// Creating states
	auto ChaseState{ std::make_unique<FChaseState>(
		Guard 
	)};
	auto AttackState{ std::make_unique<FAttackState>(
		Guard 
	)};
	auto ReloadState{ std::make_unique<FReloadState>(
		Guard 
	)};

	// Creating transitions
	FSM.AddTransition({ChaseState.get(), AttackState.get(),
		std::bind(&FAlertState::IsWithinAttackReach, this)
	});
	AttackState->DoneAttacking.AddLambda(
		[this, RawReloadState = ReloadState.get()]
		{
			FSM.TryChangingState(RawReloadState);	
		}
	);
	ReloadState->DoneReloading.AddLambda(
		[this, RawChaseState = ChaseState.get()]
		{
			FSM.TryChangingState(RawChaseState);
		}
	);

	// Adding states
	FSM.AddState(std::move(ChaseState));
	FSM.AddState(std::move(AttackState));
	FSM.AddState(std::move(ReloadState));

	// Starting the FSM
	FSM.Start();
}

void GameAI::FSM::FAlertState::OnExit()
{
	FSM.Reset();
}

void GameAI::FSM::FAlertState::Tick(float DeltaTime)
{
	FSM.Tick(DeltaTime);
}

bool GameAI::FSM::FAlertState::IsWithinAttackReach() const
{
	FVector const PlayerLocation{ GuardBlackboardComponent->GetValueAsVector(Guard.TargetLocationKeyName) };
	float const AttackRadius{ GuardBlackboardComponent->GetValueAsFloat(Guard.AttackRadiusKeyName) };
	if (AttackRadius < 1.f)
	{
		UE_LOG(LogTemp, Error, TEXT("Attack radius is too small or not set"))
	}
	
	FVector const GuardLocation{ Guard.GetActorLocation() };
	UE_LOG(LogTemp, Display, TEXT("Distance/Attack radius: %f/%f"),
		(PlayerLocation - GuardLocation).Length(),
		AttackRadius
	);

	return (PlayerLocation - GuardLocation).SquaredLength() < AttackRadius * AttackRadius;
}

