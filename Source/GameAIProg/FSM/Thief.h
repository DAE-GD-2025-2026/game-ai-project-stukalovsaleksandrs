// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Thief.generated.h"

UCLASS()
class GAMEAIPROG_API AThief final : public ACharacter
{
	GENERATED_BODY()
	
public:	
	AThief();

	// Defines a location that the actor will go to
	void SetTargetLocation(FVector const& Location);

private:
	FVector TargetLocation{};
	bool bMoving{};

	// Distance at which agent is considered to arrive at destination
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float AcceptanceRadius{ 50.f };

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float DegreesPerSec{ 100.f };

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	void EnableRotationTowardsMovementDirection();
};
