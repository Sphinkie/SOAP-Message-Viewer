#include "FileParser.h"
#include "mainwindow.h"
using namespace std;


/* ******************************************* */
/* Rappel des variables statiques de la classe */
/* ******************************************* */
bool          FileParser::insideBody;
XML_Parser    FileParser::parser;
std::string   FileParser::contentData;
QMainWindow*  FileParser::mainWindow;

/** ***************************************************************************
 * @brief Ouvre un fichier SVCLOG.
 * @param filename : le nom du fichier SVCLOG à parser.
 * @param fenetre : pointeur sur la fenetre contenant la listView de Message List.
 * **************************************************************************** */
void FileParser::open(std::string filename, const void* fenetre)
{
    fichierSVC.open(filename, ios::in);
    if (fichierSVC.fail())
    {
       cout << "Failed to open " << filename << endl;
       isReady=false;
    }
    else
    {
        cout << "Open " << filename << endl;
        isReady=true;
    }
    FileParser::mainWindow = (MainWindow*)fenetre;
}


/** ****************************************************************************
 * @brief Fonction principale pour déclencher le parsing par Expat.
 * @param useCRLF : TRUE si le fichier contient des Retour Chariot au format Windows (CRLF): typiquement des fichiers XML.
 *                  FALSE si le fichier contient des Retour Chariot au format LINUX (LF) ou 1 seule ligne. Typiquement des fichiers SVCLOG
 * @return Retourne un code d'erreur (0 si tout s'est bien passé).
 ******************************************************************************* */
int FileParser::parse(bool useCRLF)
{
    char buf[BUFFER_SIZE];
    bool done;

    if (!isReady) return XML_ERROR_NO_ELEMENTS;

    FileParser::insideBody=false;
    FileParser::contentData.clear();
    FileParser::parser = XML_ParserCreate(NULL);

    userData.contentType=NONE;
    userData.isTimeKnown=false;
    userData.containsCRLF=useCRLF;
    userData.isComputerKnown=false;

    XML_SetUserData(parser,&userData);
    XML_SetElementHandler(parser, &FileParser::startElementHandler, &FileParser::endElementHandler);
    XML_SetCharacterDataHandler(parser, &FileParser::dataHandler);

    // Par défaut, le svclog ne contient pas de balise <root>. On en ajoute une.
    XML_Parse(parser, "<root>", 6, false);
    do
    {
        fichierSVC.read(buf,BUFFER_SIZE);
        int len = fichierSVC.gcount();           // Nombre de caractères lus lors du read
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
    XML_Parse(parser, "</root>", 7, true);

    XML_ParserFree(parser);
    return XML_ERROR_NONE;
}

/** ****************************************************************************
 * @brief Ce handler (statique) est appelé à chaque balise ouvrante.
 *    Selon la balise, on cree un nouveau message, ou bien on stocke le body du message.
 *    La librairie Expat fournit une methode permettant de savoir à quel octet, on est dans le fichier XML:
 *    XML_GetCurrentByteIndex(XML_Parser p)
 * @param userData : Structure pour les données à mémoriser.
 * @param name : Nom de la balise.
 * @param atts : Tableau des attributs.
 ******************************************************************************* */
void XMLCALL FileParser::startElementHandler(void* userData, const XML_Char* name, const XML_Char **atts)
{
    // A l'intérieur des Body: on ne fait rien.
    if (insideBody) return;

    // on recupère les pointeurs sur les infos de userData
    ContentType *pContentType    = &((UserData*)userData)->contentType;
    bool        *pIsTimeKnown    = &((UserData*)userData)->isTimeKnown;
    bool        *pIsComputerKnown= &((UserData*)userData)->isComputerKnown;
    bool        *pContainsCRLF   = &((UserData*)userData)->containsCRLF;
    uint        *pMessageNb      = &((UserData*)userData)->messageNb;

    if (!strcmp(name,"E2ETraceEvent"))
    {
        // On cree un message. Il sera renseigné au fur et à mesure des startElements.
        Messages::append();
        // On se limite à 32.000 messages dans le fichier
        (*pMessageNb)++;
        if (*pMessageNb > MAX_MESSAGES)
            XML_StopParser(parser,false);
    }
    else if (!strcmp(name,"Correlation"))
    {
        Messages::setActivityID(atts[1]);       // attribut: ActivityId
    }
    else if (!strcmp(name,"MessageLogTraceRecord"))
    {
        Messages::setMessageTime(atts[1]);      // attribut: [0]='Time'   [1]=value
        Messages::setMessageSource(atts[3]);    // attribut: [2]='Source' [3]=value
    }
    else if (!strcmp(name,"Action"))
    {
        // C'est une balise de type ACTION
        *pContentType=ACTION;
        // On vide le contentData: il sera rempli par le prochain appel à dataHandler().
        FileParser::contentData.clear();
    }
    else if (!strcmp(name,"Host"))
    {
        *pContentType=HOST;
        FileParser::contentData.clear();
    }
    else if (!strcmp(name,"To"))
    {
        *pContentType=TO;
        FileParser::contentData.clear();
    }
    else if (Common::endsWith(name,":Body",5))
    {
        int nbCRLF = 0;
        // cout << XML_GetCurrentLineNumber(FileParser::parser) << endl;
        // LineNumber : car Expat compte les retours à la ligne (0A) comme 1 caractère.
        // il ne doit pas y avoir de 0D0A dans un svclog. Expart le compte comme 1 char, et seekg commme 2.
        if (*pContainsCRLF) nbCRLF = XML_GetCurrentLineNumber(FileParser::parser) -1;
        int positionInFile =  XML_GetCurrentByteIndex(FileParser::parser) -6 + nbCRLF;   //  on enleve les 6 caractères de <root>
        Messages::setCurrentBodyStartPosition(positionInFile);
        Messages::setCurrentBodyEndPosition(positionInFile);
        FileParser::insideBody=true;
    }
    else if (!*pIsTimeKnown && !strcmp(name,"TimeCreated"))
    {
        Messages::setLogDate(atts[1]);
        *pIsTimeKnown=true;
    }
    else if (!*pIsComputerKnown && !strcmp(name,"Computer"))
    {
        *pContentType=COMPUTER;
        FileParser::contentData.clear();
    }
    else if (!strcmp(name,"HttpResponse"))
    {
        // Dans le cas des GetPlaylistOnairResponse de niveau interne, on n'a pas de balise <Action>...
        // A la place, on a une balise <HttpResponse> sans contenu texte...
        // On dit que c'est une balise de type ACTION
        *pContentType=ACTION;
        // Et on force le contentData
        FileParser::contentData="http://www.sgt.eu/SOA.ONAIR/HttpResponse/StatusCode ";
        // Note: dataHandler() va ajouter un "OK" à la fin...
    }
}

/** ***************************************************************************
 * @brief Ce Handler est appelé à chaque fois qu'il y a un Content.
 *        Le ContentType a été positionné par le Handler StartElement.
 *        Si le Content est à cheval sur 2 buffers, le dataHandler est appelé deux fois, et il faut concaténer.
 *        \n Voir sur https://stackoverflow.com/questions/609376/geting-xml-data-using-xml-parser-expat#609736
 * @param userData : Non utilisé.
 * @param content : Le contenu textuel de la section.
 * @param length : Le Content n'est pas NULL-terminated: il faut utiliser length.
 * **************************************************************************** */
void XMLCALL FileParser::dataHandler(void* userData, const XML_Char* content, int length)
{
    (void)userData;

    // A l'intérieur des Body: on ne fait rien.
    if (insideBody) return;
    if (contentData.empty())
        contentData.assign(content,length); // Si ContentData est vide: on le renseigne
    else
        contentData.append(content,length); // Sinon, on le complete
}

/** ***************************************************************************
 * @brief Ce Handler est appelé à chaque balise fermante.
 *        On traite les informations stockées dans userData.
 * @param userData : Pointeur sur les données mémorisées lors de lors de l'ouverture des balises.
 * @param name : Le nom de la balise.
 * **************************************************************************** */
void XMLCALL FileParser::endElementHandler(void *userData, const XML_Char *name)
{
    bool *pContainsCRLF   = &((UserData*)userData)->containsCRLF;

     // int position =  XML_GetCurrentByteIndex(FileParser::parser) - 6 + XML_GetCurrentLineNumber(FileParser::parser) -1 + XML_GetCurrentByteCount(parser);
     // cout <<  "ByteIndex " << XML_GetCurrentByteIndex(parser) << " " << position << " " << name << endl;

    // On scrute pour détecter la fin des <Body>.
    if (insideBody)
    {
        if (Common::endsWith(name,":Body",5))
        {
            int nbCRLF = (*pContainsCRLF)? XML_GetCurrentLineNumber(parser)-1 : 0;
            int positionInFile =  XML_GetCurrentByteIndex(parser) + XML_GetCurrentByteCount(parser) -6 +nbCRLF; //  on enleve les 6 caractères de <root>
            Messages::setCurrentBodyEndPosition(positionInFile);
            insideBody=false;
        }
    }
    else
        // On traite la variable statique contentData.
        FileParser::processContentData(userData);
}

/** ***************************************************************************
 * @brief Mémorise dans le message, les informations contenue dans la variable statique contentData,
 *        en fonction du type mémorisé dans les userData.
 * @param userData : Pointeur sur les données mémorisées lors de lors de l'ouverture des balises.
 * **************************************************************************** */
void FileParser::processContentData(void *userData)
{
    ContentType *pContentType    = &((UserData*)userData)->contentType;
    bool        *pIsComputerKnown= &((UserData*)userData)->isComputerKnown;

    switch (*pContentType)
    {
    case HOST:
        Messages::setHost(contentData);
        break;
    case TO:
        Messages::setWsdl(contentData);
        break;
    case COMPUTER:
        Messages::setComputerName(contentData);
        *pIsComputerKnown=true;
        break;
    case ACTION:
    {
        string strListTexte;
        // Note: setAction modifie la string: renvoie que le dernier élement
        Messages::setAction(&contentData);
        // On ajoute l'élément dans le listWidget
        strListTexte.assign(Messages::getCurrentMessageTime());
        strListTexte.append(": ");
        strListTexte.append(contentData);
        // On ajoute le message dans la listView
        ((MainWindow*)FileParser::mainWindow)->addListItem(strListTexte);
        break;
    }
    default:
        break;
    }
    *pContentType= NONE;
    contentData.clear();
}

/** ***************************************************************************
 * @brief Ferme le fichier une fois le parsing terminé.
 * @sa open()
 ******************************************************************************/
void FileParser::close(void)
{
    fichierSVC.close();
    isReady=false;
}
