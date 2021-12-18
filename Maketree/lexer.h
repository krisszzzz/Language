#include "log.h"

enum SYNTAX_ERRORS
{
    EXPECTED_FUNCTION
}


const int   ACCURACY                       = 1e-6;

const char** VAR_DECLARATION_KEYWORD  = { "ingredients"
                                           "ml",
                                           "mg",
                                           "tst."
                                           "p."
                                         };

        

const char** BLOCKS = {
                        "ingredients",
                        "to buy list",
                        "recipe"
                      }

const int   BLOCKS_AMOUNT = sizeof(BLOCKS) / sizeof(BLOCK[0]);


const int   SIZEOF_VAR_DECLARATION_KEYWORD = sizeof(VAR_DECLARATION_KEYWORD) / sizeof(char*);

const char** FUNCTION_DECLARATION_KEYWORD = {                       // Used only to corect work CheckKeyword function
                                             "recipe",
                                             "to buy list"                
                                            };

const int   SIZEOF_FUNCTION_DECLARATION_KEYWORD = sizeof(FUNCTION_DECLARATION_KEYWORD) / sizeof(char*);


const char** KEYWORDS = { "COCKtail",
                          "everyday cumpot"
                        };

const int KEYWORDS_COUNT = sizeof(KEYWORDS) / sizeof(KEYWORDS[0]);

