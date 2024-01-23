#define BOARD_SIZE 3

#define X_VALUE 1
#define O_VALUE -1
#define EMPTY_CELL 0

//_____________________________

#define CENTRAL_HOME 10
#define CORNER_HOME 9
#define MIDDLE_HOME 8
#define NEAR_HOME 14
#define BIG_BOARD_VALUE 3

//_____________________________

#define MAP_EXPORT_FILE "map.txt"
#define STATE_EXPORT_FILE "state.txt"
#define MOVE_EXPORT_FILE "move.txt"

#include <iostream>
#include <fstream>

using namespace std;

struct Cell
{
    int board[BOARD_SIZE][BOARD_SIZE];
};

Cell cells[BOARD_SIZE * BOARD_SIZE];
bool boards[BOARD_SIZE * BOARD_SIZE];
int bigBoard[BOARD_SIZE][BOARD_SIZE];

void read_whole_map()
{
    ifstream file(MAP_EXPORT_FILE);

    int lim = BOARD_SIZE * BOARD_SIZE;
    for (int i = 0; i < lim; i++)
    {
        for (int j = 0; j < lim; j++)
        {
            int cell_x = i / BOARD_SIZE;
            int cell_y = j / BOARD_SIZE;
            int board_x = i % BOARD_SIZE;
            int board_y = j % BOARD_SIZE;
            int cell_idx = cell_x * BOARD_SIZE + cell_y;

            int element;
            file >> cells[cell_idx].board[board_x][board_y];
        }
    }

    file.close();
}

void read_available_boards()
{
    ifstream file(STATE_EXPORT_FILE);

    while (!file.eof())
    {
        int e;
        file >> e;
        boards[e] = true;
    }

    file.close();
}

void export_move(int board_idx, int cell_idx)
{
    cout << board_idx << " " << cell_idx << endl;
    ofstream file(MOVE_EXPORT_FILE);

    file << board_idx << " " << cell_idx;
    file.close();
}

//__________________________________________________________________________________________ my function :

int check_winner(int board[BOARD_SIZE][BOARD_SIZE]);

void analyze_board(Cell cells[9]);

int board_score(int board[BOARD_SIZE][BOARD_SIZE]);

int board_score_plus(int board[BOARD_SIZE][BOARD_SIZE]);

int give_score(Cell cells[BOARD_SIZE * BOARD_SIZE], int analyze_big_board[BOARD_SIZE][BOARD_SIZE]);

void copy_Cell(Cell main[BOARD_SIZE * BOARD_SIZE], Cell temp[BOARD_SIZE * BOARD_SIZE]);

void mini_max(Cell board[BOARD_SIZE * BOARD_SIZE], int depth, int turn);

void export_board();

//_______________________________________________________________________________________ move :

void move()
{
    // Start your code here
    int chosen_board_idx = 0;
    int chosen_cell_idx = 0;

    analyze_board(cells);

    // Sample code
    int lim = BOARD_SIZE * BOARD_SIZE;
    for (int t = 0; t < lim; t++)
    {
        if (boards[t]) // boards[t] == true
        {
            for (int i = 0; i < BOARD_SIZE; i++)
            {
                for (int j = 0; j < BOARD_SIZE; j++)
                {
                    if (cells[t].board[i][j] == EMPTY_CELL)
                    {
                        int cell_idx = i * BOARD_SIZE + j;

                        chosen_board_idx = t;
                        chosen_cell_idx = cell_idx;
                        mini_max(cells, 2, 1);
                        export_move(chosen_board_idx, chosen_cell_idx);
                        return;
                    }
                }
            }
        }
    }

    // End your code here
}

int main()
{

    fstream test;
    test.open("board.txt", ios::out);
    test << "";
    test.close();
    read_whole_map();
    read_available_boards();
    move();

    return 0;
}

//_________________________________________________________________________________ function :

int check_winner(int board[BOARD_SIZE][BOARD_SIZE])
{
    for (int i = 0; i < BOARD_SIZE; i++)
    {
        if (board[i][0] == board[i][1] && board[i][1] == board[i][2] && board[i][0] != EMPTY_CELL)
        {
            return board[i][0];
        }
        else if (board[0][i] == board[1][i] && board[1][i] == board[2][i] && board[0][i] != EMPTY_CELL)
        {
            return board[0][i];
        }
    }
    if (board[0][0] == board[1][1] && board[1][1] == board[2][2] && board[0][0] != EMPTY_CELL)
    {
        return board[0][0];
    }
    else if (board[0][2] == board[1][1] && board[1][1] == board[2][0] && board[0][2] != EMPTY_CELL)
    {
        return board[0][2];
    }
    return 0;
}

void analyze_board(Cell cells[9], int analyze_board[9])
{
    for (int i = 0; i < 9; i++)
    {
        analyze_board[i] = check_winner(cells[i].board);
    }
}

void analyze_board(Cell cells[9])
{
    for (int i = 0; i < BOARD_SIZE; i++)
    {
        for (int j = 0; j < BOARD_SIZE; j++)
        {
            bigBoard[i][j] = check_winner(cells[i].board);
        }
    }
}

int board_score(int board[BOARD_SIZE][BOARD_SIZE])
{
    int sum = 0;
    int k;

    for (int i = 0; i < BOARD_SIZE; i++)
    {
        for (int j = 0; j < BOARD_SIZE; j++)
        {
            k = 3 * i + j;
            if (k == 4)
            {
                sum += board[i][j] * CENTRAL_HOME;
            }
            else if (k == 0 || k == 2 || k == 6 || k == 8)
            {
                sum += board[i][j] * CORNER_HOME;
            }
            else if (k == 1 || k == 3 || k == 5 || k == 7)
            {
                sum += board[i][j] * MIDDLE_HOME;
            }
        }
    }
    return sum;
}

int board_score_plus(int board[BOARD_SIZE][BOARD_SIZE])
{
    int sum = 0, row = 0, col = 0;

    for (int i = 0; i < BOARD_SIZE; i++)
    {
        row = board[i][0] + board[i][1] + board[i][2];
        col = board[0][i] + board[1][i] + board[2][i];

        if (row == 2)
        {
            sum += NEAR_HOME;
        }
        else if (row == -2)
        {
            sum -= NEAR_HOME;
        }
        if (col == 2)
        {
            sum += NEAR_HOME;
        }
        else if (col == -2)
        {
            sum -= NEAR_HOME;
        }
    }

    row = board[0][0] + board[1][1] + board[2][2];
    col = board[2][0] + board[1][1] + board[0][2];
    if (row == 2)
    {
        sum += NEAR_HOME;
    }
    else if (row == -2)
    {
        sum -= NEAR_HOME;
    }
    if (col == 2)
    {
        sum += NEAR_HOME;
    }
    else if (col == -2) // +2 or -2 for each player (first or second ) can be different .
    {
        sum -= NEAR_HOME;
    }
    return sum;
}

int give_score(Cell cells[BOARD_SIZE * BOARD_SIZE], int analyze_big_board[BOARD_SIZE][BOARD_SIZE])
{
    // here
    int sum = 0;

    for (int i = 0; i < BOARD_SIZE * BOARD_SIZE; i++)
    {
        for (int j = 0; j < BOARD_SIZE; j++)
        {
            if (analyze_big_board[i][j] == EMPTY_CELL)
            {
                sum += board_score(cells[i].board);
                sum += board_score_plus(cells[i].board);
            }
            else
            {
                sum += board_score(analyze_big_board) * BIG_BOARD_VALUE;
                sum += board_score_plus(analyze_big_board) * BIG_BOARD_VALUE;
            }
        }
    }
    return sum;
}

void copy_Cell(Cell main[BOARD_SIZE * BOARD_SIZE], Cell temp[BOARD_SIZE * BOARD_SIZE])
{
    for (int i = 0; i < BOARD_SIZE; i++)
    {
        for (int j = 0; j < BOARD_SIZE; j++)
        {
            for (int k = 0; k < BOARD_SIZE; k++)
            {
                temp[i].board[j][k] = main[i].board[j][k];
            }
        }
    }
}

void mini_max(Cell board[BOARD_SIZE * BOARD_SIZE], int depth, int turn)
{
    if (depth == 0)
    {
        // return give_score(board);
    }
}

void export_board()
{
}
