#pragma once

#include <functional>

namespace GameAI::FSM
{
	class IState
	{
	public:
		virtual ~IState() = default;
		virtual void OnEnter() = 0;
		virtual void OnExit() = 0;
		virtual void Update(float DeltaTime) = 0;
	};

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
}
