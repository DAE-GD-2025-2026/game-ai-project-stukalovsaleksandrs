// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Agent.generated.h"

UCLASS()
class GAMEAIPROG_API AAgent : public ACharacter
{
	GENERATED_BODY()
	
public:	
	AAgent();
	virtual ~AAgent() = default;

	// Defines a location that the actor will go to
	void SetTargetLocation(FVector const& Location);

protected:
	virtual void BeginPlay() override;
	
private:
	FVector TargetLocation{};
	bool bMoving{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float DegreesPerSec{ 200.f };

	virtual void Tick(float DeltaTime) override;

	void EnableRotationTowardsMovementDirection();
};
