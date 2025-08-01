#include "FileSystem.h"

void Init_FileSystem()
{
    if (!LittleFS.begin(FORMAT_LITTLEFS_IF_FAILED)) 
    { 
        Serial.println("LittleFS Mount Failed"); 
        return; 
    }
        
    Serial.println("Files Stored on LittleFS:");
    FileSystem_ListDir(LittleFS, "/", 1);  // List the directories up to one level beginning at the root directory

    #if FS_WITH_SD_CARD == 0
        //Serial.println("Read File from Flash");
        FileSystem_ReadFile(LittleFS, String(FS_FILENAME).c_str(), false);
        // delay(500);
        do {
            Display_Header();
            Display_Line(2, "SD-Card not present", 8);
            Display_Line(3, String(FS_FILENAME) + " from Flash", 8);
            Display_Line(4, "Done", 8);
        }
        while (Display_NextPage());
    #else
        if (!SD.begin(CSSDPin)) //
        { 
            Serial.println("SD-Card not present");
            Serial.print(String(FS_FILENAME)); Serial.println(" from Flash ");

            FileSystem_ReadFile(LittleFS, String(FS_FILENAME).c_str(), false);
            // delay(500);
            do {
                Display_Header();
                Display_Line(2, "SD-Card not present", 8);
                Display_Line(3, String(FS_FILENAME) + " from Flash", 8);
                Display_Line(4, "Done", 8);
            }
            while (Display_NextPage());

        } 
        else 
        {
            // If SD Card is present then open the file
            FileSystem_WriteFile(LittleFS, String(FS_FILENAME).c_str(), "");  // Create an Empty file
            FileSystem_ReadFile(SD, String(FS_FILENAME).c_str(), true);
            Serial.println("\nDone creating file on LittleFS");

            do {
                Display_Header();
                Display_Line(2, "SD-Card present", 8);
                Display_Line(3, String(FS_FILENAME) + " from SD-Card", 8);
                Display_Line(4, "Done", 8);
            }
            while (Display_NextPage());
        }
    #endif
}

void FileSystem_ListDir(fs::FS &fs, const char *dirname, uint8_t levels) {
  //Serial.printf("Listing directory: %s\r\n", dirname);

  File root = fs.open(dirname);
  if (!root) {
    Serial.println("- failed to open directory");
    return;
  }
  if (!root.isDirectory()) {
    Serial.println(" - not a directory");
    return;
  }

  File file = root.openNextFile();
  while (file) {
    if (file.isDirectory()) {
      Serial.print("  DIR : ");
      Serial.println(file.name());
      if (levels) {
        FileSystem_ListDir(fs, file.path(), levels - 1);
      }
    } else {
      Serial.print("  FILE: ");
      Serial.print(file.name());
      Serial.print("\tSIZE: ");
      Serial.println(file.size());
    }
    file = root.openNextFile();
  }
}

// Reads all lines from a specified file on the given filesystem
std::vector<String> readLines(fs::FS &fs, const char* filename) {
    std::vector<String> lines;
    File file = fs.open(filename, "r");
    if (!file) {
        Serial.print("FileSystem: Failed to open ");
        Serial.print(filename);
        Serial.println(" for reading.");
        return lines; // Return empty vector on failure
    }

    Serial.print("FileSystem: Reading lines from ");
    Serial.println(filename);

    while (file.available()) {
        lines.push_back(file.readStringUntil('\n')); // Read line by line
    }
    file.close();
    Serial.print("FileSystem: Loaded ");
    Serial.print(lines.size());
    Serial.print(" lines from ");
    Serial.println(filename);
    return lines; // Return loaded lines
}

// Writes a vector of strings to a specified file on the given filesystem
bool writeLines(fs::FS &fs, const char* filename, const std::vector<String>& lines) {
    File file = fs.open(filename, "w"); // Open in write mode, overwrites existing file
    if (!file) {
        Serial.print("FileSystem: Failed to open ");
        Serial.print(filename);
        Serial.println(" for writing.");
        return false; // Failed to open file for writing
    }

    Serial.print("FileSystem: Writing ");
    Serial.print(lines.size());
    Serial.print(" lines to ");
    Serial.println(filename);

    for (const String& line : lines) {
        file.println(line); // Write each line
    }
    file.close();
    Serial.println("FileSystem: File written successfully.");
    return true; // Write successful
}

void FileSystem_ReadFile(fs::FS &fs, const char *path, bool saveFile) {
    //Serial.printf("Reading file: %s\n", path);

    File file = fs.open(path);
    if (!file) {
        Serial.println("Failed to open file for reading");
        do {
            Display_Line(1, "Failed to Open", 8);      
        } 
        while (Display_NextPage());
        return;
    }

    // Read data from file
    int row = 0;

    while (file.available()) 
    {
        char line[maxLineLength];
        String readLine = file.readStringUntil('\n');
        // Serial.print("Read Line:");
        // Serial.println(readLine);
        if (saveFile) // If we have to save the file into esp32 memory
        {  
            char Buf[80];
            readLine.toCharArray(Buf, 80);
            FileSystem_AppendFile(LittleFS, String(FS_FILENAME).c_str(), Buf);     //Append readed line
            FileSystem_AppendFile(LittleFS, String(FS_FILENAME).c_str(), "\n\r");  //Append readed line
        }

        String lineWithoutComment = FileSystem_GetValue(readLine, '/', 0);
        String numeratorString = FileSystem_GetValue(lineWithoutComment, ';', 0);
        String registerString = FileSystem_GetValue(lineWithoutComment, ';', 1);
        String readWriteString = FileSystem_GetValue(lineWithoutComment, ';', 2);
        String valueString = FileSystem_GetValue(lineWithoutComment, ';', 3);
        String nameOfRegisterString = FileSystem_GetValue(lineWithoutComment, ';', 4);

        valueString.toLowerCase();        // convert to lower case
        if (valueString.endsWith("h")) // Check if value ends with "h" remove last 'h'
        {  
            // Serial.print("Value ends with h:");
            // Serial.println(valueString);
            valueString = valueString.substring(0, valueString.length() - 1);
        } 
        else if (valueString.endsWith("d")) // Check if value ends on 'd' then remove last 'd' and convert it to hex first
        {                              
            valueString = valueString.substring(0, valueString.length() - 1);  // Remove last 'd'
            valueString = String(valueString.toInt(), HEX);
        } 
        else // Check if value doesn't end on 'h' then convert it to hex first
        {  
            valueString = String(valueString.toInt(), HEX);
        }

        numeratorString.trim();
        readWriteString.trim();
        registerString.trim();
        valueString.trim();
        valueString.toUpperCase();
        strncpy(values[row], valueString.c_str(), maxLineLength);  // NameOfRegister

        nameOfRegisterString.trim();
        if (numeratorString.length() == 0) continue;  // skip commented line
        // Serial.println(readLine);
        Serial.print(numeratorString);
        Serial.print(",");
        Serial.print(registerString);
        Serial.print(",");
        Serial.print(readWriteString);
        Serial.print(",");
        Serial.print(valueString);
        Serial.print(",");
        Serial.print(nameOfRegisterString);
        Serial.println(",end");
        
        // Parse line
        char *token = strtok(line, ";");
        int column = 0;

        data[row][0] = numeratorString.toInt();                   // Numeration
        data[row][1] = strtol(registerString.c_str(), NULL, 16);  // Register2Read (hexadecimal)
        data[row][3] = strtol(valueString.c_str(), NULL, 16);     // Value (hexadecimal or decimal)*****
        if ((readWriteString=="R") || (readWriteString=="r")) {readWriteString="0";}
        if ((readWriteString=="W") || (readWriteString=="w")) {readWriteString="1";}
        data[row][2] = readWriteString.toInt();  // Read Write String

        strncpy(names[row], nameOfRegisterString.c_str(), maxLineLength);  // NameOfRegister

        row++;
        if (row >= numRows) break;
    }

    numberOfRows = row;
    Serial.print("Number of lines read: ");
    Serial.println(numberOfRows);

    // Fill remaining rows with zeros
    for (int i = row; i < numRows; i++) {
        for (int j = 0; j < 2; j++) {
        data[i][j] = 0;
        }
    }
    file.close();
}

void FileSystem_WriteFile(fs::FS &fs, const char *path, const char *message) {
  //Serial.printf("Writing file: %s\r\n", path);

  File file = fs.open(path, FILE_WRITE);
  if (!file) {
    Serial.println("- failed to open file for writing");
    return;
  }
  if (file.print(message)) {
    Serial.println("- file written");
  } else {
    Serial.println("- write failed");
  }
  file.close();
}

void FileSystem_AppendFile(fs::FS &fs, const char *path, const char *message) {
  // Serial.printf("Appending to file: %s\r\n", path);

  File file = fs.open(path, FILE_APPEND);
  if (!file) {
    Serial.println("- failed to open file for appending");
    return;
  }
  if (!file.print(message)) {
    // Serial.println("- message appended");
    Serial.println("- append failed");
  }
  file.close();
}

void FileSystem_WriteRegisterValue(const char *path, uint32_t registerIndex)
{
  if (!LittleFS.begin(FORMAT_LITTLEFS_IF_FAILED)) 
  { 
      Serial.println("LittleFS Mount Failed"); 
      return; 
  }

  // Read all lines from the file first
  std::vector<String> lines = readLines(LittleFS, path);

  for (size_t i = 0; i < lines.capacity(); i++)
  {
    String line = lines[i];
    String lineWithoutComment = FileSystem_GetValue(line, '/', 0);
    String numeratorString = FileSystem_GetValue(lineWithoutComment, ';', 0);

    int numerator = numeratorString.toInt();
    if (numerator == data[registerIndex][0])
    {
      String readWrite = "";
      if (data[registerIndex][2] == 0)
      {
        readWrite = "R";
      }
      else
      {
        readWrite = "W";
      }

      String comment = line.substring(line.indexOf("//"));
      long value = strtoul(values[registerIndex], NULL, HEX);

      lines[i] = String(data[registerIndex][0]) + ";\t" + 
                 String(data[registerIndex][1], HEX) + "H;\t" + 
                 readWrite + ";\t" + 
                 String(value, HEX) + "h;\t" + 
                 String(names[registerIndex]) + ";\t" +
                 comment;
      
      Serial.println(lines[i]);

      break;
    }    
  }  
  
  writeLines(LittleFS, path, lines);  
}

String FileSystem_GetValue(String data, char separator, int index) {
  int found = 0;
  int strIndex[] = { 0, -1 };
  int maxIndex = data.length() - 1;

  for (int i = 0; i <= maxIndex && found <= index; i++) {
    if (data.charAt(i) == separator || i == maxIndex) {
      found++;
      strIndex[0] = strIndex[1] + 1;
      strIndex[1] = (i == maxIndex) ? i + 1 : i;
    }
  }
  return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}
