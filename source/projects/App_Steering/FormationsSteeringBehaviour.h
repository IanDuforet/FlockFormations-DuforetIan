#pragma once
#include "SteeringBehaviors.h"
//VELOCITY MATCH - Formations
//************************
class FormationAlignment : public ISteeringBehavior
{
public:
	FormationAlignment(SteeringAgent* leader);
	virtual ~FormationAlignment() = default;

	SteeringOutput CalculateSteering(float deltaT, SteeringAgent* pAgent);
	SteeringAgent* m_pLeader;
};
