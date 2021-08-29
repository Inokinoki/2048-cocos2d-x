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
