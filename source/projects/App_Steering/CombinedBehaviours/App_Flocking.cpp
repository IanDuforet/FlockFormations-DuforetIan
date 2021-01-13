//Precompiled Header [ALWAYS ON TOP IN CPP]
#include "stdafx.h"

//Includes
#include "App_Flocking.h"
#include "../SteeringAgent.h"
#include "../LeaderAgent.h"
#include "Flock.h"

//Destructor
App_Flocking::~App_Flocking()
{	
	SAFE_DELETE(m_pFlock);
	SAFE_DELETE(m_pEvadeTarget);
	SAFE_DELETE(m_pWander);
	SAFE_DELETE(m_pLeaderAgent);
}

//Functions
void App_Flocking::Start()
{
	DEBUGRENDERER2D->GetActiveCamera()->SetZoom(55.0f);
	DEBUGRENDERER2D->GetActiveCamera()->SetCenter(Elite::Vector2(m_TrimWorldSize / 1.5f, m_TrimWorldSize / 2));

	m_pEvadeTarget = new SteeringAgent();
	m_pWander = new Wander();
	m_pEvadeTarget->SetSteeringBehavior(m_pWander);
	m_pEvadeTarget->SetBodyColor({ 1,0,0 });
	m_pEvadeTarget->SetMaxLinearSpeed(100);
	m_pEvadeTarget->SetLinearVelocity({ 100, 100 });

	m_pFlock = new Flock(100, m_TrimWorldSize, m_pEvadeTarget, true);
	
	m_pLeaderAgent = new LeaderAgent(m_pFlock->GetAgents(),10);
	m_pLeaderAgent->SetSteeringBehavior(m_pWander);
	m_pLeaderAgent->SetBodyColor({ 0,0,1 });
}

void App_Flocking::Update(float deltaTime)
{
	//INPUT
	if (INPUTMANAGER->IsMouseButtonUp(InputMouseButton::eLeft) && m_VisualizeMouseTarget)
	{
		auto const mouseData = INPUTMANAGER->GetMouseData(InputType::eMouseButton, InputMouseButton::eLeft);
		m_MouseTarget.Position = DEBUGRENDERER2D->GetActiveCamera()->ConvertScreenToWorld({ static_cast<float>(mouseData.X), static_cast<float>(mouseData.Y) });
		m_pFlock->SetSeekTarget(m_MouseTarget.Position);
	}
	m_pFlock->Update(deltaTime);
	m_pFlock->UpdateAndRenderUI();
	m_pEvadeTarget->Update(deltaTime);
	m_pEvadeTarget->TrimToWorld(m_TrimWorldSize);
	m_pLeaderAgent->Update(deltaTime);
	m_pLeaderAgent->TrimToWorld(m_TrimWorldSize);
}

void App_Flocking::Render(float deltaTime) const
{
	m_pFlock->Render(deltaTime);
	m_pEvadeTarget->Render(deltaTime);
	m_pLeaderAgent->Render(deltaTime);
	
	std::vector<Elite::Vector2> points =
	{
		{ -m_TrimWorldSize,m_TrimWorldSize },
			{ m_TrimWorldSize,m_TrimWorldSize },
			{ m_TrimWorldSize,-m_TrimWorldSize },
			{-m_TrimWorldSize,-m_TrimWorldSize }
	};
	DEBUGRENDERER2D->DrawPolygon(&points[0], 4, { 1,0,0,1 }, 0.4f);

	//Render Target
	if(m_VisualizeMouseTarget)
		DEBUGRENDERER2D->DrawSolidCircle(m_MouseTarget.Position, 0.3f, { 0.f,0.f }, { 1.f,0.f,0.f },-0.8f);
}
