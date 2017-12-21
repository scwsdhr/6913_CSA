#include<iostream>
#include<string>
#include<vector>
#include<bitset>
#include<fstream>
using namespace std;
#define ADDU 1
#define SUBU 3
#define AND 4
#define OR  5
#define NOR 7
#define MemSize 65536 // memory size, in reality, the memory size should be 2^32, but for this lab, for the space resaon, we keep it as this large number, but the memory is still 32-bit addressable.

// Operator Overloading
bitset<32> operator + (const bitset<32> &b1, const bitset<32> &b2);
bitset<32> operator - (const bitset<32> &b1, const bitset<32> &b2);

// DumpResults
void dumpResults(bitset<32> pc, bitset<5> WrRFAdd, bitset<32> WrRFData, bitset<1> RFWrEn, bitset<32> WrMemAdd, bitset<32> WrMemData, bitset<1> WrMemEn);

// Return the type of instruction
string TypeofInstruction(string opcode);

class RF
{
    public:
        bitset<32> ReadData1, ReadData2; 
        RF()
        { 
            Registers.resize(32);  
            Registers[0] = bitset<32> (0);

            // Delete the previous RFresult.txt
            remove("RFresult.txt");
        }

        void ReadWrite(bitset<5> RdReg1, bitset<5> RdReg2, bitset<5> WrtReg, bitset<32> WrtData, bitset<1> WrtEnable)
        {   
            // implement the funciton by you.               
            ReadData1 = Registers[RdReg1.to_ulong()];
            ReadData2 = Registers[RdReg2.to_ulong()];
            if (WrtEnable.to_ulong() == 1) {
                Registers[WrtReg.to_ulong()] = WrtData;
                cout << "| Write " << WrtData << " to Reg " << WrtReg << endl;
            }
        }

        void OutputRF()
        {
            ofstream rfout;
            rfout.open("RFresult.txt",std::ios_base::app);
            if (rfout.is_open())
            {
                rfout << "A state of RF:" << endl;
                for (int j = 0; j<32; j++)
                {        
                    rfout << Registers[j] << endl;
                }

            }
            else cout<<"Unable to open file";
            rfout.close();

        }     
    private:
        vector<bitset<32> >Registers;

};

class ALU
{
    public:
        bitset<32> ALUresult;
        void ALUOperation (bitset<3> ALUOP, bitset<32> oprand1, bitset<32> oprand2)
        {   
            // implement the ALU operations by you. 
            if (ALUOP.to_ulong() == ADDU)
                ALUresult = oprand1 + oprand2;
            else if (ALUOP.to_ulong() == SUBU)
                ALUresult = oprand1 - oprand2;
            else if (ALUOP.to_ulong() == AND)
                ALUresult = oprand1 & oprand2;
            else if (ALUOP.to_ulong() == OR)
                ALUresult = oprand1 | oprand2;
            else if (ALUOP.to_ulong() == NOR)
                ALUresult = oprand1 ^ oprand2;
        }            
};

class INSMem
{
    public:
        bitset<32> Instruction;
        INSMem()
        {       
            IMem.resize(MemSize); 
            ifstream imem;
            string line;
            int i = 0;
            imem.open("imem.txt");
            if (imem.is_open())
            {
                while (getline(imem,line))
                {      
                    IMem[i] = bitset<8>(line);
                    i++;
                }

            }
            else cout<<"Unable to open file";
            imem.close();
        }

        bitset<32> ReadMemory (bitset<32> ReadAddress) 
        {    
            // implement by you. (Read the byte at the ReadAddress and the following three byte).
            int index = (int)ReadAddress.to_ulong();
            string inst;
            for (int i = 0; i < 4; i++)
            {
                inst += IMem[index].to_string();
                index++;
            }
            bitset<32> Instruction(inst);
            return Instruction;     
        }     

    private:
        vector<bitset<8> > IMem;

};

class DataMem    
{
    public:
        bitset<32> readdata;  
        DataMem()
        {
            DMem.resize(MemSize); 
            ifstream dmem;
            string line;
            int i = 0;
            dmem.open("dmem.txt");
            if (dmem.is_open())
            {
                while (getline(dmem,line))
                {      
                    DMem[i] = bitset<8>(line);
                    i++;
                }
            }
            else cout<<"Unable to open file";
            dmem.close();

            // Delete the previous dmemresult.txt
            remove("dmemresult.txt");
        }  
        void MemoryAccess (bitset<32> Address, bitset<32> WriteData, bitset<1> readmem, bitset<1> writemem) 
        {    

            // implement by you.
            int index = (int) Address.to_ulong();
            if ((readmem.to_ulong() == 1) && (writemem.to_ulong() == 0))
            {
                cout << "| Reading from DMem[" << index << "]..." << endl;
                string data;
                for (int i = 0; i < 4; i++)
                {
                    data += DMem[index].to_string();
                    index++;
                }
                readdata = bitset<32>(data);
            }
            else if ((readmem.to_ulong() == 0) && (writemem.to_ulong() == 1))
            {
                cout << "| Writing " << WriteData.to_string()
                     << " to DMem[" << index << "]..." << endl;
                for (int i = 0; i < 32; i += 8)
                {
                    string result = WriteData.to_string().substr(i,8);
                    DMem[index] = bitset<8>(result);
                    index++;
                }
            }
        }   

        void OutputDataMem()
        {
            ofstream dmemout;
            dmemout.open("dmemresult.txt");
            if (dmemout.is_open())
            {
                for (int j = 0; j< 1000; j++)
                {     
                    dmemout << DMem[j] << endl;
                }
            }
            else cout << "Unable to open file";
            dmemout.close();
        }             

    private:
        vector<bitset<8> > DMem;

};  

struct Inst {
    public:
        Inst() {
            cout << "No instruction fetched. "
                 << "Initialize the instruction with NULL." << endl;
            *this = Inst(NULL);
        }
        Inst(bitset<32> instruction) {
            // Initialize variables
            IsNULL = 0;
            IsLoad = 0;
            IsStore = 0;
            IsBranch = 0;
            IsEq = 0;
            IsHalt = 0;
            I_Type = 0;
            J_Type = 0;
            ALUOP = NULL;
            WrtEnable = NULL;
            RdReg1 = NULL;
            RdReg2 = NULL;
            WrtReg = NULL;
            SignExtendedImm = NULL;
            
            if (!IsNULL) {
                // Extract Opcode
                opcode = instruction.to_string().substr(0, 6);
                
                // Decide instruction type
                Type_instruction = TypeofInstruction(opcode);

                if (Type_instruction == "lw")
                    IsLoad = 1;
                if (Type_instruction == "sw")
                    IsStore = 1;
                if (Type_instruction == "beq")
                    IsBranch = 1;
                if (Type_instruction == "halt")
                    IsHalt = 1;
                if ((Type_instruction != "R") && (Type_instruction != "J"))
                    I_Type = 1;
                if (Type_instruction == "J")
                    J_Type = 1;

                // Decide ALUOP
                if ((Type_instruction == "lw") || (Type_instruction == "sw"))
                    ALUOP = bitset<3> (string("001"));
                else if (Type_instruction == "R")
                    ALUOP = bitset<3> (instruction.to_string().substr(29, 3));
                else 
                    ALUOP = bitset<3> (opcode.substr(2,3));

                // Decide WrtEnable
                if (IsStore || IsBranch || J_Type)
                    WrtEnable = 0;
                else
                    WrtEnable = 1;
                
                // Decide other registers
                RdReg1 = bitset<5> (instruction.to_string().substr(6, 5));
                RdReg2 = bitset<5> (instruction.to_string().substr(11, 5));
                WrtReg = bitset<5> (instruction.to_string().substr(I_Type? 11:16, 5));

                // Sign extend imm
                SignExtendedImm = bitset<32> (instruction.to_string().substr(16, 16));
                if (SignExtendedImm.test(15))
                    for (int i = 31; i > 15; i--) SignExtendedImm[i].flip();
            }
        };

        bool IsNULL;
        bool IsLoad;
        bool IsStore;
        bool IsBranch;
        bool IsEq;
        bool IsHalt;
        bool I_Type;
        bool J_Type;
        bitset<3> ALUOP;
        bitset<1> WrtEnable;
        bitset<5> RdReg1;
        bitset<5> RdReg2;
        bitset<5> WrtReg;
        bitset<32> SignExtendedImm;
    private:
        string opcode;
        string Type_instruction;
};

// Instruction fetch
void Stage_1(bitset<32> pc, bitset<32> *instruction, INSMem myInsMem);

// Instruction decode / RF read and excute
void Stage_2(bitset<32> *pc, bitset<32> instruction, Inst *myInst, RF *myRF, ALU *myALU);

// Data memory load / store and wirtes back to the RF
void Stage_3(Inst myInst, RF *myRF, ALU myALU, DataMem *myDataMem);

// Next PC
bitset<32> NextPC(bitset<32> pc, Inst myInst);

int main()
{   
    bitset<32> pc = 0;
    bitset<32> instruction = NULL;
    RF myRF;
    ALU myALU;
    INSMem myInsMem;
    DataMem myDataMem;

    cout << "===============================================" << endl;
    cout << "||         3-Pipelined MIPS Simulator        ||" << endl;
    cout << "===============================================" << endl;

    Inst myInst;

    // Delete the previous Results.txt
    remove("Results.txt");

    cout << endl << "Begin!" << endl;
    cout << "===============================================" << endl;

    // Test variables
    int count = 0;
    while (1) //each loop body corresponds to one clock cycle.
    {
        cout << "Clock Cycle " << count << endl;
        cout << "PC = " << pc.to_ulong() << endl << endl;

        // Stage 3
        Stage_3(myInst, &myRF, myALU, &myDataMem);
        
        // Halt condition
        if (instruction.count() == 32) {
            cout << "Meet Halt instruction. Terminating!" << endl;
            cout << "===============================================" << endl;
            break;
        }
        
        // Stage 2
        Stage_2(&pc, instruction,  &myInst, &myRF, &myALU);

        // Stage 1
        Stage_1(pc, &instruction, myInsMem);

        // At the end of each cycle, fill in the corresponding data into "dumpResults" function to output files.
        // The first dumped pc value should be 0.	
        dumpResults(pc, myInst.WrtReg, myInst.IsLoad? myDataMem.readdata : myALU.ALUresult, myInst.WrtEnable, myALU.ALUresult, myRF.ReadData2, myInst.IsStore);
        // myRF.OutputRF(); // dump RF; 

        pc = NextPC(pc, myInst);

        // Decide whether it is a J_Type instruction
        if (myInst.J_Type == 1) {
            cout << "| Oops. J-Type!" << endl;
            cout << "| Jump to PC " << pc.to_ulong() << endl;
            cout << "| Set the fetched instruction to be a NULL" << endl;
            instruction = NULL;
        }

        count++;

        cout << "===============================================" << endl;

        cout << endl;
        
        if (count > 20) break;

    }
    myRF.OutputRF(); // dump RF; 
    myDataMem.OutputDataMem(); // dump data mem

    return 0;

}

// Operator Overloading
bitset<32> operator + (const bitset<32> &b1, const bitset<32> &b2) {
    return b1.to_ulong() + b2.to_ulong();
}
bitset<32> operator - (const bitset<32> &b1, const bitset<32> &b2) {
    return b1.to_ulong() - b2.to_ulong();
}

void dumpResults(bitset<32> pc, bitset<5> WrRFAdd, bitset<32> WrRFData, bitset<1> RFWrEn, bitset<32> WrMemAdd, bitset<32> WrMemData, bitset<1> WrMemEn)
{
    ofstream fileout;

    fileout.open("Results.txt",std::ios_base::app);
    if (fileout.is_open())
    {
        fileout <<pc<<' '<<WrRFAdd<<' '<<WrRFData<<' '<<RFWrEn<<' '<<WrMemAdd<<' '<<WrMemData<<' '<<WrMemEn << endl;
    }
    else cout<<"Unable to open file";
    fileout.close();
}

string TypeofInstruction(string opcode) {
    if(opcode == "000000") return "R";
    else if(opcode == "000010") return "J";
    else if(opcode == "111111") return "halt";
    else if(opcode == "001001") return "addiu";
    else if(opcode == "000100") return "beq";
    else if(opcode == "100011") return "lw";
    else if(opcode == "101011") return "sw";
    else return "Invalid Opcode";
}

// Instruction fetch
void Stage_1(bitset<32> pc,bitset<32> *instruction, INSMem myInsMem) {
    cout << "Processing Stage 1..." << endl;

    // Fetch
    cout << "Fetching from PC " << pc.to_ulong() << "..." << endl;
    *instruction = myInsMem.ReadMemory(pc);
    cout << "Instruction: " << *instruction << endl;
}

// Instruction decode / RF read and excute
void Stage_2(bitset<32> *pc, bitset<32> instruction, Inst *myInst, RF *myRF, ALU *myALU) {
    cout << "Processing Stage 2..." << endl;
    *myInst = Inst(instruction);

    // Execute
    (*myRF).ReadWrite((*myInst).RdReg1, (*myInst).RdReg2, NULL, 0, 0);
    cout << "| ReadData1: " << (*myRF).ReadData1 << endl;
    cout << "| ReadData2: " << (*myRF).ReadData2 << endl;
    
    if ((*myRF).ReadData1 == (*myRF).ReadData2)
        (*myInst).IsEq = 1;
    (*myALU).ALUOperation((*myInst).ALUOP, (*myRF).ReadData1, 
            (*myInst).I_Type? (*myInst).SignExtendedImm : (*myRF).ReadData2);
    cout << "| ALUresult: " << (*myALU).ALUresult << endl;
}

// Data memory load / store and wirtes back to the RF
void Stage_3(Inst myInst, RF *myRF, ALU myALU, DataMem *myDataMem) {
    cout << "Processing Stage 3..." << endl;

    (*myDataMem).MemoryAccess(myALU.ALUresult, (*myRF).ReadData2, 
            myInst.IsLoad, myInst.IsStore);
    (*myRF).ReadWrite(NULL, NULL, myInst.WrtReg, 
            myInst.IsLoad? (*myDataMem).readdata : myALU.ALUresult, myInst.WrtEnable);
}

// Decide next PC
bitset<32> NextPC(bitset<32> pc, Inst myInst) {
    if (myInst.IsBranch && myInst.IsEq)
        pc = pc + bitset<32>(myInst.SignExtendedImm << 2);
    else if (myInst.J_Type)
        pc = bitset<32>((pc + bitset<32>((unsigned long)4)).to_string().substr(0,4)+
            (myInst.SignExtendedImm << 2).to_string().substr(4,28));
    else
        pc = pc + bitset<32>((unsigned long)4);
    return pc;
}
