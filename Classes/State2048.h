#pragma once

#include <vector>

class State2048
{
public:
	enum class Direction
	{
		Up,
		Down,
		Left,
		Right
	};

	struct TileMove
	{
		int fromX;
		int fromY;
		int toX;
		int toY;
		int oldValue;
		int newValue;
		bool merged;
		bool disappearing;
	};

	void restart();
	bool move(Direction direction, std::vector<TileMove> &moves);

	bool canMove() const;
	bool isFull() const;
	bool hasWon() const;
	bool hasSquare(int num_x, int num_y) const;

	int square_state[4][4];
	int score;

protected:
	bool squareStateEqual(const int a[4][4], const int b[4][4]) const;
	void slideLine(const int xs[4], const int ys[4], std::vector<TileMove> &moves, bool &changed);
};
