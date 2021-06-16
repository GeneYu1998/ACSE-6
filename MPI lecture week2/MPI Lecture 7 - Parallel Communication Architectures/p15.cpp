#include <iostream>
#include <cstdlib>

using namespace std;

int initial_grid[4][4] = {{1, 2, 3, 4}, {5, 6, 7, 8}, {9, 10, -1, 11}, {13, 14, 15, 12} };   //numbers 1 - 15 for blank spot


const int max_moves = 100;

enum class direction
{
    up = 0,
    down,
    left,
    right
};

bool is_correct(const int **grid)
{
     // i * 4 + j + 1
     for (int i = 0; i < 4; ++i)
          for (int j = 0; j < 4; ++j)
              if(grid[i][j]!= i * 4 + j + 1 && (i!=3||j!=3))
                 return false;
      

     return true;
}

int find_sol(int grid[4][4], int current_move_num, int i_index, int j_index, direction prev_move, direction *direction_list)
{    
     if(current_move_num >= max_moves) return -1;

    int new_grid[4][4];
    
    //copying
    for (int i = 0; i < 4; ++i)
        for(int j = 0; j < 4; ++j)
            new_grid[i][j] = grid[i][j];

     //up
     if (prev_move != direction::down && i_index > 0)
     {
         new_grid[i_index][j_index] = new_grid[i_index - 1][j_index];
         new_grid[i_index - 1][j_index] = -1;

         direction *new_direction_list = new direction[max_moves];
         for (int i = 0; i < current_move_num; i++)
             new_direction_list[i] = direction_list[i];
         
         new_direction_list[current_move_num] = direction::up;

         int ans = find_sol(new_grid, current_move_num++, i_index - 1, j_index, direction::up, new_direction_list);

         if(ans != -1)
         {
             for (int i = 0; i < ans; i++)
                 direction_list[i] = new_direction_list[i];
             
             delete[] new_direction_list;
             return ans;
         }

     }
     

     //down


     //left


     //right
}