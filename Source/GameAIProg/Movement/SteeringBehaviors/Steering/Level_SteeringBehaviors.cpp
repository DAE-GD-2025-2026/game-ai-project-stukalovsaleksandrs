// Fill out your copyright notice in the Description page of Project Settings.

#include "Level_SteeringBehaviors.h"

#include <format>
#include <string>
#include "imgui.h"


// Sets default values
ALevel_SteeringBehaviors::ALevel_SteeringBehaviors()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ALevel_SteeringBehaviors::BeginPlay()
{
	Super::BeginPlay();

	AddAgent(BehaviorTypes::Seek);
	SteeringAgents[0].Agent_Steering->SetDebugRenderingEnabled(true);
}

void ALevel_SteeringBehaviors::BeginDestroy()
{
	Super::BeginDestroy();
}

// Called every frame
void ALevel_SteeringBehaviors::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

#pragma region UI
	ImGui::SetNextWindowPos(WindowPos);
	ImGui::SetNextWindowSize(WindowSize);
	ImGui::Begin("Game AI", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);

	//Elements
	ImGui::Text("CONTROLS");
	ImGui::Indent();
	ImGui::Text("LMB: place target");
	ImGui::Text("WASD: move cam");
	ImGui::Text("Scrollwheel: zoom cam");
	ImGui::Unindent();
	
	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();
	ImGui::Spacing();
	
	ImGui::Text("STATS");
	ImGui::Indent();
	ImGui::Text("%.3f ms/frame", 1000.0f / ImGui::GetIO().Framerate);
	ImGui::Text("%.1f FPS", ImGui::GetIO().Framerate);
	ImGui::Unindent();
	
	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();
	ImGui::Spacing();
	
	ImGui::Text("Steering Behaviors");
	ImGui::Spacing();
	ImGui::Spacing();
	
	ImGui::Checkbox("Trim World", &TrimWorld->bShouldTrimWorld);
	if (TrimWorld->bShouldTrimWorld)
	{
		ImGuiHelpers::ImGuiSliderFloatWithSetter("Trim Size",
			TrimWorld->GetTrimWorldSize(), 1000.f, 3000.f,
			[this](float InVal) { TrimWorld->SetTrimWorldSize(InVal); });
	}
	ImGui::Spacing();

#pragma region PerAgentUI
	if (ImGui::Button("Add Agent"))
		AddAgent(BehaviorTypes::Seek);
	ImGui::Separator();

	for (int i{0}; i < SteeringAgents.size(); ++i)
	{
		ImGui::PushID(i);
		ImGui_Agent& Agent = SteeringAgents[i];
		
		std::string agentHeader{std::format("Agent {}:", i)};
		if (ImGui::CollapsingHeader(agentHeader.c_str()))
		{
			ImGui::Indent();
			//Actor Props
			if (ImGui::CollapsingHeader("Properties"))
			{
				float v = Agent.Agent_Steering->GetMaxLinearSpeed();
				if (ImGui::SliderFloat("Lin", &v, 0.f, 600.f, "%.2f"))
					Agent.Agent_Steering->SetMaxLinearSpeed(v);

				v = Agent.Agent_Steering->GetMaxDegreesPerSec();
				if (ImGui::SliderFloat("Ang", &v, 0.f, 360.f, "%.2f"))
					Agent.Agent_Steering->SetMaxAngularSpeed(v);

				v = Agent.Agent_Steering->GetMass();
				if (ImGui::SliderFloat("Mass ", &v, 0.f, 100.f, "%.2f"))
					Agent.Agent_Steering->SetMass(v);
			}
			
			bool bBehaviourModified = false;

			ImGui::Spacing();
			ImGui::PushID(i + 50);
			ImGui::Text(" Behavior: ");
			ImGui::SameLine();
			ImGui::PushItemWidth(100);

			// Add the names of your steering behaviors
			if (ImGui::Combo("", &Agent.SelectedBehavior, "Seek\0Flee\0Arrive\0Face\0Pursuit\0Evade\0Wander", 4))
			{
				bBehaviourModified = true;
			}
			ImGui::PopItemWidth();
			ImGui::PopID();

			
			ImGui::Spacing();
			ImGui::PushID(i + 100);
			ImGui::Text(" Target: ");
			ImGui::SameLine();
			ImGui::PushItemWidth(100);
			
			int SelectedTargetOffset = Agent.SelectedTarget + 1;
			std::string const Label{""};
			std::string Targets{};
			for (auto const & Target : TargetLabels)
			{
				Targets += Target;
				Targets += '\0';
			}
			if (ImGui::Combo(Label.c_str(), &SelectedTargetOffset, Targets.c_str()))
			{
				Agent.SelectedTarget = SelectedTargetOffset - 1;
				bBehaviourModified = true;
			}
			
			ImGui::PopItemWidth();
			ImGui::PopID();
			ImGui::Spacing();
			ImGui::Spacing();
			
			
			if (bBehaviourModified)
			{
				SetAgentBehavior(Agent);
			}

			if (ImGui::Button("x"))
			{
				AgentIndexToRemove = i;
			}

			ImGui::SameLine(0, 20);

			bool isChecked = Agent.Agent_Steering->GetDebugRenderingEnabled();
			if (ImGui::Checkbox("Debug Rendering", &isChecked))
			{
				Agent.Agent_Steering->SetDebugRenderingEnabled(isChecked);
			}

			ImGui::Unindent();
		}
#pragma endregion 
		
		ImGui::PopID();
	}

	if (AgentIndexToRemove >= 0)
	{
		RemoveAgent(AgentIndexToRemove);
		AgentIndexToRemove = -1;
	}
	
	ImGui::End();
#pragma endregion

	for (ImGui_Agent& Agent : SteeringAgents)
	{
		if (Agent.Agent_Steering)
		{
			UpdateTarget(Agent);
		}
	}
}

bool ALevel_SteeringBehaviors::AddAgent(BehaviorTypes BehaviorType, bool AutoOrient)
{
	ImGui_Agent ImGuiAgent = {};
	ImGuiAgent.Agent_Steering = GetWorld()->SpawnActor<ASteeringAgent>(SteeringAgentClass, FVector{0,0,90}, FRotator::ZeroRotator);
	if (IsValid(ImGuiAgent.Agent_Steering))
	{
		ImGuiAgent.SelectedBehavior = static_cast<int>(BehaviorType);
		ImGuiAgent.SelectedTarget = -1; // Mouse
		
		SetAgentBehavior(ImGuiAgent);

		SteeringAgents.push_back(std::move(ImGuiAgent));
		
		RefreshTargetLabels();

		return true;
	}

	return false;
}

void ALevel_SteeringBehaviors::RemoveAgent(unsigned int Index)
{
	SteeringAgents[Index].Agent_Steering->Destroy();
	SteeringAgents.erase(SteeringAgents.begin() + Index);

	RefreshTargetLabels();
	RefreshAgentTargets(Index);
}

void ALevel_SteeringBehaviors::SetAgentBehavior(ImGui_Agent& Agent_ImGui)
{
	Agent_ImGui.Behavior.reset();

	// If after arrive, restoring the speed to its original state
	if (float& OldSpeed{ Agent_ImGui.Agent_Steering->OldSpeed }; OldSpeed > 0.f)
	{
		Agent_ImGui.Agent_Steering->SetMaxLinearSpeed( OldSpeed );
		OldSpeed = -1.f;
	}
	
	// NOTE: In MSVC switch with no cases is a compilation error
	// NOTE: Can try to use a map instead of switch case 
	switch (static_cast<BehaviorTypes>(Agent_ImGui.SelectedBehavior))
	{
	case BehaviorTypes::Seek:
		Agent_ImGui.Behavior = std::make_unique<Seek>();
		break;
	case BehaviorTypes::Flee:
		Agent_ImGui.Behavior = std::make_unique<Flee>();
		break;
	case BehaviorTypes::Arrive:
		Agent_ImGui.Behavior = std::make_unique<Arrive>();
		break;
	case BehaviorTypes::Face:
		Agent_ImGui.Behavior = std::make_unique<Face>();
		break;
	case BehaviorTypes::Pursuit:
		Agent_ImGui.Behavior = std::make_unique<Pursuit>();
		break;
	case BehaviorTypes::Evade:
		Agent_ImGui.Behavior = std::make_unique<Evade>(500.f);
		break;
	case BehaviorTypes::Wander:
		Agent_ImGui.Behavior = std::make_unique<Wander>();
		break;
	default:
		assert(false && "Incorrect Agent Behavior gotten during SetAgentBehavior()");
	}

	UpdateTarget(Agent_ImGui);
	
	Agent_ImGui.Agent_Steering->SetSteeringBehavior(Agent_ImGui.Behavior.get());
}

void ALevel_SteeringBehaviors::RefreshTargetLabels()
{
	TargetLabels.clear();
	
	TargetLabels.push_back("Mouse");
	for (int i{0}; i < SteeringAgents.size(); ++i)
	{
		TargetLabels.push_back(std::format("Agent {}", i));
	}
}

void ALevel_SteeringBehaviors::UpdateTarget(ImGui_Agent& Agent)
{
	// Note: MouseTarget position is updated via Level BP every click
	
	bool const bUseMouseAsTarget = Agent.SelectedTarget < 0;
	if (!bUseMouseAsTarget)
	{
		ASteeringAgent* const TargetAgent = SteeringAgents[Agent.SelectedTarget].Agent_Steering;

		FTargetData Target;
		Target.Position = TargetAgent->GetLocation();
		Target.Orientation = TargetAgent->GetRotation();
		Target.LinearVelocity = TargetAgent->GetLinearVelocity();
		Target.AngularVelocity = TargetAgent->GetAngularVelocity();

		Agent.Behavior->SetTarget(Target);
	}
	else
	{
		Agent.Behavior->SetTarget(MouseTarget);
	}
}

void ALevel_SteeringBehaviors::RefreshAgentTargets(unsigned int const IndexRemoved)
{
	for (unsigned int i = 0; i < SteeringAgents.size(); ++i)
	{
		if (i >= IndexRemoved)
		{
			auto& Agent = SteeringAgents[i];
			if (Agent.SelectedTarget == IndexRemoved || i  == Agent.SelectedTarget)
			{
				--Agent.SelectedTarget;
			}
		}
	}
}

