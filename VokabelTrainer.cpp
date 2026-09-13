//
// Created by frederic on 27.07.26.
//
#include "VokabelTrainer.h"
#include <iostream>
#include <fstream>
#include <limits>
#include <string>
#include <vector>
#include <algorithm>
#include <random>
#include <filesystem>


void VokabelnSpeichern(const std::vector<Vokabel>& liste, const std::string& dateiname) {
    // Öffnet die Datei im Schreib-Modus (überschreibt den alten Inhalt)
    std::ofstream datei(dateiname);

    if (datei.is_open()) {
        // Wandert durch jede einzelne Vokabel in der Liste
        for (const auto& v : liste) {
            // Schreibt alle 5 Informationen exakt im neuen Hashtag-Format in die Datei
            // Format: Fremdwort#Übersetzung#Tipp#Fach#Beispielsatz
            datei << v.Fremdwort << "#"
                  << v.Übersetzung << "#"  // Dein echtes Ü!
                  << v.Tipp << "#"
                  << v.fach << "#"
                  << v.beispielsatz << "\n";
        }
        datei.close();
    }
}
void VokabelEingeben(std::vector<Vokabel>& liste, const std::string& aktuelleSprachDatei) {
    Vokabel neueVokabel;

    std::cout << "\n--- Neue Vokabel hinzufügen ---\n";

    std::cout << "Fremdwort eingeben: ";
    std::getline(std::cin, neueVokabel.Fremdwort);

    std::cout << "Deutsche Übersetzung: ";
    std::getline(std::cin, neueVokabel.Übersetzung); // Dein echtes Ü!

    std::cout << "Grammatik-Tipp: ";
    std::getline(std::cin, neueVokabel.Tipp);

    // NEU: Abfrage des Beispielsatzes für das neue 5er-Format
    std::cout << "Einen Beispielsatz eingeben: ";
    std::getline(std::cin, neueVokabel.beispielsatz);

    // Jede neue Vokabel startet im Leitner-System immer automatisch im Fach 1
    neueVokabel.fach = 1;

    // Fügt die neue Vokabel der Liste hinzu und speichert die Datei sofort auf die Festplatte
    liste.push_back(neueVokabel);
    VokabelnSpeichern(liste, aktuelleSprachDatei);

    std::cout << "[Erfolg] Vokabel wurde erfolgreich manuell hinzugefügt!\n";
}

void VokabelnLaden(std::vector<Vokabel>& liste, const std::string& dateiname) {
    // Öffnet die Textdatei im Lese-Modus
    std::ifstream datei (dateiname);

    // Prüft, ob die Datei erfolgreich auf der Festplatte gefunden und geöffnet wurde
    if (datei.is_open()) {
        // Leert die aktuelle Liste im Arbeitsspeicher (RAM), damit alte Daten gelöscht werden
        liste.clear();
        std::string zeile;

        // Liest die Datei Zeile für Zeile bis zum Ende durch
        while (std::getline(datei, zeile)) {
            // Ignoriert komplett leere Zeilen (z.B. am Ende der Datei), um Abstürze zu verhindern
            if (zeile.empty()) continue;

            // Erstellt eine leere Vokabel-Schublade (Struct) für die Daten dieser Zeile
            Vokabel geladeneVokabel;

            // ==================== NEU: FORMAT-SCHUTZ & SICHERHEITSNETZ ====================
            // Wir zählen zuerst, wie viele Raute-Zeichen (#) sich in dieser Zeile befinden
            size_t hashtagCount = 0;
            for (char c : zeile) {
                if (c == '#') hashtagCount++;
            }

            // FALLBACK: Wenn die Zeile alt ist (nur 3 Rauten / 4 Infos), fehlt der Beispielsatz!
            // Wir hängen künstlich einen leeren Beispielsatz an, damit die 5er-Logik nicht kollabiert.
            if (hashtagCount == 3) {
                zeile += "#Kein Beispielsatz vorhanden.";
            }
            // ==============================================================================


            // --- 1. SCHNITT: BEISPIELSATZ ABSCHNEIDEN ---
            // Sucht nach der ALLERLETZTEN Raute in der gesamten Zeile
            size_t satzHashtag = zeile.rfind('#');
            if (satzHashtag != std::string::npos) {
                // Holt alles RECHTS von der Raute heraus und speichert es als Beispielsatz
                geladeneVokabel.beispielsatz = zeile.substr(satzHashtag + 1);
                // Schneidet den Beispielsatz samt Raute von der Zeile ab, um sie zu verkürzen
                zeile = zeile.substr(0, satzHashtag);
            }


            // --- 2. SCHNITT: LEITNER-FACH ABSCHNEIDEN ---
            // Da der Satz weg ist, steht das Fach (die Zahl) nun ganz hinten. Sucht die letzte Raute:
            size_t fachHashtag = zeile.rfind('#');
            if (fachHashtag != std::string::npos) {
                // Holt den Text rechts von der Raute (z.B. "1")
                std::string fachText = zeile.substr(fachHashtag + 1);
                try {
                    // Wandelt den Text mit "stoi" (String-to-Integer) in eine echte Ganzzahl um
                    geladeneVokabel.fach = std::stoi(fachText);
                } catch (...) {
                    // Absturz-Schutz: Falls dort keine Zahl stand, wird das Fach sicher auf 1 gesetzt
                    geladeneVokabel.fach = 1;
                }
                // Verkürzt die Zeile wieder, indem das Fach samt Raute abgeschnitten wird
                zeile = zeile.substr(0, fachHashtag);
            }


            // --- 3. SCHNITT: GRAMMATIK-TIPP ABSCHNEIDEN ---
            // Jetzt steht der Tipp ganz hinten. Sucht die verbleibende letzte Raute:
            size_t tippHashtag = zeile.rfind('#');
            if (tippHashtag != std::string::npos) {
                // Speichert den Text rechts von der Raute als Grammatik-Tipp ab
                geladeneVokabel.Tipp = zeile.substr(tippHashtag + 1);
                // Verkürzt die Zeile ein weiteres Mal um den Tipp
                zeile = zeile.substr(0, tippHashtag);
            }


            // --- 4. SCHNITT: ÜBERSETZUNG & FREMDWORT TRENNEN ---
            // Es sind nur noch zwei Dinge übrig: Fremdwort#Übersetzung. Sucht die allerletzte Raute:
            size_t deHashtag = zeile.rfind('#');
            if (deHashtag != std::string::npos) {
                // Holt den Text rechts von der Raute heraus -> Das ist die deutsche Übersetzung
                geladeneVokabel.Übersetzung = zeile.substr(deHashtag + 1);

                // Das, was jetzt noch ganz links vor der letzten Raute übrig bleibt (Position 0 bis Raute),
                // ist das reine Fremdwort!
                geladeneVokabel.Fremdwort = zeile.substr(0, deHashtag);
            }


            // Schiebt die fertig befüllte Vokabel-Schublade hinten in deine Liste (Vector)
            liste.push_back(geladeneVokabel);
        }
        // Schließt die Datei ordnungsgemäß, da wir mit dem Lesen fertig sind
        datei.close();
        // Gibt eine Erfolgsmeldung im Terminal aus, wie viele Wörter geladen wurden
        std::cout << "[Info] " << liste.size() << " Vokabeln erfolgreich geladen.\n";
    } else {
        // Falls die Datei noch gar nicht existiert (z.B. bei einer brandneuen Sprache)
        std::cout << "[Info] Keine Vokabeln gefunden. Es wird eine neue erstellt, sobald Sie Vokabeln speichern.\n";
    }
}

void VokabelnLernen(std::vector<Vokabel>& vokabelliste, const std::string& aktuelleSprachDatei) {
    if (vokabelliste.empty()) {
        std::cout << "[Info] Es sind keine Vokabeln vorhanden. Bitte tragen Sie erst Vokabeln ein (Option 1).\n";
        return;
    }

    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    int abgefragteVokabeln = 0;

    // Zufallsgenerator vorbereiten, um die Vokabeln gut durchzumischen
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(vokabelliste.begin(), vokabelliste.end(), g);

    int richtigeAntworten = 0;
    std::cout << "\n--- Vokabeltest startet ---\n";

    int welchesFach = 1;
    std::cout << "Welches Fach möchten Sie lernen? (1-6): ";
    std::cin >> welchesFach;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    // Wandert durch die gemischte Liste
    for (size_t i = 0; i < vokabelliste.size(); i++) {
        Vokabel& v = vokabelliste[i];

        // Filtert stur alle Vokabeln heraus, die nicht im gewählten Leitner-Fach liegen
        if (v.fach != welchesFach) {
            continue;
        }

        abgefragteVokabeln++;
        std::string userEingabe;

        while (true) {
            // Deine universelle, sprachunabhängige Frage!
            std::cout << "\nWas bedeutet das Wort: " << v.Fremdwort << "?\n";
            std::cout << "Deine Antwort (Deutsch) [Oder geben Sie ? ein für einen Tipp]: ";
            std::getline(std::cin, userEingabe);

            if (userEingabe == "?") {
                std::cout << "\n [? Hilfe ?]\n";
                std::cout << "Bedeutung/Definition: " << v.Tipp << "\n";
                continue;
            }
            break;
        }

        // Überprüfung der Eingabe mit deinem echten Ü!
        if (userEingabe == v.Übersetzung) {
            std::cout << "Richtig! Gut gemacht!\n";

            // NEU: Beispielsatz als Erfolgskontext einblenden, falls vorhanden
            if (!v.beispielsatz.empty()) {
                std::cout << "Beispielsatz: " << v.beispielsatz << "\n";
            }

            richtigeAntworten++;
            // Vokabel steigt im Leitner-System ein Fach nach oben (maximal bis Fach 6)
            if (vokabelliste[i].fach < 6) {
                vokabelliste[i].fach++;
            }
        } else {
            std::cout << "Leider falsch. Die richtige Antwort ist: " << v.Übersetzung << "\n";
            // Fehler-Strafe: Vokabel fällt gnadenlos zurück in Fach 1
            vokabelliste[i].fach = 1;
        }
    }

    std::cout << "\n--- Vokabeltest beendet ---\n";

    // Prüft, ob das gewählte Karteifach komplett leer war
    if (abgefragteVokabeln == 0) {
        std::cout << "[Info] In Fach " << welchesFach << " befinden sich aktuell keine Vokabeln!\n";
    } else {
        std::cout << "Du hast " << richtigeAntworten << " von " << abgefragteVokabeln << " Vokabeln gewusst!\n";
        // Speichert die aktualisierten Leitner-Fächer felsenfest auf die Festplatte
        VokabelnSpeichern(vokabelliste, aktuelleSprachDatei);
    }
}


int VokabelnKorrigieren(std::vector<Vokabel>& liste, int gewählterIndex, const std::string& aktuelleSprachDatei) {
    int korrekturWahl = 0;
    bool wurdeGeändert = false;

    if (gewählterIndex < liste.size()) {
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        do {
            std::cout << "\n --- Eintrag Bearbeiten ---\n";
            // Allgemeingültig beschriftet:
            std::cout << "1. Fremdwort   : " << liste[gewählterIndex].Fremdwort << "\n";
            std::cout << "2. Übersetzung : " << liste[gewählterIndex].Übersetzung << "\n";
            std::cout << "3. Tipp         : " << liste[gewählterIndex].Tipp << "\n";
            std::cout << "4. Beispielsatz : " << liste[gewählterIndex].beispielsatz << "\n\n"; // NEU
            std::cout << "5. Bearbeitung speichern/abbrechen und zurück zum Hauptmenü.\n";  // Nach hinten gerutscht
            std::cout << "6. Bearbeitung speichern/abbrechen und zurück zum Verzeichnis.\n"; // Nach hinten gerutscht
            std::cout << "Deine Wahl: ";
            std::cin >> korrekturWahl;
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

            switch (korrekturWahl) {
                case 1:
                    std::cout << "Neues Fremdwort (aktuell: " << liste[gewählterIndex].Fremdwort << "):\n";
                    std::getline(std::cin, liste[gewählterIndex].Fremdwort);
                    std::cout << "[Info] Fremdwort aktualisiert!\n";
                    wurdeGeändert = true;
                    break;
                case 2:
                    std::cout << "Neue Übersetzung (aktuell: " << liste[gewählterIndex].Übersetzung << "):\n";
                    std::getline(std::cin, liste[gewählterIndex].Übersetzung);
                    std::cout << "[Info] Übersetzung aktualisiert!\n";
                    wurdeGeändert = true;
                    break;
                case 3:
                    std::cout << "Neuer Tipp (aktuell: " << liste[gewählterIndex].Tipp << "):\n";
                    std::getline(std::cin, liste[gewählterIndex].Tipp);
                    std::cout << "[Info] Tipp aktualisiert!\n";
                    wurdeGeändert = true;
                    break;
                case 4: // NEU: Beispielsatz bearbeiten
                    std::cout << "Neuer Beispielsatz (aktuell: " << liste[gewählterIndex].beispielsatz << "):\n";
                    std::getline(std::cin, liste[gewählterIndex].beispielsatz);
                    std::cout << "[Info] Beispielsatz aktualisiert!\n";
                    wurdeGeändert = true;
                    break;
                case 5: // Vorher case 4
                    if (wurdeGeändert) {
                        VokabelnSpeichern(liste, aktuelleSprachDatei);
                    }
                    return 4; // Behält deine Menü-Logik bei
                case 6: // Vorher case 5
                    if (wurdeGeändert) {
                        VokabelnSpeichern(liste, aktuelleSprachDatei);
                    }
                    return 5; // Behält deine Verzeichnis-Logik bei
                default:
                    std::cout << "[Fehler] Ungültige Wahl!\n";
                    break;
            }

            if (wurdeGeändert) {
                VokabelnSpeichern(liste, aktuelleSprachDatei);
            }

            // WICHTIG: Das automatische "return 0;" an dieser Stelle entfernen,
            // sonst würde die Schleife nach JEDER Änderung sofort abbrechen!

        } while (korrekturWahl != 5 && korrekturWahl != 6); // Wartet jetzt auf 5 oder 6
    }
    return 0;
}




void VokabelnFinden(std::vector<Vokabel>& liste, const std::string& aktuelleSprachDatei) {
    // Falls die Liste leer ist, wird die Funktion sofort abgebrochen (Sicherheitsnetz)
    if (liste.empty()) {
        std::cout << "[Info] Keine Vokabeln zum Korrigieren vorhanden! Bitte tragen Sie erst Vokabeln ein (Option 1).\n";
        return;
    }

    // Zeigt das Such-Untermenü auf dem Bildschirm an
    std::cout << "\n--- Wie möchten Sie die Vokabel finden? ---\n";
    std::cout << "1. Suchen (Gezielte Wortsuche)\n";
    std::cout << "2. Blättern im Verzeichnis (Seitenweise Ansicht)\n";
    std::cout << "Deine Wahl: ";

    int suchWahl;
    std::cin >> suchWahl;
    // Löscht verbleibende Reste und das Enter-Zeichen aus dem Eingabekanal, um getline-Abstürze zu verhindern
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    // Start-Wert für den Index. 999999 dient als unmöglicher Platzhalter (Signal für "noch nichts gewählt")
    size_t gewählterIndex = 999999;

    // ==================== MODUS 1: GEZIELTE WORTSUCHE ====================
    if (suchWahl == 1) {
        std::cout << "Geben Sie das gesuchte Wort (oder einen Teil davon) ein: ";
        std::string suchBegriff;
        std::getline(std::cin, suchBegriff);

        std::cout << "\nGefundene Treffer: \n";
        // Dieser Hilfs-Vector speichert die echten Speicher-Positionen der gefundenen Vokabeln aus der Hauptliste
        std::vector<size_t> trefferIndices;

        // Durchläuft die gesamte Vokabelliste von vorne bis hinten
        for (size_t i = 0; i < liste.size(); i++) {
            // Prüft mit "find", ob der Suchbegriff im Fremdwort ODER in der Übersetzung (mit Ü!) existiert
            if (liste[i].Fremdwort.find(suchBegriff) != std::string::npos ||
                liste[i].Übersetzung.find(suchBegriff) != std::string::npos) {

                // Gibt den Treffer formatiert aus (Nummerierung startet bei 1 statt 0)
                std::cout << trefferIndices.size() + 1 << ". " << liste[i].Fremdwort << " # " << liste[i].Übersetzung << "\n";
                // Merkt sich die originale Position (i) dieser Vokabel im Treffer-Vector
                trefferIndices.push_back(i);
            }
        }

        // Falls die Schleife keinen einzigen Treffer registriert hat, bricht die Suche hier ab
        if (trefferIndices.empty()) {
            std::cout << "[Info] Kein Wort mit diesem Suchbegriff gefunden.\n";
            return;
        }

        // Endlosschleife für die Nummern-Abfrage der Trefferliste, bis eine gültige Wahl getroffen wird
        while (true) {
            std::cout << "\nWelche Nummer aus den Treffern möchten Sie korrigieren? (oder 0 für Abbrechen): ";
            std::string eingabeText;
            std::cin >> eingabeText;

            // Bricht die Suche manuell ab und leitet den Nutzer sauber zurück
            if (eingabeText == "0") {
                std::cout << "[Info] Suche abgebrochen. Zurück zum Hauptmenü.\n";
                return;
            }

            try {
                // Konvertiert den eingegebenen Text sturzsicher in eine Ganzzahl (String-to-Integer)
                int nummer = std::stoi(eingabeText);

                // Prüft, ob die eingegebene Nummer im gültigen Bereich der Trefferliste liegt
                if (nummer >= 1 && static_cast<size_t>(nummer) <= trefferIndices.size()) {
                    // Übersetzt die Listennummer zurück in den originalen Index der Hauptliste
                    gewählterIndex = trefferIndices[nummer - 1];

                    // Ruft deine Korrektur-Funktion mit dem gefundenen Index auf und fängt die Aktion ab
                    int aktion = VokabelnKorrigieren(liste, gewählterIndex, aktuelleSprachDatei);

                    // Wenn der Nutzer in der Korrektur Option 5 gedrückt hat -> Sofortiger Rücksprung ins Hauptmenü
                    if (aktion == 4) {
                        return;
                    }
                    // Wenn er Option 6 gedrückt hat (zurück zum Verzeichnis), bricht die Schleife hier ab
                    // und die Funktion beendet sich regulär.
                    break;
                }
                else {
                    std::cout << "[Fehler] Diese Nummer existiert nicht in den Treffern. Bitte versuchen Sie es erneut!\n";
                }
            }
            catch (...) {
                // Fängt Tippfehler (z.B. Buchstaben statt Zahlen) ab, ohne dass C++ abstürzt
                std::cout << "[Fehler] Ungültige Eingabe! Bitte geben Sie eine Nummer ein.\n";
            }
        }
    }

    // ==================== MODUS 2: SEITENWEISE BLÄTTERN ====================
    else if (suchWahl == 2) {
        size_t start = 0;       // Startpunkt für die aktuelle Verzeichnis-Seite
        size_t seitenGröße = 20; // Zeigt maximal 20 Vokabeln pro Seite an

        // Endlosschleife für das Blätter-System, läuft bis der Nutzer explizit abbricht
        while (true) {
            // Berechnet die aktuelle Seitennummer mathematisch aus dem Startpunkt (z.B. 0/20 + 1 = Seite 1)
            std::cout << "\n--- Vokabel Liste (Seite " << (start / seitenGröße) + 1 << ") ---\n";

            // Verhindert ein Überlaufen der Liste am Ende (wählt das Minimum aus Listenende oder Seitenende)
            size_t ende = std::min(start + seitenGröße, liste.size());
            // Gibt die Vokabeln der aktuellen Seite auf dem Bildschirm aus
            for (size_t i = start; i < ende; ++i) {
                std::cout << i + 1 << ". " << liste[i].Fremdwort << " # " << liste[i].Übersetzung << "\n";
            }

            std::cout << "\n[Optionen]: Tippen Sie die Zahl zum Korrigieren, 'w' für weiter, 'z' für zurück, 'a' für Abbrechen ein: ";
            std::string eingabe;
            std::cin >> eingabe;

            // Blättert eine Seite nach vorne, falls noch Wörter übrig sind
            if (eingabe == "w") {
                if (start + seitenGröße < liste.size()) start += seitenGröße;
                else std::cout << "[Info] Sie sind schon auf der letzten Seite.\n";
            }
            // Blättert eine Seite nach hinten, falls wir nicht bereits auf Seite 1 sind
            else if (eingabe == "z") {
                if (start >= seitenGröße) start -= seitenGröße;
                else std::cout << "[Info] Sie sind schon auf der ersten Seite.\n";
            }
            // Verlässt den Blätter-Modus und kehrt zum Menü zurück
            else if (eingabe == "a") {
                return;
            }
            // Nutzer hat eine Zahl eingetippt, um diese spezifische Vokabel direkt zu korrigieren
            else {
                try {
                    int nummer = std::stoi(eingabe);
                    // Validiert, ob die eingegebene Vokabelnummer in der gesamten Liste existiert
                    if (nummer >= 1 && static_cast<size_t>(nummer) <= liste.size()) {
                        gewählterIndex = nummer - 1; // Umrechnung in C++ Index (0-basiert)

                        // Ruft die Korrektur auf und fängt die Aktion für das Menü-Routing ab
                        int aktion = VokabelnKorrigieren(liste, gewählterIndex, aktuelleSprachDatei);
                        // Wenn Aktion == 4 (Zurück zum Hauptmenü), beendet sich diese Funktion sofort
                        if (aktion == 4) {
                            return;
                        }
                        // Wenn Aktion == 5 (Zurück zum Verzeichnis), läuft die while-Schleife einfach weiter
                        // und der Nutzer sieht direkt wieder sein aktualisiertes Blätter-Verzeichnis!
                    }
                } catch (...) {
                    std::cout << "[Fehler] Ungültige Eingabe!\n";
                }
            }
        }
    }
    // Fängt fehlerhafte Eingaben im allerersten Such-Untermenü ab
    else {
        std::cout << "[Fehler] Ungültige Option gewählt!\n";
        return;
    }
}
// ==============================================================================
// 1. FUNKTION: SPRACHEN WECHSELN ODER NEUE ANLEGEN
// ==============================================================================
void SpracheWechselnMenü(std::string& aktuelleSprachDatei, std::vector<Vokabel>& liste) {
    namespace fs = std::filesystem;
    // Dieser Vector speichert die Namen aller gefundenen Textdateien (z.B. "Spanisch.txt")
    std::vector<std::string> gefundeneDateien;

    std::cout << "\n--- Sprachmenü ---\n";
    std::cout << "Verfügbare Sprachen im Ordner :\n";

    int index = 1;

    // Wandert durch den aktuellen Ausführungsordner des Programms
    for (const auto& entry : fs::directory_iterator(fs::current_path())) {
        // Findet normale Dateien, die auf ".txt" enden und ignoriert den CMakeCache-Müll
        if (entry.is_regular_file() && entry.path().extension() == ".txt" &&
            entry.path().stem().string() != "CMakeCache") {

            // Holt den vollständigen Dateinamen (z.B. "Englisch.txt")
            std::string dateiName = entry.path().filename().string();
            // Holt den reinen Sprachnamen ohne Endung (z.B. "Englisch")
            std::string anzeigeName = entry.path().stem().string();

            // Zeigt die Sprache nummeriert auf dem Bildschirm an
            std::cout << index << ". " << anzeigeName << "\n";
            // Schiebt den Dateinamen in unseren Zwischenspeicher-Vector
            gefundeneDateien.push_back(dateiName);
            index++; // Erhöht die Menü-Nummerierung für die nächste gefundene Datei
        }
    }

    // Der letzte Menüpunkt wird dynamisch für das Hinzufügen reserviert
    std::cout << index << ". Neue Sprache hinzufügen\n";
    std::cout << "Wähle eine Option: ";
    int wahl;
    std::cin >> wahl;

    // FALL A: Der Nutzer wählt eine bereits existierende Sprache aus der Liste
    if (wahl >= 1 && wahl <= gefundeneDateien.size()) {
        // Setzt die globale Variable auf die gewählte Datei (Index ist 0-basiert, daher wahl - 1)
        aktuelleSprachDatei = gefundeneDateien[wahl - 1];
        // Schneidet das ".txt" für die Erfolgsmeldung auf dem Bildschirm ab
        std::cout << "Sprache gewechselt zu: " << aktuelleSprachDatei.substr(0, aktuelleSprachDatei.find('.')) << "\n";

        liste.clear(); // Leert den aktuellen RAM-Speicher vor dem Laden
        VokabelnLaden(liste, aktuelleSprachDatei); // Lädt die Vokabeln der neuen Sprache
    }
    // FALL B: Der Nutzer wählt die Nummer, um eine komplett neue Sprache anzulegen
    else if (wahl == index) {
        std::string neueSprache;
        std::cout << "Wie heißt die neue Sprache?: ";
        std::cin >> neueSprache;

        // Erstellt den neuen Dateinamen (z.B. "Polnisch.txt")
        std::string neuerDateiName = neueSprache + ".txt";
        // Erzeugt die physische Datei auf der Festplatte (wird sofort wieder geschlossen, bleibt also leer)
        std::ofstream neueDatei(neuerDateiName);
        neueDatei.close();

        std::cout << "Datei " << neuerDateiName << " wurde erfolgreich erstellt!\n";

        // Setzt die neu erstellte Sprache sofort als aktive Datei
        aktuelleSprachDatei = neuerDateiName;
        liste.clear(); // Die neue Sprache startet im Arbeitsspeicher logischerweise mit 0 Vokabeln

        std::cout << "Sprache wurde auf die neue, leere Datei gewechselt.\n";
    }
}

// ==============================================================================
// 2. FUNKTION: DEN PYTHON-GENERATOR STARTEN (KI-BEFÜLLUNG)
// ==============================================================================
void datenbankFüllenLassenMenü(const std::string& aktuelleSprachDatei, std::vector<Vokabel>& liste) {
    std::cout << "\n--- KI-Datenbank befüllen ---\n";
    std::cout << "Wie viele Vokabeln möchten Sie generieren lassen? (z.B. 25/50/100): ";
    int anzahl;
    std::cin >> anzahl;

    // Putzt den Eingabekanal komplett leer, um spätere getline-Abstürze zu verhindern
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cout << "[Info] Verbinde mit KI-Server... Bitte einen Moment Geduld.\n";

    // Holt den reinen Sprachnamen aus dem Dateinamen (z.B. "Englisch.txt" -> "Englisch")
    std::string sprache = aktuelleSprachDatei.substr(0, aktuelleSprachDatei.find('.'));

    // DEIN DYNAMISCHES SCHLEIFEN-SYSTEM: Ruft das Skript auf und übergibt starr nur Sprache und Anzahl
    // Ausgeführt wird z.B.: python3 ../vokabel_api.py Englisch 25
    std::string befehl = "python3 ../vokabel_api.py " + sprache + " " + std::to_string(anzahl);

    // Führt den Befehl im Terminal deines Betriebssystems aus und wartet auf das Ende von Python
    int status = std::system(befehl.c_str());

    // Wenn Python fehlerfrei mit dem Exit-Code 0 beendet wurde
    if (status == 0) {
        std::cout << "\n[Erfolg] Die KI hat die Datenbank erfolgreich befüllt!\n";
        liste.clear(); // Bereinigt den RAM, bevor die neuen Daten eingelesen werden
        VokabelnLaden(liste, aktuelleSprachDatei); // Liest die frisch generierte Datei sofort ein
    } else {
        // Falls Python abstürzt oder nicht im System registriert ist
        std::cout << "\n[Fehler] KI - Generierung fehlgeschlagen. Ist Python im System-Pfad?\n";
    }
}

// ==============================================================================
// 3. FUNKTION: DAS HAUTPMENÜ (DASHBOARD)
// ==============================================================================
void zeigeMenü(std::vector<Vokabel>& vokabelliste, std::string& aktuelleSprachDatei) {
    int auswahl = 0;

    // Die Do-While-Schleife läuft so lange, bis der Nutzer explizit die Option 6 wählt
    do {
        std::cout << "\n--- Hauptmenü ---\n";
        // Holt den reinen Namen der aktiven Sprache für das Dashboard
        std::string anzeigeName = aktuelleSprachDatei.substr(0, aktuelleSprachDatei.find('.'));
        std::cout << "Aktive Sprache :" << anzeigeName << "\n";
        // Zeigt live im Menü an, wie viele Wörter aktuell im RAM geladen sind
        std::cout << "Aktuelle Vokabeln im System:" << vokabelliste.size() << "\n\n";
        std::cout << "1. Vokabeln eingeben\n";
        std::cout << "2. Vokabeln lernen\n";
        std::cout << "3. Vokabeln korrigieren\n";
        std::cout << "4. Sprachen wechseln\n";
        std::cout << "5. Datenbank Füllen lassen (KI)\n";
        std::cout << "6. Beenden\n";
        std::cout << "Deine Wahl: ";

        std::cin >> auswahl;

        // Verzweigt das Programm basierend auf der Nutzereingabe in die passende Unterfunktion
        switch (auswahl) {
            case 1:
                VokabelEingeben(vokabelliste, aktuelleSprachDatei);
                break;
            case 2:
                VokabelnLernen(vokabelliste, aktuelleSprachDatei);
                break;
            case 3:
                VokabelnFinden(vokabelliste, aktuelleSprachDatei); // Ruft die Such- und Verzeichnislogik auf
                break;
            case 4:
                SpracheWechselnMenü(aktuelleSprachDatei, vokabelliste);
                break;
            case 5:
                datenbankFüllenLassenMenü(aktuelleSprachDatei, vokabelliste);
                break;
            case 6:
                std::cout << "[INFO] Du hast `Beenden` gewählt.\n";
                break;
            default:
                // Fängt falsche Zahlen oder ungültige Zeichen im Hauptmenü ab
                std::cout << "[Fehler] Ungültige Eingabe! Bitte erneut versuchen.\n";
                break;
        }
    } while (auswahl != 6); // Bedingung: Wenn auswahl == 6 wird die Schleife beendet
}
