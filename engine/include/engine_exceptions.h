#ifndef ENGINE_EXCEPTIONS_H
#define ENGINE_EXCEPTIONS_H

#include <exception>

//Create a custon exception inheriting from the standart exception class
class HashKeysNotInitialisedException : public std::exception{
    
    public:
    //Override the default message
        const char* what(); 
};

//Create a custon exception inheriting from the standart exception class
class CannotFindMagicNumberException : public std::exception{

    public:
    //Override the default message
        const char* what(); 

};

//Create a custon exception inheriting from the standart exception class
class MagicNumberNotInitialisedException : public std::exception{
    
    public:
    //Override the default message
        const char* what(); 
        
};

#endif