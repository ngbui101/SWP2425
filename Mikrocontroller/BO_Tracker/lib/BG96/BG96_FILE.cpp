#include "BG96_FILE.h"
/**
 * @brief Konstruktor der Klasse _BG96_FILE.
 *
 * Initialisiert das Objekt _BG96_FILE.
 */
_BG96_FILE::_BG96_FILE()
{

}

/**
 * @brief Destruktor der Klasse _BG96_FILE.
 *
 * Bereinigt den Empfangspuffer, um verbleibende Daten zu löschen.
 */
_BG96_FILE::~_BG96_FILE()
{
    cleanBuffer();
}

/**
 * @brief Konstruktor der Klasse _BG96_FILE mit Parametern.
 *
 * Initialisiert das Objekt _BG96_FILE und übergibt die AT- und Debug-Serielle Schnittstellen.
 *
 * @param atserial Referenz auf die serielle Schnittstelle für AT-Befehle.
 * @param dserial Referenz auf die serielle Schnittstelle für Debug-Ausgaben.
 */
_BG96_FILE::_BG96_FILE(Stream &atserial, Stream &dserial) : _BG96_TCPIP(atserial, dserial)
{

}

/**
 * @brief Abfrage des verfügbaren und des gesamten Speicherplatzes.
 *
 * Diese Methode verwendet den AT-Befehl AT+QFLDS, um Informationen über den freien und den gesamten Speicherplatz im UFS-Speicher zu erhalten.
 *
 * @param free_bytes Referenz auf die Variable, die die freien Bytes speichern wird.
 * @param total_bytes Referenz auf die Variable, die die gesamten Bytes speichern wird.
 * @return true bei Erfolg, false bei Fehler.
 *
 * @see Quectel BG96 File AT Commands, Abschnitt 2.1 (Seite 7)
 */
bool _BG96_FILE::GetFliesSpace(unsigned long &free_bytes, unsigned long &total_bytes)
{
    char cmd[16]; 
    strcpy(cmd,FILE_SPACE_INFORMATION);
    strcat(cmd,"=\"UFS\"");

    if(sendAndSearch(cmd, RESPONSE_OK, RESPONSE_ERROR, 2)){
        char *sta_buf = searchStrBuffer(": ");
        char *med_buf = searchChrBuffer(',');
        char *end_buf = searchStrBuffer(RESPONSE_CRLF_OK);
        *med_buf = '\0';
        *end_buf = '\0';
        free_bytes = atol(sta_buf + 2);
        total_bytes = atol(med_buf + 1);
        return true;
    }
    return false;
}

/**
 * @brief Liste der Dateien im UFS abrufen.
 *
 * Diese Methode verwendet den AT-Befehl AT+QFLST, um eine Liste aller Dateien im UFS-Speicher abzurufen.
 *
 * @param list Speicher für die Liste der Dateien.
 * @return true bei Erfolg, false bei Fehler.
 *
 * @see Quectel BG96 File AT Commands, Abschnitt 2.2 (Seite 8)
 */
bool _BG96_FILE::GetFilesList(char *list)
{
    char cmd[16];
    strcpy(cmd, FILE_LIST_FILES);
    if(sendAndSearch(cmd, RESPONSE_OK, RESPONSE_ERROR, 2)){
        char *sta_buf = searchStrBuffer(FILE_LIST_FILES);
        char *end_buf = searchStrBuffer(RESPONSE_CRLF_OK);
        *end_buf = '\0';
        strcpy(list, sta_buf);
        return true;
    }
    return false;
}

/**
 * @brief Löschen einer Datei im UFS-Speicher.
 *
 * Diese Methode verwendet den AT-Befehl AT+QFDEL, um eine Datei oder mehrere Dateien im UFS-Speicher zu löschen.
 *
 * @param filename Name der Datei, die gelöscht werden soll.
 * @return true bei Erfolg, false bei Fehler.
 *
 * @see Quectel BG96 File AT Commands, Abschnitt 2.3 (Seite 9)
 */
bool _BG96_FILE::DeleteFiles(const char *filename)
{
    char cmd[32],buf[32];
    strcpy(cmd, FILE_DELETE_FILES);
    sprintf(buf, "=\"%s\"", filename);
    strcat(cmd, buf);
    if(sendAndSearch(cmd, RESPONSE_OK, RESPONSE_ERROR, 2)){
        return true;
    }
    return false;
}

/**
 * @brief Hochladen einer Datei in den UFS-Speicher.
 *
 * Diese Methode verwendet den AT-Befehl AT+QFUPL, um eine Datei in den UFS-Speicher hochzuladen.
 *
 * @param filename Name der Datei, die hochgeladen werden soll.
 * @param u_file Inhalt der hochzuladenden Datei.
 * @return true bei Erfolg, false bei Fehler.
 *
 * @see Quectel BG96 File AT Commands, Abschnitt 2.4 (Seite 10)
 */
bool _BG96_FILE::UploadFiles(char *filename,const char *u_file)
{
    char cmd[32],buf[32];
    strcpy(cmd, FILE_UPLOAD_FILES);
    sprintf(buf, "=\"%s\",%d", filename, strlen(u_file));
    strcat(cmd, buf);
    if(sendAndSearch(cmd, RESPONSE_CONNECT, RESPONSE_ERROR, 5)){
        if(sendDataAndCheck(u_file, RESPONSE_OK, RESPONSE_ERROR, 10)){
            return true;
        }
    }
    return false;
}

/**
 * @brief Herunterladen einer Datei aus dem UFS-Speicher.
 *
 * Diese Methode verwendet den AT-Befehl AT+QFDWL, um eine Datei aus dem UFS-Speicher herunterzuladen.
 *
 * @param filename Name der Datei, die heruntergeladen werden soll.
 * @param d_file Puffer für den Inhalt der heruntergeladenen Datei.
 * @return true bei Erfolg, false bei Fehler.
 *
 * @see Quectel BG96 File AT Commands, Abschnitt 2.5 (Seite 12)
 */
bool _BG96_FILE::DownloadFiles(char *filename, char *d_file)
{
    char cmd[32],buf[32];
    strcpy(cmd, FILE_DOWNLOAD_FILE);
    sprintf(buf, "=\"%s\"", filename);
    strcat(cmd, buf);
    if(sendAndSearch(cmd, RESPONSE_OK, RESPONSE_ERROR, 30)){
        char *end_buf = searchStrBuffer(FILE_DOWNLOAD_FILE);
        *end_buf = '\0';
        char *sta_buf = searchStrBuffer(RESPONSE_CONNECT);
        strcpy(d_file, sta_buf + strlen(RESPONSE_CONNECT) + 2);
        return true;
    }
    return false;
}

/**
 * @brief Öffnet eine Datei im UFS-Speicher.
 *
 * Diese Methode verwendet den AT-Befehl AT+QFOPEN, um eine Datei im UFS-Speicher zu öffnen.
 *
 * @param filename Name der Datei, die geöffnet werden soll.
 * @param mode Modus, in dem die Datei geöffnet werden soll (Lesen, Schreiben usw.).
 * @param file_index Index der geöffneten Datei, wird zurückgegeben.
 * @return true bei Erfolg, false bei Fehler.
 *
 * @see Quectel BG96 File AT Commands, Abschnitt 2.6 (Seite 13)
 */
bool _BG96_FILE::OpenFile(char *filename, Open_File_Mode_t mode, unsigned int &file_index)
{
    char cmd[32],buf[32];
    strcpy(cmd, FILE_OPEN_FILE);
    sprintf(buf, "=\"%s\",%d", filename, mode);
    strcat(cmd, buf);
    if(sendAndSearch(cmd, RESPONSE_OK, RESPONSE_ERROR, 2)){
        char *sta_buf = searchStrBuffer(": ");
        char *end_buf = searchStrBuffer(RESPONSE_CRLF_OK);
        *end_buf = '\0';
        file_index = atoi(sta_buf + 2);
        return true;
    }
    return false;
}

/**
 * @brief Liest den Inhalt einer Datei im UFS-Speicher.
 *
 * Diese Methode verwendet den AT-Befehl AT+QFREAD, um den Inhalt einer Datei zu lesen.
 *
 * @param file_index Index der Datei, die gelesen werden soll.
 * @param read_data Puffer, in den der gelesene Inhalt geschrieben wird.
 * @return true bei Erfolg, false bei Fehler.
 *
 * @see Quectel BG96 File AT Commands, Abschnitt 2.7 (Seite 14)
 */
bool _BG96_FILE::ReadFile(unsigned int file_index, char *read_data)
{
    char cmd[16],buf[8];
    strcpy(cmd, FILE_READ_FILE);
    sprintf(buf, "=%d", file_index);
    strcat(cmd, buf);
    if(sendAndSearch(cmd, RESPONSE_OK, RESPONSE_ERROR, 10)){
        char *sta_buf = searchStrBuffer("\r\n");
        char *end_buf = searchStrBuffer("\r\nOK");
        *end_buf = '\0';
        strcpy(read_data, sta_buf + 2);
        return true;
    }
    return false;
}

/**
 * @brief Schreibt Daten in eine geöffnete Datei.
 *
 * Diese Methode verwendet den AT-Befehl AT+QFWRITE, um Daten in eine geöffnete Datei zu schreiben.
 *
 * @param file_index Index der geöffneten Datei.
 * @param write_data Die zu schreibenden Daten.
 * @return true bei Erfolg, false bei Fehler.
 *
 * @see Quectel BG96 File AT Commands, Abschnitt 2.8 (Seite 15)
 */
bool _BG96_FILE::WriteFile(unsigned int file_index, char *write_data)
{
    char cmd[32],buf[16];
    strcpy(cmd, FILE_WRITE_FILE);
    sprintf(buf, "=%d,%d", file_index, strlen(write_data));
    strcat(cmd, buf);
    if(sendAndSearch(cmd, RESPONSE_CONNECT, RESPONSE_ERROR, 5)){
        if(sendDataAndCheck(write_data, RESPONSE_OK, RESPONSE_ERROR, 10)){
            return true;
        }
    }
    return false;
}

/**
 * @brief Schließt eine geöffnete Datei.
 *
 * Diese Methode verwendet den AT-Befehl AT+QFCLOSE, um eine geöffnete Datei zu schließen.
 *
 * @param file_index Index der zu schließenden Datei.
 * @return true bei Erfolg, false bei Fehler.
 *
 * @see Quectel BG96 File AT Commands, Abschnitt 2.9 (Seite 16)
 */
bool _BG96_FILE::CloseFlie(unsigned int file_index)
{
    char cmd[16],buf[8];
    strcpy(cmd, FILE_CLOSE_FILE);
    sprintf(buf, "=%d", file_index);
    strcat(cmd, buf);
    if(sendAndSearch(cmd, RESPONSE_OK, RESPONSE_ERROR, 5)){
        return true;
    }
    return false;
}

/**
 * @brief Setzt den Dateizeiger in einer geöffneten Datei.
 *
 * Diese Methode verwendet den AT-Befehl AT+QFSEEK, um den Dateizeiger an eine bestimmte Position zu setzen.
 *
 * @param file_index Index der Datei, in der der Zeiger gesetzt wird.
 * @param offset Offset, auf den der Zeiger gesetzt wird.
 * @param p_mode Modus für den Dateizeiger (absolut, relativ).
 * @return true bei Erfolg, false bei Fehler.
 *
 * @see Quectel BG96 File AT Commands, Abschnitt 2.10 (Seite 17)
 */
bool _BG96_FILE::SetFilePointer(unsigned int file_index, unsigned int offset, Pointer_Mode_t p_mode)
{
    char cmd[32],buf[16];
    strcpy(cmd, FILE_SET_POSITION_FILE);
    sprintf(buf, "=%d,%d,%d", file_index, offset, p_mode);
    strcat(cmd, buf);
    if(sendAndSearch(cmd, RESPONSE_OK, RESPONSE_ERROR, 5)){
        return true;
    }
    return false;
}

/**
 * @brief Liest die aktuelle Position des Dateizeigers.
 *
 * Diese Methode verwendet den AT-Befehl AT+QFPOSITION, um die aktuelle Position des Dateizeigers zu ermitteln.
 *
 * @param file_index Index der Datei, deren Dateizeiger gelesen wird.
 * @param offset Variable, in die die Position des Dateizeigers geschrieben wird.
 * @return true bei Erfolg, false bei Fehler.
 *
 * @see Quectel BG96 File AT Commands, Abschnitt 2.11 (Seite 18)
 */
bool _BG96_FILE::GetFilePointer(unsigned int file_index, unsigned int &offset)
{
    char cmd[32],buf[16];
    strcpy(cmd, FILE_GET_POSITION_FILE);
    sprintf(buf, "=%d", file_index);
    strcat(cmd, buf);
    if(sendAndSearch(cmd, RESPONSE_OK, RESPONSE_ERROR, 5)){
        char *sta_buf = searchStrBuffer(": ");
        char *end_buf = searchStrBuffer(RESPONSE_CRLF_OK);
        *end_buf = '\0';
        offset = atoi(sta_buf + 2);
        return true;
    }
    return false;
}

/**
 * @brief Trunkiert (verkürzt) eine Datei auf die aktuelle Position des Dateizeigers.
 *
 * Diese Methode verwendet den AT-Befehl AT+QFTRUNC, um eine Datei ab der aktuellen Position des Dateizeigers zu kürzen.
 *
 * @param file_index Index der Datei, die trunkiert werden soll.
 * @return true bei Erfolg, false bei Fehler.
 *
 * @see Quectel BG96 File AT Commands, Abschnitt 2.12 (Seite 19)
 */
bool _BG96_FILE::TruncateFile(unsigned int file_index)
{
    char cmd[16],buf[8];
    strcpy(cmd, FILE_TRUNCATE_FILE);
    sprintf(buf, "=%d", file_index);
    strcat(cmd, buf);
    if(sendAndSearch(cmd, RESPONSE_OK, RESPONSE_ERROR, 5)){
        return true;
    }
    return false;
}
