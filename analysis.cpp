#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <chrono>

#include "NNUE.hpp"

using namespace std;

struct piece{
    int row, col;
};

struct Analysis{
    int bad = 0, ok = 0, good = 0, excelent = 0, brilliant = 0;
};

vector<uint16_t> encode(int board[8][8]){
    vector<uint16_t> input;//(81920, 0)
    int pieces[10] = {1, 2, 3, 4, 5, -1, -2, -3, -4, -5};
    for(int i = 0; i < 8; i++){
        for(int j = 0; j < 8; j++){
            for(int p = 0; p < 10; p++){
                for(int i1 = 0; i1 < 8; i1++){
                    for(int j1 = 0; j1 < 8; j1++){
                        if(board[i][j] == -6 && board[i1][j1] == pieces[p]){
                            input.push_back(1);
                        }else{
                            input.push_back(0);
                        }
                    }
                }
            }
        }
    }
    for(int i = 0; i < 8; i++){
        for(int j = 0; j < 8; j++){
            for(int p = 0; p < 10; p++){
                for(int i1 = 0; i1 < 8; i1++){
                    for(int j1 = 0; j1 < 8; j1++){
                        if(board[i][j] == 6 && board[i1][j1] == pieces[p]){
                            input.push_back(1);
                        }else{
                            input.push_back(0);
                        }
                    }
                }
            }
        }
    }
    return input;
}

bool isEmpty(std::ifstream& pFile){
    return pFile.peek() == std::ifstream::traits_type::eof();
}

void printBoard(int board[8][8]){
    int val = 8;
    char pieces[13] = {'-', 'P', 'T', 'C', 'B', 'D', 'R', 'p', 't', 'c', 'b', 'd', 'r'};
    for(int i = 0; i < 8; i++){
        cout << val << ' ';
        for(int j = 0; j < 8; j++){
            if(board[i][j] < 0){
                cout << pieces[abs(board[i][j])+6];
            }else{
                cout << pieces[board[i][j]];
            }
            if(j < 7){
                cout << " | ";
            }
        }
        cout << '\n';
        val--;
    }
    val = 0;
    for(int i = 0; i < 8; i++){
        cout << "  " << char(val+97) << " ";
        val++;
    }
    cout << '\n';
}

void reset(int board[8][8]){
    int nboard[8][8] = {
        {-2, -3, -4, -5, -6, -4, -3, -2},
        {-1, -1, -1, -1, -1, -1, -1, -1},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {1, 1, 1, 1, 1, 1, 1, 1},
        {2, 3, 4, 5, 6, 4, 3, 2}
    };
    for(int i = 0; i < 8; i++){
        for(int j = 0; j < 8; j++){
            board[i][j] = nboard[i][j];
        }
    }
}

int main(){
    NNUE *n = new NNUE(0);
    ifstream weigths("./weights/weight.txt");

    Analysis white, black;

    if(!isEmpty(weigths)){
        n->loadTrain(weigths);
    }
    cout << "Loaded NNUE with " << n->tot << " iterations.\n";

    int board[8][8] = {
        {-2, -3, -4, -5, -6, -4, -3, -2},
        {-1, -1, -1, -1, -1, -1, -1, -1},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {1, 1, 1, 1, 1, 1, 1, 1},
        {2, 3, 4, 5, 6, 4, 3, 2}
    };

    n->setInput(encode(board));

    printBoard(board);
    cout << n->getOutput() << '\n';

    piece wKing = {7, 4};
    piece bKing = {0, 4};

    int fCol;
    int fRow;
    int tCol;
    int tRow;
    string movement;
    int player = 1;
    int piece;
    int16_t output = n->getOutput();
    cout << "The white pieces are always on bottom\nWrite a movement like e2e4\nFor castling write king move, like e1g1 and put a C at the end\nFor en-passant write the pawn movement and an E at the end\nWrite stop to close the program\nWrite reset to restart the board\n";
    while(cin >> movement && movement != "stop"){
        if(movement == "reset"){
            reset(board);

            white = {0,0,0,0,0};
            black = {0,0,0,0,0};

            player = 1;

            n->setInput(encode(board));

            printBoard(board);
            cout << n->getOutput() << '\n';

            output = n->getOutput();
        }else{
            fCol = movement[0]-97;
            fRow = 56-(int)movement[1];
            tCol = movement[2]-97;
            tRow = 56-(int)movement[3];
            if(movement.length() > 4){
                if(movement[4] == 'C'){
                    int dir = tCol - fCol;
                    if(dir > 0){
                        if(player == -1){
                            board[0][5] = board[0][7];
                            board[0][7] = 0;
                        }else{
                            board[7][5] = board[7][7];
                            board[7][7] = 0;
                        }
                    }else{
                        if(player == -1){
                            board[0][5] = board[0][0];
                            board[0][0] = 0;
                        }else{
                            board[7][3] = board[7][0];
                            board[7][0] = 0;
                        }
                    }
                }else if(movement[4] == 'E'){
                    if(player == -1){
                        board[tRow+1][tCol] = 0;
                    }else{
                        board[tRow-1][tCol] = 0;
                    }
                }
            }
            piece = abs(board[fRow][fCol]);

            if(piece == 6){
                wKing = {fRow, fCol};
            }else if(piece == -6){
                bKing = {fRow, fCol};
            }

            board[tRow][tCol] = board[fRow][fCol];
            board[fRow][fCol] = 0;
            if(piece != 6){
                if(player == 1){
                    n->changeInput(
                        wKing.row,
                        wKing.col,
                        bKing.row,
                        bKing.col,
                        tRow,
                        tCol,
                        fRow,
                        fCol,
                        board[tRow][tCol]
                    );
                }else{
                    n->changeInput(
                        bKing.row,
                        bKing.col,
                        wKing.row,
                        wKing.col,
                        tRow,
                        tCol,
                        fRow,
                        fCol,
                        board[tRow][tCol]
                    );
                }
            }else{
                n->setInput(encode(board));
            }

            printBoard(board);
            auto start = chrono::steady_clock::now();
            int16_t nOutput = n->getOutput();
            auto end = chrono::steady_clock::now();
            auto time = chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
            int16_t variation = abs(nOutput)-abs(output);
            if(variation < 60 && variation > -60){
                if(player == 1){
                    white.ok++;
                }else{
                    black.ok++;
                }
                cout << "Ok ";
            }else if(variation < 100 && variation > 59){
                if(player == 1){
                    white.good++;
                }else{
                    black.good++;
                }
                cout << "Good ";
            }else if(variation < 150 && variation > 99){
                if(player == 1){
                    white.excelent++;
                }else{
                    black.excelent++;
                }
                cout << "Excelent ";
            }else if(variation < 250 && variation > 149){
                if(player == 1){
                    white.brilliant++;
                }else{
                    black.brilliant++;
                }
                cout << "Brilliant ";
            }else{
                if(player == 1){
                    white.bad++;
                }else{
                    black.bad++;
                }
                cout << "Bad ";
            }
            cout << "move from " << output << " to " << nOutput << '\n';
            cout << "Elapsed(ns) = " << time << " for NNUE evaluation\n";
            output = nOutput;
            player *= -1;
        }
    }

    delete n;
    n = NULL;

    return 0;
}