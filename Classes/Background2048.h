#pragma once

#include "cocos2d.h"

#include "State2048.h"

#include <string>
#include <vector>

#define REF_HEIGHT	1920
#define REF_WIDTH	1080

class Background2048 : public cocos2d::LayerColor
{
	State2048 gameState;

	int height;
	int width;

	int square_location_x[4];
	int square_location_y[4];

	int square_width;
	float tileScale;

	cocos2d::Vec2 touchBegin;
	cocos2d::Sprite *tileSprites[4][4];
	cocos2d::Label *scoreLabel;
	cocos2d::Label *bestLabel;
	cocos2d::Node *overlay;
	cocos2d::Label *overlayTitle;
	cocos2d::MenuItemLabel *continueItem;
	cocos2d::MenuItemLabel *retryItem;

	bool animating;
	bool inputBlocked;
	bool wonShown;
	int bestScore;
	int moveGeneration;

	cocos2d::SpriteFrameCache *cache;

	void randomCreateSquare(bool animate);
	void handleMove(State2048::Direction direction);
	void applyMoveAnimations(const std::vector<State2048::TileMove> &moves);
	cocos2d::Sprite *createTileSprite(int value, int x, int y, bool animate);
	std::string frameNameForValue(int value) const;
	cocos2d::Vec2 positionFor(int x, int y) const;
	void updateScoreLabels();
	void clearTiles();
	void restartGame();
	void showWinOverlay();
	void showGameOverOverlay();
	void hideOverlay();
	void finishMove();

public:
	static cocos2d::Scene* createScene();

	void menuCloseCallback(cocos2d::Ref* pSender);
	void menuRestartCallback(cocos2d::Ref* pSender);
	void menuContinueCallback(cocos2d::Ref* pSender);
	void onKeyReleased(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event);
	bool onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event);
	void onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event);

	virtual bool init();

	CREATE_FUNC(Background2048);
};
