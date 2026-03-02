// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Flock.h"
#include "Shared/Level_Base.h"
#include "Level_Flocking.generated.h"

UCLASS()
class GAMEAIPROG_API ALevel_Flocking : public ALevel_Base
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ALevel_Flocking();

	virtual void Tick(float DeltaTime) override;
	
protected:
	bool bUseMouseTarget{true};

	int const FlockSize{ 25 };

	TUniquePtr<FFlock> Flock{};
	
	UPROPERTY(EditAnywhere, Category = "Flocking")
	ASteeringAgent* AgentToEvade{}; // non owning ref

	std::unique_ptr<Seek> SeekBehavior{};

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
};
