#pragma once

#include <format>
#include <string_view>

#include "CoreMinimal.h"
#include "Agent.h"
#include "Components/TextRenderComponent.h"
#include "Components/Widget.h"
#include "Components/WidgetComponent.h"
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

	void Attack() const
	{
		UE_LOG(LogTemp, Display, TEXT("Attacking"));
		// StateText->SetText(FText::FromString(TEXT("Attacking")));
	}

	void SetStateText(std::string_view const Text) const
	{
		if (Text.empty())
		{
			StateText->SetText(FText::FromString(""));
			return;
		}
		
		std::string const Message{ std::format("State: {}", Text.data()) };
		StateText->SetText(FText::FromString(Message.data()));
	}

	void SetSubstateText(std::string_view const Text) const
	{
		if (Text.empty())
		{
			SubstateText->SetText(FText::FromString(""));
			return;
		}
		std::string const Message{ std::format("Substate: {}", Text.data()) };
		SubstateText->SetText(FText::FromString(Message.data()));
	}
	
protected:
	virtual void BeginPlay() override
	{
		Super::BeginPlay();
		
		StateText = FindComponentByTag<UTextRenderComponent>(TEXT("State"));
		if (!StateText)
		{
			UE_LOG(LogTemp, Error, TEXT("State text not set"));
		}

		SubstateText = FindComponentByTag<UTextRenderComponent>(TEXT("Substate"));
		if (!SubstateText)
		{
			UE_LOG(LogTemp, Error, TEXT("Substate text not set"));
		}
	}

private:
	UTextRenderComponent* StateText{};
	UTextRenderComponent* SubstateText{};
};
