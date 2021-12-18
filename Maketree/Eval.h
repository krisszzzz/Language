#if !defined EVAL_INCLUDED
#define EVAL_INCLUDED

double CalculateResult(char **expression);

double GetN(char **expression);

double GetP(char **expression);

double GetT(char **expression);

double GetE(char **expression);

double GetC(char **expression);

double GetG(char **expression);

void SyntaxError();

int Require(char **expression, char to_cmp);

#endif