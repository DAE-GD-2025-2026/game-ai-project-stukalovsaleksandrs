#pragma once

#include "FSM.h"

namespace GameAI::FSM
{
	class FPatrolState final : public IState
	{
	public:
		explicit FPatrolState(AAgent& ControlledAgent, TArray<FVector> PatrolPoints)
			: ControlledAgent{ ControlledAgent }
			, PatrolPoints{ PatrolPoints }
		{}
		
		virtual void OnEnter() override
		{
			ControlledAgent.SetTargetLocation( PatrolPoints[CurrentPatrolPointIdx] );
		}
		
		virtual void OnExit() override{}
		
		virtual void Update(float DeltaTime) override
		{
			if (PatrolPoints.IsEmpty()) return;
			FVector const ToCurrentPoint = PatrolPoints[CurrentPatrolPointIdx] - ControlledAgent.GetActorLocation();
			// TODO: See if it'll work without squaring the right one
			// NOTE: Not considering the height
			double const Dist{ FVector2D(ToCurrentPoint.X, ToCurrentPoint.Y).SizeSquared() };
			UE_LOG(LogTemp, Display, TEXT("%f"), Dist);
			if (Dist < FMath::Square(KINDA_SMALL_NUMBER) + 1)
			{
				// TODO: Stay idle for a few seconds
				// Changing the point
				CurrentPatrolPointIdx = (CurrentPatrolPointIdx + 1) % PatrolPoints.Num();

				ControlledAgent.SetTargetLocation( PatrolPoints[CurrentPatrolPointIdx] );
			}

			// Debug rendering patrol points
			for (auto PatrolPoint: PatrolPoints)
			{
				DrawDebugSphere(ControlledAgent.GetWorld(), PatrolPoint, 100, 10, FColor::Red, false, -1, 0, 8);
			}
		}

	private:
		AAgent& ControlledAgent;
		TArray<FVector> PatrolPoints;
		int32_t CurrentPatrolPointIdx{};
		
	};

}
