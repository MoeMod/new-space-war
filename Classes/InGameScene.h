/****************************************************************************
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 
 http://www.cocos2d-x.org
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************/

#ifndef __INGAME_SCENE_H__
#define __INGAME_SCENE_H__

#include "cocos2d.h"
#include <vector>

class InGameScene : public cocos2d::Scene
{
public:
	class CPlane : public cocos2d::Sprite
	{
	public:
		CREATE_FUNC(CPlane);
		virtual bool init() override;
	};

	class CEnemy : public cocos2d::Sprite
	{
	public:
		CREATE_FUNC(CEnemy);
		virtual bool init() override;
	};

	class CBullet : public cocos2d::Sprite
	{
	public:
		CREATE_FUNC(CBullet);
		virtual bool init() override;
	};


    static cocos2d::Scene* createScene();

    virtual bool init();
    
    // a selector callback
	void OnReturnButtonClicked(cocos2d::Ref* pSender);
    
    // implement the "static create()" method manually
    CREATE_FUNC(InGameScene);

	virtual void OnCreateEnemy(float dt);
	virtual void PlaneShoot();
	virtual void OnPlaneBeingHit();

	virtual void OnBulletHitEnemy(CBullet *bullet, CEnemy *enemy);

	virtual void update(float dt) override;
	virtual void onEnter() override;
	virtual void onExit() override;
	
	void UpdateStats();
	cocos2d::EventListenerPhysicsContact *createContactListener();
	cocos2d::EventListenerMouse *createMouseListener();

public:
	CPlane *m_pPlane;
	cocos2d::Label *m_pScores;
	std::vector<CEnemy *> m_pEnemies;
	std::vector<CBullet *> m_pBullets;
	int m_iShotCount;
	int m_iKilled;
};

#endif // __INGAME_SCENE_H__
