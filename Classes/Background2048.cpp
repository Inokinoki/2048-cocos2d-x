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
