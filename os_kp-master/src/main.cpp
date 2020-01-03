#include <iostream>
#include <functional>
#include <sstream>
#include <vector>
#include <map>

#include "file.h"

using args = std::vector<std::string>;

Answer Ask(int off)
{
    std::string c;
    std::cout << "Replace at offset " << off << "? (y/n/s/a): ";
    std::getline(std::cin, c);

    switch (std::tolower(c[0]))
    {
    case 'y': return Answer::yes;
    case 'a': return Answer::all;
    case 's': return Answer::stop;
    }
    return Answer::no;
}
void help(args)
{
    std::cout << "List of commands:" << std::endl <<
        " quit -- exit program" << std::endl <<
        " help -- shows this text" << std::endl <<
        " setpos P -- set position of chunk in file to P. P mist be multiple of page size" << std::endl <<
        " move D -- moves chunk by D chars. D must be multiple of page size" << std::endl <<
        " diap -- show chunk borders" << std::endl <<
        " mode -- show file editing mode" << std::endl <<
        " size -- show file size" << std::endl <<
        " print A B -- print chars from A position in chunk to B" << std::endl <<
        " write A Str -- write string Str to position A" << std::endl <<
        " find -- find regular expression in file" << std::endl <<
        " replace -- replace one string with another. Strings must be same length" << std::endl;
}
unsigned int toUint(std::string s)
{
    //check if is int
    if (s.size() == 0) throw std::logic_error("not a positive integer"); //empty
    for (int i = 0; i < s.size(); i++)
        if (s[i] < '0' || s[i] > '9') throw std::logic_error("not a positive integer");
    return atoi(s.c_str()); //convert if is int
}

int main(int argc, char *argv[])
{
    //args: ./a.out filename mode=rw size=15
    std::string filename;
    bool canRead = true, canWrite = true;
    int size = 15;

    //get args
    switch (argc)
    {
    case 4:
        size = atoi(argv[3]);
        if (size <= 0)
        {
            std::cout << "Size must be positive" << std::endl;
            return 3;
        } //break must apsent here
    case 3:
    {
        std::string strmode = argv[2];
        if (strmode != "rw")
        {
            if (strmode == "w") canRead = false;
            else if (strmode == "r") canWrite = false;
            else
            {
                std::cout << "Available modes:" << std::endl <<
                    "r -- read only" << std::endl <<
                    "w -- write only" << std::endl <<
                    "rw -- read and write" << std::endl;
                return 2;
            }
        }
    } //and here
    case 2:
        filename = argv[1];
        break; //but here must be

    default:
        std::cout << "Usage: " << argv[0] << " filename [mode=rw [size=15]]" << std::endl;
        return 1;
    }

    //std::cout << "ARGS: " << filename << " " << (int)mode << " " << size << std::endl;
    try
    {
        File file(filename, canRead, canWrite, size); //open file

        std::map<std::string, std::function<void(args)>> functions; //user functions
        std::map<std::string, unsigned int> argsCount; //functions arguments count

        bool cycle = true; //loop variable
        std::string input; //contains user input

        //UI functions
        argsCount["quit"] = 0;
        functions["quit"] = [&cycle](args){ cycle = false; };
        argsCount["help"] = 0;
        functions["help"] = help;
        //file functions
        argsCount["setpos"] = 1;
        functions["setpos"] = [&file](args a){ file.SetPos(toUint(a[0])); };
        argsCount["move"] = 1;
        functions["move"] = [&file](args a){ file.Move(toUint(a[0])); };
        argsCount["diap"] = 0;
        functions["diap"] = [&file](args)
        {
            auto a = file.Diapason();
            std::cout << "Diapason: [" << a.first << ", " << a.second << "]" << std::endl;
        };
        argsCount["mode"] = 0;
        functions["mode"] = [&file](args)
        {
            std::cout << "Mode: " << file.Mode() << std::endl;
        };
        argsCount["size"] = 0;
        functions["size"] = [&file](args)
        {
            std::cout << "File size: " << file.FileSize() << std::endl;
        };

        argsCount["print"] = 2;
        functions["print"] = [&file](args a){ std::cout << file.Print(toUint(a[0]), toUint(a[1])) << std::endl; };
        argsCount["write"] = 2;
        functions["write"] = [&file](args a){ file.Write(toUint(a[0]), a[1]); };

        argsCount["find"] = 0;
        functions["find"] = [&file](args a)
        {
            std::string pattern;
            std::cout << "Find what: "; std::getline(std::cin, pattern);

            auto v = file.Find(std::regex(pattern));
            for (std::pair<int, std::string> p : v)
                std::cout << "Found " << p.second << " at offset " << p.first << std::endl;
        };

        argsCount["replace"] = 0;
        functions["replace"] = [&file](args)
        {
            std::string what, with;
            std::cout << "Replace what: "; std::getline(std::cin, what);
            std::cout << "Replace with: "; std::getline(std::cin, with);

            file.Replace(what, with, Ask);
        };

        while (cycle)
        {
            std::cout << "> ";
            std::getline(std::cin, input); //read uer input

            //split string
            std::stringstream ss(input);
            std::vector<std::string> words;
            for (std::string s; ss >> s; ) words.push_back(s); //split into words
            if (words.size() == 0) continue;

            auto iter = functions.find(words[0]);
            if (iter == functions.end()) //not a command in map
            {
                std::cout << "unknowm command \"" << words[0] << "\"" << std::endl;
                continue;
            }
            //check arguments
            unsigned int argCount = argsCount[words[0]];
            if (argCount != words.size() - 1)
            {
                std::cout << words[0] << " has " << argCount << " arguments" << std::endl;
                continue;
            }
            //execute
            auto com = (*iter).second;
            words.erase(words.begin());

            try
            {
                com(words);
                std::cout << "OK." << std::endl;
            }
            catch(std::exception &e)
            {
                std::cout << "ERROR: " << e.what() << std::endl;
            }
        }
    }
    catch (std::exception &e)
    {
        std::cout << "ERROR: " << e.what() << std::endl; //error in initialisation
        return 4;
    }
    return 0;
}