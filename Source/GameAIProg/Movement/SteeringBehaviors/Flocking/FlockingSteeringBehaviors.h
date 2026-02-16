#pragma once
#include "Movement/SteeringBehaviors/Steering/SteeringBehaviors.h"
class Flock;

//COHESION - FLOCKING
//*******************
class Cohesion final : public Seek
{
public:
	Cohesion(Flock* const pFlock) :pFlock(pFlock) {};

	/**
	 * @def Moves towards the average position of all neighbors 
	 */
	SteeringOutput CalculateSteering(float DeltaTime, ASteeringAgent& Agent) override;

private:
	Flock* pFlock{};
};

//SEPARATION - FLOCKING
//*********************
class Separation final : public ISteeringBehavior
{
public:
	Separation(Flock* const pFlock) :pFlock(pFlock) {};

	/**
	 * @def Moving away from neighbors within a certain radius
	 * with a velocity equal to the sum of the inverses of the distance 
	 * to each boid. The distance is later divided by the avoided boid count to
	 * make the value independent of this count. Then it is also scaled by a
	 * separation factor, which thus controls how fast boids move away from each other.
	*/
	SteeringOutput CalculateSteering(float DeltaTime, ASteeringAgent& Agent) override;

private:
	Flock* pFlock{};

	float constexpr AvoidanceRadius{ 200.f },
		SeparationFactor{ 1.2f };
	
};

//VELOCITY MATCH - FLOCKING
//************************
class VelocityMatch final : public Seek
{
public:
	VelocityMatch(Flock* const pFlock) : pFlock(pFlock) {};

	SteeringOutput CalculateSteering(float DeltaTime, ASteeringAgent& Agent) override;

private:
	Flock* pFlock{};
};
