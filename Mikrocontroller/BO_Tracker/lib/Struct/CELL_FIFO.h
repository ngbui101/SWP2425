#ifndef CELL_FIFO_H
#define CELL_FIFO_H

#include "Cell.h"

#define MAX_SIZE 6  // Maximale Größe des FIFO

// FIFO-Klasse für `Cell`-Objekte
class CellFIFO {
private:
    Cell* buffer[MAX_SIZE];  // Array von Zeigern auf `Cell`-Objekte
    int size;                // Anzahl der belegten Plätze

public:
    // Konstruktor
    CellFIFO() : size(0) {
        for (int i = 0; i < MAX_SIZE; i++) {
            buffer[i] = new Cell("FILLER", 0, 0, 0, 0, 0);  // Zeiger initialisieren
        }
    }

    // Destruktor (zur Speicherbereinigung)
    ~CellFIFO() {
        clearBuffer();  // Speicher für alle dynamischen Objekte freigeben
    }

    // FIFO leeren und Speicher freigeben
    void clearBuffer() {
        for (int i = 0; i < MAX_SIZE; i++) {
            if (buffer[i] != nullptr) {
                delete buffer[i];  // Speicher freigeben
                buffer[i] = new Cell("FILLER", 0, 0, 0, 0, 0);  // Platzhalter-Cell
            }
        }
        size = 0;
    }

    // Überprüfen, ob eine `CellID` existiert
    bool exists(long cellID) const {
        for (int i = 0; i < MAX_SIZE; i++) {
            if (buffer[i] != nullptr && buffer[i]->getCellID() == cellID) {
                return true;
            }
        }
        return false;
    }

    // Entfernen eines Elements mit einer bestimmten `CellID`
    void remove(long cellID) {
        for (int i = 0; i < MAX_SIZE; i++) {
            if (buffer[i] != nullptr && buffer[i]->getCellID() == cellID) {
                delete buffer[i];  // Speicher der `Cell` freigeben
                // Elemente nachrücken
                for (int j = i; j < MAX_SIZE - 1; j++) {
                    buffer[j] = buffer[j + 1];
                }
                // Letzten Platz mit einer Platzhalter-`Cell` füllen
                buffer[MAX_SIZE - 1] = new Cell("FILLER", 0, 0, 0, 0, 0);
                return;
            }
        }
    }

    // Methode `addAll()` für das Hinzufügen eines Arrays
    void addAll(Cell* newCells[MAX_SIZE]) {
        bool hasMatch = false;

        // Prüfen, ob mindestens ein `Cell`-Objekt im aktuellen FIFO enthalten ist
        for (int i = 0; i < MAX_SIZE; i++) {
            if (exists(newCells[i]->getCellID())) {
                hasMatch = true;
                break;
            }
        }

        if (!hasMatch) {
            // Keine Übereinstimmung -> FIFO komplett überschreiben
            clearBuffer();
            for (int i = 0; i < MAX_SIZE; i++) {
                buffer[i] = new Cell(*newCells[i]);  // Neue `Cell` kopieren und Zeiger setzen
            }
            size = MAX_SIZE;
        } else {
            // Übereinstimmung vorhanden -> Elemente hinzufügen
            for (int i = MAX_SIZE - 1; i >= 0; i--) {
                if (newCells[i]->getCellID() != 0) {  // Nur gültige `Cell`-Objekte hinzufügen
                    if (exists(newCells[i]->getCellID())) {
                        remove(newCells[i]->getCellID());
                    }
                    // FIFO nach vorne verschieben und neues Element an den Anfang setzen
                    for (int j = MAX_SIZE - 1; j > 0; j--) {
                        buffer[j] = buffer[j - 1];
                    }
                    buffer[0] = new Cell(*newCells[i]);  // Element dynamisch hinzufügen
                }
            }
        }
    }
    bool addCellsToJsonArray(JsonArray *cellsArray){
        for (int i = 0; i < MAX_SIZE; i++) {
            if ((buffer[i]->getCellID() != 0)&&(buffer[i] != nullptr)) {
                JsonObject cellObj = cellsArray->add<JsonObject>();
                buffer[i]->toJson(cellObj);   
            }
        }
        // Serial.println("addCellsToJsonArray");
        return true;
    }
};

#endif
