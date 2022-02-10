#include "ArgParser.hpp"

ArgParser::ArgParser(int argc, char** args)
{
    if (argc != 3)
        this->printUsage();

    this->_address = args[1];
    this->_port = args[2];
    
    if (this->addressCheck() == false)
        this->printUsage();
    if (this->portCheck() == false)
        this->printUsage();

    return ;
}

ArgParser::~ArgParser(){}

void    ArgParser::printUsage(void)
{
    std::cout << "Usage: ./client [IP address] [Port] [Password]\nExample: ./client 127.17.0.1 44500\n";
    exit(1);
}

bool    ArgParser::addressCheck(void)
{
    // If localhost address is 127.0.0.1
    if (this->_address == "localhost")
        this->_address = "127.0.0.1";
    

    // Check if invalid or if alias exists in hosts file
    if (!isdigit(this->_address[0]) || !isdigit(this->_address[this->_address.length() - 1]))
    {
        struct hostent *he = NULL;
        struct in_addr **addr_list = NULL;
        if ((he = gethostbyname(this->_address.c_str())) == NULL)
        {
            return false;
        }
        else
        {
            addr_list = (struct in_addr **)he->h_addr_list;
            this->_address = std::string(inet_ntoa(*addr_list[0]));
        }
    }

    // Check for correct format
    int dots = 0;
    for(unsigned long i=0;i<this->_address.length();i++)
    {
        if (this->_address[i] == '.')
            dots++;
    }
    if (dots != 3)
        return false;

    for(unsigned long i=0;i<this->_address.length();i++)
    {
        if (this->_address[i] != '.' && !isdigit(this->_address[i]))
            return false;
        if ( (i != 0) && (i != (this->_address.length() - 1)) && this->_address[i] == '.' )
        {
            if (!isdigit(this->_address[i-1]) || !isdigit(this->_address[i+1]))
                return false;
        }
    }

    return (true);
}

bool    ArgParser::portCheck(void)
{
    for(unsigned long i=0;i<this->_port.length();i++)
    {
        if (!isdigit(this->_port[i]))
            return false;
    }
    const char *temp = this->_port.c_str();
    this->_portInt = atoi(temp);
    if (this->_portInt < 0 || this->_portInt > 65535)
        return false;
    return true;
}

int ArgParser::getPort(void)
{
    return (this->_portInt);
}

std::string&    ArgParser::getAddress(void)
{
    return (this->_address);
}

std::string&    ArgParser::operator[](int num)
{
    if (num == 1)
        return (this->_address);
    else
        return (this->_port);
}
