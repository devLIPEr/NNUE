#include <iostream>
#include <random>
#include <vector>
namespace std{
    #pragma once
    template <class T>
    class Matrix{
        public:
            int rows, cols;
            void setValue();
            void acumValue();
            void getValue();
            int getRows();
            int getCols();
            T rotate();
            T *transpose();
            void sum(T *a, T *b, T *res);
            void mult(T *a, T *b, T *res);
    };

    class Matrix8 : public Matrix<Matrix8>{
        private:
            vector<vector<int8_t>> mat;
        public:
            Matrix8(int rows, int cols, bool isRandom){
                this->rows = rows;
                this->cols = cols;
                
                default_random_engine generator;
                normal_distribution<float> distribution(0, 255);
                for(int i = 0; i < rows; i++){
                    vector <int8_t> inp;
                    mat.push_back(inp);
                    for(int j = 0; j < cols; j++){
                        if(isRandom){
                            mat.at(i).push_back((int8_t)distribution(generator));
                        }else{
                            mat.at(i).push_back(0);
                        }
                    }
                }
            }
            Matrix8(){}

            void setValue(int8_t val, int r, int c){
                this->mat.at(r).at(c) = val;
            }
            void acumValue(int8_t val, int r, int c){
                #pragma omp simd simdlen(8)
                this->mat.at(r).at(c) += val;
            }

            int8_t getValue(int r, int c){
                return this->mat.at(r).at(c);
            }
            int getRows(){
                return this->rows;
            }
            int getCols(){
                return this->cols;
            }

            Matrix8 *rotate(){
                Matrix8 *m = new Matrix8(this->rows, this->cols, false);
                int r = this->rows-1, c = this->cols-1;
                for(int i = r; i >= 0; i--){
                    for(int j = c; j >= 0; j--){
                        m->setValue(this->getValue(i, j), r-i, c-j);
                    }
                }
                return m;
            }
            Matrix8 *transpose(){
                Matrix8 *m = new Matrix8(this->cols, this->rows, false);
                for(int i = 0; i < this->rows; i++){
                    for(int j = 0; j < this->cols; j++){
                        m->setValue(this->getValue(i, j), j, i);
                    }
                }
                return m;
            }

            void sum(Matrix8 *a, Matrix8 *b, Matrix8 *res){
                int r = a->getRows(), c = a->getCols();
                #pragma omp simd simdlen(8)
                for(int i = 0; i < r; i++){
                    for(int j = 0; j < c; j++){
                        res->setValue(a->getValue(i, j) + b->getValue(i, j), i, j);
                    }
                }
            }
            void sub(Matrix8 *a, Matrix8 *b, Matrix8 *res){
                int r = a->getRows(), c = a->getCols();
                #pragma omp simd simdlen(8)
                for(int i = 0; i < r; i++){
                    for(int j = 0; j < c; j++){
                        res->setValue(a->getValue(i, j) - b->getValue(i, j), i, j);
                    }
                }
            }
            void subLine(Matrix8 *a, Matrix8 *b, Matrix8 *res){
                int r = a->getRows(), c = a->getCols();
                #pragma omp simd simdlen(8)
                for(int i = 0; i < r; i++){
                    for(int j = 0; j < c; j++){
                        res->setValue(a->getValue(i, j) - b->getValue(i, 0), i, j);
                    }
                }
            }
            void addLine(Matrix8 *a, Matrix8 *b, Matrix8 *res){
                int r = a->getRows(), c = a->getCols();
                #pragma omp simd simdlen(8)
                for(int i = 0; i < r; i++){
                    for(int j = 0; j < c; j++){
                        res->setValue(a->getValue(i, j) + b->getValue(i, 0), i, j);
                    }
                }
            }
            void mult(Matrix8 *a, Matrix8 *b, Matrix8 *res){
                int r = a->getRows(), c = a->getCols(), d = b->getCols();
                #pragma omp simd simdlen(8)
                for(int i = 0; i < r; i++){
                    for(int j = 0; j < d; j++){
                        int8_t val = 0;
                        for(int k = 0; k < c; k++){
                            if(a->getValue(i, k) != 0 && b->getValue(k, j) != 0){
                                val += a->getValue(i, k) * b->getValue(k, j);
                            }
                        }
                        res->setValue(val, i, j);
                    }
                }
            }
            void multE(Matrix8 *a, int8_t e, Matrix8 *res){
                int r = a->getRows(), c = a->getCols();
                #pragma omp simd simdlen(8)
                for(int i = 0; i < r; i++){
                    for(int j = 0; j < c; j++){
                        res->setValue((e*a->getValue(i, j))%256, i, j);
                    }
                }
            }
    };

    class Matrix16 : public Matrix<Matrix16>{
        private:
            vector<vector<int16_t>> mat;
        public:
            Matrix16(int rows, int cols, bool isRandom = true){
                this->rows = rows;
                this->cols = cols;
                
                default_random_engine generator;
                normal_distribution<float> distribution(0, 65535);
                for(int i = 0; i < rows; i++){
                    vector <int16_t> inp;
                    mat.push_back(inp);
                    for(int j = 0; j < cols; j++){
                        if(isRandom){
                            mat.at(i).push_back((int16_t)distribution(generator));
                        }else{
                            mat.at(i).push_back(0);
                        }
                    }
                }
            }
            Matrix16(){}

            void setValue(int16_t val, int r, int c){
                this->mat.at(r).at(c) = val;
            }
            void acumValue(int16_t val, int r, int c){
                #pragma omp simd simdlen(16)
                this->mat.at(r).at(c) += val;
            }

            int16_t getValue(int r, int c){
                return this->mat.at(r).at(c);
            }
            int getRows(){
                return this->rows;
            }
            int getCols(){
                return this->cols;
            }

            Matrix16 *rotate(){
                Matrix16 *m = new Matrix16(this->rows, this->cols, false);
                int r = this->rows-1, c = this->cols-1;
                for(int i = r; i >= 0; i--){
                    for(int j = c; j >= 0; j--){
                        m->setValue(this->getValue(i, j), r-i, c-j);
                    }
                }
                return m;
            }
            Matrix16 *transpose(){
                Matrix16 *m = new Matrix16(this->cols, this->rows, false);
                for(int i = 0; i < this->rows; i++){
                    for(int j = 0; j < this->cols; j++){
                        m->setValue(this->getValue(i, j), j, i);
                    }
                }
                return m;
            }

            void sum(Matrix16 *a, Matrix16 *b, Matrix16 *res){
                int r = a->getRows(), c = a->getRows();
                #pragma omp simd simdlen(16)
                for(int i = 0; i < r; i++){
                    for(int j = 0; j < c; j++){
                        res->setValue(a->getValue(i, j) + b->getValue(i, j), i, j);
                    }
                }
            }
            void sub(Matrix16 *a, Matrix16 *b, Matrix16 *res){
                int r = a->getRows(), c = a->getRows();
                #pragma omp simd simdlen(16)
                for(int i = 0; i < r; i++){
                    for(int j = 0; j < c; j++){
                        res->setValue(a->getValue(i, j) - b->getValue(i, j), i, j);
                    }
                }
            }
            void subLine(Matrix16 *a, Matrix16 *b, Matrix16 *res){
                int r = a->getRows(), c = a->getRows();
                #pragma omp simd simdlen(16)
                for(int i = 0; i < r; i++){
                    for(int j = 0; j < c; j++){
                        res->setValue(a->getValue(i, j) - b->getValue(i, 0), i, j);
                    }
                }
            }
            void addLine(Matrix16 *a, Matrix16 *b, Matrix16 *res){
                int r = a->getRows(), c = a->getRows();
                #pragma omp simd simdlen(16)
                for(int i = 0; i < r; i++){
                    for(int j = 0; j < c; j++){
                        res->setValue(a->getValue(i, j) + b->getValue(i, 0), i, j);
                    }
                }
            }
            void mult(Matrix16 *a, Matrix16 *b, Matrix16 *res){
                int r = a->getRows(), c = a->getCols(), d = b->getCols();
                #pragma omp simd simdlen(16)
                for(int i = 0; i < r; i++){
                    for(int j = 0; j < d; j++){
                        int16_t val = 0;
                        for(int k = 0; k < c; k++){
                            if(a->getValue(i, k) != 0 && b->getValue(k, j) != 0){
                                val += a->getValue(i, k) * b->getValue(k, j);
                            }
                        }
                        res->setValue(val, i, j);
                    }
                }
            }
            void multE(Matrix16 *a, int16_t e, Matrix16 *res){
                int r = a->getRows(), c = a->getCols();
                #pragma omp simd simdlen(16)
                for(int i = 0; i < r; i++){
                    for(int j = 0; j < c; j++){
                        res->setValue((e*a->getValue(i, j))%65536, i, j);
                    }
                }
            }
    };
}