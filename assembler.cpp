#include<bits/stdc++.h>
using namespace std;
typedef long long int ll;

class instruction
{
    public:
    string name;
    string opcode;
    string format;
    string func3;
    string func7;
};
map<ll, bitset<8>> memory;

ll start_address = 0x10000000;

void writeMemoryToFile() {
    ofstream outFile("output.mc", ios::app); // Open in append mode
    if (!outFile) {
        cerr << "Error opening file: output.mc" << endl;
        return;
    }
    
    outFile << "Memory Dump (Address → Value):\n";
    for (auto &entry : memory) {
        outFile << "0x" << hex << entry.first << " " << entry.second << "\n";
    }
    
    outFile.close();
}
void writetofile(string filename,string data)
{
    ofstream file;
    file.open(filename,ios::app);
    file<<data;
    file.close();
}
string parseImm(string imm){
    // if imm is hex or binary then convert it to decimal
    // dont use stoi as it is not able to convert hex to decimal
    if(imm[0] == '0' && imm[1] == 'x')
    {
        int num = 0;
        for(int i = 2; i < imm.size(); i++)
        {
            num = num * 16;
            if(imm[i] >= '0' && imm[i] <= '9')
            {
                num += imm[i] - '0';
            }
            else
            {
                num += imm[i] - 'a' + 10;
            }
        }
        imm = to_string(num);
    }
    else if(imm[0] == '0' && imm[1] == 'b')
    {
        int num = 0;
        for(int i = 2; i < imm.size(); i++)
        {
            num = num * 2;
            num += imm[i] - '0';
        }
        imm = to_string(num);
    }
    return imm;
}

map<string, long long> labelMap; // Stores label names with their PC values
map<long long, string> instructionMap; // Stores PC values with instructions

bool checkLabel(string stmt) {
    int i = stmt.size() - 1;
    while (i >= 0 && stmt[i] != ':') {
        i--;
    }
    return (i >= 0); // If ':' is found, it's a valid label
}

void makeLabel(string stmt, long long pc) {
    int i = 0;
    string lname = "";
    
    // Extract label name before ':'
    while (stmt[i] != ' ' && stmt[i] != ':') {
        lname += stmt[i];
        i++;
    }

    // Move to ':' and check if only spaces follow
    while (stmt[i] != ':') {
        i++;
    }
    for (int j = i + 1; j < stmt.size(); j++) {
        if (stmt[j] != ' ') {
            return; // Ignore malformed labels
        }
    }

    // Store label with its PC value
    labelMap[lname] = pc;
}



void processDataSegment() {
    ifstream fin("input.asm");  // Always reads from "input.asm"
    if (!fin) {  
        cerr << "Error opening file!" << endl;
        return;
    }

    string stmt;

    // *Step 1: Find .data section*
    while (getline(fin, stmt)) {
        if (stmt == ".data") break;
    }

    // *Step 2: Read .data section*
    while (getline(fin, stmt)) {
        if (stmt.empty()) continue;  // Ignore empty lines
        if (stmt == ".text") break;  // Stop at .text section
        if(fin.eof()) break;  // Stop at EOF

        int i = 0;
        string label = "";
        
        // *Extract label name*
        while (stmt[i] != ':') {
            label += stmt[i];
            i++;
        }
        i++;  // Move past :

        // *Extract directive type*
        string directive = "";
        while (stmt[i] == ' ') i++;  // Skip spaces
        while (stmt[i] != ' ') {
            directive += stmt[i];
            i++;
        }
        i++;

        vector<int> values;
        string temp = "";

        // *Handle .asciiz (String)*
        if (directive == ".asciiz") {
            i++;  // Skip first quote
            while (i < stmt.size() && stmt[i] != '"') {
                values.push_back(int(stmt[i]));  // Store ASCII value
                i++;
            }
            values.push_back(0);  // Null terminator
        } 
        // *Handle Numerical Directives*
        else {
            while (i < stmt.size()) {
                if (stmt[i] == ' ' || i == stmt.size() - 1) {
                    if (i == stmt.size() - 1) temp += stmt[i];  // Last value
                    values.push_back(stoi(temp));
                    temp = "";
                } else {
                    temp += stmt[i];
                }
                i++;
            }
        }

        // *Step 3: Store values in memory*
        for (int val : values) {
            ll temp = val;
            int size = 0;

            if (directive == ".byte") size = 1;
            else if (directive == ".half") size = 2;
            else if (directive == ".word") size = 4;
            else if (directive == ".dword") size = 8;
            else if (directive == ".asciiz") size = 1;

            for (int j = 0; j < size; j++) {
                memory[start_address] = bitset<8>(temp);  // Store byte-wise
                temp >>= 8;
                start_address++;
            }
        }
    }

    fin.close();
}

void printMemory() {
    cout << "Memory Dump (Address → Value):\n";
    for (auto &entry : memory) {
        cout << "0x" << hex << entry.first << " → " << entry.second << "\n";
    }
}


map<string,instruction> instruction_map = {
    {"add", {"add", "0110011", "R", "000", "0000000"}},
    {"and", {"and", "0110011", "R", "111", "0000000"}},
    {"or", {"or", "0110011", "R", "110", "0000000"}},
    {"sll", {"sll", "0110011", "R", "001", "0000000"}},
    {"slt", {"slt", "0110011", "R", "010", "0000000"}},
    {"sra", {"sra", "0110011", "R", "101", "0100000"}},
    {"srl", {"srl", "0110011", "R", "101", "0000000"}},
    {"sub", {"sub", "0110011", "R", "000", "0100000"}},
    {"xor", {"xor", "0110011", "R", "100", "0000000"}},
    {"mul", {"mul", "0110011", "R", "000", "0000001"}},
    {"div", {"div", "0110011", "R", "100", "0000001"}},
    {"rem", {"rem", "0110011", "R", "110", "0000001"}},
    {"addi", {"addi", "0010011", "I", "000", "0000000"}},
    {"andi", {"andi", "0010011", "I", "111", "0000000"}},
    {"ori", {"ori", "0010011", "I", "110", "0000000"}},
    {"lb", {"lb", "0000011", "I", "000", "0000000"}},
    {"ld", {"ld", "0000011", "I", "011", "0000000"}},
    {"lh", {"lh", "0000011", "I", "001", "0000000"}},
    {"lw", {"lw", "0000011", "I", "010", "0000000"}},
    {"jalr", {"jalr", "1100111", "I", "000", "0000000"}},
    {"sb", {"sb", "0100011", "S", "000", "0000000"}},
    {"sw", {"sw", "0100011", "S", "010", "0000000"}},
    {"sd", {"sd", "0100011", "S", "011", "0000000"}},
    {"sh", {"sh", "0100011", "S", "001", "0000000"}},
    {"beq", {"beq", "1100011", "SB", "000", "0000000"}},
    {"bne", {"bne", "1100011", "SB", "001", "0000000"}},
    {"bge", {"bge", "1100011", "SB", "101", "0000000"}},
    {"blt", {"blt", "1100011", "SB", "100", "0000000"}},
    {"auipc", {"auipc", "0010111", "U", "000", "0000000"}},
    {"lui", {"lui", "0110111", "U", "000", "0000000"}},
    {"jal", {"jal", "1101111", "UJ", "000", "0000000"}}
};

map<string,string> register_map = {
    {"x0", "00000"},
    {"x1", "00001"},
    {"x2", "00010"},
    {"x3", "00011"},
    {"x4", "00100"},
    {"x5", "00101"},
    {"x6", "00110"},
    {"x7", "00111"},
    {"x8", "01000"},
    {"x9", "01001"},
    {"x10", "01010"},
    {"x11", "01011"},
    {"x12", "01100"},
    {"x13", "01101"},
    {"x14", "01110"},
    {"x15", "01111"},
    {"x16", "10000"},
    {"x17", "10001"},
    {"x18", "10010"},
    {"x19", "10011"},
    {"x20", "10100"},
    {"x21", "10101"},
    {"x22", "10110"},
    {"x23", "10111"},
    {"x24", "11000"},
    {"x25", "11001"},
    {"x26", "11010"},
    {"x27", "11011"},
    {"x28", "11100"},
    {"x29", "11101"},
    {"x30", "11110"},
    {"x31", "11111"}
};



void rformat(ll pc,string instr)
{
    string original_instr = instr;
    string machine_code = "";
    string name = instr.substr(0, instr.find(" "));
    instr = instr.substr(instr.find(" ") + 1);
    string rd = instr.substr(0, instr.find(","));
    instr = instr.substr(instr.find(" ") + 1);
    string rs1 = instr.substr(0, instr.find(","));
    instr = instr.substr(instr.find(" ") + 1);
    string rs2 = instr; // No need to find ",", it's the last part

    machine_code += instruction_map[name].func7;
    machine_code += register_map[rs2];
    machine_code += register_map[rs1];
    machine_code += instruction_map[name].func3;
    machine_code += register_map[rd];
    machine_code += instruction_map[name].opcode;

    // Convert the machine code to a bitset of 32 bits
    bitset<32> b(machine_code);

    // Convert to hexadecimal
    stringstream ss;
    ss << hex << uppercase << setw(8) << setfill('0') << b.to_ulong();

    // above two printed lines should also be sent to output.mc file as it is
    writetofile("output.mc","0x" + to_string(pc) + " 0x" + ss.str() + " , " + original_instr + " # " + instruction_map[name].opcode + "-" + instruction_map[name].func3 + "-" + instruction_map[name].func7 + "-" + register_map[rd] + "-" + register_map[rs1] + "-" + register_map[rs2] + "-NULL\n");
    

}

void sformat(ll pc, string instr)
{
    string original_instr = instr;
    string machine_code = "";
    
    string name = instr.substr(0, instr.find(" "));
    instr = instr.substr(instr.find(" ") + 1);
    
    string rs2 = instr.substr(0, instr.find(","));
    instr = instr.substr(instr.find(" ") + 1);
    
    string rs1, imm;
    
    if (instr.find("(") != string::npos) {
        // **Bracket notation** → rs2, offset(rs1)
        imm = instr.substr(0, instr.find("("));  // Extract offset
        instr = instr.substr(instr.find("(") + 1);
        rs1 = instr.substr(0, instr.find(")"));  // Extract rs1
    } else {
        // **Comma notation** → rs2, imm, rs1
        imm = instr.substr(0, instr.find(","));
        instr = instr.substr(instr.find(" ") + 1);
        rs1 = instr;  // Remaining part is rs1
    }

    imm = parseImm(imm);  // Convert imm to decimal if it's hex or binary
    // Convert imm (offset) to 12-bit binary (handles negative values using 2's complement)
    int imm_val = stoi(imm);
    bitset<12> imm_bin(imm_val & 0xFFF);  

    // Format S-type instruction: imm[11:5] | rs2 | rs1 | func3 | imm[4:0] | opcode
    machine_code += imm_bin.to_string().substr(0, 7);  // imm[11:5]
    machine_code += register_map[rs2];
    machine_code += register_map[rs1];
    machine_code += instruction_map[name].func3;
    machine_code += imm_bin.to_string().substr(7, 5);  // imm[4:0]
    machine_code += instruction_map[name].opcode;

    // Convert to hexadecimal and print
    bitset<32> b(machine_code);
    stringstream ss;
    ss << hex << uppercase << setw(8) << setfill('0') << b.to_ulong();
    writetofile("output.mc","0x" + to_string(pc) + " 0x" + ss.str() + " , " + original_instr + " # " + instruction_map[name].opcode + "-" + instruction_map[name].func3 + "-NULL-NULL-" + register_map[rs1] + "-" + register_map[rs2] + "-" + imm_bin.to_string().substr(0, 7) + imm_bin.to_string().substr(7, 5) + "\n");
}

void iformat(ll pc, string instr)
{
    string original_instr = instr;
    string machine_code = "";
    string name = instr.substr(0, instr.find(" "));
    instr = instr.substr(instr.find(" ") + 1);
    string rd = instr.substr(0, instr.find(","));
    instr = instr.substr(instr.find(" ") + 1);

    string rs1, imm;
    
    bool isLoadInstruction = (name == "ld") || (name == "lb") || (name == "lw") || (name == "lh");
    bool isImmediateArithmetic = (name == "addi") || (name == "andi") || (name == "xori") || (name == "ori");
    bool isJalr = (name == "jalr");

    if (isLoadInstruction) {
        if (instr.find("(") != string::npos) {
            // **Case 1: Load format with brackets** → rd, offset(rs1)
            imm = instr.substr(0, instr.find("("));  // Extract offset
            instr = instr.substr(instr.find("(") + 1);
            rs1 = instr.substr(0, instr.find(")"));  // Extract rs1
        } else {
            // **Case 2: Load format without brackets** → rd, imm, rs1
            imm = instr.substr(0, instr.find(","));
            instr = instr.substr(instr.find(" ") + 1);
            rs1 = instr;  // Remaining part is rs1
        }
    } 
    else if (isImmediateArithmetic || isJalr) {
        // **For arithmetic I-format & jalr:** "rd, rs1, imm"
        rs1 = instr.substr(0, instr.find(","));
        instr = instr.substr(instr.find(" ") + 1);
        imm = instr;  // Remaining part is immediate value
    } 
    else {
        cerr << "Error: Unsupported I-type instruction: " << name << endl;
        return;
    }

    imm = parseImm(imm);  // Convert imm to decimal if it's hex or binary
    // Convert imm (offset) to 12-bit binary (handles negative values using 2's complement)
    int num = stoi(imm);
    bitset<12> immBin(num & 0xFFF);  // Mask to 12-bit two's complement

    // Add binary values to machine code
    machine_code += immBin.to_string();
    machine_code += register_map[rs1];
    machine_code += instruction_map[name].func3;
    machine_code += register_map[rd];
    machine_code += instruction_map[name].opcode;

    // Convert to hexadecimal and print
    bitset<32> b(machine_code);
    stringstream ss;
    ss << hex << uppercase << setw(8) << setfill('0') << b.to_ulong();

    // send to output.mc
    writetofile("output.mc","0x" + to_string(pc) + " 0x" + ss.str() + " , " + original_instr + " # " + instruction_map[name].opcode + "-" + instruction_map[name].func3 + "-NULL-" + register_map[rd] + "-" + register_map[rs1] + "-NULL-" + immBin.to_string() + "\n");
}

void sbformat(ll pc,string instr)
{
    string original_instr = instr;
    string machine_code = "";
    string name = instr.substr(0, instr.find(" "));
    instr = instr.substr(instr.find(" ") + 1);
    string rs1 = instr.substr(0, instr.find(","));
    instr = instr.substr(instr.find(" ") + 1);
    string rs2 = instr.substr(0, instr.find(","));
    instr = instr.substr(instr.find(" ") + 1);
    string imm = instr;
    bool chang = false;


    //convert imm to binary
    //check if the immediate is a label or a number
    if(imm[0] >= '0' && imm[0] <= '9')
    {
        int num = stoi(imm);
        imm = "";
        for(int i = 0; i < 13; i++)
        {
            imm = to_string(num % 2) + imm;
            num /= 2;

        }
    }
    else
    {
        //find the address of the label and then convert it to binary
        string imm1="";
        for(auto c: imm){
            if(c==' ' || c=='#') break;
            imm1+=c;
        }
        
        ll num = labelMap[imm1];
        num -= pc;
        if(num<0){
            //create 13 bit 2's complement
            num=abs(num);
            num=(1<<12)-num;
            chang = true;

        }
        imm = "";
        for(int i = 0; i < 13; i++)
        {
            imm = to_string(num % 2) + imm;
            num /= 2;
        }
    }
    imm = parseImm(imm);  // Convert imm to decimal if it's hex or binary
    //add the binary imm to machine code
    if(chang){
        imm[0] = '1';
    }
    machine_code += imm[0];
    machine_code += imm.substr(2, 6);
    machine_code += register_map[rs2];
    machine_code += register_map[rs1];
    machine_code += instruction_map[name].func3;
    machine_code += imm.substr(8, 4);
    machine_code += imm[1];
    machine_code += instruction_map[name].opcode;
    
    //do the similar conversion to hexadecimal and print the machine code in the given format
    bitset<32> b(machine_code);
    stringstream ss;
    ss << hex << uppercase << setw(8) << setfill('0') << b.to_ulong();
    // above two printed lines should also be sent to output.mc file as it is
    writetofile("output.mc","0x" + to_string(pc) + " 0x" + ss.str() + " , " + original_instr + " # " + instruction_map[name].opcode + "-" + instruction_map[name].func3 + "-NULL-NULL-" + register_map[rs1] + "-" + register_map[rs2] + "-" + imm[0] + imm.substr(2, 6) + imm.substr(8, 4) + imm[1] + "\n");
}

void uformat(ll pc,string instr)
{
    string original_instr = instr;
    string machine_code = "";
    
    // Extract instruction name
    string name = instr.substr(0, instr.find(" "));
    instr = instr.substr(instr.find(" ") + 1);
    
    // Extract destination register
    string rd = instr.substr(0, instr.find(","));
    instr = instr.substr(instr.find(",") + 2);  // Skip ", "

    // Extract immediate
    string imm = instr;

    imm = parseImm(imm);
    // Convert immediate value to integer (supports both decimal and hex)
    int imm_int = stoi(imm, nullptr, 0);  
    int upper_imm = (imm_int);  // Extract upper 20 bits

    // Convert upper immediate to binary
    bitset<20> imm_bin(upper_imm);

    // Construct the machine code
    machine_code += imm_bin.to_string();   // 20-bit immediate
    machine_code += register_map[rd];      // 5-bit destination register
    machine_code += instruction_map[name].opcode; // 7-bit opcode

    //do the similar conversion to hexadecimal and print the machine code in the given format
    bitset<32> b(machine_code);
    stringstream ss;
    ss << hex << uppercase << setw(8) << setfill('0') << b.to_ulong();

    // above two printed lines should also be sent to output.mc file as it is
    writetofile("output.mc","0x" + to_string(pc) + " 0x" + ss.str() + " , " + original_instr + " # " + instruction_map[name].opcode + "-NULL-NULL-" + register_map[rd] + "-NULL-NULL-" + imm + "\n");
}

void ujformat(ll pc,string instr)
{
    string original_instr = instr;
    string machine_code = "";
    string name = instr.substr(0, instr.find(" "));
    instr = instr.substr(instr.find(" ") + 1);
    string rd = instr.substr(0, instr.find(","));
    instr = instr.substr(instr.find(" ") + 1);
    string imm = instr;
    bool chang = false;
    if(imm[0] >= '0' && imm[0] <= '9')
    {
        imm=parseImm(imm);
        int num = stoi(imm);
        imm = "";
        for(int i = 0; i < 21; i++)
        {
            imm = to_string(num % 2) + imm;
            num /= 2;
        }
    }

    else
    {
        //find the address of the label and then convert it to binary
        ll num = labelMap[imm];
        num -= pc;
        if(num<0){
            //create 21 bit 2's complement
            num=abs(num);
            num=(1<<20)-num;
            chang = true;

        }

        imm = "";
        for(int i = 0; i < 21; i++)
        {
            imm = to_string(num % 2) + imm;
            num /= 2;
        }
    }
    if(chang){
        imm[0] = '1';
    }
    imm = parseImm(imm);  // Convert imm to decimal if it's hex or binary
    //add the binary imm to machine code
    machine_code += imm[0];
    machine_code += imm.substr(10, 10);
    machine_code += imm[9];
    machine_code += imm.substr(1, 8);
    machine_code += register_map[rd];
    machine_code += instruction_map[name].opcode;

    //do the similar conversion to hexadecimal and print the machine code in the given format
    bitset<32> b(machine_code);
    stringstream ss;
    ss << hex << uppercase << setw(8) << setfill('0') << b.to_ulong();

    writetofile("output.mc","0x" + to_string(pc) + " 0x" + ss.str() + " , " + original_instr + " # " + instruction_map[name].opcode + "-NULL-NULL-" + register_map[rd] + "-NULL-NULL-" + imm[0] + imm.substr(10, 10) + imm[9] + imm.substr(1, 8) + "\n");
    
}


// Function to process instructions based on their format
void processInstructions() {
    for (const auto& instrPair : instructionMap) {
        long long pc = instrPair.first;
        string instr = instrPair.second;

        // Extract the instruction name from the instr string and save it in instrName
        string instrName = instr.substr(0, instr.find(" "));
        


        // Find the instruction in instruction_map
        auto it = instruction_map.find(instrName);
        if (it != instruction_map.end()) {
            instruction instrDetails = it->second;
            string format = instrDetails.format;

            // Call the corresponding function based on format
            if (format == "R") {
                rformat(pc, instr);
            } else if (format == "I") {
                iformat(pc, instr);
            } else if (format == "S") {
                sformat(pc, instr);
            } else if (format == "SB") {
                sbformat(pc, instr);
            } else if (format == "U") {
                uformat(pc, instr);
            } else if (format == "UJ") {
                ujformat(pc, instr);
            }
        } else {
            cout << "Unknown instruction at PC " << pc << ": " << instrName << endl;
        }
    }
}
void parseTextSegment() {
    ifstream file("input.asm"); // Always use input.asm
    if (!file) {
        cerr << "Error opening file: input.asm" << endl;
        return;
    }

    string line;
    long long pc = 0; // Initialize PC to 0
    bool inTextSegment = false; // Track if we're inside the .text segment

    while (!file.eof()) { // Check for EOF
        getline(file, line);
        if (file.eof()) break; // Ensure last line is processed correctly

        // Remove leading/trailing spaces
        stringstream ss(line);
        string stmt;
        getline(ss, stmt);

        if (stmt.empty()) continue; // Ignore empty lines

        // Check for segment headers
        if (stmt.find(".data") != string::npos) {
            inTextSegment = false; // Ignore .data section
            continue;
        }
        if (stmt.find(".text") != string::npos) {
            inTextSegment = true; // Start parsing instructions from here
            continue;
        }

        if (!inTextSegment) continue; // Skip everything outside .text

        // If it's a label, store it
        if (checkLabel(stmt)) {
            makeLabel(stmt, pc);
            // pc += 4; // Increment PC by 4 for each label
        } else {
            // Otherwise, store the instruction
            // instructionMap[pc] = stmt;
            pc += 4; // Increment PC by 4 for each instruction
        }
    }

    file.close();
    ifstream file1("input.asm"); // Always use input.asm
    if (!file1) {
        cerr << "Error opening file: input.asm" << endl;
        return;
    }

    line;
    pc = 0; // Initialize PC to 0
    inTextSegment = false; // Track if we're inside the .text segment

    while (!file1.eof()) { // Check for EOF
        getline(file1, line);
        if (file1.eof()) break; // Ensure last line is processed correctly

        // Remove leading/trailing spaces
        stringstream ss(line);
        string stmt;
        getline(ss, stmt);

        if (stmt.empty()) continue; // Ignore empty lines

        // Check for segment headers
        if (stmt.find(".data") != string::npos) {
            inTextSegment = false; // Ignore .data section
            continue;
        }
        if (stmt.find(".text") != string::npos) {
            inTextSegment = true; // Start parsing instructions from here
            continue;
        }

        if (!inTextSegment) continue; // Skip everything outside .text

        // If it's a label, store it
        if (checkLabel(stmt)) {
            // makeLabel(stmt, pc);
            continue;
            // pc += 4; // Increment PC by 4 for each label
        } else {
            // Otherwise, store the instruction
            instructionMap[pc] = stmt;
            pc += 4; // Increment PC by 4 for each instruction
        }
    }

    file1.close();
}

void displayParsedData() {
    cout << "Label Map:\n";
    for (const auto &entry : labelMap) {
        cout << "Label: " << entry.first << " -> PC: " << entry.second << endl;
    }

    cout << "\nInstruction Map:\n";
    for (const auto &entry : instructionMap) {
        cout << "PC: " << entry.first << " -> Instruction: " << entry.second << endl;
    }
}

int main()
{
   
    parseTextSegment();
    // displayParsedData();
    processInstructions();


    processDataSegment();
    writeMemoryToFile();
    // printMemory();
    
    // iformat(0,"addi x1, x0, 16");
    // iformat(0,"addi x1, x0, 0x10");
    // iformat(0,"addi x1, x0, 0b10000");

    // sformat(0,"sb x1, 16(x2)");
    // sformat(0,"sb x1, 0x10(x2)");
    // sformat(0,"sb x1, 0b10000(x2)");

    // sbformat(0,"beq x1, x2, 16");
    // sbformat(0,"beq x1, x2, 0x10");
    // sbformat(0,"beq x1, x2, 0b10000");

    // uformat(0,"lui x1, 16");
    // uformat(0,"lui x1, 0x10");
    // uformat(0,"lui x1, 0b10000");

    // ujformat(0,"jal x1, 16");
    // ujformat(0,"jal x1, 0x10");
    // ujformat(0,"jal x1, 0b10000");
    displayParsedData();

    return 0;
}

