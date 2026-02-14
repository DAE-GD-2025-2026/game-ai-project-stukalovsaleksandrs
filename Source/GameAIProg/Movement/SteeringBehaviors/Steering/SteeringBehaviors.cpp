#include "SteeringBehaviors.h"
#include "GameAIProg/Movement/SteeringBehaviors/SteeringAgent.h"
#include "DrawDebugHelpers.h"

// Seek
SteeringOutput Seek::CalculateSteering(float const DeltaTime, ASteeringAgent& Agent)
{
	SteeringOutput Steering{};
	// NOTE: There is no need to normalize since AddMovementInput already normalizes the vector.
	Steering.Velocity = Target.Position - Agent.GetPosition();
	return Steering;
}

// Flee
SteeringOutput Flee::CalculateSteering(float const DeltaTime, ASteeringAgent& Agent)
{
	SteeringOutput Steering{};
	Steering.Velocity = Agent.GetPosition() - Target.Position;
	return Steering;
}

// Arrive
SteeringOutput Arrive::CalculateSteering(float const DeltaTime, ASteeringAgent& Agent)
{
	// Preserving the old speed if relevant
	if (Agent.GetMaxLinearSpeed() > Agent.OldSpeed) Agent.OldSpeed = Agent.GetMaxLinearSpeed();

	if (Agent.GetDebugRenderingEnabled())
	{
		// Drawing debug circles
		DrawDebugCircle(Agent.GetWorld(), Agent.GetActorLocation(), m_TargetRadius, 32, FColor::Blue, false, 0.025f, 0, 5, FVector(0, 1, 0), FVector(1, 0, 0), false);
		DrawDebugCircle(Agent.GetWorld(), Agent.GetActorLocation(), m_SlowRadius, 32, FColor::Orange, false, 0.025f, 0, 5, FVector(0, 1, 0), FVector(1, 0, 0), false);
	}

	SteeringOutput Steering{};
	// Returning early if the target is in the origin
	if (Target.Position.Length() < 1.f) return Steering;// Not updating
	// Performing steering logic
	Steering.Velocity = Target.Position - Agent.GetPosition();// Norm is distance
	double const Distance{ Steering.Velocity.Length() };
	
	// Returning early if closer than TargetRadius
	if (Distance < m_TargetRadius)
	{
		Agent.SetMaxLinearSpeed(0.f);
		return Steering;
	}
	
	// Clamping the distance between the target and slow radii
	// NOTE: The clamping is performed in a way that the distances smaller or equal to TargetRadius get equated to 0.
	double const ClampedDistance{ FMath::Clamp(Distance - m_TargetRadius, 0, m_SlowRadius - m_TargetRadius )};
	double const SpeedFactor{ ClampedDistance / (m_SlowRadius - m_TargetRadius) };

	// Updating the agent's max speed
	Agent.SetMaxLinearSpeed(SpeedFactor * Agent.OldSpeed);
	
	return Steering;
}

float GetDegreesBetweenVectors(FVector2D const& Lhs, FVector2D const& Rhs)
{
	checkf(!FMath::IsNearlyZero(Lhs.SquaredLength()) && !FMath::IsNearlyZero(Rhs.SquaredLength()),
		TEXT("GetRadiansBetweenVectors() called with a zero vector"));
	float const LhsRadians(FMath::Atan2(Lhs.Y, Lhs.X)),
		RhsRadians(FMath::Atan2(Rhs.Y, Rhs.X));
	return FMath::RadiansToDegrees(FMath::FindDeltaAngleRadians(LhsRadians, RhsRadians));
}

// Face
SteeringOutput Face::CalculateSteering(float const DeltaTime, ASteeringAgent& Agent)
{
	SteeringOutput Steering{};

	// From Agent to Target
	FVector2D const DistanceVector{ Target.Position - Agent.GetPosition() };

	// Early returning if target is the agent's position
	if (FMath::IsNearlyZero(DistanceVector.SquaredLength()))
	{
		return Steering;
	}

	// Getting forward vector. Limiting to 2D since the project is top-down
	FVector2D const AgentForwardVector{ Agent.GetActorForwardVector().X, Agent.GetActorForwardVector().Y };
		
	// Getting the angle between the agent's forward vector and the deltaVector
	float const DeltaDegrees{ GetDegreesBetweenVectors(AgentForwardVector, DistanceVector) };

	// Stopping upon arrival at the destination
	if (FMath::IsNearlyZero(DeltaDegrees))
	{
		Agent.SetMaxAngularSpeed(0.f);
		return Steering;
	}
	
	if (FMath::IsNearlyZero(Agent.GetAngularVelocity()))// Not yet rotating
	{
		Steering.DegreesPerSec = 
			FMath::Clamp(
				DeltaDegrees / m_SecToRotate,
				-Agent.GetMaxDegreesPerSec(),
				Agent.GetMaxDegreesPerSec()
			);
	}
	else // Rotating already
	{
		// Making sure the agent does not overshoot by setting the angular speed
		// to the DeltaRadians if the former is larger than the latter
		if (DeltaDegrees > Agent.GetAngularVelocity())
		{
			Steering.DegreesPerSec = DeltaDegrees;
		}
	}
	
	return Steering;
}

// Pursuit 
SteeringOutput Pursuit::CalculateSteering(float const DeltaTime, ASteeringAgent& Agent)
{
	// Calculating how much time it will take for the agent to reach the target
	// considering that target stands still.
	double const Distance{ (Target.Position - Agent.GetPosition()).Length() },
		SecToReachTarget{ Distance / Agent.GetMaxLinearSpeed() };// t = d / v

	// Calculating location of the target after the time calculated previously elapses
	FVector2D const NewTargetPosition{ Target.Position + Target.LinearVelocity * SecToReachTarget };

	// Moving the agent towards the previously calculated position
	Target.Position = NewTargetPosition;
	return Seek::CalculateSteering(DeltaTime, Agent);
}

// Evade 
SteeringOutput Evade::CalculateSteering(float const DeltaTime, ASteeringAgent& Agent)
{
	// NOTE: Identical to Pursuit except that it moves away(Flees) from the target
	double const Distance{ (Target.Position - Agent.GetPosition()).Length() },
		SecToReachTarget{ Distance / Agent.GetMaxLinearSpeed() };
	
	FVector2D const NewTargetPosition{ Target.Position + Target.LinearVelocity * SecToReachTarget };

	Target.Position = NewTargetPosition;
	return Flee::CalculateSteering(DeltaTime, Agent);
}

// Wander
SteeringOutput Wander::CalculateSteering(float const DeltaTime, ASteeringAgent& Agent)
{
	SteeringOutput Steering{};
	return Steering;
}
