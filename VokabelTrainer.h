//
// Created by frederic on 27.07.26.
//

#ifndef VOKABEL_TRAINER_VOKABELTRAINER_H
#define VOKABEL_TRAINER_VOKABELTRAINER_H

#include <string>
#include <vector>

class VokabelTrainer {
};

struct Vokabel {
    std::string Fremdwort;   // Vorher: Spanisch
    std::string Übersetzung; // Vorher: Deutsch
    std::string Tipp;
    int fach = 1;
    std::string beispielsatz; // Der Satz für die spätere Sprachausgabe
};


void VokabelEingeben(std::vector<Vokabel>& liste);
void VokabelnSpeichern(const std::vector<Vokabel>& liste, const std::string& dateiname);
void VokabelnLaden(std::vector<Vokabel>& liste, const std::string& dateiname);
void VokabelnLernen(std::vector<Vokabel>& liste);
int VokabelnKorrigieren(std::vector<Vokabel>& liste);
void VokabelnFinden(std::vector<Vokabel>& liste);
void SpracheWechselnMenü(std::string& aktuelleSprachDatei, std::vector<Vokabel>& liste );
void datenbankFüllenLassenMenü(const std::string& aktuelleSprachDatei, std::vector<Vokabel>& liste);
void zeigeMenü(std::vector<Vokabel>& vokabelliste, std::string& aktuelleSprachDatei);


#endif //VOKABEL_TRAINER_VOKABELTRAINER_H
