/* ****************************************************************************
 * FileConverter transforme un fichier .SCVLOG en un fichier .FASTER.XML
 * en supprimant des balises inutiles, ce qui accélère les futurs parsings
 * du fichier.
 *
 * Utilise le parser XML 'Expat' qui est open-source, très rapide, et SAX (parsing au fil de l'eau).
 * Elle gère les gros fichiers grace à un système de buffer.
 * **************************************************************************** */
#include "FileConverter.h"
using namespace std;

// Initialisation des variables statiques
std::string             FileConverter::contentData;
std::ofstream           FileConverter::fichierFASTER;
std::stack<std::string> FileConverter::baliseStack;

/* ****************************************************************************
 *   filename: le nom du fichier SVCLOG à convertir.
 * **************************************************************************** */
void FileConverter::open(std::string filename)
{
    std::string filenameOut = filename.substr(0, filename.length()-7);  // on enleve l'extension svclog
    filenameOut.append(".faster.xml");
    isReady=false;

    fichierSVC.open(filename, ios::in);
    if (fichierSVC.fail())
    {
        errorMessage.assign("Failed to open ");
        errorMessage.append(filename);
        cout << errorMessage << endl;
    }
    else
    {
        cout << filename << " opened" << endl;
        fichierFASTER.open(filenameOut, ios::out);
        if(fichierFASTER.fail())
        {
            errorMessage.assign("Failed to open ");
            errorMessage.append(filenameOut);
            cout << errorMessage << endl;
        }
        else
        {
            cout << filenameOut << " opened" << endl;
            isReady=true;
        }
    }
}

/* ****************************************************************************
 * Fonction principale pour déclencher le parsing par Expat
 * Retourne un code d'erreur (0 si tout s'est bien passé)
 * **************************************************************************** */
int FileConverter::convert()
{
    char buf[BUFFER_SIZE];
    bool done;

    XML_Parser parser;
    if (!isReady) return XML_ERROR_NO_ELEMENTS;

    FileConverter::contentData.clear();
    while (!FileConverter::baliseStack.empty()) baliseStack.pop();
    this->userData.depth=0;
    this->userData.keepSubElements=false;
    this->userData.isElementClosed=false;
    parser = XML_ParserCreate(NULL);
    XML_SetUserData(parser,&this->userData);
    XML_SetElementHandler(parser, &FileConverter::startElementHandler, &FileConverter::endElementHandler);
    XML_SetCharacterDataHandler(parser, &FileConverter::dataHandler);

    // Par défaut, le svclog ne contient pas de balise <root>.
    // On est obligé d'en ajouter une (sinon XML_ERROR)
    XML_Parse(parser, "<root>", 6, false);
    do
    {
        fichierSVC.read(buf,BUFFER_SIZE);
        unsigned int len = fichierSVC.gcount();           // Nombre de caractères lus lors du read (ne pas depasser 35.000)
        done = fichierSVC.eof();
        XML_Status status = XML_Parse(parser, buf, len, false);
        if (status == XML_STATUS_ERROR)
        {
            XML_Error  errcode = XML_GetErrorCode(parser);
            std::string errmsg = XML_ErrorString(errcode);
            cout << "XML error code " << errcode << ": " << errmsg << endl;
            XML_ParserFree(parser);
            return errcode;
        }
    } while (!done);
    while (!FileConverter::baliseStack.empty())
    {
        FileConverter::fichierFASTER << endl;
        for (unsigned int i=1; i<baliseStack.size(); i++) FileConverter::fichierFASTER << "\t";
        FileConverter::fichierFASTER << "</" << FileConverter::baliseStack.top() << ">" ;
        FileConverter::baliseStack.pop();
    }
    XML_Parse(parser, "</root>", 7, true);
    XML_ParserFree(parser);

    return XML_ERROR_NONE;
}

/* ****************************************************************************
 * Handler (statique) appelé à chaque balise ouvrante
 * **************************************************************************** */
void XMLCALL FileConverter::startElementHandler(void* userData, const XML_Char* name, const XML_Char **attrs)
{
    // on recupère les pointeurs sur les infos de userData
    int  *pDepth          = &((UserData*)userData)->depth;
    bool *pKeepSubElements= &((UserData*)userData)->keepSubElements;
    bool *pIsElementClosed= &((UserData*)userData)->isElementClosed;
    bool writeTag = false;

    // cout << "startElement " << name << endl;

    // Cas à l'intérieur d'un Boby
    if (*pKeepSubElements)
        writeTag=true;

    // Cas de debut de Body
    else if ( (0==strcmp(name,"s:Body")) || (0==strcmp(name,"soap:Body")) )
    {
        writeTag=true;
        *pKeepSubElements=true;
    }

    // Cas des autres balises à conserver
    else if
        (  (0==strcmp(name,"E2ETraceEvent"))
        || (0==strcmp(name,"TimeCreated"))
        || (0==strcmp(name,"Correlation"))
        || (0==strcmp(name,"Computer"))
        || (0==strcmp(name,"MessageLogTraceRecord"))
        || (0==strcmp(name,"Host"))
        || (0==strcmp(name,"SOAPAction"))
        || (0==strcmp(name,"To"))
        || (0==strcmp(name,"Action"))
       )
    {
        writeTag=true;
    }

    if (writeTag)
    {
        // Indentation
        FileConverter::fichierFASTER << endl;
        for (int i=0; i<*pDepth;i++) FileConverter::fichierFASTER << "\t";
        // on reporte la balise ouvrante dans le fichier de sortie.
        FileConverter::fichierFASTER << "<" << name;
        // on reporte les attributs.
        for (int i=0; attrs[i]; i+=2)
        {
            FileConverter::fichierFASTER << " " << attrs[i] << "=\"" << attrs[i+1] << "\"";
        }
        FileConverter::fichierFASTER << ">";
        FileConverter::contentData.clear();
        FileConverter::baliseStack.push(name);
        *pIsElementClosed=false;
        (*pDepth)++;
    }
}

/* ****************************************************************************
 * Ce Handler est appelé à chaque fois qu'il y a un Content.
 * **************************************************************************** */
void XMLCALL FileConverter::dataHandler(void* userData, const XML_Char* content, int length)
{
    (void)userData;

    if (FileConverter::contentData.empty())
        FileConverter::contentData.assign(content,length); // Si ContentData est vide: on le renseigne
    else
        FileConverter::contentData.append(content,length); // Sinon, on le complete
}

/* ****************************************************************************
 * Ce Handler est appelé à chaque balise fermante.
 * **************************************************************************** */
void XMLCALL FileConverter::endElementHandler(void *userData, const XML_Char *name)
{
    int  *pDepth          = &((UserData*)userData)->depth;
    bool *pKeepSubElements= &((UserData*)userData)->keepSubElements;
    bool *pIsElementClosed= &((UserData*)userData)->isElementClosed;
    bool writeTag = false;

    // Si on est dans un body, on reporte bien sûr les balises fermantes
    if (*pKeepSubElements)
        writeTag= true;

    // Liste des Elements à conserver
    if (   (0==strcmp(name,"E2ETraceEvent"))
           || (0==strcmp(name,"TimeCreated"))
           || (0==strcmp(name,"Correlation"))
           || (0==strcmp(name,"Computer"))
           || (0==strcmp(name,"MessageLogTraceRecord"))
           || (0==strcmp(name,"Host"))
           || (0==strcmp(name,"SOAPAction"))
           || (0==strcmp(name,"To"))
           || (0==strcmp(name,"Action"))
           )
        writeTag= true;

    // Cas de la fin d'un Body
    if (  (0==strcmp(name,"s:Body"))
       || (0==strcmp(name,"soap:Body")) )
    {
        *pKeepSubElements=false;
        writeTag= true;
    }

    if (writeTag)
    {
        // on reporte le content dans le fichier de sortie.
        FileConverter::fichierFASTER << FileConverter::contentData;
        // On indente:
        (*pDepth)--;
        if(*pIsElementClosed)
        {
            FileConverter::fichierFASTER << endl;
            for (int i=0; i<*pDepth;i++) FileConverter::fichierFASTER << "\t";
        }
        // on reporte la balise fermante dans le fichier de sortie.
        FileConverter::fichierFASTER << "</" << name << ">";
        FileConverter::baliseStack.pop();
    }

    FileConverter::contentData.clear();
    *pIsElementClosed=true;
}

/* ****************************************************************************
 * Ferme les fichiers une fois la conversion terminée
 * **************************************************************************** */
void FileConverter::close(void)
{
    isReady=false;
    fichierSVC.close();
    FileConverter::fichierFASTER.close();
}
