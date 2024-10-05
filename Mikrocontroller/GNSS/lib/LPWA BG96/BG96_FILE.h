/*
* Implementierung der BG96_File
*mehr: quectel_bg96_file_at_commands_manual_v1-1
 */

#ifndef __BG96_FILE_H_
#define __BG96_FILE_H_

#include "BG96_TCPIP.h"

typedef enum open_file_mode{
    CREATE_OR_OPEN = 0,
    CREATE_OR_CLEAR = 1,
    ONLY_READ = 2,
}Open_File_Mode_t;

typedef enum pointer_mode{
    FILE_BEGINNING = 0,
    FILE_CURRENT = 1,
    FILE_ENDING = 2,
}Pointer_Mode_t;

class _BG96_FILE : public _BG96_TCPIP
{
  public:

    _BG96_FILE();

    ~_BG96_FILE();

    _BG96_FILE(Stream &atserial, Stream &dserial);

    bool GetFliesSpace(unsigned long &free_bytes, unsigned long &total_bytes);

    bool GetFilesList(char *list);

    bool DeleteFiles(const char *filename);

    bool UploadFiles(char *filename, char *u_file);

    bool DownloadFiles(char *filename, char *d_file);

    bool OpenFile(char *filename, Open_File_Mode_t mode, unsigned int &file_index);

    bool ReadFile(unsigned int file_index, char *read_data);

    bool WriteFile(unsigned int file_index, char *write_data);

    bool CloseFlie(unsigned int file_index);

    bool SetFilePointer(unsigned int file_index, unsigned int offset, Pointer_Mode_t p_mode);

    bool GetFilePointer(unsigned int file_index, unsigned int &offset);

    bool TruncateFile(unsigned int file_index);

  private:

};

#endif