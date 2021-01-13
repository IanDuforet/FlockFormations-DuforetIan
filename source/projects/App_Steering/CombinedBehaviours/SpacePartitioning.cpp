#include "stdafx.h"
#include "SpacePartitioning.h"
#include "projects\App_Steering\SteeringAgent.h"

// --- Cell ---
// ------------
Cell::Cell(float left, float bottom, float width, float height)
{
	boundingBox.bottomLeft = Elite::Vector2{ left, bottom };
	boundingBox.width = width;
	boundingBox.height = height;
}

std::vector<Elite::Vector2> Cell::GetRectPoints() const
{
	auto left = boundingBox.bottomLeft.x;
	auto bottom = boundingBox.bottomLeft.y;
	auto width = boundingBox.width;
	auto height = boundingBox.height;

	std::vector<Elite::Vector2> rectPoints =
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
CellSpace::CellSpace(float width, float height, int rows, int cols, int maxEntities)
	: m_SpaceWidth(width)
	, m_SpaceHeight(height)
	, m_NrOfRows(rows)
	, m_NrOfCols(cols)
	, m_Neighbors(maxEntities)
	, m_NrOfNeighbors(0)
{
	float cellWidth = m_SpaceWidth / (float)cols;
	float cellHeight = m_SpaceHeight / (float)rows;

	m_CellWidth = cellWidth;
	m_CellHeight = cellHeight;
	
	for(int r = 0; r < rows; r++)
	{
		for(int c = 0; c < cols; c++)
		{		
			m_Cells.push_back(Cell(((cellWidth * c) - m_SpaceWidth/2), (cellHeight * r) - m_SpaceHeight/2, cellWidth, cellHeight));
		}
	}

	
}

void CellSpace::AddAgent(SteeringAgent* agent)
{
	int index = PositionToIndex(agent->GetPosition());

	//Add to cell
	m_Cells[index].agents.push_back(agent);
}

void CellSpace::UpdateAgentCell(SteeringAgent* agent, const Elite::Vector2& oldPos)
{
	//Check if they have the same index
	int currentIndex = PositionToIndex(agent->GetPosition());
	int oldIndex = PositionToIndex(oldPos);

	if (currentIndex != oldIndex)
	{
		//Move to new cell
		m_Cells[currentIndex].agents.push_back(agent);
		//Remove from old cell
		m_Cells[oldIndex].agents.remove(agent);
	}
}

void CellSpace::RegisterNeighbors(const Elite::Vector2& pos, float queryRadius)
{
	Elite::Rect agentRect = Elite::Rect(pos - Elite::Vector2{ queryRadius, queryRadius }, queryRadius * 2, queryRadius * 2);

	std::vector<SteeringAgent*> inRadiusAgents{}; 

	m_NrOfNeighbors = 0;
	
	for (const Cell& c : m_Cells)
	{
		if (Elite::IsOverlapping(agentRect, c.boundingBox))
		{
			for(SteeringAgent* pA : c.agents)
			{
				if (pA->GetPosition() != pos) //check if not itself
				{
					//get distance to current agent
					float distance = Elite::DistanceSquared(pos, pA->GetPosition());
					if (distance <= (queryRadius * queryRadius)) //Check squared to eliminate sqrt
					{
						m_Neighbors[m_NrOfNeighbors] = pA;
						m_NrOfNeighbors++;
					}
				}
			}
		}
	}

}

void CellSpace::RenderCells() const
{
	for(const Cell& c : m_Cells)
	{
		//Make cell polygon
		Elite::Polygon polygon = Elite::Polygon{c.GetRectPoints()};
		DEBUGRENDERER2D->DrawPolygon(&polygon, Elite::Color{1,0,0});
		DEBUGRENDERER2D->DrawString(polygon.GetCenterPoint(), std::to_string(c.agents.size()).c_str());
	}
}

int CellSpace::PositionToIndex(const Elite::Vector2 pos) const
{
	int colIndex = (pos.x + m_SpaceWidth/2)/ m_CellWidth;
	int rowIndex = (pos.y + m_SpaceHeight/2) / m_CellHeight;

	//Check if on grid
	if (colIndex >= m_NrOfCols)
		colIndex = 0;
	else if (colIndex < 0)
		colIndex = m_NrOfCols - 1;

	if (rowIndex >= m_NrOfRows)
		rowIndex = 0;
	else if (rowIndex < 0)
		rowIndex = m_NrOfRows - 1;

	Elite::Clamp(colIndex, 0, m_NrOfCols-1);
	
	//Convert to 1D
	int index = rowIndex * m_NrOfCols + colIndex;
	return index;
}