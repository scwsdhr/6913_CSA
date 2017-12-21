#include<iostream>
#include<vector>
#include<bitset>

// Branch History Register
class BHR {
    public:
        BHR(const int k);
        void push(bool p);
        void disp();
        int getindex();
    private:
        std::vector<bool> reg;
        int index;
};

// Pattern History Table
class PHT {
    public:
        PHT(const int m, const int k);
        void update(const unsigned int offset, const int index, bool t);
        void disp(const unsigned int offset, const int index);
        bool prediction(const unsigned int offset, const int index);
        ~PHT();
    private:
        std::bitset<2> **sc;
        int row;
        int col;
};
