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
	FCellSpace(UWorld* World, float Width, float Height, uint32_t Rows, uint32_t Cols, int MaxEntities);

	void AddAgent(ASteeringAgent& Agent);
	void RemoveAgent(ASteeringAgent& Agent);
	// Moves agent from one cell to another if necessary
	void UpdateAgentCell(ASteeringAgent& Agent, const FVector2D& OldLocation);

	void RegisterNeighbors(ASteeringAgent const& Agent, float QueryRadius);
	const TArray<ASteeringAgent*>& GetNeighbors() const { return Neighbors; }
	int GetNrOfNeighbors() const { return NeighborCount; }

	//empties the cells of entities
	void EmptyCells();
	void RenderCells()const;

private:
	// For debug draw purposes
	UWorld* World{};
	
	// Cells and properties
	std::vector<FCell> Cells;// Row major
	FVector2f GridBottomLeft{};// From the camera's perspective
	
	float SpaceWidth, SpaceHeight;

	uint32_t RowCount, ColCount;

	float CellWidth, CellHeight;

	// Members to avoid memory allocation on every frame
	TArray<ASteeringAgent*> Neighbors;
	int NeighborCount{};

	// Helper functions
	FCell& GetCell(uint32_t const Col, uint32_t const Row) { return Cells[Row * ColCount + Col]; }
	uint32_t GetCellCollFromX(float X) const;
	uint32_t GetCellRowFromY(float Y) const;
	uint32_t GetCellIndexFromLocation(FVector2D const &) const;
	
	bool DoRectsOverlap(FRect const& RectA, FRect const& RectB);
};
