#pragma once
#include "Movement/SteeringBehaviors/Steering/SteeringBehaviors.h"
class FFlock;

//COHESION - FLOCKING
//*******************
class Cohesion final : public Seek
{
public:
	Cohesion(FFlock* const pFlock) :pFlock(pFlock) {};

	/**
	 * @def Moves towards the average position of all neighbors 
	 */
	SteeringOutput CalculateSteering(float DeltaTime, ASteeringAgent& Agent) override;

private:
	FFlock* pFlock{};
};

//SEPARATION - FLOCKING
//*********************
class Separation final : public ISteeringBehavior
{
public:
	Separation(FFlock* const pFlock) :Flock(pFlock) {};

	/**
	 * @def Moving away from neighbors with a velocity equal to the
	 * sum of the inverses of the distance to each boid.
	 * The distance is later divided by the avoided boid count to
	 * make the value independent of this count. Then it is also scaled by a
	 * separation factor, which thus controls how fast boids move away from each other.
	*/
	virtual SteeringOutput CalculateSteering(float DeltaTime, ASteeringAgent& Agent) override;

	[[nodiscard]] float GetSeparationFactor() const { return SeparationFactor; }
	void SetSeparationFactor(float const SeparationFactor) { this->SeparationFactor = SeparationFactor; };

	static void DebugDraw(const ASteeringAgent* Agent, float const NeighborhoodRadius);
	
private:
	FFlock* Flock{};

	float SeparationFactor{ 150.f };
};

//VELOCITY MATCH - FLOCKING
//************************
class VelocityMatch final : public ISteeringBehavior
{
public:
	VelocityMatch(FFlock* const pFlock) : m_pFlock(pFlock) {};

	/**
	 * @def Matches the average velocity of the agent's neighbors
	 */
	SteeringOutput CalculateSteering(float DeltaTime, ASteeringAgent& Agent) override;

private:
	FFlock* m_pFlock{};
	
};
