//Precompiled Header [ALWAYS ON TOP IN CPP]
#include "stdafx.h"

//Includes
#include "SteeringBehaviors.h"
#include "SteeringAgent.h"

//SEEK
//****
SteeringOutput Seek::CalculateSteering(float deltaT, SteeringAgent* pAgent)
{
	SteeringOutput steering = {};

	steering.LinearVelocity = (m_Target).Position - pAgent->GetPosition(); //Desired Velocity
	steering.LinearVelocity.Normalize(); //Normalize Desired Velocity
	steering.LinearVelocity *= pAgent->GetMaxLinearSpeed(); //Rescale to Max Speed

	//DEBUG RENDERING
	if (pAgent->CanRenderBehavior())
		DEBUGRENDERER2D->DrawDirection(pAgent->GetPosition(), steering.LinearVelocity, 5, { 0,1,0 }, 0.4f);

	return steering;
}

//WANDER (base> SEEK)
//******
SteeringOutput Wander::CalculateSteering(float deltaT, SteeringAgent* pAgent)
{
	const float halfJitter = m_MaxJitterOffset / 2;
	const Elite::Vector2 randomOffset = Elite::Vector2{
	randomFloat(-halfJitter, halfJitter),
	randomFloat(-halfJitter, halfJitter) };
	m_WanderTarget += randomOffset;
	m_WanderTarget.Normalize();
	m_WanderTarget *= m_Radius;

	//Add offset
	Elite::Vector2 offset = pAgent->GetLinearVelocity();
	offset.Normalize();
	offset *= m_Offset;

	m_Target = TargetData{ pAgent->GetPosition() + offset + m_WanderTarget };

	if (pAgent->CanRenderBehavior())
	{
		Elite::Vector2 pos = pAgent->GetPosition();
		DEBUGRENDERER2D->DrawSegment(pos, pos + offset, { 0.f,0.f,1.f,0.5f });
		DEBUGRENDERER2D->DrawCircle(pos + offset, m_Radius, { 0.f,0.f,1.f,0.5f }, 0.4f);
		DEBUGRENDERER2D->DrawSolidCircle(pos + offset + m_WanderTarget, 0.5f, { 0,0 }, { 0.f, 1.f, 0.f, 0.5f }, 0.3f);
	}

	return Seek::CalculateSteering(deltaT, pAgent);
}

//FLEE
//***
SteeringOutput Flee::CalculateSteering(float deltaT, SteeringAgent* pAgent)
{
	SteeringOutput steering{};
	steering.LinearVelocity = pAgent->GetPosition() - m_Target.Position;
	steering.LinearVelocity.Normalize();
	steering.LinearVelocity *= pAgent->GetMaxLinearSpeed();

	//Debug rendering
	if (pAgent->CanRenderBehavior())
	{
		Elite::Vector2 pos = pAgent->GetPosition();
		DEBUGRENDERER2D->DrawDirection(pAgent->GetPosition(),
			steering.LinearVelocity,
			steering.LinearVelocity.Magnitude(),
			{ 0.f,1.f,0.f,0.5f });
		DEBUGRENDERER2D->DrawCircle(pos, m_Radius, { 0.f,0.f,1.f,0.5f }, 0.4f);
		DEBUGRENDERER2D->DrawCircle(m_Target.Position, 2, { 0.f,1.f,1.f,0.5f }, 0.4f);

	}
	
	return steering;
}

//ARRIVE
//***
SteeringOutput Arrive::CalculateSteering(float deltaT, SteeringAgent* pAgent)
{
	SteeringOutput steering{};

	steering.LinearVelocity = m_Target.Position - pAgent->GetPosition();
	const float distance = steering.LinearVelocity.Magnitude();
	steering.LinearVelocity.Normalize();

	if (distance < m_Radius)
	{
		steering.LinearVelocity *= pAgent->GetMaxLinearSpeed() * (distance / m_Radius);
	}
	else
	{
		steering.LinearVelocity *= pAgent->GetMaxLinearSpeed();
	}

	//Debug rendering
	if (pAgent->CanRenderBehavior())
	{
		DEBUGRENDERER2D->DrawDirection(pAgent->GetPosition(),
			steering.LinearVelocity,
			steering.LinearVelocity.Magnitude(),
			{ 0.f,1.f,0.f,0.5f });
		DEBUGRENDERER2D->DrawCircle(m_Target.Position, m_Radius, { 0.0f,1.f,0.f,0.5f }, 0.6);
	}

	return steering;
}

//FACE
//***
SteeringOutput Face::CalculateSteering(float deltaT, SteeringAgent* pAgent)
{
	SteeringOutput steering{};
	return steering;
}


//PURSUIT
//***
SteeringOutput Pursuit::CalculateSteering(float deltaT, SteeringAgent* pAgent)
{
	SteeringOutput steering{};


	float distance = Elite::Distance(pAgent->GetPosition(), m_Target.Position);
	float T = distance / pAgent->GetMaxLinearSpeed();
	Elite::Vector2 futurePos = m_Target.Position + (m_Target.LinearVelocity * T);
	m_Target = TargetData(futurePos);

	if (pAgent->CanRenderBehavior())
	{
		Elite::Vector2 pos = pAgent->GetPosition();
		DEBUGRENDERER2D->DrawCircle(m_Target.Position, 2, { 0.f,1.f,1.f,0.5f }, 0.4f);
	}

	return Seek::CalculateSteering(deltaT, pAgent);
}

//EVADE
//****
SteeringOutput Evade::CalculateSteering(float deltaT, SteeringAgent* pAgent)
{
	float distance = Elite::Distance(pAgent->GetPosition(), m_Target.Position);
	float T = distance / pAgent->GetMaxLinearSpeed();
	Elite::Vector2 futurePos = m_Target.Position + m_Target.LinearVelocity * T;

	const auto distanceToTarget = Elite::Distance(pAgent->GetPosition(), m_Target.Position);
	if (distanceToTarget > m_Radius)
	{
		SteeringOutput steering{};
		steering.IsValid = false;

		if (pAgent->CanRenderBehavior())
		{
			Elite::Vector2 pos = pAgent->GetPosition();
			DEBUGRENDERER2D->DrawCircle(pos, m_Radius, { 0.f,0.f,1.f,0.5f }, 0.4f);
		}
		return steering;
	}


	m_Target = TargetData(futurePos);

	SteeringOutput output = Flee::CalculateSteering(deltaT, pAgent);
	output.IsValid = true;

	return output;
}