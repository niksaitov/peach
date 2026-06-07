#ifndef ENGINE_EXCEPTIONS_H
#define ENGINE_EXCEPTIONS_H

#include <exception>

// Create a custon exception inheriting from the standart exception class
class HashKeysNotInitialisedException : public std::exception
{

public:
    // Override the default message
    const char *what() const noexcept override
    {
        return "Invalid hash keys: hash keys not initialised";
    }
};

// Create a custon exception inheriting from the standart exception class
class CannotFindMagicNumberException : public std::exception
{

public:
    // Override the default message
    const char *what() const noexcept override
    {
        return "Invalid magic numbers: magic number not found";
    }
};

// Create a custon exception inheriting from the standart exception class
class MagicNumberNotInitialisedException : public std::exception
{

public:
    // Override the default message
    const char *what() const noexcept override
    {
        return "Invalid magic numbers: magic numbers not initialised";
    }
};

#endif