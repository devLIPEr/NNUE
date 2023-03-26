#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>

#include "NNUE.hpp"

using namespace std;

vector<uint16_t> encode(int board[8][8]){
    vector<uint16_t> input;
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

struct Piece{
    int row, col;
};

bool isEmpty(std::ifstream& pFile){
    return pFile.peek() == std::ifstream::traits_type::eof();
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
    ifstream file("./moveData.txt"), weigths("./weights/weight.txt");

    NNUE *n = new NNUE(0.03);

    if(!isEmpty(weigths)){
        n->loadTrain(weigths);
    }
    if(file.is_open()){
        int size;
        string move;
        file >> size;

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

        Piece wKing = {7, 4}, bKing = {0, 4};
        int player = 1;

        for(int k = n->tot; k < size; k++){
            bool changeInput = false, brek = false;
            Input inp;
            int target;
            file >> move >> target;
            if(move == "reset"){
                reset(board);

                player = 1;
                wKing = {7, 4};
                bKing = {0, 4};

                n->setInput(encode(board));
                file >> move >> target;
            }
            int fCol = move[0]-97;
            int fRow = (int)move[1]-49;
            int tCol = move[2]-97;
            int tRow = (int)move[3]-49;
            
            if(board[fRow][fCol] == 6){
                wKing = {tRow, tCol};
                board[fRow][fCol] = 0;
                board[tRow][tCol] = 6;
                changeInput = true;
            }else if(board[fRow][fCol] == -6){
                bKing = {tRow, tCol};
                board[fRow][fCol] = 0;
                board[tRow][tCol] = -6;
                changeInput = true;
            }
            for(int i = 0; i < 8; i++){
                for(int j = 0; j < 8; j++){
                    if(player == 1){
                        if(board[fRow][fCol] > 0){
                            board[tRow][tCol] = board[fRow][fCol];
                            board[fRow][fCol] = 0;
                            brek = true;
                            break;
                        }
                    }else{
                        if(board[fRow][fCol] < 0){
                            board[tRow][tCol] = board[fRow][fCol];
                            board[fRow][fCol] = 0;
                            brek = true;
                            break;
                        }
                    }
                }
                if(brek){
                    break;
                }
            }
            if(changeInput){
                n->setInput(encode(board));
                inp = {-1,-1,-1,-1,-1,-1,-1,-1,-1};
            }else{
                int kR, kC, eKR, eKC, lPR, lPC, pC, pR, pID;
                bool rotate;
                if(player == 1){
                    kR = wKing.row;
                    kC = wKing.col;
                    eKR = bKing.row;
                    eKC = bKing.col;
                    lPR = fRow;
                    lPC = fCol;
                    pR = tRow;
                    pC = tCol;
                    pID = board[tRow][tCol];
                }else{
                    kR = bKing.row;
                    kC = bKing.col;
                    eKR = wKing.row;
                    eKC = wKing.col;
                    lPR = fRow;
                    lPC = fCol;
                    pR = tRow;
                    pC = tCol;
                    pID = board[tRow][tCol];
                }
                inp = {kR, kC, eKR, eKC, pID, pR, pC, lPR, lPC};
            }

            n->train(target, inp);
            cout << "Batch " << k+1 << " done.\n";
            player *= -1;
        }
        n->saveTrain("./weights/weight.txt");

        int b[8][8] = {
            {-2, -3, -4, -5, -6, -4, -3, -2},
            {-1, -1, -1, -1, -1, -1, -1, -1},
            {0, 0, 0, 0, 0, 0, 0, 0},
            {0, 0, 0, 0, 0, 0, 0, 0},
            {0, 0, 0, 1, 0, 0, 0, 0},
            {0, 0, 0, 0, 0, 0, 0, 0},
            {1, 1, 1, 0, 1, 1, 1, 1},
            {2, 3, 4, 5, 6, 4, 3, 2}
            // {0, 0, 0, -5, -6, 0, 0, 0},
            // {0, 0, 0, 0, 0, 0, 0, 0},
            // {0, 0, 0, 0, 0, 0, 0, 0},
            // {0, 0, 0, 0, 0, 0, 0, 0},
            // {0, 0, 0, 0, 0, 0, 0, 0},
            // {0, 0, 0, 0, 0, 0, 0, 0},
            // {0, 0, 0, 0, 0, 0, 0, 0},
            // {0, 0, 0, 5, 6, 0, 2, 0}
        };

        int pieces[10] = {1, 2, 3, 4, 5, -1, -2, -3, -4, -5};
        n->setInput(encode(b));
        auto start = chrono::steady_clock::now();
        cout << n->getOutput() << '\n';
        auto end = chrono::steady_clock::now();
        auto time = chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        cout << "Elapsed(ms) = " << time << '\n';
        // int hg = 0;
        // cin >> hg;
    }

    return 0;
}