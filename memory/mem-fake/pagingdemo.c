/**
 * @file pagingdemo.c
 * @author Tadeusz Puźniakowski (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2022-05-15
 *
 * @copyright Copyright (c) 2022
 *
 * MIT License
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/// nie mozna zmienic, bo na stale zalozylem ze mamy po 4 bity na numer strony i offset
#define FRAME_SIZE 16

// z punktu widzenia systemu
unsigned char physical_memory[16*FRAME_SIZE]; // 256 bajtów pamięci fizycznej
unsigned char frame_table[16] = {0,0,1,0,1,0,1,1,1,1,0,0,0,0,0,0}; // ktore ramki sa zajete - jesli > 0 to numer "procesu"

// z punktu widzenia procesu - tu pokazujemy jakby 1 proces
// proces moze zarezerwowac maksymalnie polowe pamieci
unsigned char page_table_size = 0;
unsigned char page_table[8] = {0,0,0,0,0,0,0,0}; // 64 = 1000000b
unsigned char page_table_reserved[8] = {0,0,0,0,0,0,0,0}; // 64 = 1000000b

/// struktura adresu: ssssoooo - adres logiczny, gdzie polowa to segment, polowa to offset

unsigned char get_memory(unsigned char logical_address) {
    return physical_memory[(page_table[logical_address >> 4] << 4) + (logical_address & 0x0f)];
}
void set_memory(unsigned char logical_address, unsigned char value) {
    physical_memory[(page_table[logical_address >> 4] << 4) + (logical_address & 0x0f)] = value;
}

/// uwaga - tu jest jakis blad...
unsigned char alloc_memory(unsigned char size) {
    unsigned char test_size = size;
    for (int i =1; i < 16; i++) {
        if (frame_table[i] == 0) {
            if (test_size < FRAME_SIZE) {
                test_size = 0;
                break;
            }
            test_size -= FRAME_SIZE; //
        }
    }
    if (test_size) return 0; // error - nie ma tyle pamieci

    unsigned char ret_addr = 0;

    size += FRAME_SIZE;
    for (int i = 1; (i < 16) && (size > 0); i++) {
        if (frame_table[i] == 0) {
            if (ret_addr == 0) ret_addr = i << 4;
            frame_table[i] = 1; /// przypisujemy proces 1
            page_table_reserved[page_table_size] = 1; // zapisujemy ze zarezerwowalismy
            page_table[page_table_size++] = i; // ustawiamy odpowiedni adres strony
            if (page_table_size > 8) {
                return 0; // blad allokacji}
            }
            if (size < FRAME_SIZE) break;
            size -= FRAME_SIZE; //
        }
    }
    return ret_addr;
}

void dump_all() {
    for (int i = 0 ; i < 16; i++) {
        printf("MEM: %3d (%3d):", i, frame_table[i]);
        for (int j = 0; j < 16; j++) {
            unsigned char val = physical_memory[(i<<4)+j];
            if ((val < 16) || (val > 127)) val = '.';
            printf("%c", (val));
        }
        printf("\n");
    }
    printf("\n");

    for (int i = 0 ; i < 8; i++) {
        printf("PAGE: %3d (%3d): %d", i, page_table_reserved[i],page_table[i]);
        printf("\n");
    }
    printf("\n");

    printf("Logic view: \n");
    for (int i = 0 ; i < 256; i++) {
        unsigned char val = get_memory(i);
        if ((val < 16) || (val > 127)) val = '.';
        printf("%c", (val));
    }
    printf("\n");
}


int main (int argc, char *argv[]) {

    unsigned char test_text[] = "testowy tekst do sprawdzenia czy ladnie dziala.";

    dump_all();

    unsigned char myaddr0 = alloc_memory(strlen(test_text));

    printf("addr: 0x0%x\n", myaddr0);

    for (int i = 0; i < strlen(test_text); i++) set_memory(myaddr0 + i,test_text[i]);

    dump_all();


    return 0;
}