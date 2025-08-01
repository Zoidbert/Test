#include "FS.h"
#include <LittleFS.h>
#include "SD.h"
#include <vector>

#ifndef __CONFIG_H__
    #include "Config.h"
#endif

#ifndef __DISPLAY_H__
    #include "Display.h"
#endif


#ifndef __FILESYSTEM_H__
    #define __FILESYSTEM_H__


#endif

void Init_FileSystem();
void FileSystem_ListDir(fs::FS &fs, const char *dirname, uint8_t levels);
void FileSystem_ReadFile(fs::FS &fs, const char *path, bool saveFile);
void FileSystem_WriteFile(fs::FS &fs, const char *path, const char *message);
void FileSystem_AppendFile(fs::FS &fs, const char *path, const char *message);
void FileSystem_WriteRegisterValue(const char *path, uint32_t registerIndex);
String FileSystem_GetValue(String data, char separator, int index);
 // Reads all lines from a file on a given filesystem
std::vector<String> readLines(fs::FS &fs, const char* filename);
// Writes a vector of strings to a file on a given filesystem
bool writeLines(fs::FS &fs, const char* filename, const std::vector<String>& lines);