#ifndef ARGPARSER_HPP
# define ARGPARSER_HPP

# include <iostream>
# include <string>
# include <netdb.h>
# include <arpa/inet.h>
# include <stdlib.h> //for exit

class ArgParser
{
    public:
        ArgParser(int argc, char** args);
        ~ArgParser();
        std::string&    operator[](int num);
        int             getPort(void);
        std::string&    getAddress(void);
    private:
        void        printUsage(void);
        bool        addressCheck(void);
        bool        portCheck(void);
        std::string _address;
        std::string _port;
        int         _portInt;
};

#endif
