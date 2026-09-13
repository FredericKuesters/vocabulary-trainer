#include <iostream>
#include <vector>

#include "VokabelTrainer.h"


int main() {
    std::string aktuelleSprachDatei = "Spanisch.txt";
    std::vector<Vokabel> meineVokabeln;
    VokabelnLaden(meineVokabeln, aktuelleSprachDatei);
    zeigeMenü(meineVokabeln, aktuelleSprachDatei);
    return 0;
}
