#ifndef BODY_PRINTABLEPARSER_H
#define BODY_PRINTABLEPARSER_H

#include <fstream>
#include <Expat/include/expat.h>

/** ****************************************************************************
 * @brief La classe BodyPrintableParser parse le flux XML d'un Body, et l'écrit
 *        dans un fichier texte.
 * @author David de Lorenzo
 **************************************************************************** */
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
    static std::ofstream fichierBody;             ///< Le fichier à écrire.
    static const long  MAX_BODY_SIZE = 1024*512;  ///< Le buffer de Expat v2.2 était limité à 32kB. Avec Expat v2.4, cela semble fonctionner avec 512kB...

    // userData qui sont passés aux Handlers
    struct UserData
    {
        int   depth=0;                     ///< Indique le niveau d'imbrication de la balise courante.
        bool  isElementClosed = false;     ///< Indique si la balise courante est ouverte ou fermée.
    };
    UserData userData;                     ///< Données temporaire d'analyse du flux.
    bool isGood = true;                    ///< Indique si le fichier a bien pu être ouvert.

};

#endif // BODY_PRINTABLEPARSER_H
