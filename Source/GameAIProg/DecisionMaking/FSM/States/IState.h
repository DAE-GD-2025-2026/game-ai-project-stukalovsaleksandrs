#pragma once

namespace GameAI::FSM
{
	class IState
	{
	public:
		virtual ~IState() = default;
		virtual void OnEnter() = 0;
		virtual void OnExit() = 0;
		virtual void Update() = 0;
	};
}
