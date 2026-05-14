// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// Engine
#include "CoreMinimal.h"
#include "BrainComponent.h"
// Game
#include "FSM.h"
#include "FSMComponent.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class GAMEAIPROG_API UFSMComponent : public UBrainComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UFSMComponent();

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
	
	virtual void StartLogic() override;
	
	virtual void StopLogic(const FString& Reason) override;
	
	virtual bool IsRunning() const override; 
	
	void AddState(std::unique_ptr<GameAI::FSM::IState>&& NewState);

	void AddTransition(GameAI::FSM::FTransition const&);
		
protected:
	virtual void BeginPlay() override;

private:
	GameAI::FSM::FSM FSM; 

};
