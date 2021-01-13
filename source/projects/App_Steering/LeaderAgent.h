#pragma once
#include "projects/App_Steering/SteeringAgent.h"

class ISteeringBehavior;
class LeaderAgent : public SteeringAgent
{
public://--- Constructor & Destructor ---
	LeaderAgent(const std::vector<SteeringAgent*>& otherAgents);
	LeaderAgent(const std::vector<SteeringAgent*>& otherAgents, float circleRadius);
	virtual ~LeaderAgent() = default;

	
	//--- Agent Functions ---
	void Update(float dt) override;
	void Render(float dt) override;

	void SetCircleRadius(float r);
	float GetCircleRadius() const;


	
private:
	//Helper functions
	void MakeTargets();
	void UpdateTargets();
	void CalculateOffsets();
	void UpdateSurrounding();
	void UpdateIMGUI();
	
	//Datamembers
	bool m_RenderRadius = false;
	bool m_RenderTargets = false;
	bool m_DoFormation = false;
	float m_CircleRadius;
	int m_Slots = 10;

	ISteeringBehavior* m_FlockBehaviour;
	std::vector<SteeringAgent*> m_OtherAgents;
	std::vector<SteeringAgent*> m_SuroundingAgents;
	std::vector<TargetData> m_Targets;
	std::vector<Elite::Vector2> m_Offsets;
};

