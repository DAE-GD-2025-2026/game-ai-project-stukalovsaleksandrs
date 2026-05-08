// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "EnhancedInput/Public/InputAction.h"
#include "Thief_PC.generated.h"

class UInputMappingContext;

/**
 * Basic input management for an actor moving to a designated location upon request
 */
UCLASS()
class GAMEAIPROG_API AThief_PC final : public APlayerController
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EnhancedInput", meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* InputMappingContext;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EnhancedInput", meta = (AllowPrivateAccess = "true"))
	UInputAction* IA_Click;
	
	virtual void SetupInputComponent() override;

	UFUNCTION()
	void MoveThiefToMouseLocation(FInputActionInstance const& InputActionInstance);
};
