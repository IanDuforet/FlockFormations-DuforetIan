#pragma once
#include "../SteeringBehaviors.h"

class Flock;

//SEPARATION - FLOCKING
//*********************
class Seperation : public Evade
{
public:
	Seperation(Flock* pFlock);
	virtual ~Seperation() = default;

	SteeringOutput CalculateSteering(float deltaT, SteeringAgent* pAgent) override;
private:
	Flock* m_pFlock;
};

//COHESION - FLOCKING
//*******************
class Cohesion : public Seek
{
public:
	Cohesion(Flock* pFlock);
	virtual ~Cohesion() = default;

	SteeringOutput CalculateSteering(float deltaT, SteeringAgent* pAgent) override;

private:
	Elite::Vector2 m_NeighbourhoodCenter;
	Flock* m_pFlock;
};

//VELOCITY MATCH - FLOCKING
//************************
class Alignment : public ISteeringBehavior
{
public:
	Alignment(Flock* pFlock);
	virtual ~Alignment() = default;

	SteeringOutput CalculateSteering(float deltaT, SteeringAgent* pAgent);
	Flock* m_pFlock;
};

