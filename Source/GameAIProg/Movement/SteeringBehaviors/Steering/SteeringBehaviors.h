#pragma once

#include <Movement/SteeringBehaviors/SteeringHelpers.h>

class ASteeringAgent;

// SteeringBehavior base, all steering behaviors should derive from this.
class ISteeringBehavior
{
public:
	ISteeringBehavior() = default;
	virtual ~ISteeringBehavior() = default;

	// Override to implement your own behavior
	virtual SteeringOutput CalculateSteering(float DeltaT, ASteeringAgent & Agent) = 0;

	void SetTarget(const FTargetData& NewTarget) { Target = NewTarget; }
	
	template<class T, std::enable_if_t<std::is_base_of_v<ISteeringBehavior, T>>* = nullptr>
	T* As()
	{ return static_cast<T*>(this); }

protected:
	FTargetData Target;
};

// Your own SteeringBehaviors should follow here...
class Seek : public ISteeringBehavior
{
public:
	~Seek() = default;
	SteeringOutput CalculateSteering(float DeltaTime, ASteeringAgent & Agent) override;
};

class Flee : public ISteeringBehavior
{
public:
	~Flee() = default;
	SteeringOutput CalculateSteering(float DeltaTime, ASteeringAgent & Agent) override;
};

class Arrive final : public ISteeringBehavior
{
public:
	SteeringOutput CalculateSteering(float DeltaTime, ASteeringAgent & Agent) override;
	
private:
	float m_SlowRadius{400}, m_TargetRadius{100},
	m_MaxSpeed{ -1.f };// Speed cannot be negative,
	// it's initial value that will be overridden immediately.
	
};

class Face final : public ISteeringBehavior
{
public:
	// Rotates the agent to look at the target position by using
	// AngularVelocity instead of pAgent->SetRotation().
	SteeringOutput CalculateSteering(float DeltaTime, ASteeringAgent & Agent) override;
	
private:
	// NOTE: Regardless of the angle, the rotation will take the same time,
	// which arguably is more realistic than having a fixed angular speed,
	// because in reality, we turn around with a larger speed than when we turn slightly.
	float const m_SecToRotate{ .25f };// How many seconds it takes to perform any rotation.
	
};

class Pursuit final : public Seek
{
public:
	// Moves and rotates towards the predicted location of the target 
	virtual SteeringOutput CalculateSteering(float DeltaTime, ASteeringAgent & Agent) override;
};

class Evade final : public Flee
{
public:
	virtual SteeringOutput CalculateSteering(float DeltaTime, ASteeringAgent & Agent) override;
};

class Wander final : public ISteeringBehavior
{
public:
	virtual SteeringOutput CalculateSteering(float DeltaTime, ASteeringAgent & Agent) override;

private:
	// Radius of a circle for selecting random target points,
	// where center is the agent's center + agent's front vector times the offset.
	float const m_TargetCircleRadius{ 200 },
		m_TargetCircleOffset{ 400 }, 
		// Max offset between 2 consecutive random angles
		// NOTE: Added for smoothness
		m_MaxTargetDegreesOffset{ 1.f };
	float m_LastTargetDegrees{};
};
