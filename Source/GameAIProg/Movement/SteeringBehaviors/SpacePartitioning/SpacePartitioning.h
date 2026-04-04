/*=============================================================================*/
// SpacePartitioning.h: Contains Cell and Cellspace which are used to partition a space in segments.
// Cells contain pointers to all the agents within.
// These are used to avoid unnecessary distance comparisons to agents that are far away.

// Heavily based on chapter 3 of "Programming Game AI by Example" - Mat Buckland
/*=============================================================================*/

#pragma once
#include <list>
#include <vector>
#include <iterator>

#include "Debug/ReporterGraph.h"
#include "Movement/SteeringBehaviors/SteeringAgent.h"

// --- Cell ---
// ------------
struct FCell final
{
	FCell(float Left, float Bottom, float Width, float Height);

	std::vector<FVector2D> GetRectPoints() const;
	
	// all the agents currently in this cell
	std::list<ASteeringAgent*> Agents;
	FRect BoundingBox;
};

// --- Partitioned Space ---
// -------------------------
class FCellSpace final
{
public:
	FCellSpace(UWorld* World, float Width, float Height, int32_t Rows, int32_t Cols, int MaxEntities);

	void AddAgent(ASteeringAgent* Agent);
	// Moves agent from one cell to another if necessary
	void UpdateAgentCell(ASteeringAgent* Agent, const FVector2D& OldLocation);

	void RegisterNeighbors(ASteeringAgent const& Agent, float QueryRadius);
	TArray<ASteeringAgent*> const & GetNeighbors() const { return Neighbors; }
	int GetNeighborCount() const { return NeighborCount; }

	//empties the cells of entities
	void EmptyCells();
	void RenderCells()const;

private:
	// For debug draw purposes
	UWorld* World{};
	
	// Cells and properties
	std::vector<FCell> Cells;
	FVector2f GridBottomLeft{};// From the camera's perspective
	
	float SpaceWidth, SpaceHeight;// Dimensions of the grid containing all the cells

	int32_t RowCount, ColCount;

	float CellWidth, CellHeight;

	// Members to avoid memory allocation on every frame
	TArray<ASteeringAgent*> Neighbors;
	int NeighborCount{};

	// Helper functions
	FCell& GetCell(int32_t const Col, int32_t const Row) { return Cells[Row * ColCount + Col]; }
	int32_t GetCellCollFromX(float X) const;
	int32_t GetCellRowFromY(float Y) const;
	int32_t GetCellIndexFromLocation(FVector2D const &) const;

	static bool DoRectsOverlap(FRect const& RectA, FRect const& RectB);
};
