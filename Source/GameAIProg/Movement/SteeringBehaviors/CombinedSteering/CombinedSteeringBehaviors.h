#pragma once
#include <vector>

#include "../Steering/SteeringBehaviors.h"

//****************
//BLENDED STEERING
class FBlendedSteering final: public ISteeringBehavior
{
public:
	struct FWeightedBehavior
	{
		ISteeringBehavior* Behavior{};
		float Weight{};

		FWeightedBehavior(ISteeringBehavior* const pBehavior, float const Weight) :
			Behavior(pBehavior),
			Weight(Weight)
		{};
	};

	explicit FBlendedSteering(const std::vector<FWeightedBehavior>& WeightedBehaviors);

	void AddBehaviour(const FWeightedBehavior& WeightedBehavior) { m_WeightedBehaviors.push_back(WeightedBehavior); }
	virtual SteeringOutput CalculateSteering(float DeltaTime, ASteeringAgent& Agent) override;

	float* GetWeight(ISteeringBehavior* const SteeringBehavior);
	
	// returns a reference to the weighted behaviors, can be used to adjust weighting. Is not intended to alter the behaviors themselves.
	std::vector<FWeightedBehavior>& GetWeightedBehaviorsRef() { return m_WeightedBehaviors; }

private:
	std::vector<FWeightedBehavior> m_WeightedBehaviors = {};
};

//*****************
//PRIORITY STEERING
class FPrioritySteering final: public ISteeringBehavior
{
public:
	explicit FPrioritySteering(const std::vector<ISteeringBehavior*>& PriorityBehaviors)
		: m_PriorityBehaviors(PriorityBehaviors) 
	{}

	void AddBehaviour(ISteeringBehavior* const pBehavior) { m_PriorityBehaviors.push_back(pBehavior); }
	virtual SteeringOutput CalculateSteering(float DeltaT, ASteeringAgent& Agent) override;

private:
	std::vector<ISteeringBehavior*> m_PriorityBehaviors = {};
};
