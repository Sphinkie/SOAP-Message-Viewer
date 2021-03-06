#ifndef BODYTEXTPARSER_H
#define BODYTEXTPARSER_H

#include <string>
#include <stdlib.h>   // pour strncpy_s
#include <Expat/include/expat.h>

class BodyTextParser
{
public:
    static int  parse(std::string blob, bool hideNamespaces);
    static int  parse(char* buffer, bool hideNamespaces);
    static std::string  getFormatedBlob();

    // Methodes du parser
    static void XMLCALL startElementHandler(void *userData, const XML_Char *name, const XML_Char **attrs);
    static void XMLCALL endElementHandler(void *userData, const XML_Char *name);
    static void XMLCALL commentHandler(void *userData, const XML_Char *data);
    static void XMLCALL dataHandler(void *userData, const XML_Char *content, int length);

private:
    static const long  MAX_BODY_SIZE = 1024*28;
    static int         profondeur;
    static bool        elementClosed;
    static bool        keepNamespaces;
    static std::string formatedBlob;
    static std::string errMessage;
//    bool               userData;      // pour Expat

};

#endif // BODYTEXTPARSER_H
