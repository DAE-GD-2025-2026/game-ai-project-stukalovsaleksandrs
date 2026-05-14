#pragma once

// Engine
#include "BehaviorTree/BlackboardComponent.h"
// Standard
#include <functional>
#include <memory>
#include <unordered_set>

namespace GameAI::FSM
{
#pragma region State
	class IState
	{
	public:
		virtual ~IState() = default;
		virtual void OnEnter() = 0;
		virtual void OnExit() = 0;
		virtual void Update(float DeltaTime) = 0;
	};
#pragma endregion State
	
#pragma region Transition
	struct FTransition final
	{
		IState *From{}, *To{};
		std::function<bool()> EvalFunc;

		friend bool operator==(const FTransition& lhs, const FTransition& rhs);
	};

	inline bool operator==(const FTransition& lhs, const FTransition& rhs)
	{
		// NOTE: std::function instances are not comparable
		return lhs.From == rhs.From && lhs.To == rhs.To;
	}

	struct TransitionHash
	{
		std::size_t operator()(FTransition const& transition) const
		{
			auto const h1{ std::hash<IState*>{}(transition.From) };
			auto const h2{ std::hash<IState*>{}(transition.To) };
			return h1 ^ (h2 << 1);
		}
	};

	struct TransitionEqual
	{
		bool operator()(FTransition const& lhs, FTransition const& rhs) const
		{
			return lhs == rhs;
		}
	};
#pragma endregion Transition

#pragma region FSM
	class FSM final
	{
	public:
		void AddState(std::unique_ptr<IState>&& NewState);

		void AddTransition(FTransition const&);

		void Start();
		void Stop(){ bRunning = false; };
		[[nodiscard]] bool IsRunning() const { return bRunning; }
		
		void Tick(float DeltaTime);

	private:
		bool bRunning{};
		// NOTE: Not using a set, because sets do not work with move-only types
		std::vector<std::unique_ptr<IState>> States;
		IState* CurrentState{};
		std::unordered_set<
			FTransition,
			TransitionHash,
			TransitionEqual> Transitions;

		UBlackboardComponent* Blackboard{};

		void ChangeState(IState* NewState);
		
	};
#pragma endregion FSM
	
}
