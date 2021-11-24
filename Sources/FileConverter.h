#ifndef FILECONVERTER_H
#define FILECONVERTER_H

#include <string>
#include <iostream>     // pour cout
#include <fstream>
#include <stack>
#include <Expat/include/expat.h>

/** ****************************************************************************
 * @brief FileConverter transforme un fichier \c .SCVLOG en un fichier \c .FASTER.XML
 *        en supprimant des balises inutiles, ce qui accélère les futurs parsings
 *        du fichier.
 *
 * La classe FileConverter utilise le parser XML \a Expat qui est open-source,
 * très rapide, et SAX (parsing au fil de l'eau).
 * Elle gère les gros fichiers grace à un système de buffer.
 * *****************************************************************************/
class FileConverter
{

public:
    void open(std::string filename);
    int  convert();
    void close();
    bool ready();
    std::string getErrorMessage();

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
    static const long               BUFFER_SIZE = 1024*8;  //!< Recommandation Expat: doit etre largement en dessous de INT_MAX soit 32.767.

};

#endif // FILECONVERTER_H
