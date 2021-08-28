#pragma once

#include "cocos2d.h"

#define REF_HEIGHT	1920
#define REF_WIDTH	1080

class Background2048 : public cocos2d::Layer
{
public:
	static cocos2d::Scene* createScene();

	// a selector callback
	void menuCloseCallback(cocos2d::Ref* pSender);

	virtual bool init();

	// implement the "static create()" method manually
	CREATE_FUNC(Background2048);
};

