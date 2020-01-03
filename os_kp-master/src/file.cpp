#include "file.h"
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>

File::File(std::string fname, bool canRead, bool canWrite, int size) : 
    m_canRead(canRead), m_canWrite(canWrite), m_maxChunkSize(size * 1024)
{
    //open mode
    int openflag = O_RDWR;
    //if (!canRead) openflag = O_WRONLY; else //THIS MUST APSENT BECAUSE OF MMAP WEIRD BEHAVIOUR
    if (!canWrite) openflag = O_RDONLY;
    
    //open file
    m_fileDescr = open(fname.c_str(), openflag);
    if (!m_fileDescr) throw std::logic_error("cannot open file \"" + fname + "\""); //error

    //file size
    struct stat st;
    if (fstat(m_fileDescr, &st) < 0) throw std::logic_error("cannot get size of file \"" + fname + "\""); //error
    m_fileSize = (int)st.st_size;

    //mmap
    m_map = nullptr;
    Mmap(0);
}
File::~File()
{
    if (m_map) munmap((void*)m_map, m_chunkSize); //unmap chunk
    close(m_fileDescr); //close file
}

//mmap file
void File::Mmap(int pos)
{
    if (m_map)
    {
        msync((void*)m_map, m_chunkSize, MS_SYNC);
        munmap((void*)m_map, m_chunkSize); //unmap previous chunk
    }
    //setup diapason
    m_chunkPos = pos;
    m_chunkSize = std::min(m_maxChunkSize, m_fileSize - m_chunkPos);
    //mmap mode
    int mode = 0;
    if (m_canRead) mode |= PROT_READ;
    if (m_canWrite) mode |= PROT_WRITE;
    //mmap
    void *vmap = mmap(NULL, m_chunkSize, mode, MAP_SHARED, m_fileDescr, m_chunkPos);
    if (vmap == MAP_FAILED) throw std::logic_error("cannot mmap file");
    m_map = (char*)vmap;
}

std::vector<std::pair<int,std::string>> File::Find(std::regex what) //find ALL entrances of pattern
{
    //check if can read
    if (!m_canRead) throw std::logic_error("cannot read from file");

    std::vector<std::pair<int,std::string>> finds;

    int pos = 0;
    while (1)
    {
        auto match = *std::cregex_iterator(&m_map[pos], &m_map[m_chunkSize], what);
        if (match.size() == 0) break;

        finds.push_back(std::make_pair<int, std::string>(match.position(0) + pos, match[0].str()));
        pos += match.position(0) + match[0].str().size();
    }
    return finds;
}

bool File::CompareCharString(int off, std::string str)
{
    for (int i = 0; i < str.size(); i++) if (m_map[off + i] != str[i]) return false;
    return true;
}
void File::Replace(std::string what, std::string with, std::function<Answer(int)> confirm) //replace
{
    //check if can read and write
    if (!m_canRead || !m_canWrite) throw std::logic_error("cannot read from and write to file");
    //strings must be same size
    if (what.size() != with.size()) throw std::logic_error("strings must be same length");

    //find "what" in file
    bool all = false, stop = false;
    for (int i = 0; i <= m_chunkSize - what.size() && !stop; i++)
    {
        //compare and confirm
        if (CompareCharString(i, what))
        {
            bool rep = true;
            if (!all)
            {
                Answer ans = confirm(i); //ask
                switch (ans)
                {
                case Answer::no:
                    rep = false;
                    break;
                case Answer::stop:
                    rep = false;
                    stop = true;
                    break;
                case Answer::all:
                    all = true;
                    break;
                }
            }
            if (rep)
            {
                for (int j = 0; j < with.size(); j++) m_map[i + j] = with[j]; //replace
                i += with.size() - 1;
            }
        }
    }
}
std::string File::Print(int from, int to)
{
    //check if can read
    if (!m_canRead) throw std::logic_error("cannot read from file");
    //check diapason
    if (to < from || to < 0 || to >= m_chunkSize)
        throw std::logic_error("incorrect diapason [" + std::to_string(from) + ", " + std::to_string(to) + "]");
    
    //read from file
    std::string res;
    for (int i = from; i <= to; i++) res += m_map[i];
    return res;
}
void File::Write(int to, std::string what) //set diapason of chars
{
    //check if can write
    if (!m_canWrite) throw std::logic_error("cannot write to file");
    //check diapason
    if (to < 0 || to > m_chunkSize - what.size())
        throw std::logic_error("cannot fit string into chunk at this pos");
    
    //write to file
    for (int i = 0; i < what.size(); i++) m_map[to + i] = what[i];
}

//move chunk
void File::Move(int delta)
{
    SetPos(m_chunkPos + delta);
}
void File::SetPos(int pos)
{
    if (pos < 0 || pos >= m_fileSize) throw std::logic_error("moving outside of file");

    int ps = getpagesize();
    if (pos % ps) throw std::logic_error("Position MUST be multiple of page size: " + std::to_string(ps));

    Mmap(pos);
}

std::pair<int, int> File::Diapason()
{
    return std::make_pair<int, int>(std::move(m_chunkPos), m_chunkPos + m_chunkSize - 1);
}
std::string File::Mode()
{
    if (!m_canRead) return "Write only";
    if (!m_canWrite) return "Read only";
    return "Read and write";
}
int File::FileSize()
{
    return m_fileSize;
}