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
class Seek final : public ISteeringBehavior
{
public:
	SteeringOutput CalculateSteering(float DeltaTime, ASteeringAgent & Agent) override;
};

class Flee final : public ISteeringBehavior
{
public:
	SteeringOutput CalculateSteering(float DeltaTime, ASteeringAgent & Agent) override;
};

class Arrive final : public ISteeringBehavior
{
public:
	SteeringOutput CalculateSteering(float DeltaTime, ASteeringAgent & Agent) override;
	
private:
	float SlowRadius{400}, TargetRadius{100},
	MaxSpeed{ -1.f };// Speed cannot be negative,
	// it's initial value that will be overridden immediately.
	
};

class Face final : public ISteeringBehavior
{
public:
	SteeringOutput CalculateSteering(float DeltaTime, ASteeringAgent & Agent) override;
};

class Pursuit final : public ISteeringBehavior
{
public:
	virtual SteeringOutput CalculateSteering(float DeltaTime, ASteeringAgent & Agent) override;
};

class Evade final : public ISteeringBehavior
{
public:
	virtual SteeringOutput CalculateSteering(float DeltaTime, ASteeringAgent & Agent) override;
};

class Wander final : public ISteeringBehavior
{
public:
	virtual SteeringOutput CalculateSteering(float DeltaTime, ASteeringAgent & Agent) override;
};
