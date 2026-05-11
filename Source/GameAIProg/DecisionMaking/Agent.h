// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Agent.generated.h"

UCLASS()
class GAMEAIPROG_API AAgent final : public ACharacter
{
	GENERATED_BODY()
	
public:	
	AAgent();

	// Defines a location that the actor will go to
	void SetTargetLocation(FVector const& Location);

private:
	FVector TargetLocation{};
	bool bMoving{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float DegreesPerSec{ 200.f };

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	void EnableRotationTowardsMovementDirection();
};
