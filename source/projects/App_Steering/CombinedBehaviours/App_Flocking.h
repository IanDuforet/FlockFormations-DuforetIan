#ifndef STEERINGBEHAVIORS_APPLICATION_H
#define STEERINGBEHAVIORS_APPLICATION_H
//-----------------------------------------------------------------
// Includes & Forward Declarations
//-----------------------------------------------------------------
#include "framework/EliteInterfaces/EIApp.h"
#include "../SteeringBehaviors.h"

class SteeringAgent;
class LeaderAgent;
class Flock;

//-----------------------------------------------------------------
// Application
//-----------------------------------------------------------------
class App_Flocking final : public IApp
{
public:
	//Constructor & Destructor
	App_Flocking() = default;
	virtual ~App_Flocking();

	//App Functions
	void Start() override;
	void Update(float deltaTime) override;
	void Render(float deltaTime) const override;

private:
	//Datamembers
	TargetData m_MouseTarget = {};
	Flock* m_pFlock = nullptr;

	//Evade target
	SteeringAgent* m_pEvadeTarget = nullptr;
	Wander* m_pWander = nullptr;

	//Leader
	LeaderAgent* m_pLeaderAgent = nullptr;
	
	bool m_UseMouseTarget = true;
	bool m_VisualizeMouseTarget = true;
	
	float m_TrimWorldSize = 100.f;

	//C++ make the class non-copyable
	App_Flocking(const App_Flocking&) = delete;
	App_Flocking& operator=(const App_Flocking&) = delete;
};
#endif