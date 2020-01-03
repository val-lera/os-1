#pragma once
#include <regex>

enum class Answer : int
{
    no,
    yes,
    all,
    stop
};

class File
{
public:
    File(std::string fname, bool canRead, bool canWrite, int size);
    ~File();

    std::vector<std::pair<int,std::string>> Find(std::regex what); //find ALL entrances of pattern
    void Replace(std::string what, std::string with, std::function<Answer(int)> confirm); //replace

    std::string Print(int from, int to); //show diapason of chars
    void Write(int to, std::string what); //set diapason of chars

    void Move(int delta);
    void SetPos(int pos);

    std::pair<int, int> Diapason();
    std::string Mode();
    int FileSize();

private:
    int m_fileDescr, m_fileSize; //file descriptor and size of file
    char *m_map; //mapped piece of file

    bool m_canRead, m_canWrite; //can read from/write to file
    int m_chunkPos, m_chunkSize, m_maxChunkSize; //position in file and size of chunk

    void Mmap(int pos);
    bool CompareCharString(int off, std::string str);
};