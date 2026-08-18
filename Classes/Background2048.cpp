#include "Background2048.h"

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <memory>
#include <string>
#include <utility>
#include <vector>

USING_NS_CC;

namespace
{
	const int kTileTag = 0x2048;
	const Color3B kTextBrown(0x77, 0x6e, 0x65);
	const Color3B kTextWhite(255, 255, 255);
	const Color4B kOverlayColor(238, 228, 218, 196);
	const float kMoveDuration = 0.12f;
	const float kSwipeThreshold = 24.0f;
}

Scene* Background2048::createScene()
{
	auto scene = Scene::create();
	auto layer = Background2048::create();
	scene->addChild(layer);
	return scene;
}

bool Background2048::init()
{
	Color4B black(0, 0, 0, 0);
	Color4B bgColor(0xfa, 0xf8, 0xef, 0xff);
	Color4F bgColorF(bgColor);
	Color4B boardColor(0xbb, 0xad, 0xa0, 0xff);
	Color4F boardColorF(boardColor);
	Color4B squareColor(0xcc, 0xc0, 0xb3, 0xff);
	Color4F squareColorF(squareColor);
	Color4F hudBoxColor(boardColorF);

	if (!LayerColor::initWithColor(black))
	{
		return false;
	}

	std::memset(tileSprites, 0, sizeof(tileSprites));
	animating = false;
	inputBlocked = false;
	wonShown = false;
	moveGeneration = 0;
	scoreLabel = nullptr;
	bestLabel = nullptr;
	overlay = nullptr;
	overlayTitle = nullptr;
	continueItem = nullptr;
	retryItem = nullptr;
	bestScore = UserDefault::getInstance()->getIntegerForKey("best_score", 0);

	auto visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

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

	float hudBottom = origin.y + marginHeight + boardMargin + boardWidth + width * 0.05f;
	float hudHeight = width * 0.14f;
	float boxWidth = width * 0.32f;
	float boxGap = width * 0.04f;
	float totalBoxes = boxWidth * 2 + boxGap;
	float scoreBoxX = origin.x + marginWidth + (width - totalBoxes) / 2;
	float bestBoxX = scoreBoxX + boxWidth + boxGap;
	int captionFont = std::max(12, (int)(width * 0.038f));
	int scoreFont = std::max(20, (int)(width * 0.065f));
	int titleFont = std::max(36, (int)(width * 0.13f));
	int buttonFont = std::max(18, (int)(width * 0.05f));

	auto hudNode = DrawNode::create();
	auto drawBox = [&](float x, float y, float w, float h) {
		Vec2 box[4] = {
			Vec2(x, y),
			Vec2(x + w, y),
			Vec2(x + w, y + h),
			Vec2(x, y + h)
		};
		hudNode->drawPolygon(box, 4, hudBoxColor, 1, hudBoxColor);
	};
	drawBox(scoreBoxX, hudBottom, boxWidth, hudHeight);
	drawBox(bestBoxX, hudBottom, boxWidth, hudHeight);
	this->addChild(hudNode, -2);

	auto scoreCaption = Label::createWithTTF("SCORE", "fonts/arial.ttf", captionFont);
	scoreCaption->setColor(Color3B(0xee, 0xe4, 0xda));
	scoreCaption->setPosition(Vec2(scoreBoxX + boxWidth / 2, hudBottom + hudHeight * 0.72f));
	this->addChild(scoreCaption, 1);

	auto bestCaption = Label::createWithTTF("BEST", "fonts/arial.ttf", captionFont);
	bestCaption->setColor(Color3B(0xee, 0xe4, 0xda));
	bestCaption->setPosition(Vec2(bestBoxX + boxWidth / 2, hudBottom + hudHeight * 0.72f));
	this->addChild(bestCaption, 1);

	scoreLabel = Label::createWithTTF("0", "fonts/arial.ttf", scoreFont);
	scoreLabel->setColor(kTextWhite);
	scoreLabel->setPosition(Vec2(scoreBoxX + boxWidth / 2, hudBottom + hudHeight * 0.32f));
	this->addChild(scoreLabel, 1);

	bestLabel = Label::createWithTTF("0", "fonts/arial.ttf", scoreFont);
	bestLabel->setColor(kTextWhite);
	bestLabel->setPosition(Vec2(bestBoxX + boxWidth / 2, hudBottom + hudHeight * 0.32f));
	this->addChild(bestLabel, 1);

	auto title2048 = Label::createWithTTF("2048", "fonts/Marker Felt.ttf", titleFont);
	title2048->setColor(kTextBrown);
	title2048->setPosition(Vec2(origin.x + marginWidth + width / 2,
		hudBottom + hudHeight + width * 0.22f));
	this->addChild(title2048, 1);

	Vector<MenuItem*> menuItems;

	auto closeItem = MenuItemImage::create(
		"CloseNormal.png",
		"CloseSelected.png",
		CC_CALLBACK_1(Background2048::menuCloseCallback, this));

	closeItem->setPosition(Vec2(origin.x + visibleSize.width - closeItem->getContentSize().width / 2,
		origin.y + closeItem->getContentSize().height / 2));

	auto newGameLabel = Label::createWithTTF("New Game", "fonts/arial.ttf", buttonFont);
	newGameLabel->setColor(kTextWhite);
	auto newGameItem = MenuItemLabel::create(
		newGameLabel,
		CC_CALLBACK_1(Background2048::menuRestartCallback, this));
	newGameItem->setPosition(Vec2(origin.x + marginWidth + width / 2,
		hudBottom + hudHeight + width * 0.08f));

	auto newGameBg = DrawNode::create();
	float ngw = width * 0.32f;
	float ngh = width * 0.08f;
	Vec2 ngBox[4] = {
		Vec2(-ngw / 2, -ngh / 2),
		Vec2(ngw / 2, -ngh / 2),
		Vec2(ngw / 2, ngh / 2),
		Vec2(-ngw / 2, ngh / 2)
	};
	newGameBg->drawPolygon(ngBox, 4, Color4F(Color4B(0x8f, 0x7a, 0x66, 0xff)), 1, Color4F(Color4B(0x8f, 0x7a, 0x66, 0xff)));
	newGameBg->setPosition(newGameItem->getPosition());
	this->addChild(newGameBg, 0);

	menuItems.pushBack(closeItem);
	menuItems.pushBack(newGameItem);

	auto menu = Menu::createWithArray(menuItems);
	menu->setPosition(Vec2::ZERO);
	this->addChild(menu, 1);

	auto label = Label::createWithTTF("Inoki Cocos2d-x 2048", "fonts/Marker Felt.ttf", 24);
	label->setColor(kTextBrown);
	label->setPosition(Vec2(origin.x + visibleSize.width / 2,
		origin.y + visibleSize.height - label->getContentSize().height));
	this->addChild(label, 1);

	overlay = Node::create();
	overlay->setVisible(false);
	this->addChild(overlay, 20);

	auto overlayVeil = DrawNode::create();
	overlayVeil->drawPolygon(boardRectangle, 4, Color4F(kOverlayColor), 1, Color4F(kOverlayColor));
	overlay->addChild(overlayVeil);

	overlayTitle = Label::createWithTTF("", "fonts/Marker Felt.ttf", 48);
	overlayTitle->setColor(kTextBrown);
	overlayTitle->setPosition(Vec2(
		origin.x + marginWidth + boardMargin + boardWidth / 2,
		origin.y + marginHeight + boardMargin + boardWidth * 0.62f));
	overlay->addChild(overlayTitle);

	auto continueLabel = Label::createWithTTF("Continue", "fonts/arial.ttf", buttonFont);
	continueLabel->setColor(Color3B(0x8f, 0x7a, 0x66));
	continueItem = MenuItemLabel::create(
		continueLabel,
		CC_CALLBACK_1(Background2048::menuContinueCallback, this));

	auto retryLabel = Label::createWithTTF("Try Again", "fonts/arial.ttf", buttonFont);
	retryLabel->setColor(Color3B(0x8f, 0x7a, 0x66));
	retryItem = MenuItemLabel::create(
		retryLabel,
		CC_CALLBACK_1(Background2048::menuRestartCallback, this));

	continueItem->setPosition(Vec2(
		origin.x + marginWidth + boardMargin + boardWidth / 2,
		origin.y + marginHeight + boardMargin + boardWidth * 0.42f));
	retryItem->setPosition(Vec2(
		origin.x + marginWidth + boardMargin + boardWidth / 2,
		origin.y + marginHeight + boardMargin + boardWidth * 0.28f));

	auto overlayMenu = Menu::create(continueItem, retryItem, nullptr);
	overlayMenu->setPosition(Vec2::ZERO);
	overlay->addChild(overlayMenu);

	gameState.restart();
	updateScoreLabels();

	auto listener = EventListenerKeyboard::create();
	listener->onKeyReleased = CC_CALLBACK_2(Background2048::onKeyReleased, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);

	auto touchListener = EventListenerTouchOneByOne::create();
	touchListener->onTouchBegan = CC_CALLBACK_2(Background2048::onTouchBegan, this);
	touchListener->onTouchEnded = CC_CALLBACK_2(Background2048::onTouchEnded, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener, this);

	cache = SpriteFrameCache::getInstance();
	cache->addSpriteFramesWithFile("squares.plist");
	tileScale = 1.0f;

	randomCreateSquare(true);
	randomCreateSquare(true);

	return true;
}

void Background2048::menuCloseCallback(Ref* pSender)
{
	Director::getInstance()->popToRootScene();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	exit(0);
#endif
}

void Background2048::menuRestartCallback(Ref* pSender)
{
	restartGame();
}

void Background2048::menuContinueCallback(Ref* pSender)
{
	hideOverlay();
}

void Background2048::onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event)
{
	switch (keyCode)
	{
	case EventKeyboard::KeyCode::KEY_UP_ARROW:
	case EventKeyboard::KeyCode::KEY_DPAD_UP:
	case EventKeyboard::KeyCode::KEY_W:
		handleMove(State2048::Direction::Up);
		break;
	case EventKeyboard::KeyCode::KEY_DOWN_ARROW:
	case EventKeyboard::KeyCode::KEY_DPAD_DOWN:
	case EventKeyboard::KeyCode::KEY_S:
		handleMove(State2048::Direction::Down);
		break;
	case EventKeyboard::KeyCode::KEY_LEFT_ARROW:
	case EventKeyboard::KeyCode::KEY_DPAD_LEFT:
	case EventKeyboard::KeyCode::KEY_A:
		handleMove(State2048::Direction::Left);
		break;
	case EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
	case EventKeyboard::KeyCode::KEY_DPAD_RIGHT:
	case EventKeyboard::KeyCode::KEY_D:
		handleMove(State2048::Direction::Right);
		break;
	case EventKeyboard::KeyCode::KEY_ESCAPE:
		menuCloseCallback(nullptr);
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
	if (offset.length() < kSwipeThreshold) {
		return;
	}

	if (std::abs(offset.x) > std::abs(offset.y))
	{
		if (offset.x < 0) {
			handleMove(State2048::Direction::Left);
		}
		else {
			handleMove(State2048::Direction::Right);
		}
	}
	else
	{
		if (offset.y < 0) {
			handleMove(State2048::Direction::Down);
		}
		else {
			handleMove(State2048::Direction::Up);
		}
	}
}

void Background2048::handleMove(State2048::Direction direction)
{
	if (animating || inputBlocked) {
		return;
	}

	std::vector<State2048::TileMove> moves;
	if (!gameState.move(direction, moves)) {
		return;
	}

	applyMoveAnimations(moves);
}

void Background2048::applyMoveAnimations(const std::vector<State2048::TileMove> &moves)
{
	animating = true;
	const int generation = ++moveGeneration;

	Sprite *sourceSprites[4][4];
	std::memcpy(sourceSprites, tileSprites, sizeof(tileSprites));
	std::memset(tileSprites, 0, sizeof(tileSprites));

	if (moves.empty()) {
		if (generation == moveGeneration) {
			finishMove();
		}
		return;
	}

	auto remaining = std::make_shared<int>((int)moves.size());

	for (const auto &move : moves) {
		Sprite *sprite = sourceSprites[move.fromX][move.fromY];
		sourceSprites[move.fromX][move.fromY] = nullptr;
		if (!sprite) {
			if (--(*remaining) == 0 && generation == moveGeneration) {
				finishMove();
			}
			continue;
		}

		sprite->setLocalZOrder(move.disappearing ? 4 : 3);
		auto dest = positionFor(move.toX, move.toY);
		auto moveTo = MoveTo::create(kMoveDuration, dest);
		auto done = CallFunc::create([this, sprite, move, remaining, generation]() {
			if (generation != moveGeneration) {
				return;
			}

			if (move.disappearing) {
				sprite->removeFromParent();
			}
			else {
				sprite->setLocalZOrder(2);
				tileSprites[move.toX][move.toY] = sprite;
				if (move.merged) {
					auto frame = cache->getSpriteFrameByName(frameNameForValue(move.newValue));
					if (frame) {
						sprite->setSpriteFrame(frame);
					}
					sprite->runAction(Sequence::create(
						ScaleTo::create(0.08f, tileScale * 1.15f),
						ScaleTo::create(0.08f, tileScale),
						nullptr));
				}
			}

			if (--(*remaining) == 0) {
				finishMove();
			}
		});
		sprite->runAction(Sequence::create(EaseOut::create(moveTo, 2.0f), done, nullptr));
	}
}

void Background2048::finishMove()
{
	randomCreateSquare(true);
	updateScoreLabels();

	if (gameState.hasWon() && !wonShown) {
		wonShown = true;
		animating = false;
		showWinOverlay();
		return;
	}

	if (!gameState.canMove()) {
		animating = false;
		showGameOverOverlay();
		return;
	}

	animating = false;
}

void Background2048::randomCreateSquare(bool animate)
{
	std::vector<std::pair<int, int>> empties;
	for (int x = 0; x < 4; x++) {
		for (int y = 0; y < 4; y++) {
			if (gameState.square_state[x][y] == 0) {
				empties.push_back(std::make_pair(x, y));
			}
		}
	}

	if (empties.empty()) {
		return;
	}

	int index = cocos2d::random(0, (int)empties.size() - 1);
	int num_x = empties[index].first;
	int num_y = empties[index].second;
	int value = (cocos2d::random(0, 9) == 0) ? 4 : 2;
	gameState.square_state[num_x][num_y] = value;
	createTileSprite(value, num_x, num_y, animate);
}

Sprite *Background2048::createTileSprite(int value, int x, int y, bool animate)
{
	auto frame = cache->getSpriteFrameByName(frameNameForValue(value));
	auto sprite = Sprite::createWithSpriteFrame(frame);
	sprite->setAnchorPoint(Vec2(0, 0));
	sprite->setTag(kTileTag);
	tileScale = (float)square_width / sprite->getContentSize().width;
	sprite->setPosition(positionFor(x, y));
	sprite->setScale(animate ? 0.0f : tileScale);
	this->addChild(sprite, 2);
	tileSprites[x][y] = sprite;

	if (animate) {
		sprite->runAction(EaseBackOut::create(ScaleTo::create(0.18f, tileScale)));
	}
	return sprite;
}

std::string Background2048::frameNameForValue(int value) const
{
	if (value < 2) {
		value = 2;
	}
	if (value > 8192) {
		value = 8192;
	}
	char buf[32];
	std::snprintf(buf, sizeof(buf), "_%d.png", value);
	return buf;
}

Vec2 Background2048::positionFor(int x, int y) const
{
	return Vec2(square_location_x[x], square_location_y[y]);
}

void Background2048::updateScoreLabels()
{
	if (gameState.score > bestScore) {
		bestScore = gameState.score;
		UserDefault::getInstance()->setIntegerForKey("best_score", bestScore);
		UserDefault::getInstance()->flush();
	}

	if (scoreLabel) {
		scoreLabel->setString(StringUtils::toString(gameState.score));
	}
	if (bestLabel) {
		bestLabel->setString(StringUtils::toString(bestScore));
	}
}

void Background2048::clearTiles()
{
	Vector<Node*> children = this->getChildren();
	for (auto child : children) {
		if (child && child->getTag() == kTileTag) {
			child->stopAllActions();
			child->removeFromParent();
		}
	}
	std::memset(tileSprites, 0, sizeof(tileSprites));
}

void Background2048::restartGame()
{
	moveGeneration++;
	animating = false;
	clearTiles();
	hideOverlay();
	gameState.restart();
	wonShown = false;
	updateScoreLabels();
	randomCreateSquare(true);
	randomCreateSquare(true);
}

void Background2048::showWinOverlay()
{
	inputBlocked = true;
	overlayTitle->setString("You Win!");
	continueItem->setVisible(true);
	retryItem->setVisible(true);
	overlay->setVisible(true);
}

void Background2048::showGameOverOverlay()
{
	inputBlocked = true;
	overlayTitle->setString("Game Over!");
	continueItem->setVisible(false);
	retryItem->setVisible(true);
	overlay->setVisible(true);
}

void Background2048::hideOverlay()
{
	inputBlocked = false;
	overlay->setVisible(false);
}
