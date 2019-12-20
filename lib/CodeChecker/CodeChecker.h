#include <Arduino.h>
const char numChar [] = "0123456789*";

class CodeChecker
{
    private:
        String * codes;
        int size;
        int codeLength;
    public:
        CodeChecker();
        CodeChecker(int numCodes, int codeSize);
        void Init();
        void setCode(int index, String code);
        String getCode(int index); 
        bool readCode(char readChar, int index);
};