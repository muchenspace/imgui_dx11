module;

#include <fstream>
#include <sstream>
#include <string>

export module file;

export class File
{
private:
    std::string filename;  // �ļ���

public:
    File(std::string filename);
    std::string ReadFile();
    bool SearchInFile(std::string text);
    void WriteToFile(std::string text);
    void AppendToFile(std::string text);
    void ReplaceInFile(std::string old_text, std::string new_text);
};