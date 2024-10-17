module;

#include <fstream>
#include <sstream>
#include <string>
#include <vector>

export module file;

export class File
{
private:
    std::string filename;

public:
    File();
    File(std::string filename);
    std::string ReadFile();
    bool SearchInFile(std::string text);
    void WriteToFile(std::string text);
    void AppendToFile(std::string text);
    void ReplaceInFile(std::string old_text, std::string new_text);
    std::vector<std::string> ListDir(const std::filesystem::path& path);
};
