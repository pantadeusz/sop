/**
 * @file hello.c
 * @author Tadeusz Puźniakowski 
 * @brief Klasyczny Witaj w Świecie
 * @version 0.1
 * @date 2022-04-03
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#include <stdio.h>

#ifndef TEKST
#define TEKST "Witajmy w Świecie!\n"
#endif

// Funkcja od ktorej zaczynamy
int main (int /* to jest liczba argumentow programu */ argc, char **argv) {
    printf(TEKST);
    return 0;
}
