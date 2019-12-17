#include "CodeChecker.h"

CodeChecker::CodeChecker()
{
    size = 1;
    codeLength = 5;
    codes = new String[size];
}

CodeChecker::CodeChecker(int numCodes, int codeSize)
{
    size = numCodes;
    codeLength = codeSize;
    codes = new String[size];
}

void CodeChecker::Init()
{
    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < codeLength; j++)
        {
            codes[i] = codes[i] + numChar[random(sizeof(numChar))];
        }
    }
}

void CodeChecker::setCode(int index, String code)
{
    codes[index] = code;
}

String CodeChecker::getCode(int index)
{
    return codes[index];
}

bool CodeChecker::readCode(char readChar, int index)
{
    static String guessCode;
    static int guessIndex;
    if (guessIndex != index)
    {
        guessIndex = index;
        guessCode = readChar;
        return false;
    }
    else
    {
        guessCode += readChar;
        if (codes[guessIndex] == guessCode)
        {
            return true;
        }
        else
        {
            guessCode = "";
            return false;
        }
        
    }
}