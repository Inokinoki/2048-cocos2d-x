#pragma once

#include "cocos2d.h"

#include "State2048.h"

#define REF_HEIGHT	1920
#define REF_WIDTH	1080

class Background2048 : public cocos2d::LayerColor
{
	State2048 gameState;

	int height;
	int width;

	int square_location_x[4];
	int square_location_y[4];
public:
	static cocos2d::Scene* createScene();

	// a selector callback
	void menuCloseCallback(cocos2d::Ref* pSender);
	void onKeyReleased(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event);

	virtual bool init();

	// implement the "static create()" method manually
	CREATE_FUNC(Background2048);
};

