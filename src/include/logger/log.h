
#ifndef log
#define log
// Doesnt crash
void error(char* message);
void warning(char* message);
void info(char* message);
void success(char* message);

// Intentionally Crashes
void panic(char* message);

#endif