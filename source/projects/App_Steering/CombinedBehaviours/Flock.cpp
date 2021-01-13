#include "stdafx.h"
#include "Flock.h"

#include "../SteeringAgent.h"
#include "../LeaderAgent.h"
#include "../SteeringBehaviors.h"
#include "CombinedSteeringBehaviors.h"
#include "SpacePartitioning.h"

using namespace Elite;

//Constructor & Destructor
Flock::Flock(
	int flockSize /*= 50*/,
	float worldSize /*= 100.f*/,
	SteeringAgent* pAgentToEvade /*= nullptr*/,
	bool trimWorld /*= false*/)

	: m_WorldSize{ worldSize }
	, m_FlockSize{ flockSize }
	, m_TrimWorld{ trimWorld }
	, m_pAgentToEvade{ pAgentToEvade }
	, m_NeighborhoodRadius{ 5 }
	, m_NrOfNeighbors{ 0 }
	, m_CanDebugRender(false)
	, m_RenderNeighbourhood(true)
	, m_UsePartitioning(true)
	, m_RenderPartitioning(true)
{
	//Make steering
	m_pSeek = new Seek();
	m_pSeek->SetTarget(Elite::Vector2{-m_WorldSize*2,-m_WorldSize*2});
	m_pWander = new Wander();
	m_pCohesion = new Cohesion(this);
	m_pSeperation = new Seperation(this);
	m_pAlingment = new Alignment(this);
	m_pBlendedSteering = new BlendedSteering({
		{m_pSeek, 1.f},
		{m_pWander, 0.0f },
		{m_pCohesion, 0.0f},
		{m_pSeperation, 1.f},
		{m_pAlingment, 1.f}});

	m_pEvade = new Evade();
	m_pEvade->SetTarget(m_pAgentToEvade->GetPosition());

	m_pPrioritySteering = new PrioritySteering{ {m_pEvade, m_pBlendedSteering} };

	//Make Cellspace
	m_pCellSpace = new CellSpace(m_WorldSize *2, m_WorldSize * 2, 10, 10, m_FlockSize);

	//Make empty vector for agents
	m_Agents.reserve(m_FlockSize);
	m_Agents.resize(m_FlockSize);

	m_OldPositions.reserve(m_FlockSize);
	m_OldPositions.resize(m_FlockSize);
	
	//Make Agents
	for(int i = 0; i <= flockSize-1; i++)
	{
		SteeringAgent* pAgent{ new SteeringAgent() };
		pAgent->SetAutoOrient(true);
		pAgent->SetSteeringBehavior(m_pPrioritySteering);
		Elite::Vector2 randomPos{ Elite::randomVector2(m_WorldSize) };
		pAgent->SetPosition(randomPos);
		pAgent->TrimToWorld(m_WorldSize);
		pAgent->SetMaxLinearSpeed(50.f);
		m_Agents[i] = pAgent;
		m_OldPositions[i] = randomPos;
		
		//Add Agent to CellSpace
		m_pCellSpace->AddAgent(pAgent);
	}
	//Make empty vector for neighbours
	m_Neighbors.reserve(m_FlockSize);
	m_Neighbors.resize(m_FlockSize);

}

Flock::~Flock()
{
	SAFE_DELETE(m_pSeek);
	SAFE_DELETE(m_pWander);
	SAFE_DELETE(m_pCohesion);
	SAFE_DELETE(m_pSeperation);
	SAFE_DELETE(m_pAlingment);
	SAFE_DELETE(m_pBlendedSteering);
	SAFE_DELETE(m_pEvade);
	SAFE_DELETE(m_pPrioritySteering);
	SAFE_DELETE(m_pCellSpace);
	
	for(SteeringAgent* pAgent : m_Agents)
	{
		SAFE_DELETE(pAgent);
	}
}

void Flock::Update(float deltaT)
{
	// loop over all the boids
	for (unsigned int i = 0; i < m_FlockSize; i++)
	{
		//Update Cells
		m_pCellSpace->UpdateAgentCell(m_Agents[i], m_OldPositions[i]);
		// register its neighbors
		RegisterNeighbors(m_Agents[i]);
		// update it
		m_Agents[i]->Update(deltaT);
		// trim it to the world
		//Save old pos
		m_OldPositions[i] = m_Agents[i]->GetPosition();
		m_Agents[i]->TrimToWorld(m_WorldSize);

	}

	//Update evade target
	m_pEvade->SetTarget(m_pAgentToEvade->GetPosition());
}

void Flock::Render(float deltaT)
{
	for (size_t i = 0; i < m_FlockSize; i++)
	{
		//m_Agents[i]->Render(deltaT);
		m_Agents[i]->SetRenderBehavior(m_CanDebugRender);
	}

	//Render Neighbourhood
	if(m_RenderNeighbourhood)
	{
		DEBUGRENDERER2D->DrawCircle(m_Agents[0]->GetPosition(), m_NeighborhoodRadius, { 0,1,0,0.5 }, 0);
		RegisterNeighbors(m_Agents[0]);
		for (int i = 0; i < m_NrOfNeighbors; i++)
		{
			SteeringAgent* currentAgent = m_Neighbors[i];

			if (currentAgent != nullptr)
			{
				DEBUGRENDERER2D->DrawSolidCircle(currentAgent->GetPosition(), 1, { 0,0 }, { 0,1,0, 0.5f }, 0);
			}
		}
	}

	if(m_RenderPartitioning)
		m_pCellSpace->RenderCells();
}

void Flock::SetSeekTarget(const Elite::Vector2& target)
{
	m_pSeek->SetTarget(target);
}

void Flock::UpdateAndRenderUI()
{
	//Setup
	int menuWidth = 235;
	int const width = DEBUGRENDERER2D->GetActiveCamera()->GetWidth();
	int const height = DEBUGRENDERER2D->GetActiveCamera()->GetHeight();
	bool windowActive = true;
	ImGui::SetNextWindowPos(ImVec2((float)width - menuWidth - 10, 10));
	ImGui::SetNextWindowSize(ImVec2((float)menuWidth, (float)height - 20));
	ImGui::Begin("Gameplay Programming", &windowActive, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
	ImGui::PushAllowKeyboardFocus(false);

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

	ImGui::Text("Flocking");
	ImGui::Spacing();

	ImGui::Checkbox("Debug Rendering", &m_CanDebugRender);
	ImGui::Checkbox("Debug render Neighbourhood", &m_RenderNeighbourhood);
	ImGui::Checkbox("Use partitioning", &m_UsePartitioning);
	if (m_UsePartitioning)
		ImGui::Checkbox("Debug render Cells", &m_RenderPartitioning);
	else
		m_RenderPartitioning = false;
	// Implement checkboxes and sliders here
	ImGui::Text("Behaviour Weights");
	ImGui::Spacing();

	ImGui::SliderFloat("Seek", &m_pBlendedSteering->m_WeightedBehaviors[0].weight, 0.f, 1.f, "%.2");
	ImGui::SliderFloat("Wander", &m_pBlendedSteering->m_WeightedBehaviors[1].weight, 0.f, 1.f, "%.2");
	ImGui::SliderFloat("Cohesion", &m_pBlendedSteering->m_WeightedBehaviors[2].weight, 0.f, 1.f, "%.2");
	ImGui::SliderFloat("Seperation", &m_pBlendedSteering->m_WeightedBehaviors[3].weight, 0.f, 1.f, "%.2");
	ImGui::SliderFloat("Alignement", &m_pBlendedSteering->m_WeightedBehaviors[4].weight, 0.f, 1.f, "%.2");
	//End
	ImGui::PopAllowKeyboardFocus();
	
}

void Flock::RegisterNeighbors(SteeringAgent* pAgent)
{
	// register the agents neighboring the currently evaluated agent
	// store how many they are, so you know which part of the vector to loop over
	m_NrOfNeighbors = 0;

	if (m_UsePartitioning)
	{
		m_pCellSpace->RegisterNeighbors(pAgent->GetPosition(), m_NeighborhoodRadius);
		m_Neighbors = m_pCellSpace->GetNeighbors();
		m_NrOfNeighbors = m_pCellSpace->GetNrOfNeighbors();
		return;
	}

	//Don't do this if using partitioning
	for(SteeringAgent* pA : m_Agents)
	{
		if(pA != pAgent)
		{
			//get distance to current agent
			float distance = Elite::DistanceSquared(pA->GetPosition(), pAgent->GetPosition());
			if(distance <= (m_NeighborhoodRadius*m_NeighborhoodRadius)) //Check squared to eliminate sqrt
			{
				m_Neighbors[m_NrOfNeighbors] = pA;
				m_NrOfNeighbors++;
			}
		}
	}
}

Elite::Vector2 Flock::GetAverageNeighborPos() const
{
	Elite::Vector2 averagePos{0,0};
	
	for(int i{0}; i < m_NrOfNeighbors; i++)
	{
		averagePos += m_Neighbors[i]->GetPosition();
	}
	if(m_NrOfNeighbors != 0)
		averagePos /= float(m_NrOfNeighbors);
	
	return averagePos;

}

Elite::Vector2 Flock::GetAverageNeighborVelocity() const
{
	Elite::Vector2 avarageSpeed{ 0,0 };
	for (int i = 0; i < m_NrOfNeighbors; i++)
	{
		avarageSpeed += m_Neighbors[i]->GetLinearVelocity();
	}
	if (m_NrOfNeighbors != 0)
		avarageSpeed /= float(m_NrOfNeighbors);
	return avarageSpeed;
}


float* Flock::GetWeight(ISteeringBehavior* pBehavior) 
{
	if (m_pBlendedSteering)
	{
		auto& weightedBehaviors = m_pBlendedSteering->m_WeightedBehaviors;
		auto it = find_if(weightedBehaviors.begin(),
			weightedBehaviors.end(),
			[pBehavior](BlendedSteering::WeightedBehavior el)
			{
				return el.pBehavior == pBehavior;
			}
		);

		if(it!= weightedBehaviors.end())
			return &it->weight;
	}

	return nullptr;
}
