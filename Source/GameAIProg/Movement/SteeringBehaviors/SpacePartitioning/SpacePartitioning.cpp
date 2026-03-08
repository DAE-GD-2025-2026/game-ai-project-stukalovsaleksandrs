#include "SpacePartitioning.h"
#include <ranges>

#include "AssetTypeCategories.h"
#include "IntVectorTypes.h"
#include "IPropertyTable.h"
#include "StaticMeshAttributes.h"

// NOTE: Space partitioning uses doubles all around the place, because UE classes, such
// as FRect are primarily double-based, and their float-based alternatives, like FSlateRect
// are designed for specific purposes and do not share the same interface  

// --- Cell ---
// ------------
FCell::FCell(float Left, float Bottom, float Width, float Height)
{
	BoundingBox.Min = { Left, Bottom };
	BoundingBox.Max = { BoundingBox.Min.X + Width, BoundingBox.Min.Y + Height };
}

std::vector<FVector2D> FCell::GetRectPoints() const
{
	const float left = BoundingBox.Min.X;
	const float bottom = BoundingBox.Min.Y;
	const float width = BoundingBox.Max.X - BoundingBox.Min.X;
	const float height = BoundingBox.Max.Y - BoundingBox.Min.Y;

	std::vector<FVector2D> rectPoints =
	{
		{ left , bottom  },
		{ left , bottom + height  },
		{ left + width , bottom + height },
		{ left + width , bottom  },
	};

	return rectPoints;
}

// --- Partitioned Space ---
// -------------------------
FCellSpace::FCellSpace(UWorld* const World, float const Width, float const Height, uint32_t const Rows, uint32_t const Cols, int const MaxEntities)
	: World{World}
	, GridBottomLeft(-0.5f * Width, -0.5f * Height)
	, SpaceWidth{Width}
	, SpaceHeight{Height}
	, RowCount{Rows}
	, ColCount{Cols}
	, CellWidth{ Width / Cols }
	, CellHeight{ Height / Rows }
{
	Neighbors.SetNum(MaxEntities);
	
	// Creating cells
	Cells.reserve(Rows * Cols);
	// NOTE: Storing the cells in row-major order
	for (int Row{}; Row < Rows; ++Row)
	{
		for (int Col{}; Col < Cols; ++Col)
		{
			// GridBottomLeft + Width * 
			Cells.emplace_back(
				GridBottomLeft.X + static_cast<float>(Col) * CellWidth,
				GridBottomLeft.Y + static_cast<float>(Row) * CellHeight,
				CellWidth,
				CellHeight
			);
		}
	}
}

void FCellSpace::AddAgent(ASteeringAgent* Agent)
{
	// 1. Getting the cell index
	auto const GridIndex{ GetCellIndexFromLocation(Agent->GetLocation()) };
	// 2. Making sure the agent is not listed already
	if (auto& CellAgentList{ Cells[GridIndex].Agents };
		!std::ranges::binary_search(CellAgentList, Agent))
	{
		// 3. Listing the agent
		CellAgentList.push_back(Agent);
	}
}

void FCellSpace::UpdateAgentCell(ASteeringAgent* Agent, const FVector2D& OldLocation)
{
	// 1. Getting the index of the old cell
	auto const OldCellIdx{ GetCellIndexFromLocation(OldLocation) },
		NewCellIdx{ GetCellIndexFromLocation(Agent->GetLocation()) };
	if (OldCellIdx != NewCellIdx)
	{
		// 2. Removing the agent from the previous cell
		std::erase(Cells[OldCellIdx].Agents, Agent);
		// 3. Adding the agent to the new cell
		AddAgent(Agent);
	}
}

void FCellSpace::RegisterNeighbors(ASteeringAgent const & Agent, float const QueryRadius)
{
	// NOTE: Testing against the circumscribed square instead of the query circle for simplicity
	
	// 1. Getting the bottom left cell
	FVector2D const QueryRadii{ FVector2D(QueryRadius, QueryRadius) },
		BottomLeft{ Agent.GetLocation() - QueryRadii };
	uint32_t const BottomLeftCol{ GetCellCollFromX(BottomLeft.X) },
		BottomLeftRow{ GetCellRowFromY(BottomLeft.Y) };
	
	// 2. Getting the width and height in cells of the area of the square to check
	uint32_t const AreaCellWidth{ static_cast<uint32_t>(2.f * QueryRadius / CellWidth) },
		AreaCellHeight{ static_cast<uint32_t>(2.f * QueryRadius / CellHeight) };
	
	// 3. Getting the circumscribed AABB around the circle with QueryRadius
	FRect const CircumscribedAABB( BottomLeft, Agent.GetLocation() + QueryRadii );
	
	// 4. Iterating over all the cells in the area
	std::vector<FCell*> CellsToCheck;
	for (uint32_t Col{ BottomLeftCol }; Col < BottomLeftCol + AreaCellWidth; ++Col)
	{
		for (uint32_t Row{ BottomLeftRow }; Row < BottomLeftRow + AreaCellHeight; ++Row)
		{
			// 5. Checking if every cell is within the circumscribed square
			if (FCell& Cell{ GetCell(Col, Row) };
				DoRectsOverlap(Cell.BoundingBox, CircumscribedAABB))
			{
				CellsToCheck.push_back(&Cell);
			}
		}
	}

	// 5. Adding all the neighbors from these cells
	Neighbors.Empty();
	NeighborCount = 0;
	for (FCell const * const CellToCheck : CellsToCheck)
	{
		for (ASteeringAgent* Neighbor: CellToCheck->Agents)
		{
			// Taking only neighbors within the radius
			if ((Neighbor->GetLocation() - Agent.GetLocation()).SquaredLength() < QueryRadius * QueryRadius)
			{
				Neighbors.Push(Neighbor);
				++NeighborCount;
			}
		}
	}
}

void FCellSpace::EmptyCells()
{
	for (FCell& c : Cells)
		c.Agents.clear();
}

void FCellSpace::RenderCells() const
{
	// 2. Rendering cell boundaries
	float const GridTop{ GridBottomLeft.Y + SpaceHeight }
		, GridBottom{ GridBottomLeft.Y }
		, GridLeft{ GridBottomLeft.X }
		, GridRight{ GridBottomLeft.X + SpaceWidth }
		, LineZ{ 50.f };
	// 2.1. Rendering vertical lines
	for (uint32_t ColIdx{}; ColIdx < ColCount; ++ColIdx)
	{
		float const LineX{ GridLeft + ColIdx * CellWidth };
		FVector const LineStart{ LineX, GridTop, LineZ }
			, LineEnd{ LineX, GridBottom, LineZ };
		
		DrawDebugLine(World, LineStart, LineEnd, FColor::Black, true);
	}
	// 2.2. Rendering horizontal lines
	for (uint32_t RowIdx{}; RowIdx < RowCount; ++RowIdx)
	{
		float const LineY{ GridBottom + RowIdx * CellHeight };
		FVector const LineStart{ GridLeft, LineY, LineZ }
			, LineEnd{ GridRight, LineY, LineZ };
		
		DrawDebugLine(World, LineStart, LineEnd, FColor::Black, true);
	}
	// 3. Rendering the agent count inside each cell
	FlushDebugStrings(World);
	for (FCell Cell : Cells)
	{
		FVector2f const Center2D{0.5f * (Cell.BoundingBox.Min + Cell.BoundingBox.Max)};
		FVector const Center3D{
			// NOTE: Offsetting to bring the text closer to the bottom left corner of each cell
			Center2D.X + 0.5f * CellWidth,
			Center2D.Y - 0.5f * CellHeight,
			LineZ
		};
		DrawDebugString(
			World,
			Center3D,
			FString::FromInt(Cell.Agents.size())
		);
	}
}

uint32_t FCellSpace::GetCellCollFromX(float const X) const
{
	// NOTE: Clamping to the valid column and row values to avoid reading outside of bounds
	return std::clamp(
		static_cast<uint32_t>((X + 0.5f * SpaceWidth) / CellWidth),
		0u,
		ColCount - 1
	);
}

uint32_t FCellSpace::GetCellRowFromY(float const Y) const
{
	return std::clamp(
		// static_cast<uint32_t>((Y - GridBottomLeft.Y) / CellHeight),
		static_cast<uint32_t>((Y + 0.5f * SpaceHeight) / CellHeight),
		0u,
		RowCount - 1
	);
}

uint32_t FCellSpace::GetCellIndexFromLocation(FVector2D const & Location) const
{
	return GetCellRowFromY(Location.Y) * ColCount + GetCellCollFromX(Location.X);
}

bool FCellSpace::DoRectsOverlap(FRect const & RectA, FRect const & RectB)
{
	// Check if the rectangles are separated on either axis
	if (RectA.Max.X < RectB.Min.X || RectA.Min.X > RectB.Max.X) return false;
	if (RectA.Max.Y < RectB.Min.Y || RectA.Min.Y > RectB.Max.Y) return false;
    
	// If they are not separated, they must overlap
	return true;
}
