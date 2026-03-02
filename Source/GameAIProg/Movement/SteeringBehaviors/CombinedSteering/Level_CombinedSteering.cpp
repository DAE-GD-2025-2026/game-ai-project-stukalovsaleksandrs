#include "Level_CombinedSteering.h"

#include "imgui.h"


// Sets default values
ALevel_CombinedSteering::ALevel_CombinedSteering()
{
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ALevel_CombinedSteering::BeginPlay()
{
    Super::BeginPlay();
    // NOTE: Member fields are initialized here instead of constructor to
    // reset them every time the game is booted.
    // 1. Initializing the steering behaviors on BeginPlay and not in the constructor
    // for them to be re-created every time the game is restarted
    // NOTE: Using the "new" keyword here, because the function accepts a raw
    // pointer, but MakeUnique<T> returns a unique pointer, so I'll have to
    // call Release on that, which is too much fiddling around
    SeekBehavior = std::make_unique<Seek>();
    assert(SeekBehavior);
    WanderBehavior = std::make_unique<Wander>();
    assert(WanderBehavior);
    EvadeBehavior = std::make_unique<Evade>(650.f);
    assert(EvadeBehavior);
    
    BlendedBehavior = std::make_unique<FBlendedSteering>(
        std::vector<FBlendedSteering::FWeightedBehavior>{
            { SeekBehavior.get(), 0.5f },
            { WanderBehavior.get(), 0.5f }
        }
    );
    assert(BlendedBehavior);
    PriorityBehavior = std::make_unique<FPrioritySteering>(
        std::vector<ISteeringBehavior*>{
            EvadeBehavior.get(),// NOTE: Must precede Wander for it to trigger first
            WanderBehavior.get()
        }
    );
    assert(PriorityBehavior);
    
    // 2. Initializing the agents
    BlendedSteeringAgent = GetWorld()->SpawnActor<ASteeringAgent>(
        SteeringAgentClass,
        FVector{0, 0, 90},
        FRotator::ZeroRotator
    );
    BlendedSteeringAgent->SetSteeringBehavior(BlendedBehavior.get());
    assert(BlendedSteeringAgent);
    
    PrioritySteeringAgent = GetWorld()->SpawnActor<ASteeringAgent>(
        SteeringAgentClass,
        FVector{0, 0, 90},
        FRotator::ZeroRotator
    );
    assert(PrioritySteeringAgent);
    PrioritySteeringAgent->SetSteeringBehavior(PriorityBehavior.get());
}

void ALevel_CombinedSteering::BeginDestroy()
{
    Super::BeginDestroy();
    // NOTE: Not deleting agents, because their lifecycle is managed
    // by the world
}

// Called every frame
void ALevel_CombinedSteering::Tick(float const DeltaTime)
{
    Super::Tick(DeltaTime);
    
#pragma region UI
    //UI
    {
        //Setup
        bool windowActive = true;
        ImGui::SetNextWindowPos(WindowPos);
        ImGui::SetNextWindowSize(WindowSize);
        ImGui::Begin("Game AI", &windowActive, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
    
        //Elements
        ImGui::Text("CONTROLS");
        ImGui::Indent();
        ImGui::Text("LMB: place target");
        ImGui::Text("RMB: move cam.");
        ImGui::Text("Scrollwheel: zoom cam.");
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
    
        ImGui::Text("Flocking");
        ImGui::Spacing();
        ImGui::Spacing();
    
        if (ImGui::Checkbox("Debug Rendering", &CanDebugRender))
        {
            // Drawing the evade radius
            DrawDebugCircle(
                PrioritySteeringAgent->GetWorld(),
                PrioritySteeringAgent->GetActorLocation(),
                EvadeBehavior->GetEvadeRadius(),
                32, FColor::Purple, false, 0.025f, 0, 5, FVector(0, 1, 0), FVector(1, 0, 0), false
            );
        }
        ImGui::Checkbox("Trim World", &TrimWorld->bShouldTrimWorld);
        if (TrimWorld->bShouldTrimWorld)
        {
            ImGuiHelpers::ImGuiSliderFloatWithSetter("Trim Size",
                TrimWorld->GetTrimWorldSize(), 1000.f, 3000.f,
                [this](float const InVal) { TrimWorld->SetTrimWorldSize(InVal); });
        }
        
        ImGui::Spacing();
        ImGui::Spacing();
        ImGui::Spacing();
    
        ImGui::Text("Behavior Weights");
        ImGui::Spacing();

        ImGuiHelpers::ImGuiSliderFloatWithSetter("Seek",
            BlendedBehavior->GetWeightedBehaviorsRef()[0].Weight, 0.f, 1.f,
            [this](float const InVal) { BlendedBehavior->GetWeightedBehaviorsRef()[0].Weight = InVal; }, "%.2f");
        
        ImGuiHelpers::ImGuiSliderFloatWithSetter("Wander",
        BlendedBehavior->GetWeightedBehaviorsRef()[1].Weight, 0.f, 1.f,
        [this](float const InVal) { BlendedBehavior->GetWeightedBehaviorsRef()[1].Weight = InVal; }, "%.2f");
    
        //End
        ImGui::End();
    }
#pragma endregion

    // Combined Steering Update
    // Setting behavior targets
    SeekBehavior->SetTarget(MouseTarget);
    EvadeBehavior->SetTarget(FTargetData{
        BlendedSteeringAgent->GetLocation(),
        BlendedSteeringAgent->GetRotation(),
        BlendedSteeringAgent->GetLinearVelocity(),
        BlendedSteeringAgent->GetAngularVelocity(),
    });
}
