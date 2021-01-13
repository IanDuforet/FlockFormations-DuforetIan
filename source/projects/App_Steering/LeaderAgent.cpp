#include "stdafx.h"
#include "LeaderAgent.h"
#include "SteeringBehaviors.h"
#include "CombinedBehaviours/CombinedSteeringBehaviors.h"
#include "FormationsSteeringBehaviour.h"
LeaderAgent::LeaderAgent(const std::vector<SteeringAgent*>& otherAgents)
	: SteeringAgent()
	, m_CircleRadius(10)
	, m_OtherAgents(otherAgents)
{
	MakeTargets();
	CalculateOffsets();
	m_FlockBehaviour = otherAgents[0]->GetSteeringBehavior();
}

LeaderAgent::LeaderAgent(const std::vector<SteeringAgent*>& otherAgents, float circleRadius)
	: SteeringAgent()
	, m_CircleRadius(circleRadius)
	,m_OtherAgents(otherAgents)
{
	MakeTargets();
	CalculateOffsets();
	m_FlockBehaviour = otherAgents[0]->GetSteeringBehavior();
}
void LeaderAgent::MakeTargets()
{
	for (size_t i = 0; i < m_Slots; i++)
	{
		float posX = m_CircleRadius * cos((E_PI*2 / (m_Slots-1)) * i);
		float posY = m_CircleRadius * sin((E_PI*2 / (m_Slots-1)) * i);
		Elite::Vector2 targetPos = { posX, posY };
		m_Targets.push_back(TargetData(targetPos));
	}
}

void LeaderAgent::UpdateTargets()
{
	m_Targets.clear();
	for (size_t i = 0; i < m_Slots; i++)
	{
		float posX = m_CircleRadius * cos((E_PI*2 / (m_Slots-1)) * i);
		float posY = m_CircleRadius * sin((E_PI*2 / (m_Slots-1)) * i);
		Elite::Vector2 targetPos = { posX, posY };
		m_Targets.push_back(TargetData(targetPos));
	}
}


void LeaderAgent::CalculateOffsets()
{
	Elite::Vector2 leaderPos = GetPosition();
	for (size_t i = 0; i < m_Slots; i++)
	{
		Elite::Vector2 offset = leaderPos - m_Targets[i].Position;
		m_Offsets.push_back(offset);
	}
	
}

void LeaderAgent::UpdateSurrounding()
{
	for (size_t i = 0; i < m_SuroundingAgents.size(); i++)
	{
		m_SuroundingAgents[i]->GetSteeringBehavior()->SetTarget(m_Targets[i]);
	}
}

void LeaderAgent::UpdateIMGUI()
{
	ImGui::Separator();
	ImGui::Text("LEADER");
	ImGui::Spacing();
	ImGui::Checkbox("Leader Formation", &m_DoFormation);
	if(m_DoFormation)
	{
		ImGui::Checkbox("Show Range", &m_RenderRadius);
		ImGui::Checkbox("Render Targets", &m_RenderTargets);
	}
	ImGui::End();
}

void LeaderAgent::Update(float dt)
{
	UpdateIMGUI();
	SteeringAgent::Update(dt);
	if(m_DoFormation)
	{
		UpdateSurrounding();
	}
	else
	{
		//If no formation, empty surrounding agents
		if(m_SuroundingAgents.size() != 0)
		{
			for(SteeringAgent* agent : m_SuroundingAgents)
			{
				agent->SetSteeringBehavior(m_FlockBehaviour);
			}
			m_SuroundingAgents.clear();
		}
	}

	//Update targets
	for (size_t i = 0; i < m_Slots; i++)
	{
		m_Targets[i].Position = GetPosition() + m_Offsets[i];
	}
	
	//Only do the following is the circle isn't full and it can do formations
	if (m_DoFormation)
	{
		if (m_SuroundingAgents.size() >= m_Slots)
			return;
		
		//Check if any other agent is close to the leader
		for (int i{ 0 }; i < m_OtherAgents.size(); i++)
		{
			//Check distance
			if (Elite::DistanceSquared(m_OtherAgents[i]->GetPosition(), GetPosition()) < Elite::Square(m_CircleRadius+1))
			{
				//Make sure it does not add itself
				auto it = std::find_if(m_SuroundingAgents.begin(), m_SuroundingAgents.end(), [this, i](SteeringAgent* a) {return a == m_OtherAgents[i]; });
				if(it != m_SuroundingAgents.end())
					continue;
				
				//If the ring is already full when turning formation on, exit the loop
				//This avoids from adding too many agents
				if (m_SuroundingAgents.size() >= m_Slots)
					return;

				m_SuroundingAgents.push_back(m_OtherAgents[i]);
				
				Seek* newSeek = new Seek();
				newSeek->SetTarget(m_Targets[m_SuroundingAgents.size() - 1]);
				//FormationAlignment* newAlign = new FormationAlignment(this);
				//BlendedSteering* blended = new BlendedSteering({ { newSeek, 0.8f }, { newAlign, 0.2f } });
				m_OtherAgents[i]->SetSteeringBehavior(newSeek);
			}
		}
	}
	
}
void LeaderAgent::Render(float dt)
{
	SteeringAgent::Render(dt);
	if(m_DoFormation)
	{
		if(m_RenderRadius)
			DEBUGRENDERER2D->DrawCircle(GetPosition(),m_CircleRadius, { 1,0,0, 0.5f }, 0);
		if(m_RenderTargets)
		{
			for(TargetData t : m_Targets)
			{
				DEBUGRENDERER2D->DrawSolidCircle(t.Position, 1, {0,0}, { 1,0,0, 0.1f }, 0);
			}	
		}
	}


	for(SteeringAgent* a : m_SuroundingAgents)
	{
		DEBUGRENDERER2D->DrawSolidCircle(a->GetPosition(), 1, { 0,0 }, { 1,0,0, 0.1f }, 0);
	}
	
}

void LeaderAgent::SetCircleRadius(float r)
{
	m_CircleRadius = r;
}

float LeaderAgent::GetCircleRadius() const
{
	return m_CircleRadius;
}

