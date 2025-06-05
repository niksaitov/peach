#include "engine_exceptions.h"

//Default message override
const char* HashKeysNotInitialisedException::what(){
    return "Invalid hash keys: hash keys not initialised";
}

//Default message override
const char* CannotFindMagicNumberException::what(){
    return "Invalid magic numbers: magic number not found";
}

//Default message override
const char* MagicNumberNotInitialisedException::what(){
    return "Invalid magic numbers: magic numbers not initialised";
}
