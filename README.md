# RISC-V Assembler Project

## Overview
This project involves building a **32-bit RISC-V assembler**. The assembler reads RISC-V assembly code from an input file (`input.asm`) and generates the corresponding machine code in an output file (`output.mc`). The project is designed to help understand the RISC-V instruction set architecture (**RV32I**) and the process of converting assembly code to machine code.

## Project Details

###  Conversion of Assembly Code to Machine Code**
- **Input File**: `input.asm` (contains RISC-V assembly instructions)
- **Output File**: `output.mc` (contains machine code and related information)
- **Memory Segments**:
  - **Code segment** starts at `0x00000000`
  - **Data segment** starts at `0x10000000`

### **Supported Instructions**
- **R format**: `add`, `and`, `or`, `sll`, `slt`, `sra`, `srl`, `sub`, `xor`, `mul`, `div`, `rem`
- **I format**: `addi`, `andi`, `ori`, `lb`, `ld`, `lh`, `lw`, `jalr`
- **S format**: `sb`, `sw`, `sd`, `sh`
- **SB format**: `beq`, `bne`, `bge`, `blt`
- **U format**: `auipc`, `lui`
- **UJ format**: `jal`

### **Assembler Directives**
- `.text`, `.data`, `.byte`, `.half`, `.word`, `.dword`, `.asciz`

### **Output Format**
Each line in the `output.mc` file should follow this format:
```
<address of instruction> <machine code of the instruction> , <assembly instruction> # <opcode-func3-func7-rd-rs1-rs2-immediate>
```
Example:
```
0x0 0x003100B3 , add x1,x2,x3 # 0110011-000-0000000-00001-00010-00011-NULL
0x4 0x00A37293 , andi x5,x6,10 # 0010011-111-NULL-00101-00110-000000001010
```




## **Acknowledgements**
- Special thanks to **Dr. T.V. Kalyan** for his guidance.
- Inspired by the **RISC-V architecture** and the desire to create a learning tool for it.
- Thanks to the contributors of the **RISC-V ISA specifications** and related documentation.

## **How to Run**
1. **Clone the repository.**
2. **Compile the assembler code using a C/C++/Java compiler.**
3. **Run the assembler with the `input.asm` file as input.**
4. **Check the `output.mc` file for the generated machine code.**
