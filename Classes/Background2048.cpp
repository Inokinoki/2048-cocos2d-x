#include "Background2048.h"

USING_NS_CC;

Scene* Background2048::createScene()
{
	// 'scene' is an autorelease object
	auto scene = Scene::create();

	// 'layer' is an autorelease object
	auto layer = Background2048::create();

	// add layer as a child to scene
	scene->addChild(layer);

	// return the scene
	return scene;
}

// on "init" you need to initialize your instance
bool Background2048::init()
{
	//////////////////////////////
	// 1. super init first
	Color4B white(0xff, 0xff, 0xff, 0xff);
	Color4B black(0, 0, 0, 0);
	Color3B black3(0, 0, 0);
	Color4B bgColor(0xfa, 0xf8, 0xef, 0xff);
	Color4F bgColorF(bgColor);
	Color4B boardColor(0xbb, 0xad, 0xa0, 0xff);
	Color4F boardColorF(boardColor);
	Color4B squareColor(0xcc, 0xc0, 0xb3, 0xff);
	Color4F squareColorF(squareColor);
	if (!LayerColor::initWithColor(black))
	{
		return false;
	}

	auto visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	// Create group background
	height = visibleSize.height;
	width = visibleSize.width;
	if (height * REF_WIDTH / REF_HEIGHT > width) {
		height = width * REF_HEIGHT / REF_WIDTH;
	}
	else {
		width = height * REF_WIDTH / REF_HEIGHT;
	}
	int marginWidth = (visibleSize.width - width) / 2;
	int marginHeight = (visibleSize.height - height) / 2;
	auto bgRectNode = DrawNode::create();
	Vec2 bgRectangle[4];
	bgRectangle[0] = Vec2(origin.x + marginWidth, origin.y + marginHeight);
	bgRectangle[1] = Vec2(origin.x + marginWidth + width, origin.y + marginHeight);
	bgRectangle[2] = Vec2(origin.x + marginWidth + width, origin.y + marginHeight + height);
	bgRectangle[3] = Vec2(origin.x + marginWidth, origin.y + marginHeight + height);
	bgRectNode->drawPolygon(bgRectangle, 4, bgColorF, 1, bgColorF);
	this->addChild(bgRectNode, -5);

	int boardMargin = width * 0.05;
	int boardWidth = width - 2 * boardMargin;
	auto boardRectNode = DrawNode::create();
	Vec2 boardRectangle[4];
	boardRectangle[0] = Vec2(origin.x + marginWidth + boardMargin,
								origin.y + marginHeight + boardMargin);
	boardRectangle[1] = Vec2(origin.x + marginWidth + boardMargin + boardWidth,
								origin.y + marginHeight + boardMargin);
	boardRectangle[2] = Vec2(origin.x + marginWidth + boardMargin + boardWidth,
								origin.y + marginHeight + boardMargin + boardWidth);
	boardRectangle[3] = Vec2(origin.x + marginWidth + boardMargin,
								origin.y + marginHeight + boardMargin + boardWidth);
	boardRectNode->drawPolygon(boardRectangle, 4, boardColorF, 1, boardColorF);
	this->addChild(boardRectNode, -4);

	// init square localtion
	square_width = (int)(boardWidth * 0.2);
	int square_margin = (int)(boardWidth * 0.04);
	int square_group_y = origin.y + marginHeight + boardMargin;
	int square_group_x = origin.x + marginWidth + boardMargin;
	for (int i=0;i<=3;i++) {
		square_location_x[i] = (int)(square_group_x +
			square_margin * (i + 1) +
			square_width * i);
	}
	for (int i = 0; i <= 3; i++) {
		square_location_y[i] = (int)(square_group_y +
			square_margin * (i + 1) +
			square_width * i);
	}
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++) {
			auto rectNode = DrawNode::create();
			Vec2 rectangle[4];
			rectangle[0] = Vec2(square_location_x[j], square_location_y[i]);
			rectangle[1] = Vec2(square_location_x[j] + square_width, square_location_y[i]);
			rectangle[2] = Vec2(square_location_x[j] + square_width, square_location_y[i] + square_width);
			rectangle[3] = Vec2(square_location_x[j], square_location_y[i] + square_width);
			rectNode->drawPolygon(rectangle, 4, squareColorF, 1, squareColorF);
			this->addChild(rectNode, -3);
		}
	}

	/////////////////////////////
	// 2. add a menu item with "X" image, which is clicked to quit the program
	//    you may modify it.
	Vector<MenuItem*> menuItems;

	// add a "close" icon to exit the progress. it's an autorelease object
	auto closeItem = MenuItemImage::create(
		"CloseNormal.png",
		"CloseSelected.png",
		CC_CALLBACK_1(Background2048::menuCloseCallback, this));

	closeItem->setPosition(Vec2(origin.x + visibleSize.width - closeItem->getContentSize().width / 2,
		origin.y + closeItem->getContentSize().height / 2));

	menuItems.pushBack(closeItem);

	// create menu, it's an autorelease object
	auto menu = Menu::createWithArray(menuItems);
	menu->setPosition(Vec2::ZERO);
	this->addChild(menu, 1);

	/////////////////////////////
	// 3. add your codes below...

	// add a label shows "Hello World"
	// create and initialize a label

	auto label = Label::createWithTTF("Inoki Cocos2d-x 2048", "fonts/Marker Felt.ttf", 24);
	label->setColor(black3);

	// position the label on the center of the screen
	label->setPosition(Vec2(origin.x + visibleSize.width / 2,
		origin.y + visibleSize.height - label->getContentSize().height));

	// add the label as a child to this layer
	this->addChild(label, 1);

	// Reset the game state
	gameState.restart();

	// Add keyboard listener
	auto listener = EventListenerKeyboard::create();
	listener->onKeyReleased = CC_CALLBACK_2(Background2048::onKeyReleased, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);

	// Add touch listener
	auto touchListener = EventListenerTouchOneByOne::create();
	touchListener->onTouchBegan = CC_CALLBACK_2(Background2048::onTouchBegan, this);
	touchListener->onTouchEnded = CC_CALLBACK_2(Background2048::onTouchEnded, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener, this);

	// Create sprite
	cache = SpriteFrameCache::getInstance();
	cache->addSpriteFramesWithFile("squares.plist");

	// Create the first square
	randomCreateSquare();

	return true;
}

void Background2048::menuCloseCallback(Ref* pSender)
{
	//Close the cocos2d-x game scene and quit the application
	Director::getInstance()->popToRootScene();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	exit(0);
#endif

	/*To navigate back to native iOS screen(if present) without quitting the application  ,do not use Director::getInstance()->end() and exit(0) as given above,instead trigger a custom event created in RootViewController.mm as below*/

	//EventCustom customEndEvent("game_scene_close_event");
	//_eventDispatcher->dispatchEvent(&customEndEvent);


}

void Background2048::onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event)
{
	log("Key with keycode %d released", keyCode);
	switch (keyCode)
	{
	case EventKeyboard::KeyCode::KEY_UP_ARROW:
	case EventKeyboard::KeyCode::KEY_DPAD_UP:
		// Up
		break;
	case EventKeyboard::KeyCode::KEY_DOWN_ARROW:
	case EventKeyboard::KeyCode::KEY_DPAD_DOWN:
		// Down
		break;
	case EventKeyboard::KeyCode::KEY_LEFT_ARROW:
	case EventKeyboard::KeyCode::KEY_DPAD_LEFT:
		// Left
		break;
	case EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
	case EventKeyboard::KeyCode::KEY_DPAD_RIGHT:
		// Right
		break;
	default:
		break;
	}
}

bool Background2048::onTouchBegan(Touch* touch, Event* event)
{
	touchBegin = touch->getLocation();
	return true;
}

void Background2048::onTouchEnded(Touch* touch, Event* event)
{
	Vec2 offset = touch->getLocation() - touchBegin;
	if (abs(offset.x) > abs(offset.y))
	{
		if (offset.x < -5)
		{
			// TODO: LEFT
			log("left");
		}
		if (offset.x > 5)
		{
			// TODO: RIGHT
			log("right");
		}
	}
	else
	{
		if (offset.y < -5)
		{
			// TODO: DOWN
			log("down");
		}
		if (offset.y > 5)
		{
			// TODO: UP
			log("up");
		}
	}
	log("touch ended");
}

void Background2048::randomCreateSquare()
{
	int num_x = 0, num_y = 0;

	num_x = random() % 4;
	num_y = random() % 4;

	while (gameState.square_state[num_x][num_y] != 0)
	{
		num_x = random() % 4;
		num_y = random() % 4;
	}

	int radom_num = random() %50;
	if (radom_num < 49)
	{
		gameState.square_state[num_x][num_y] = 2;
		auto s = Sprite::createWithSpriteFrame(cache->getSpriteFrameByName("_2.png"));
		s->setPosition(Vec2(square_location_x[num_x], square_location_y[num_y]));
		s->setAnchorPoint(Vec2(0, 0));
		s->setScale((float)square_width / s->getContentSize().width);
		this->addChild(s, 0);

		// Test action
		auto moveBy = MoveBy::create(2, Vec2(200, 0));
		s->runAction(moveBy);
	}
	else
	{
		gameState.square_state[num_x][num_y] = 4;
		auto s = Sprite::createWithSpriteFrame(cache->getSpriteFrameByName("_4.png"));
		s->setPosition(Vec2(square_location_x[num_x], square_location_y[num_y]));
		s->setAnchorPoint(Vec2(0, 0));
		s->setScale((float)square_width / s->getContentSize().width);
		this->addChild(s, 0);
	}
}
