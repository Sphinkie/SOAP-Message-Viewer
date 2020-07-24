#ifndef FILECONVERTER_H
#define FILECONVERTER_H

#include <string>
#include <iostream>     // pour cout
#include <fstream>
#include <stack>
#include <Expat/include/expat.h>


class FileConverter
{

public:
    void open(std::string filename);
    int  convert();
    void close();
    bool ready() {return isReady;};
    std::string getErrorMessage() {return errorMessage;};

    static void XMLCALL startElementHandler(void *userData, const XML_Char *name, const XML_Char **attrs);
    static void XMLCALL endElementHandler(void *userData, const XML_Char *name);
    static void XMLCALL dataHandler(void *userData, const XML_Char *content, int length);

private:
    std::ifstream fichierSVC;
    bool          isReady=false;
    std::string   errorMessage;

    // userData qui sont passés aux Handlers
    struct UserData
    {
        int   depth=0;
        bool  keepSubElements = false;
        bool  isElementClosed = false;
    };
    UserData userData;

    // Variables statiques utilisées par les membres statiques
    static std::string              contentData;
    static std::ofstream            fichierFASTER;
    static std::stack<std::string>  baliseStack;
    static const long               BUFFER_SIZE = 1024*8;  // recommandation Expat: doit etre largement en dessous de INT_MAX soit 32.767.

};

#endif // FILECONVERTER_H
