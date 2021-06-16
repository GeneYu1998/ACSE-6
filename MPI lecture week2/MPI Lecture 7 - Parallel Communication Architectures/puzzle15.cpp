#include <iostream>

using namespace std;

int initial_grid[4][4] = { {1,2,3,4},{5,-1,6,8},{9,10,7,11},{13,14,15,12} };			//numbers 1- 15 or -1 for blank spot

const int max_moves = 10;

enum class direction
{
	up = 0,
	down,
	left,
	right,
	none
};


bool is_correct(int grid[4][4])
{
	//index i * 4 + j + 1
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			if (grid[i][j] != i * 4 + j + 1 && (i != 3 || j != 3))
				return false;

	return true;
}


int find_solution_from_grid(int grid[4][4], int current_move_num, int i_index, int j_index, direction prev_move, direction* direction_list)			//returns steps to solution or -1 for no solution
{
	if (current_move_num >= max_moves)
		return -1;

	direction* new_direction_list[4];
	int ans[4] = { -1,-1,-1,-1 };

	for (int i = 0; i < 4; i++)
		new_direction_list[i] = new direction[max_moves];

	//up
	if (prev_move != direction::down && i_index > 0)
	{
		int new_grid[4][4];
		for (int i = 0; i < 4; i++)
			for (int j = 0; j < 4; j++)
				new_grid[i][j] = grid[i][j];

		new_grid[i_index][j_index] = new_grid[i_index - 1][j_index];
		new_grid[i_index - 1][j_index] = -1;

		if (is_correct(new_grid))
		{
			direction_list[current_move_num] = direction::up;
			for (int i = 0; i < 4; i++)
				delete[] new_direction_list[i];
			return current_move_num + 1;
		}


		for (int i = 0; i < current_move_num; i++)
			new_direction_list[0][i] = direction_list[i];
		new_direction_list[0][current_move_num] = direction::up;

		ans[0] = find_solution_from_grid(new_grid, current_move_num + 1, i_index - 1, j_index, direction::up, new_direction_list[0]);
	}

	//down
	if (prev_move != direction::up && i_index < 3)
	{
		int new_grid[4][4];
		for (int i = 0; i < 4; i++)
			for (int j = 0; j < 4; j++)
				new_grid[i][j] = grid[i][j];

		new_grid[i_index][j_index] = new_grid[i_index + 1][j_index];
		new_grid[i_index + 1][j_index] = -1;

		if (is_correct(new_grid))
		{
			direction_list[current_move_num] = direction::down;
			for (int i = 0; i < 4; i++)
				delete[] new_direction_list[i];
			return current_move_num + 1;
		}

		for (int i = 0; i < current_move_num; i++)
			new_direction_list[1][i] = direction_list[i];
		new_direction_list[1][current_move_num] = direction::down;

		ans[1] = find_solution_from_grid(new_grid, current_move_num + 1, i_index + 1, j_index, direction::down, new_direction_list[1]);
	}

	//left
	if (prev_move != direction::right && j_index > 0)
	{
		int new_grid[4][4];
		for (int i = 0; i < 4; i++)
			for (int j = 0; j < 4; j++)
				new_grid[i][j] = grid[i][j];

		new_grid[i_index][j_index] = new_grid[i_index][j_index - 1];
		new_grid[i_index][j_index - 1] = -1;

		if (is_correct(new_grid))
		{
			direction_list[current_move_num] = direction::left;
			for (int i = 0; i < 4; i++)
				delete[] new_direction_list[i];
			return current_move_num + 1;
		}

		for (int i = 0; i < current_move_num; i++)
			new_direction_list[2][i] = direction_list[i];
		new_direction_list[2][current_move_num] = direction::left;

		ans[2] = find_solution_from_grid(new_grid, current_move_num + 1, i_index, j_index - 1, direction::left, new_direction_list[2]);
	}

	//right
	if (prev_move != direction::left && j_index < 3)
	{
		int new_grid[4][4];
		for (int i = 0; i < 4; i++)
			for (int j = 0; j < 4; j++)
				new_grid[i][j] = grid[i][j];

		new_grid[i_index][j_index] = new_grid[i_index][j_index + 1];
		new_grid[i_index][j_index + 1] = -1;

		if (is_correct(new_grid))
		{
			direction_list[current_move_num] = direction::right;
			for (int i = 0; i < 4; i++)
				delete[] new_direction_list[i];
			return current_move_num + 1;
		}
		for (int i = 0; i < current_move_num; i++)
			new_direction_list[3][i] = direction_list[i];
		new_direction_list[3][current_move_num] = direction::right;

		ans[3] = find_solution_from_grid(new_grid, current_move_num + 1, i_index, j_index + 1, direction::right, new_direction_list[3]);
	}

	int best_index = -1;
	int best_ans = max_moves + 1;

	for (int i = 0; i < 4; i++)
	{
		if (ans[i] != -1 && ans[i] < best_ans)
		{
			best_ans = ans[i];
			best_index = i;
		}
	}

	if (best_index != -1)
	{
		for (int i = 0; i < best_ans; i++)
			direction_list[i] = new_direction_list[best_index][i];
		for (int i = 0; i < 4; i++)
			delete[] new_direction_list[i];
		return best_ans;			//some recursive call ha found the solution
	}

	for (int i = 0; i < 4; i++)
		delete[] new_direction_list[i];

	return -1;
}


int main(int argc, char* argv[])
{
	direction* direction_list = new direction[max_moves];
	int ans = find_solution_from_grid(initial_grid, 0, 1, 1, direction::none, direction_list);

	if (ans == -1)
	{
		cout << "No solution found within " << max_moves << " moves" << endl;
	}
	else
	{
		cout << "The answer sequence: ";
		for (int i = 0; i < ans; i++)
		{
			if (direction_list[i] == direction::up)
				cout << "up\t";
			else if (direction_list[i] == direction::down)
				cout << "down\t";
			else if (direction_list[i] == direction::left)
				cout << "left\t";
			else if (direction_list[i] == direction::right)
				cout << "right\t";
		}
	}

	return(0);
}