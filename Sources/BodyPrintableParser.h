#ifndef BODYPRINTABLEPARSER_H
#define BODYPRINTABLEPARSER_H

#include <fstream>
#include <Expat/include/expat.h>



class BodyPrintableParser
{
public:
    BodyPrintableParser(std::string filename);
    ~BodyPrintableParser();
    int saveBody(std::string blob);

    // Methodes du parser
    static void XMLCALL startElementHandler(void *userData, const XML_Char *name, const XML_Char **attrs);
    static void XMLCALL endElementHandler(void *userData, const XML_Char *name);
    static void XMLCALL commentHandler(void *userData, const XML_Char *data);
    static void XMLCALL dataHandler(void *userData, const XML_Char *content, int length);

private:
    static std::ofstream fichierBody;
    static const long  MAX_BODY_SIZE = 1024*512;  /** Le buffer de Expat v2.2 était limité à 32kB. Avec Expat v2.4, cela semble fonctionner avec 5128kB... */

    // userData qui sont passés aux Handlers
    struct UserData
    {
        int   depth=0;
        bool  isElementClosed = false;
    };
    UserData userData;
    bool isGood = true;

};

#endif // BODYPRINTABLEPARSER_H
