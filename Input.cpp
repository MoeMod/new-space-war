#include "Input.h"
#include "InGameScene.h"

using namespace cocos2d;

InGameScene *GetInGameSceneInstance()
{
	return Director::getInstance()->getRunningScene()->getChildByName<InGameScene *>("InGameScene");
	//return dynamic_cast<InGameScene *>(Director::getInstance()->getRunningScene());
}

void Input_OnPosChanged(float x, float y)
{
	auto ps = GetInGameSceneInstance();
	if (!ps)
		return;
	
	if(ps->m_pPlane)
		ps->m_pPlane->setPosition(Director::getInstance()->convertToGL({ x,y }));
	
}

void Input_OnRotated(double delta_in_degrees)
{
	auto ps = GetInGameSceneInstance();
	if (!ps)
		return;
	
	if (ps->m_pPlane)
	{
		ps->m_pPlane->setRotation(ps->m_pPlane->getRotation() + delta_in_degrees);
	}
		
	
}

void Input_OnClicked()
{
	auto ps = GetInGameSceneInstance();
	if (!ps)
		return;

	ps->PlaneShoot();
}