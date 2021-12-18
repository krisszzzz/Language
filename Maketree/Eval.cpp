#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "log.h"
#include "Eval.h"
#include <string.h>

int main()
{
    char *expression = (char*)calloc(512, sizeof(char));
    char *expr_begin = expression;
    printf("Enter a mathematic expression with following operation: +-*/, also you can use brackets ()\n");

    scanf("%s", expression);
    
    printf("result: %lf", CalculateResult(&expression));
    free(expr_begin);
}


double CalculateResult(char **expression)
{
    return GetG(expression);
}

static double TransferStringToDouble(char **expression) 
{
    double value = 0;
    while('0' <= **expression && **expression <= '9') 
    {
        value = value * 10 + (**expression - '0');
        ++*expression;
    }
    double frac_part = 0;
    if(**expression == ',' || **expression == '.') {
        ++*expression;
        for(int iter_count = 1; '0' <= **expression && **expression <= '9'; ++iter_count) 
        {
            frac_part += (**expression - '0') / pow(10, iter_count);   
            ++*expression;
        }
    }
    return value + frac_part;

}

double GetN(char **expression)
{
    //double value = 0;
    const char expr_begin = **expression;
    double value = TransferStringToDouble(expression);
    if(**expression == expr_begin) {
        
        double constant = GetC(expression);
        
        if(constant == 0) {
            SyntaxError();
        } else {
           return constant;
        }
    }

    return value;
}

double GetP(char **expression)
{
    if(**expression == '(') {
        ++*expression;
        double value = GetE(expression);
        if(!Require(expression, ')')) {
            SyntaxError();
            return -1;
        }
        ++*expression;
        
        return value;
    } 
    return GetN(expression);
}

double GetT(char **expression)
{
    double value = GetP(expression);

    while(**expression == '*' || **expression == '/')
    {
        char operation = **expression;
        ++*expression;
        double next_value = GetP(expression);

        if(operation == '*') {
            value *= next_value;
        } else {
            value /= next_value;
        }
    }
    return value;
}


double GetE(char **expression)
{
    double value = GetT(expression);

    while(**expression == '+' || **expression == '-')
    {
        char operation    = **expression;
        ++*expression;
        double next_value = GetT(expression);

        if(operation == '+') {
            value += next_value;
        } else {
            value -= next_value;
        }
    }
    return value;
}

double GetC(char **expression)
{
    if(strncmp(*expression, "pi", 2) == 0) {   
        (*expression) += 2;
        return 3.1416;
    }
    if(strncmp(*expression, "e", 1) == 0) {
        ++*expression;
        return 2.7182818;
    }
    return 0;
}

double GetG(char **expression) 
{
    double value = GetE(expression);
    IGNORE_SPACES();
    if(**expression != '$') {
        SyntaxError();
    } 
    return value;
}

void SyntaxError() 
{
    ErrorPrint("Syntax error in expression\n");
}

int Require(char **expression, char to_cmp)
{
    return (**expression == to_cmp) ? 1 : 0;
}

