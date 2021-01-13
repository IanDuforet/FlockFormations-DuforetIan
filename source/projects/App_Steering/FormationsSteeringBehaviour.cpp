#include "stdafx.h"
#include "FormationsSteeringBehaviour.h"
#include "SteeringAgent.h"
//*************************
//VELOCITY MATCH (FLOCKING)
FormationAlignment::FormationAlignment(SteeringAgent* leader)
	: m_pLeader(leader)
{
}
SteeringOutput FormationAlignment::CalculateSteering(float deltaT, SteeringAgent* pAgent)
{
	SteeringOutput steering;
	steering.LinearVelocity = m_pLeader->GetLinearVelocity();
	return steering;

}