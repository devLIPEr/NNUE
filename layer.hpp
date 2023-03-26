#include <iostream>
#include <vector>
#include <variant>

#include "neuron.hpp"
#include "matrix.hpp"


namespace std{
    #pragma once
    template <class T, class D>
    class Layer{
        private:
            vector<T> neurons;
            int size;
        public:
            Layer(int size, vector<T> neurons){
                this->size = size;
                this->neurons = neurons;
            }
            Layer(int size){
                this->size = size;
                for(int i = 0; i < size; i++){
                    T *n = new T(0);
                    neurons.push_back(*n);
                }
            }
            Layer(){}

            void merge(Layer *a, Layer *b){
                vector<Neuron8> aa = a->getNeurons();
                vector<Neuron8> bb = b->getNeurons();
                int sa = aa.size();
                int sb = bb.size();
                for(int i = 0; i < sa; i++){
                    this->neurons.at(i) = (aa.at(i));
                }
                for(int i = 0; i < sb; i++){
                    this->neurons.at(sa+i) = (bb.at(i));
                }
            }

            void activate(){
                for(int i = 0; i < this->size; i++){
                    neurons.at(i).activate();
                    neurons.at(i).derive();
                }
            }

            void setValues(D *a){
                int r = a->getRows();
                int c = a->getCols();
                for(int i = 0; i < r; i++){
                    for(int j = 0; j < c; j++){
                        this->neurons.at(j) = a->getValue(i, j);
                    }
                }
            }

            vector<T> getNeurons(){
                return neurons;
            }

            D *matrixifyValues(){
                D *m = new D(this->size, 1);
                for(int i = 0; i < this->size; i++){
                    m->setValue(neurons.at(i).getValue(), i, 0);
                }
                return m;
            }
            D *matrixifyActivatedValues(){
                D *m = new D(this->size, 1, false);
                for(int i = 0; i < this->size; i++){
                    m->setValue(neurons.at(i).getActivatedValue(), i, 0);
                }
                return m;
            }
            D *matrixifyDerivedValues(){
                D *m = new D(this->size, 1, false);
                for(int i = 0; i < this->size; i++){
                    m->setValue(neurons.at(i).getDerivedValue(), i, 0);
                }
                return m;
            }
    };
}