#include "stdafx.h"
#include "FlockingSteeringBehaviors.h"
#include "Flock.h"
#include "../SteeringAgent.h"
#include "../SteeringHelpers.h"


//*********************
//SEPARATION (FLOCKING)
Seperation::Seperation(Flock* pFlock)
	: m_pFlock{pFlock}
{
}
SteeringOutput Seperation::CalculateSteering(float deltaT, SteeringAgent* pAgent)
{
	SteeringOutput steering;
	for (int i = 0; i < m_pFlock->GetNrOfNeighbors(); i++)
	{
		SteeringAgent* currentAgent = m_pFlock->GetNeighbors()[i];
		
		if(currentAgent != pAgent && currentAgent != nullptr)
		{
			const float distance = Elite::Distance(currentAgent->GetPosition(), pAgent->GetPosition());
			m_Target = currentAgent->GetPosition();
			steering.LinearVelocity += Evade::CalculateSteering(deltaT, pAgent).LinearVelocity.GetNormalized() * pow(distance, -1) * pAgent->GetMaxLinearSpeed();
		}
	}
	return steering;
}



//*******************
//COHESION (FLOCKING)
Cohesion::Cohesion(Flock* pFlock)
	: m_pFlock(pFlock)
{
}

SteeringOutput Cohesion::CalculateSteering(float deltaT, SteeringAgent* pAgent)
{
	m_Target = TargetData{ m_pFlock->GetAverageNeighborPos()};
	return Seek::CalculateSteering(deltaT, pAgent);
}



//*************************
//VELOCITY MATCH (FLOCKING)
Alignment::Alignment(Flock* pFlock)
	: m_pFlock(pFlock)
{
}
SteeringOutput Alignment::CalculateSteering(float deltaT, SteeringAgent* pAgent)
{
	SteeringOutput steering;
	
	steering.LinearVelocity = m_pFlock->GetAverageNeighborVelocity();
	return steering;
	
}