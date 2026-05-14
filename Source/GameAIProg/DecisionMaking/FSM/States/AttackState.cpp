#include "AttackState.h"


GameAI::FSM::FAttackState::FAttackState(AGuard& ControlledGuard)
	: Guard{ ControlledGuard }
{}

void GameAI::FSM::FAttackState::OnEnter()
{
	Guard.SetSubstateText("Attack");
	Guard.Attack();
	CurrentAttackSec = 0.f;
}

void GameAI::FSM::FAttackState::OnExit()
{
	Guard.SetSubstateText("");
}

void GameAI::FSM::FAttackState::Tick(float DeltaTime)
{
	CurrentAttackSec += DeltaTime;
	if (CurrentAttackSec > AttackSec)
	{
		DoneAttacking.Broadcast();
	}
}
