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
    return content.find(text) != std::string::npos;//���û�ҵ�ָ���ı���find�����᷵��npos
}

void File::WriteToFile(std::string text)
{
    std::ofstream file(filename);
    file << text;
}//�Ḳ�ǣ����Ҫ׷����ʹ��append_to_file

void File::AppendToFile(std::string text)
{
    std::ofstream file(filename, std::ios_base::app);//׷��ģʽ
    file << text;//׷�ӵ��ļ�ĩβ
}

void File::ReplaceInFile(std::string old_text, std::string new_text)
{
    std::string content = ReadFile();
    size_t pos = content.find(old_text);
    if (pos != std::string::npos)
    {
        content.replace(pos, old_text.length(), new_text);//�滻�����ı�
        WriteToFile(content);//д��
    }
}