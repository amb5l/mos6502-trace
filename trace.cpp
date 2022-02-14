#include <string>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <filesystem>
namespace fs = std::filesystem;

#include <stdint.h>
#include <stdio.h>

#include "mos6502.h"

uint8_t mem[65536];

uint8_t MemRd(uint16_t address) {
    return mem[address];
}

void MemWr(uint16_t address, uint8_t value) {
    mem[address] = value;
}

int main(int argc, char **argv) {
    std::string s, binfile;    
    uint16_t rstvec;
    uint8_t v;
    mos6502 cpu(MemRd, MemWr);
    uint64_t cycles;
    uint16_t pc_prev;
    uint32_t count_instructions = 0;

    fprintf(stderr, "mos6502 trace utility\n");

    if ((argc < 2) || (argc > 8)) {
        fprintf(stderr, "usage: trace <binfile> [RSTV [S [P [A [X [Y]]]]]]\n");
        fprintf(stderr, "  binfile: binary file path/name e.g. bin/myfile.bin\n");
        fprintf(stderr, "     RSTV: reset vector (4 digit hex)\n");
        fprintf(stderr, "        S: stack pointer initial value (2 digit hex)\n");
        fprintf(stderr, "        P: processor status register initial value (2 digit hex)\n");
        fprintf(stderr, "        A: accumulator initial value (2 digit hex)\n");
        fprintf(stderr, "        X: X index register initial value (2 digit hex)\n");
        fprintf(stderr, "        Y: Y index register initial value (2 digit hex)\n");
        exit(-1);
    }
    binfile = argv[1];    

    if (!fs::exists(binfile)) {
        fprintf(stderr, "Error: could not find binary file (%s)\n", binfile.c_str());
        exit(-1);
    }
    fprintf(stderr, "binary file: %s\n", argv[1]);

    if (argc >= 3) {
        s = argv[2];
        if ((s.length() != 4) ||
            (s.find_first_not_of("0123456789abcdefABCDEF", 2) != std::string::npos)) {
            fprintf(stderr, "Error: expected 4 digit hex reset vector\n");
            exit(-1);
        }    
        rstvec = stoi(s, 0, 16);
        fprintf(stderr, "reset vector : %04X\n", rstvec);
    }

    if (argc >= 4) {
        s = argv[3];
        if ((s.length() != 2) ||
        (s.find_first_not_of("0123456789abcdefABCDEF", 2) != std::string::npos)) {
            fprintf(stderr, "Error: expected 2 digit hex initial stack pointer value\n");
            exit(-1);            
        }
        v = stoi(s, 0, 16);
        cpu.SetResetS(v);        
    }

    if (argc >= 5) {
        s = argv[4];
        if ((s.length() != 2) ||
        (s.find_first_not_of("0123456789abcdefABCDEF", 2) != std::string::npos)) {
            fprintf(stderr, "Error: expected 2 digit hex initial status register value\n");
            exit(-1);            
        }
        v = stoi(s, 0, 16);
        cpu.SetResetP(v);
    }

    if (argc >= 6) {
        s = argv[5];
        if ((s.length() != 2) ||
        (s.find_first_not_of("0123456789abcdefABCDEF", 2) != std::string::npos)) {
            fprintf(stderr, "Error: expected 2 digit hex initial Accumulator value\n");
            exit(-1);            
        }
        v = stoi(s, 0, 16);
        cpu.SetResetA(v);
    }

    if (argc >= 7) {
        s = argv[6];
        if ((s.length() != 2) ||
        (s.find_first_not_of("0123456789abcdefABCDEF", 2) != std::string::npos)) {
            fprintf(stderr, "Error: expected 2 digit hex initial X register value\n");
            exit(-1);            
        }
        v = stoi(s, 0, 16);
        cpu.SetResetX(v);
    }

    if (argc == 8) {
        s = argv[7];
        if ((s.length() != 2) ||
        (s.find_first_not_of("0123456789abcdefABCDEF", 2) != std::string::npos)) {
            fprintf(stderr, "Error: expected 2 digit hex initial Y register value\n");
            exit(-1);            
        }
        v = stoi(s, 0, 16);
        cpu.SetResetY(v);
    }

    fprintf(stderr, "register values at reset:\n");
    fprintf(stderr, "    stack pointer = %02X\n", cpu.GetResetS());
    fprintf(stderr, "  status register = %02X\n", cpu.GetResetP());        
    fprintf(stderr, "     accumulator  = %02X\n", cpu.GetResetA());        
    fprintf(stderr, "      X register  = %02X\n", cpu.GetResetX());        
    fprintf(stderr, "      Y register  = %02X\n", cpu.GetResetY());        

    std::ifstream f(binfile, ios::in | ios::binary );
    f.read((char*)mem, sizeof(mem));
    f.close();

    MemWr(0xFFFC, rstvec & 0xFF);
    MemWr(0xFFFD, rstvec >> 8);
    cpu.Reset();
    pc_prev = 0;

    while(cpu.GetPC() != pc_prev) {
        pc_prev = cpu.GetPC();
        fprintf(stdout, "%04X %02X %02X %02X %02X %02X\n",
            cpu.GetPC(), cpu.GetS(), cpu.GetP(), cpu.GetA(), cpu.GetX(), cpu.GetY() );
        cpu.Run(1, cycles, cpu.CYCLE_COUNT);
        count_instructions++;
        if ((count_instructions % 1000000) == 0)
            fprintf(stderr, "%d\n", count_instructions);
    }
    fprintf(stdout, "%04X %02X %02X %02X %02X %02X\n",
        cpu.GetPC(), cpu.GetS(), cpu.GetP(), cpu.GetA(), cpu.GetX(), cpu.GetY() );
    fprintf(stderr, "%d\n", count_instructions);
    exit(0);
}
