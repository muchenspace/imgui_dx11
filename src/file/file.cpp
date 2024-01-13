import file;

#include <fstream>
#include <sstream>
#include <string>

File::File(std::string filename):filename(filename)
{

}

std::string File::ReadFile()
{
    std::ifstream file{filename};
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

bool File::SearchInFile(std::string text)
{
    std::string content = ReadFile();
    return content.find(text) != std::string::npos;//如果没找到指定文本，find方法会返回npos
}

void File::WriteToFile(std::string text)
{
    std::ofstream file(filename);
    file << text;
}//会覆盖，如果要追加请使用append_to_file

void File::AppendToFile(std::string text)
{
    std::ofstream file(filename, std::ios_base::app);//追加模式
    file << text;//追加到文件末尾
}

void File::ReplaceInFile(std::string old_text, std::string new_text)
{
    std::string content = ReadFile();
    size_t pos = content.find(old_text);
    if (pos != std::string::npos)
    {
        content.replace(pos, old_text.length(), new_text);//替换掉旧文本
        WriteToFile(content);//写入
    }
}