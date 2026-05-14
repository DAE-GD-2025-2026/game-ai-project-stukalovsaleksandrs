#include "FSM.h"

void GameAI::FSM::FSM::AddState(std::unique_ptr<IState>&& NewState)
{
	// Adding only non-repeating states
	// NOTE: Not using std::unordered_set, because it does not work with the non-copyable types
	if (!std::ranges::binary_search(States.begin(), States.end(), NewState))
	{
		// 1. Adding the state
		States.push_back(std::move(NewState));
	}
	
	// The first state added is the starting state
	if (!CurrentState) CurrentState = States.begin()->get();
}

void GameAI::FSM::FSM::AddTransition(FTransition const& Transition)
{
	Transitions.insert(Transition);
}

void GameAI::FSM::FSM::Start()
{
	CurrentState->OnEnter();
	bRunning = true;
}

void GameAI::FSM::FSM::Tick(float const DeltaTime)
{
	if (!bRunning) return;
	if (CurrentState) CurrentState->Update(DeltaTime);

	// TODO: Process transitions
	for (auto const& Transition : Transitions)
	{
		// 1. Skipping all the transitions not from current state
		if (Transition.From != CurrentState) continue;
		// 2. Trying to change state
		if (Transition.EvalFunc())
		{
			ChangeState(Transition.To);
		}
	}
}

void GameAI::FSM::FSM::ChangeState(IState* NewState)
{
	if (NewState == CurrentState) return;
	CurrentState->OnExit();
	CurrentState = NewState;
	CurrentState->OnEnter();
}
