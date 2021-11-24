#ifndef FILEPARSER_H
#define FILEPARSER_H

#include <stdio.h>
#include <stdlib.h>     // pour strncpy_s
#include <iostream>
#include <fstream>
#include <QMainWindow>
#include <Expat/include/expat.h>
#include "messages.h"
#include "Common.h"

/** ****************************************************************************
 * @brief La classe FileParser parse un fichier .SCVLOG et insère autant de lignes
 *        dans la ListView de la fenêtre qu'il y a de messages SOAP dans le fichier.
 *        La classe utilise le parser XML 'Expat' qui est open-source, très rapide,
 *        et SAX (parsing au fil de l'eau).
 *        Elle gère les gros fichiers grace à un système de buffer.
 * @author David de Lorenzo
 * **************************************************************************** */
class FileParser
{
public:
    void open(std::string filename, const void* fenetre);
    int  parse(bool useCRLF);
    void close();

    static void XMLCALL startElementHandler(void *userData, const XML_Char *name, const XML_Char **atts);
    static void XMLCALL endElementHandler(void *userData, const XML_Char *name);
    static void XMLCALL dataHandler(void *userData, const XML_Char *content, int length);
    static void processContentData(void *userData);

    static const unsigned int MAX_MESSAGES = 32000;   ///< Qt ne peut afficher que 32762 messages dans la ListView

private:
    bool isReady=false;
    std::ifstream fichierSVC;

    // userData qui sont passés aux Handlers
    enum ContentType {NONE, HOST, TO, ACTION, COMPUTER};
    struct UserData
    {
        ContentType  contentType = NONE;
        bool         isTimeKnown = false;
        bool         isComputerKnown = false;
        bool         containsCRLF = false;
        unsigned int messageNb = 0;
    };
    UserData userData;

    // Variables statiques utilisées par les membres statiques
    static bool          insideBody;
    static std::string   contentData;
    static QMainWindow*  mainWindow;
    static XML_Parser    parser;                ///< Parser Expat. On en a besoin dans les fonctions statiques pour repérer le début et la fin des Body
    static const long    BUFFER_SIZE = 1024*8;  ///< Recommandation Expat: doit etre largement en dessous de INT_MAX soit 32.767.

};

#endif // FILEPARSER_H
