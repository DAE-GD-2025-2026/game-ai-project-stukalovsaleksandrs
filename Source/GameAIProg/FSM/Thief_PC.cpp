// Fill out your copyright notice in the Description page of Project Settings.

#include "FSM/Thief_PC.h"
// Engine
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Thief.h"
#include "Engine/LocalPlayer.h"

void AThief_PC::SetupInputComponent()
{
	Super::SetupInputComponent();

	// Adding IMC to the input subsystem
	ensureAlways(InputMappingContext);
	UEnhancedInputLocalPlayerSubsystem* const InputSubsystem{
		ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(
			GetLocalPlayer()
		)
	};
	ensureAlways(InputSubsystem);
	InputSubsystem->ClearAllMappings();
	InputSubsystem->AddMappingContext(InputMappingContext, 0);

	// Subscribing the movement callback to the LMB action
	ensureAlways(IA_Click);
	if (auto* EnhancedInputComponent{ Cast<UEnhancedInputComponent>(InputComponent) }; EnhancedInputComponent)
	{
		EnhancedInputComponent->BindAction(IA_Click, ETriggerEvent::Triggered, this, &AThief_PC::MoveThiefToMouseLocation);
	}
}


// Disabling the warning, because BindAction() does not work with const functions
// ReSharper disable once CppMemberFunctionMayBeConst
void AThief_PC::MoveThiefToMouseLocation(FInputActionInstance const&)
{
	AThief* const Thief{ Cast<AThief>(GetPawn()) };
	if (!Thief) return;
	FHitResult hitResult{};
	GetHitResultUnderCursor(ECC_Visibility, false, hitResult);
	if (!hitResult.bBlockingHit) return;
	Thief->SetTargetLocation(hitResult.Location);
}
