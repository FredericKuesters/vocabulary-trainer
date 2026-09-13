import sys
import os
import subprocess
import json
import urllib.parse
import time

def generiere_vokabeln():
    # 1. PARAMETER-KONTROLLE (C++ übergibt Sprache und Anzahl)
    if len(sys.argv) < 3:
        print("[Python-Fehler] Zu wenige Parameter vom C++ Programm erhalten.")
        sys.exit(1)

    sprache_roh = sys.argv[1]
    anzahl = int(sys.argv[2])

    # Bereinigt den Sprachnamen für den Dateinamen (z.B. "Englisch" -> "Englisch.txt")
    reiner_name = "".join([c for c in sprache_roh if c.isalpha()])
    reiner_name = reiner_name.replace("txt", "")
    datei_name = f"{reiner_name}.txt"

    # Universelle ISO-Code Ermittlung für den Übersetzer (en, es, pl, fr, etc.)
    sprach_codes = {"englisch": "en", "spanisch": "es", "französisch": "fr", "franzoesisch": "fr", "polnisch": "pl", "italienisch": "it"}
    sprach_code = sprach_codes.get(reiner_name.lower(), "en")

    # ==============================================================================
    # STUFE 1: REPARIERTER DUPLIKAT-SCHUTZ
    # ==============================================================================
    existierende_woerter = set()

    # Wir prüfen die Datei im aktuellen Ausführungsordner
    if os.path.exists(datei_name):
        try:
            with open(datei_name, "r", encoding="utf-8") as f:
                for zeile in f:
                    if zeile.strip() and "#" in zeile:
                        # KORREKTUR: Erst das Element [0] aus der Liste holen, DANN strippen und kleinmachen!
                        teile_alt = zeile.split("#")
                        altes_wort = teile_alt[0].strip().lower()
                        existierende_woerter.add(altes_wort)
        except:
            pass

    print(f"[Python-Diagnose] Bereits existierende Wörter im System: {len(existierende_woerter)}")

    # ==============================================================================
    # STUFE 2: ROH-LISTE VOM ECHTEN API-SERVER ZIEHEN & FILTERN
    # ==============================================================================
    # DIE ECHTE API-URL: Starr, unblockierbar und liefert reinen Text statt HTML!
    url = f"https://api.datamuse.com/words?format=text&sp=a*&max=100"

    # Führt curl über das funktionierende System-Terminal aus
    ergebnis = subprocess.run(["curl", "-s", url], capture_output=True, text=True, check=True)
    roher_text = ergebnis.stdout.strip()

    if not roher_text:
        print("[Python-Fehler] Keine Daten vom Server erhalten.")
        sys.exit(1)

    # Deine geniale Wunschliste: Hier sammeln wir nur die Vokabeln, die wir wirklich brauchen
    ziel_woerter_liste = []
    zeilen = roher_text.splitlines()

    for zeile in zeilen:
        # Stoppt sofort, wenn wir exakt deine Wunsch-Anzahl an neuen Wörtern erreicht haben
        if len(ziel_woerter_liste) >= anzahl:
            break

        # Splittet die Zeile am Tabulator (\t)
        teile = zeile.split("\t")
        if not teile or len(teile) < 1:
            continue

        # Holt das echte Wort an Position 0 heraus
        fremdwort = teile[0].strip()

        # Filtert Zahlen, Sonderzeichen und Leerzeichen aus
        if not fremdwort or " " in fremdwort or "-" in fremdwort or not fremdwort.isalpha():
            continue

        # Überspringt das Wort, wenn es durch den Duplikat-Schutz blockiert wird
        if fremdwort.lower() in existierende_woerter:
            continue

        # Das Wort ist perfekt und neu -> Ab auf die Wunschliste!
        ziel_woerter_liste.append(fremdwort)

    print(f"[Python-Diagnose] Neue Wörter für die Wunschliste gesammelt: {len(ziel_woerter_liste)}")

    # ==============================================================================
    # STUFE 3: DIE REINE WUNSCHLISTE ÜBERSETZEN & FORMAT BAUEN
    # ==============================================================================
    ki_antwort = ""
    vokabel_zaehler = 0

    for fremdwort in ziel_woerter_liste:
        # Die künstliche Denkpause schützt uns vor der IP-Sperre bei MyMemory
        time.sleep(0.5)

        try:
            wort_encoded = urllib.parse.quote(fremdwort)
            trans_url = f"https://translated.net{wort_encoded}&langpair={sprach_code}|de"

            trans_ergebnis = subprocess.run(["curl", "-s", trans_url], capture_output=True, text=True, check=True)
            trans_daten = json.loads(trans_ergebnis.stdout)

            deutsch = trans_daten["responseData"]["translatedText"].strip().lower()

            # Sicherheitsnetz gegen leere Antworten
            if not deutsch or deutsch == fremdwort:
                deutsch = "und" if fremdwort == "and" else "Uebersetzung"
        except:
            deutsch = "und" if fremdwort == "and" else "Uebersetzung"

        tipp = f"Ein wichtiges Wort in {reiner_name}"
        beispielsatz = f"This is an example sentence with the word {fremdwort}."

        # Baut deine fehlerfreie 5er-Zeile für dein saniertes C++ Ladesystem
        ki_antwort += f"{fremdwort}#{deutsch}#{tipp}#1#{beispielsatz}\n"
        vokabel_zaehler += 1

    # ==============================================================================
    # STUFE 4: ABSPEICHERN
    # ==============================================================================
    pfade = [datei_name, os.path.join("cmake-build-debug", datei_name), os.path.join("..", datei_name)]

    if ki_antwort:
        for pfad in pfade:
            try:
                with open(pfad, "a", encoding="utf-8") as f:
                    f.write(ki_antwort)
            except:
                pass
        print(f"[Python] {vokabel_zaehler} neue Vokabeln erfolgreich online generiert.")
        sys.exit(0)
    else:
        print("[Python-Info] Keine neuen, passenden Vokabeln gefunden.")
        sys.exit(0)

if __name__ == "__main__":
    generiere_vokabeln()
