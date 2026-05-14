#pragma once

#include "CoreMinimal.h"
#include "Agent.h"
#include "Guard.generated.h"

UCLASS()
class GAMEAIPROG_API AGuard final : public AAgent
{
	GENERATED_BODY()
	
public:
	// TODO: Find a better way to select the key identifiers
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName TargetLocationKeyName;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName DetectionRadiusKeyName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName AttackRadiusKeyName;

	static void Attack()
	{
		UE_LOG(LogTemp, Display, TEXT("Attacking"));
	}
	
};
