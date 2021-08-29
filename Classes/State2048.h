#pragma once



class State2048
{
public:
	void restart();
	void update();

	int square_state[4][4];
	int score;

protected:
	bool canMove();
	bool squareStateEqual(int **square_state_1, int **square_state_2);
	bool isFull();
	bool hasSquare(int num_x, int num_y);
};

