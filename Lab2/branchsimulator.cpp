#include<iostream>
#include<string>
#include<vector>
#include<fstream>
#include<cstdlib>
#include<numeric>
#include<cmath>
#include "branchsimulator.h"

void Load_Config_to(int* m, int* k, char* fin);

int bit2dec(int p, int q);

bool fetch_trace(std::ifstream *trace, const int m, unsigned int *offset, bool *t);

int main(int argc, char** argv) {
    // test instruction
    if (argc != 3) {
        std::cout <<  "Missing arguments!" << std::endl;
        throw "Missing arguments!";
    }

    int m;
    int k;
    unsigned int offset;
    bool t;
    int counter_mis = 0;
    int counter_all = 0;
    std::ifstream trace;
    std::ofstream trace_out;

    // Remove the previous output file
    remove("trace.txt.out");

    // Load config file
    Load_Config_to(&m, &k, argv[1]);
    std::cout << "m = " << m << std::endl << "k = " << k << std::endl;

    // Build BHR
    BHR myBHR(k);

    // Build PHT
    PHT myPHT(m, k);

    // Open trace file
    trace.open(argv[2]);

    // Open output file
    trace_out.open("trace.txt.out", std::ios_base::app);

    // Fetch a pair of PC and Taken bit
    while ((fetch_trace(&trace, m, &offset, &t)) && (trace_out.is_open())) {
        // PHT and BHR in the previous clock cycle
        std::cout << "PHT before: ";
        myPHT.disp(offset, myBHR.getindex());
        std::cout << "BHR before: ";
        myBHR.disp();

        // The prediction result
        std::cout << "Prediction: " << myPHT.prediction(offset, myBHR.getindex()) << std::endl;

        // Print the result to output file
        trace_out << myPHT.prediction(offset, myBHR.getindex()) << std::endl;

        // Update counters
        counter_all += 1;
        counter_mis += (t != myPHT.prediction(offset, myBHR.getindex()));

        // Update PHT and BHR
        myPHT.update(offset, myBHR.getindex(), t);
        std::cout << "PHT after: ";
        myPHT.disp(offset, myBHR.getindex());
        myBHR.push(t);
    }

    // Close trace file
    trace.close();

    // Display a conclusion
    printf("\n** Conclusion **\n");
    printf("m = %d, k = %d.\n", m, k);
    printf("%d instructions in total.\n", counter_all);
    printf("The misprediction rate is %0.4f%%.\n", (double)counter_mis / counter_all * 100);

    return 0;
}

BHR::BHR(const int k) {
    // Initialize with true
    reg.resize(k, 1);
    // Initialize the value of index
    index = accumulate(reg.begin(), reg.end(), 0, bit2dec);
}

void BHR::push(bool p) {
    // Pop the last bit and insert before the first bit
    reg.insert(reg.begin(), p);
    reg.pop_back();

    // Update the value of index
    index = accumulate(reg.begin(), reg.end(), 0, bit2dec);
}

void BHR::disp() {
    // Go through the vector
    for (std::vector<bool>::iterator it = reg.begin(); it != reg.end(); it++) {
        std::cout << *it;
    }
    std::cout << std::endl;
}

int BHR::getindex() {
    // Get index
    return index;
}

PHT::PHT(const int m, const int k) {
    // Compute the number of rows and cols
    row = int(pow(2, m));
    col = int(pow(2, k));

    // Allocate memory for saturationg counters
    sc = new std::bitset<2> *[row];
    for (int i = 0; i < row; i++) {
        sc[i] = new std::bitset<2>[col];
        for (int j = 0; j < col; j++) {
            // Set the initial value to 1
            sc[i][j].set();
        }
    }
}

// Update the value in a certain saturation counter
void PHT::update(const unsigned int offset, const int index, bool t) {
    if (sc[offset][index][0] == t) sc[offset][index][1] = t;
    else sc[offset][index][0] = t;
}

// Display the value in a certain saturation counter
void PHT::disp(const unsigned int offset, const int index) {
    std::cout << sc[offset][index] << ' ';
    std::cout << std::endl;
}

// Return the prediction
bool PHT::prediction(const unsigned int offset, const int index) {
    return sc[offset][index][1];
}

// Destructer of PHT
PHT::~PHT() {
    // Release the pointer sc
    if (sc) {
        for (int i = 0; i < col; i++) {
            delete []sc[i];
        }
        delete []sc;
    }
}

// Load the values in config file to variable m and variable k
void Load_Config_to(int* m, int* k, char* config_name) {
    std::ifstream config;
    std::string line;
    // Open config file
    config.open(config_name);

    if (config.is_open()) {
        std::cout << "Loading config file..." << std::endl;

        // Assign the first line to m
        getline(config, line);
        *m = atoi(line.c_str());

        // Assign the second line to k
        getline(config, line);
        *k = atoi(line.c_str());

        // If not reach the end
        if (config.peek() != EOF) {
            // Throw an exception and display a message
            std::cout <<  "Wrong config file." << std::endl;
            throw "Wrong config file.";
        }
        // If reach the end
        else {
            // Display the completion message and return
            std::cout << "Loading config file finished." << std::endl;
            return;
        }
    }

    // If failed to open file
    else {
        std::cout << "Unable to open file." << std::endl;
        return;
    }

    // Close the config file
    config.close();
}

// Accumulate each bit
int bit2dec(int p, int q) {
    return (p << 1) + q;
}

// Fetch a line from trace file, if failed, return false
bool fetch_trace(std::ifstream *trace, const int m, unsigned int *offset, bool *t) {
    unsigned long pc;   // PC in Dec
    if ((*trace).is_open()) {
        std::cout << "Fetching..." << std::endl;

        // For each line , assign the first value to pc and the second to t
        (*trace) >> std::hex >> pc;
        (*trace) >> (*t);

        // Pick the 4 LSBs as offset
        (*offset) = pc << 32 - m;
        (*offset) >>= 32 - m;

        // If reach the end
        if ((*trace).peek() == EOF) {
            std::cout << "Reach the end!" << std::endl;
            return 0;
        }
        std::cout << "PC: " << std::bitset<32>(pc) << std::endl;
        std::cout << "offset: " << (*offset) << " is_Taken: " << (*t) << std::endl;
        return 1;
    }
    else {
        return 0;
    }
}
