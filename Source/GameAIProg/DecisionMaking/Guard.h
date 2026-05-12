#pragma once

#include "CoreMinimal.h"
#include "Agent.h"
#include "BehaviorTree/BehaviorTreeTypes.h"
#include "Guard.generated.h"

UCLASS()
class GAMEAIPROG_API AGuard final : public AAgent
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName TargetLocationKeyName;
	
};
