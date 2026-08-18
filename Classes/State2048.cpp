#include "State2048.h"

void State2048::restart()
{
	for (int i = 0; i <= 3; i++) {
		for (int j = 0; j <= 3; j++) {
			square_state[i][j] = 0;
		}
	}
	score = 0;
}

bool State2048::move(Direction direction, std::vector<TileMove> &moves)
{
	moves.clear();
	bool changed = false;

	switch (direction)
	{
	case Direction::Left:
		for (int y = 0; y < 4; y++) {
			const int xs[4] = { 0, 1, 2, 3 };
			const int ys[4] = { y, y, y, y };
			slideLine(xs, ys, moves, changed);
		}
		break;
	case Direction::Right:
		for (int y = 0; y < 4; y++) {
			const int xs[4] = { 3, 2, 1, 0 };
			const int ys[4] = { y, y, y, y };
			slideLine(xs, ys, moves, changed);
		}
		break;
	case Direction::Down:
		for (int x = 0; x < 4; x++) {
			const int xs[4] = { x, x, x, x };
			const int ys[4] = { 0, 1, 2, 3 };
			slideLine(xs, ys, moves, changed);
		}
		break;
	case Direction::Up:
		for (int x = 0; x < 4; x++) {
			const int xs[4] = { x, x, x, x };
			const int ys[4] = { 3, 2, 1, 0 };
			slideLine(xs, ys, moves, changed);
		}
		break;
	}

	return changed;
}

bool State2048::canMove() const
{
	if (!isFull()) {
		return true;
	}

	for (int x = 0; x < 4; x++) {
		for (int y = 0; y < 4; y++) {
			int value = square_state[x][y];
			if (x < 3 && square_state[x + 1][y] == value) {
				return true;
			}
			if (y < 3 && square_state[x][y + 1] == value) {
				return true;
			}
		}
	}

	return false;
}

bool State2048::isFull() const
{
	for (int x = 0; x < 4; x++) {
		for (int y = 0; y < 4; y++) {
			if (square_state[x][y] == 0) {
				return false;
			}
		}
	}
	return true;
}

bool State2048::hasWon() const
{
	for (int x = 0; x < 4; x++) {
		for (int y = 0; y < 4; y++) {
			if (square_state[x][y] >= 2048) {
				return true;
			}
		}
	}
	return false;
}

bool State2048::hasSquare(int num_x, int num_y) const
{
	if (num_x < 0 || num_x > 3 || num_y < 0 || num_y > 3) {
		return false;
	}
	return square_state[num_x][num_y] != 0;
}

bool State2048::squareStateEqual(const int a[4][4], const int b[4][4]) const
{
	for (int x = 0; x < 4; x++) {
		for (int y = 0; y < 4; y++) {
			if (a[x][y] != b[x][y]) {
				return false;
			}
		}
	}
	return true;
}

void State2048::slideLine(const int xs[4], const int ys[4], std::vector<TileMove> &moves, bool &changed)
{
	struct Source
	{
		int x;
		int y;
		int value;
	};

	struct Slot
	{
		int value;
		Source from[2];
		int fromCount;
	};

	Source sources[4];
	int sourceCount = 0;
	for (int i = 0; i < 4; i++) {
		int value = square_state[xs[i]][ys[i]];
		if (value != 0) {
			sources[sourceCount].x = xs[i];
			sources[sourceCount].y = ys[i];
			sources[sourceCount].value = value;
			sourceCount++;
		}
	}

	Slot slots[4];
	int slotCount = 0;
	for (int i = 0; i < sourceCount; i++) {
		if (slotCount > 0 &&
			slots[slotCount - 1].fromCount == 1 &&
			slots[slotCount - 1].value == sources[i].value) {
			slots[slotCount - 1].value *= 2;
			slots[slotCount - 1].from[1] = sources[i];
			slots[slotCount - 1].fromCount = 2;
			score += slots[slotCount - 1].value;
		}
		else {
			slots[slotCount].value = sources[i].value;
			slots[slotCount].from[0] = sources[i];
			slots[slotCount].fromCount = 1;
			slotCount++;
		}
	}

	for (int i = 0; i < 4; i++) {
		square_state[xs[i]][ys[i]] = 0;
	}

	for (int i = 0; i < slotCount; i++) {
		int destX = xs[i];
		int destY = ys[i];
		square_state[destX][destY] = slots[i].value;

		for (int f = 0; f < slots[i].fromCount; f++) {
			const Source &src = slots[i].from[f];
			bool merged = (slots[i].fromCount == 2 && f == 0);
			bool disappearing = (slots[i].fromCount == 2 && f == 1);
			if (src.x != destX || src.y != destY || merged || disappearing) {
				changed = true;
			}

			TileMove move;
			move.fromX = src.x;
			move.fromY = src.y;
			move.toX = destX;
			move.toY = destY;
			move.oldValue = src.value;
			move.newValue = slots[i].value;
			move.merged = merged;
			move.disappearing = disappearing;
			moves.push_back(move);
		}
	}
}
