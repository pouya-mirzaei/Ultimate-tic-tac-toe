#define BOARD_SIZE 3

#define X_VALUE 1
#define O_VALUE -1
#define EMPTY_CELL 0

//_____________________________

#define CENTRAL_HOME 12
#define CORNER_HOME 9
#define MIDDLE_HOME 8
#define NEAR_HOME 13
#define BIG_BOARD_VALUE 7
#define WIN_VALUE 1000

#define DEFAULT_DEPTH 6

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

int coordinates[3];

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

int checkWinner(int board[BOARD_SIZE][BOARD_SIZE]);

void analyze_board(Cell cells[9]);

int board_score(int board[BOARD_SIZE][BOARD_SIZE]);

int board_score_plus(int board[BOARD_SIZE][BOARD_SIZE]);

int giveScore(Cell cells[BOARD_SIZE * BOARD_SIZE], int bigBoard[BOARD_SIZE][BOARD_SIZE]);

void copyArr(bool copy[], bool paste[]);

int minimax(Cell board[BOARD_SIZE * BOARD_SIZE], int depth, int turn);

void export_board();

void makeTempMove(Cell board[], int bigBoardIndex, int smallBoardI, int smallBoardJ, int player, bool boards[]);
void removeTempMove(Cell board[], int bigBoardIndex, int smallBoardI, int smallBoardJ, bool tempBoards[]);

//_______________________________________________________________________________________ move :

void move()
{
    // Start your code here
    int chosen_board_idx = 0;
    int chosen_cell_idx = 0;

    analyze_board(cells);

    minimax(cells, DEFAULT_DEPTH, X_VALUE);

    int big_id = coordinates[0];
    int small_id = coordinates[1] * 3 + coordinates[2];
    export_move(big_id, small_id);

    // End your code here
}

int main()
{

    fstream test;
    test.open("board.txt", ios::out);
    test << "";
    test.close();

    test.open("board.txt", ios::out);
    test.close();
    read_whole_map();
    read_available_boards();
    move();

    return 0;
}

//_________________________________________________________________________________ Function :

int checkWinner(int board[BOARD_SIZE][BOARD_SIZE])
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

void analyze_board(Cell cells[9])
{
    for (int i = 0; i < BOARD_SIZE; i++)
    {
        for (int j = 0; j < BOARD_SIZE; j++)
        {
            bigBoard[i][j] = checkWinner(cells[i * 3 + j].board);
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
    else if (col == -2)
    {
        sum -= NEAR_HOME;
    }
    return sum;
}

int giveScore(Cell cells[BOARD_SIZE * BOARD_SIZE], int bigBoard[BOARD_SIZE][BOARD_SIZE])
{
    int sum = 0;
    for (int i = 0; i < BOARD_SIZE; i++)
    {
        for (int j = 0; j < BOARD_SIZE; j++)
        {
            if (bigBoard[i][j] == EMPTY_CELL)
            {
                sum += board_score(cells[3 * i + j].board);
                sum += board_score_plus(cells[3 * i + j].board);
            }
        }
    }
    sum += board_score(bigBoard) * BIG_BOARD_VALUE;
    sum += board_score_plus(bigBoard) * BIG_BOARD_VALUE;
    sum += checkWinner(bigBoard) * WIN_VALUE;

    return sum;
}

int minimax(Cell board[BOARD_SIZE * BOARD_SIZE], int depth, int turn)
{
    if (depth == 0 || checkWinner(bigBoard) != 0)
        return giveScore(board, bigBoard);

    int lim = BOARD_SIZE * BOARD_SIZE;

    int maxScore = -999999;
    int minScore = 999999;

    for (int t = 0; t < lim; t++)
    {
        if (boards[t])
        {
            for (int i = 0; i < BOARD_SIZE; i++)
            {
                for (int j = 0; j < BOARD_SIZE; j++)
                {
                    if (board[t].board[i][j] == EMPTY_CELL)
                    {
                        bool tempBoards[9];
                        copyArr(boards, tempBoards);

                        makeTempMove(cells, t, i, j, turn, boards);
                        // export_board();

                        if (turn == X_VALUE)
                        {
                            int score = minimax(board, depth - 1, O_VALUE);
                            if (score > maxScore)
                            {
                                maxScore = score;
                                if (depth == DEFAULT_DEPTH)
                                {
                                    coordinates[0] = t;
                                    coordinates[1] = i;
                                    coordinates[2] = j;
                                }
                            }
                        }
                        else
                        {
                            int score = minimax(board, depth - 1, X_VALUE);
                            if (score < minScore)
                            {
                                minScore = score;
                            }
                        }
                        removeTempMove(board, t, i, j, tempBoards);
                    }
                }
            }
        }
    }

    if (turn == X_VALUE)
        return maxScore;
    return minScore;
}

void export_board()
{
    fstream file;
    file.open("board.txt", ios::app);
    for (int i = 0; i < 9; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            for (int k = 0; k < 3; k++)
            {
                if (cells[i].board[j][k] == X_VALUE)
                    file << "x ";
                else if (cells[i].board[j][k] == O_VALUE)
                    file << "o ";
                else
                    file << "- ";
            }
            file << endl;
        }
        file << endl;
    }

    file << endl;
    file << "score : " << giveScore(cells, bigBoard);
    file << "\nboards :\n";

    int counter = 0;

    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            counter++;
            file << boards[3 * i + j] << " ";
            if (counter == 3)
            {
                file << endl;
                counter = 0;
            }
        }
    }

    file << "\n----------------------------------------------------------------------------------------";
    file << endl
         << endl;
    file.close();
}

void makeTempMove(Cell board[], int bigBoardIndex, int smallBoardI, int smallBoardJ, int player, bool boards[])
{
    board[bigBoardIndex].board[smallBoardI][smallBoardJ] = player;

    int boardIndex = smallBoardI * 3 + smallBoardJ;

    if (checkWinner(board[boardIndex].board) == EMPTY_CELL)
    {
        boards[boardIndex] = 1;
        for (int i = 0; i < 9; i++)
        {
            if (i != boardIndex)
                boards[i] = 0;
        }
    }
    else
    {
        for (int i = 0; i < 9; i++)
        {
            if (checkWinner(board[i].board) == EMPTY_CELL)
            {
                boards[i] = 1;
            }
            else
            {
                boards[i] = 0;
            }
        }
    }

    analyze_board(board);
}

void removeTempMove(Cell board[], int bigBoardIndex, int smallBoardI, int smallBoardJ, bool tempBoards[])
{
    board[bigBoardIndex].board[smallBoardI][smallBoardJ] = EMPTY_CELL;
    copyArr(tempBoards, boards);
}

void copyArr(bool copy[], bool paste[])
{
    for (int i = 0; i < 9; i++)
    {
        paste[i] = copy[i];
    }
}