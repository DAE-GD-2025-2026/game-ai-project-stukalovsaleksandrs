// Fill out your copyright notice in the Description page of Project Settings.


#include "Level_GraphTheory.h"

#include "Algorithms/EulerianPath.h"
#include "Shared/GameAISpectator.h"

using namespace GameAI;

// Sets default values
ALevel_GraphTheory::ALevel_GraphTheory()
{
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ALevel_GraphTheory::BeginPlay()
{
    Super::BeginPlay();
    // NOTE: Creating the graph renderer here due
    // to dependency on the world that can only be satisfied
    // once actor is already created
    Renderer = GraphRenderer(GetWorld());
    
    // Add the graph editor to our player
    if (PlayerController = Cast<APlayerController>(GetWorld()->GetFirstLocalPlayerFromController()->PlayerController); 
        GraphEditorClass && PlayerController)
    {
        PlayerGraphEditor = NewObject<UGraphEditorComponent>(PlayerController->GetPawn(), GraphEditorClass);
        PlayerGraphEditor->RegisterComponent();
        PlayerGraphEditor->SetEditedGraph(&Graph);
        PlayerGraphEditor->SetNodeFactory(&NodeFactory);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Unable to get PlayerController from LocalPlayer or GraphEditorClass is null"))
        return;
    }
    
    // Make the view orthogonal for less perspective issues
    if (AGameAISpectator* Player = Cast<AGameAISpectator>(PlayerController->GetPawnOrSpectator()); Player)
    {
        Player->SetCameraProjection(ECameraProjectionMode::Orthographic);
    }
    
    // DONE_TODO Make the graph and a couple connected nodes here...
    InitializeGraph();
    
    // Spawn the Agent
    Agent = GetWorld()->SpawnActor<ASteeringAgent>(SteeringAgentClass, 
        FVector{0,0,90}, FRotator::ZeroRotator);
    
    Agent->SetSteeringBehavior(&PathFollow);
}

void ALevel_GraphTheory::BeginDestroy()
{
    Super::BeginDestroy();
}

void ALevel_GraphTheory::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    
#pragma region UI
    {
        //Setup
        bool windowActive = true;
        ImGui::SetNextWindowPos(WindowPos);
        ImGui::SetNextWindowSize(WindowSize);
        ImGui::Begin("Gameplay Programming", &windowActive, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
        ImGui::SetWindowFocus();
        ImGui::PushItemWidth(70);
        //Elements
        ImGui::Text("CONTROLS");
        ImGui::Indent();
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

        ImGui::Text("Graph Theory");
        ImGui::Spacing();
        ImGui::Spacing();

        //End
        ImGui::End();
    }
#pragma endregion UI
    
    Renderer.RenderGraph(Graph);
    
    // DONE_TODO Check if the graph has updated
    if (!PlayerGraphEditor->HasGraphUpdated()) return;
    
    // DONE_TODO if so, run the EulerianPath algorithm
    EulerianPath eulerianPath{ &Graph };
    Eulerianity eulerianity{};
    auto eulerianPathNodes{ eulerianPath.FindPath(eulerianity) };
    
    // DONE_TODO if a path is found, have the agent follow it
    if (eulerianPathNodes.size() > 0)
    {
        UpdateAgentPath(eulerianPathNodes);
    }
}

void ALevel_GraphTheory::InitializeGraph()
{
    // Adding nodes
    auto addNode{ [&](FVector2D const location){ Graph.AddNode(NodeFactory.CreateNode(location)); } };
    addNode({-250.f, -250.f});// Bottom left
    addNode({-250.f, 250.f});// Top left
    addNode({250.f, 250.f});// Top right
    addNode({250.f, -250.f});// Bottom right
    
    // Adding connections
    Graph.AddConnection(0, 1);
    Graph.AddConnection(1, 2);
    Graph.AddConnection(2, 3);
    Graph.AddConnection(3, 0);
}

void ALevel_GraphTheory::UpdateAgentPath(std::vector<Node*> const& Trail)
{
    std::vector<FVector2D> path{};
    
    // DONE_TODO convert Node vector to positions vector
    std::ranges::transform(Trail, std::back_inserter(path), 
        [&](auto const * const node)
        {
            return node->GetPosition();
        }
    );
    
    PathFollow.SetPath(path);
    if (path.size() > 0)
    {
        Agent->SetPosition(path[0]);
    }
}
