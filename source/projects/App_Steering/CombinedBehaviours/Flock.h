#pragma once
#include "../SteeringHelpers.h"
#include "FlockingSteeringBehaviors.h"

class ISteeringBehavior;
class SteeringAgent;
class BlendedSteering;
class PrioritySteering;
class CellSpace;

class Flock
{
public:
	Flock(
		int flockSize = 50, 
		float worldSize = 50.f, 
		SteeringAgent* pAgentToEvade = nullptr, 
		bool trimWorld = false);
	~Flock();

	void Update(float deltaT);
	void UpdateAndRenderUI();
	void Render(float deltaT);

	void SetSeekTarget(const Elite::Vector2& target);
	void RegisterNeighbors(SteeringAgent* pAgent);
	int GetNrOfNeighbors() const { return m_NrOfNeighbors; }
	const vector<SteeringAgent*>& GetAgents() { return m_Agents; }
	const vector<SteeringAgent*>& GetNeighbors() const { return m_Neighbors; }

	Elite::Vector2 GetAverageNeighborPos() const;
	Elite::Vector2 GetAverageNeighborVelocity() const;

private:
	// flock agents
	int m_FlockSize = 0;
	vector<SteeringAgent*> m_Agents;
	vector<Elite::Vector2> m_OldPositions;
	
	// neighborhood agents
	vector<SteeringAgent*> m_Neighbors;
	float m_NeighborhoodRadius = 5.f;
	int m_NrOfNeighbors = 0;

	// evade target
	SteeringAgent* m_pAgentToEvade = nullptr;


	
	// world info
	bool m_TrimWorld = false;
	float m_WorldSize = 0.f;
	
	// steering Behaviors
	Seek* m_pSeek = nullptr;
	Wander* m_pWander = nullptr;
	Cohesion* m_pCohesion = nullptr;
	Seperation* m_pSeperation = nullptr;
	Alignment* m_pAlingment = nullptr;

	Evade* m_pEvade = nullptr;
	
	BlendedSteering* m_pBlendedSteering = nullptr;
	PrioritySteering* m_pPrioritySteering = nullptr;

	//CellSpace
	CellSpace* m_pCellSpace = nullptr;
	
	//Other
	bool m_CanDebugRender;
	bool m_RenderNeighbourhood;
	bool m_UsePartitioning;
	bool m_RenderPartitioning;
	
	// private functions
	float* GetWeight(ISteeringBehavior* pBehaviour);

private:
	Flock(const Flock& other);
	Flock& operator=(const Flock& other);
};