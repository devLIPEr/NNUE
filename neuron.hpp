#include <iostream>

namespace std{
    #pragma once
    class Neuron{
        public:
            void activate();
            void derive();
    };

    class Neuron8 : public Neuron{
        private:
            int8_t value, activatedValue, derivedValue;
        public:
            Neuron8(int8_t val){
                this->value = val;
            };
            Neuron8(){}

            void setValue(int8_t val){
                this->value = val;
            }

            void activate(){
                // if(this->value > 1){
                //     this->activatedValue = 1;
                // }else if(this->value < 0){
                //     this->activatedValue = 0;
                // }else{
                    this->activatedValue = this->value;
                // }
            }
            void derive(){
                // if(this->value <= 0){
                //     this->derivedValue = 0;
                // }else{
                    this->derivedValue = 1;
                // }
            }

            int8_t getValue(){
                return this->value;
            }
            int8_t getActivatedValue(){
                return this->activatedValue;
            }
            int8_t getDerivedValue(){
                return this->derivedValue;
            }
    };

    class Neuron16 : public Neuron{
        private:
            int16_t value, activatedValue, derivedValue;
        public:
            Neuron16(int16_t val){
                this->value = val;
            };
            Neuron16(){}

            void setValue(int16_t val){
                this->value = val;
            }

            void activate(){
                // if(this->value > 1){
                //     this->activatedValue = 1;
                // }else if(this->value < 0){
                //     this->activatedValue = 0;
                // }else{
                    this->activatedValue = this->value;
                // }
            }
            void derive(){
                // if(this->value < 0){
                //     this->derivedValue = 0;
                // }else{
                    this->derivedValue = 1;
                // }
            }

            int16_t getValue(){
                return this->value;
            }
            int16_t getActivatedValue(){
                return this->activatedValue;
            }
            int16_t getDerivedValue(){
                return this->derivedValue;
            }
    };
}