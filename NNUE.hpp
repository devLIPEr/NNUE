#include <vector>
#include <chrono>

#include "layer.hpp"
#include "neuron.hpp"
#include "matrix.hpp"

namespace std{
    #pragma once
    class Input{
        public:
            int kingRow, kingCol, enemyKingRow, enemyKingCol, pieceID, pieceRow, pieceCol, lastPieceRow, lastPieceCol;
    };

    class NNUE{
        private:
            Layer<Neuron16, Matrix16> *i1, *i2;//division of input Layer
            Matrix16 *w0;
            Layer<Neuron8, Matrix8> *l11, *l12, *l1;//division of first Layer and its merge
            Matrix8 *w1;
            Layer<Neuron8, Matrix8> *l2, *l3;//other Layers
            Matrix8 *w2, *w3;
            Layer<Neuron8, Matrix8> *o;//output
            Matrix16 *res1, *res2;
            double learningRate = 0.01;
        public:
            NNUE(double learningRate){
                this->learningRate = learningRate;

                i1 = new Layer<Neuron16, Matrix16>(40960);
                i2 = new Layer<Neuron16, Matrix16>(40960);
                l11 = new Layer<Neuron8, Matrix8>(256);
                l12 = new Layer<Neuron8, Matrix8>(256);
                l1 = new Layer<Neuron8, Matrix8>(512);
                l2 = new Layer<Neuron8, Matrix8>(32);
                l3 = new Layer<Neuron8, Matrix8>(32);
                o = new Layer<Neuron8, Matrix8>(1);

                w0 = new Matrix16(256, 40960, true);
                w1 = new Matrix8(32, 512, true);
                w2 = new Matrix8(32, 32, true);
                w3 = new Matrix8(1, 32, true);

                res1 = new Matrix16(256, 1, false);
                res2 = new Matrix16(256, 1, false);
            }
            ~NNUE(){
                delete i1;
                delete i2;
                delete l11;
                delete l12;
                delete l1;
                delete l2;
                delete l3;
                delete o;
                delete w0;
                delete w1;
                delete w2;
                delete w3;
                delete res1;
                delete res2;
                i1 = NULL;
                i2 = NULL;
                l11 = NULL;
                l12 = NULL;
                l1 = NULL;
                l2 = NULL;
                l3 = NULL;
                o = NULL;
                w0 = NULL;
                w1 = NULL;
                w2 = NULL;
                w3 = NULL;
                res1 = NULL;
                res2 = NULL;
            }
        
            void mergeL1(){
                Matrix8 *r1 = new Matrix8(res1->getRows(), res1->getCols(), false);
                Matrix8 *r2 = new Matrix8(res2->getRows(), res2->getCols(), false);
                for(int i = 0; i < res1->getRows(); i++){
                    for(int j = 0; j < res1->getCols(); j++){
                        r1->setValue(res1->getValue(i, j), i, j);
                        r2->setValue(res2->getValue(i, j), i, j);
                    }
                }
                l11->setValues(r1);
                l12->setValues(r2);
                l1->merge(l11, l12);
            }
            void rotate(){
                Matrix16 *aux = NULL;

                aux = res1->rotate();
                delete res1;
                res1 = aux;

                aux = res2->rotate();
                delete res2;
                res2 = aux;

                aux = res1;
                res1 = res2;
                res2 = aux;
            }
            void setInput(vector<uint16_t> input){
                int size = input.size();
                Matrix16 *a = new Matrix16(1, size/2, false);
                for(int i = 0; i < size/2; i++){
                    a->setValue(input.at(i), 0, i);
                }
                i1->setValues(a);
                for(int i = size/2; i < size; i++){
                    a->setValue(input.at(i), 0, i-size/2);
                }
                i2->setValues(a);

                Matrix16 *i1Values = i1->matrixifyValues();
                Matrix16 *i2Values = i2->matrixifyValues();
                delete res1;
                delete res2;
                res1 = new Matrix16(256, 1, false);
                res2 = new Matrix16(256, 1, false);
                for(int i = 0; i < 40960; i++){
                    uint16_t val1 = i1Values->getValue(i, 0);
                    uint16_t val2 = i2Values->getValue(i, 0);
                    if(val1 != 0){
                        for(int j = 0; j < 256; j++){
                            res1->acumValue(w0->getValue(j, i), j, 0);
                        }
                    }
                    if(val2 != 0){
                        for(int j = 0; j < 256; j++){
                            res2->acumValue(w0->getValue(j, i), j, 0);
                        }
                    }
                }
                mergeL1();
                feedForward();

                delete a;
                delete i1Values;
                delete i2Values;
                a = NULL;
                i1Values = NULL;
                i2Values = NULL;
            }
            void changeInput(int kR, int kC, int eKR, int eKC, int pR, int pC, int lPR, int lPC, int pID){
                int x, y, x1, y1;
                if(pID < 0){
                    pID *= -1;
                    pID += 5;
                }
                pID--;
                x = kR*5120 + kC*640 + pID * 64 + lPR*8 + lPC;
                y = kR*5120 + kC*640 + pID * 64 + pR*8 + pC;

                x1 = (eKR*5120 + eKC*640 + pID * 64 + lPR*8 + lPC);
                y1 = (eKR*5120 + eKC*640 + pID * 64 + pR*8 + pC);
                for(int i = 0; i < 256; i++){
                    res1->acumValue(-w0->getValue(i, x), i, 0);
                    res1->acumValue(w0->getValue(i, y), i, 0);
                    res2->acumValue(-w0->getValue(i, x1), i, 0);
                    res2->acumValue(w0->getValue(i, y1), i, 0);
                }
                mergeL1();
                
                auto start = chrono::steady_clock::now();
                feedForward();
                auto end = chrono::steady_clock::now();
                auto time = chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
                cout << "Time Elapsed(ms): " << time << '\n';
                rotate();
                mergeL1();
            }
            void changeKingInput(int kR, int kC, int eKR, int eKC, int pR, int pC, int lPR, int lPC){
                // vector<uint16_t> input(81920, 0);
                int pieces[10] = {1, 2, 3, 4, 5, -1, -2, -3, -4, -5};
                for(int i = 0; i < 10; i++){
                    int pID = pieces[i];
                    if(pID < 0){
                        pID *= -1;
                        pID += 5;
                    }
                    pID--;

                    int x = kR*5120 + kC*640 + pID * 64 + lPR*8 + lPC;//was
                    int y = kR*5120 + kC*640 + pID * 64 + pR*8 + pC;//is

                    int x1 = (eKR*5120 + eKC*640 + pID * 64 + lPR*8 + lPC);
                    int y1 = (eKR*5120 + eKC*640 + pID * 64 + pR*8 + pC);

                    // input.at(x) = 0;
                    // input.at(y) = 1;
                    // input.at(x1) = 0;
                    // input.at(y1) = 1;
                    for(int i = 0; i < 256; i++){
                        res1->acumValue(-w0->getValue(i, x), i, 0);
                        res1->acumValue(w0->getValue(i, y), i, 0);
                        res2->acumValue(-w0->getValue(i, x1), i, 0);
                        res2->acumValue(w0->getValue(i, y1), i, 0);
                    }
                }
                mergeL1();
                
                auto start = chrono::steady_clock::now();
                feedForward();
                auto end = chrono::steady_clock::now();
                auto time = chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
                cout << "Time Elapsed(ms): " << time << '\n';
                rotate();
                mergeL1();
            }
            
            void mult816(Matrix8 *a, Matrix16 *b, Matrix16 *res){
                int r = a->getRows(), c = a->getCols(), d = b->getCols();
                #pragma omp simd simdlen(16)
                for(int i = 0; i < r; i++){
                    for(int j = 0; j < d; j++){
                        uint8_t val = 0;
                        for(int k = 0; k < c; k++){
                            if(a->getValue(i, k) != 0 && b->getValue(k, j) != 0){
                                val += a->getValue(i, k) * b->getValue(k, j);
                            }
                        }
                        res->setValue(val, i, j);
                    }
                }
            }
            
            void feedForward(){
                l1->activate();
                Matrix8 *lMat = l1->matrixifyActivatedValues();
                Matrix8 *res = new Matrix8(lMat->getCols(), w1->getRows(), false);
                w1->mult(lMat->transpose(), w1->transpose(), res);

                l2->setValues(res);
                l2->activate();
                lMat = l2->matrixifyActivatedValues();
                delete res;
                res = new Matrix8(lMat->getCols(), w2->getRows(), false);
                w2->mult(lMat->transpose(), w2->transpose(), res);

                l3->setValues(res);
                l3->activate();
                lMat = l3->matrixifyActivatedValues();
                delete res;
                res = new Matrix8(lMat->getCols(), w3->getRows(), false);
                w3->mult(lMat->transpose(), w3->transpose(), res);

                o->setValues(res);
                o->activate();

                delete res;
                res = NULL;
            }
            void backPropagation(int8_t target){
                #pragma omp simd simdlen(8)
                int8_t result = this->getOutput();
                int8_t err = (target-result)/2;
                // // w3->multE(w3, err*learningRate, w3);
                Matrix8 *d = o->matrixifyDerivedValues();
                Matrix8 *copy = new Matrix8(w3->getRows(), w3->getCols(), false);
                for(int i = 0; i < w3->getRows(); i++){
                    for(int j = 0; j < w3->getCols(); j++){
                        copy->setValue(w3->getValue(i, j), i, j);
                    }
                }
                if(err > 0){
                    w3->addLine(copy, d, w3);
                }else if(err < 0){
                    w3->subLine(copy, d, w3);
                }

                d = l3->matrixifyDerivedValues();
                // d->multE(d, learningRate, d);
                delete copy;
                copy = new Matrix8(w2->getRows(), w2->getCols(), false);
                for(int i = 0; i < w2->getRows(); i++){
                    for(int j = 0; j < w2->getCols(); j++){
                        copy->setValue(w2->getValue(i, j), i, j);
                    }
                }
                if(err > 0){
                    w2->addLine(copy, d, w2);
                }else if(err < 0){
                    w2->subLine(copy, d, w2);
                }

                d = l2->matrixifyDerivedValues();
                // d->multE(d, learningRate, d);
                delete copy;
                copy = new Matrix8(w1->getRows(), w1->getCols(), false);
                for(int i = 0; i < w1->getRows(); i++){
                    for(int j = 0; j < w1->getCols(); j++){
                        copy->setValue(w1->getValue(i, j), i, j);
                    }
                }
                if(err > 0){
                    w1->addLine(copy, d, w1);
                }else if(err < 0){
                    w1->subLine(copy, d, w1);
                }

                // d = l2->matrixifyDerivedValues();
                // // d->multE(d, learningRate, d);
                // delete copy;
                // copy = new Matrix8(w1->getRows(), w1->getCols(), false);
                // for(int i = 0; i < w1->getRows(); i++){
                //     for(int j = 0; j < w1->getCols(); j++){
                //         copy->setValue(w1->getValue(i, j), i, j);
                //     }
                // }
                // w1->subLine(copy, d, w1);

                d = l1->matrixifyDerivedValues();
                // d->multE(d, learningRate, d);

                Matrix8 *k = new Matrix8(2, 256, false);
                for(int i = 0; i < 2; i++){
                    for(int j = 0; j < 256; j++){
                        k->setValue(d->getValue(i*256+j, 0), i, j);
                    }
                }
                Matrix16 *copyy = new Matrix16(w0->getRows(), w0->getCols(), false);
                for(int i = 0; i < w0->getRows(); i++){
                    for(int j = 0; j < w0->getCols(); j++){
                        copyy->setValue(w0->getValue(i, j), i, j);
                    }
                }
                mult816(k, copyy, w0);
                delete k;
                delete copy;
                delete copyy;
                k = NULL;
                copy = NULL;
                copyy = NULL;
            }
            int tot  = 0;
            void train(int8_t target, Input input){
                if(tot%250 == 0){
                    saveTrain("./weights/weight.txt");
                    cout << "Saved weights at ./weights/weight.txt with " << tot << " batchs\n";
                }
                if(input.kingRow != -1){
                    changeInput(
                        input.kingRow,
                        input.kingCol,
                        input.enemyKingRow,
                        input.enemyKingCol,
                        input.pieceRow,
                        input.pieceCol,
                        input.lastPieceRow,
                        input.lastPieceCol,
                        input.pieceID
                    );
                }
                backPropagation(target);
                tot++;
            }
            int16_t getOutput(){
                return o->matrixifyActivatedValues()->getValue(0, 0);
            }
    
            void saveTrain(string path){
                ofstream file(path);
                file << tot << '\n';
                for(int i = 0; i < w0->getRows(); i++){
                    for(int j = 0; j < w0->getCols(); j++){
                        if(j < w0->getCols()-1){
                            file << (int)w0->getValue(i, j) << ' ';
                        }else{
                            file << (int)w0->getValue(i, j);
                        }
                    }
                    file << '\n';
                }
                for(int i = 0; i < w1->getRows(); i++){
                    for(int j = 0; j < w1->getCols(); j++){
                        if(j < w1->getCols()-1){
                            file << (int)w1->getValue(i, j) << ' ';
                        }else{
                            file << (int)w1->getValue(i, j);
                        }
                    }
                    file << '\n';
                }
                for(int i = 0; i < w2->getRows(); i++){
                    for(int j = 0; j < w2->getCols(); j++){
                        if(j < w2->getCols()-1){
                            file << (int)w2->getValue(i, j) << ' ';
                        }else{
                            file << (int)w2->getValue(i, j);
                        }
                    }
                    file << '\n';
                }
                for(int i = 0; i < w3->getRows(); i++){
                    for(int j = 0; j < w3->getCols(); j++){
                        if(j < w3->getCols()-1){
                            file << (int)w3->getValue(i, j) << ' ';
                        }else{
                            file << (int)w3->getValue(i, j);
                        }
                    }
                    file << '\n';
                }
                file.close();
            }
            void loadTrain(std::ifstream& file){
                int val;
                file >> tot;
                for(int i = 0; i < w0->getRows(); i++){
                    for(int j = 0; j < w0->getCols(); j++){
                        file >> val;
                        w0->setValue(val, i, j);
                    }
                }
                for(int i = 0; i < w1->getRows(); i++){
                    for(int j = 0; j < w1->getCols(); j++){
                        file >> val;
                        w1->setValue(val, i, j);
                    }
                }
                for(int i = 0; i < w2->getRows(); i++){
                    for(int j = 0; j < w2->getCols(); j++){
                        file >> val;
                        w2->setValue(val, i, j);
                    }
                }
                for(int i = 0; i < w3->getRows(); i++){
                    for(int j = 0; j < w3->getCols(); j++){
                        file >> val;
                        w3->setValue(val, i, j);
                    }
                }
                file.close();
            }
    };
}