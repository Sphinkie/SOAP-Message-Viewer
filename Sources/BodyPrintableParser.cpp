#include "BodyPrintableParser.h"

/* ******************************************* */
/* Rappel des variables statiques de la classe */
/* ******************************************* */
std::ofstream BodyPrintableParser::fichierBody;     ///< Fichier XML généré.

/** ****************************************************************************
 * @brief Constructeur: ouvre le fichier XML à écrire.
 * @param filename : Le nom du fichier XML.
 ** *****************************************************************************/
BodyPrintableParser::BodyPrintableParser(std::string filename)
{
    fichierBody.open(filename);
    isGood = fichierBody.good();
}


/** ****************************************************************************
 * @brief Destructeur: referme le fichier XML.
 * @sa BodyPrintableParser()
 ** **************************************************************************** */
BodyPrintableParser::~BodyPrintableParser()
{
    fichierBody.close();
}


/** ****************************************************************************
 * @brief Fonction principale. Parse le flux XML fourni avec Expat, et l'écrit dans un fichier XML.
 * @param blob : le flux XML à parser.
 * @return an error code (0 si tout s'est bien passé)
 ** *************************************************************************** */
int BodyPrintableParser::saveBody(std::string blob)
{
    int bufferSize= blob.length()+1;
    if (!isGood) return XML_ERROR_NO_ELEMENTS;
    XML_Error errcode = XML_ERROR_NONE;

    if (bufferSize<MAX_BODY_SIZE)
    {
        this->userData.depth=0;
        this->userData.isElementClosed=false;
        XML_Parser parser = XML_ParserCreate(NULL);

        XML_SetUserData(parser,&this->userData);
        XML_SetStartElementHandler(parser, &BodyPrintableParser::startElementHandler);
        XML_SetEndElementHandler(parser, &BodyPrintableParser::endElementHandler);
        XML_SetCharacterDataHandler(parser, &BodyPrintableParser::dataHandler);

        XML_Status status = XML_Parse(parser, blob.c_str(), bufferSize, true);
        if (status == XML_STATUS_ERROR)
            {
                errcode = XML_GetErrorCode(parser);
                // errorMessage.assign(XML_ErrorString(errcode));
            }
        XML_ParserFree(parser);
    }
    return errcode;
}


/** ***************************************************************************
 * @brief Handler appelé à chaque balise ouvrante.
 *        On écrit le nom de la balise et les attributs dans le fichier texte.
 * @param userData : Pointeur sur les données temporaires.
 * @param name : Nom de la balise.
 * @param attrs : Tableau des attributs.
 * **************************************************************************** */
void XMLCALL BodyPrintableParser::startElementHandler(void* userData, const XML_Char* name, const XML_Char **attrs)
{
    int  *pDepth          = &((UserData*)userData)->depth;
    bool *pIsElementClosed= &((UserData*)userData)->isElementClosed;

    if (*pDepth>0) fichierBody << std::endl;
    for (int i=0; i<*pDepth; i++) fichierBody << "    ";
    fichierBody << "<" << name;
    for (int i=0; attrs[i]; i+=2)
    {
        fichierBody << " " << attrs[i] << "=\"" << attrs[i+1] << "\"";
    }
    fichierBody << ">";
    (*pDepth)++;
    *pIsElementClosed=false;
}


/** ***************************************************************************
 * @brief Ce Handler est appelé à chaque balise fermante.
 *        On écrit le nom de la balise dans le fichier texte.
 * @param userData : Pointeur sur les données temporaires.
 * @param name : Nom de la balise.
 * **************************************************************************** */
void XMLCALL BodyPrintableParser::endElementHandler(void *userData, const XML_Char *name)
{
    int  *pDepth          = &((UserData*)userData)->depth;
    bool *pIsElementClosed= &((UserData*)userData)->isElementClosed;

    (*pDepth)--;
    if(*pIsElementClosed)
    {
        fichierBody << std::endl;
        for (int i=0; i<*pDepth; i++) fichierBody << "    ";
    }
    fichierBody << "</" << name << ">";
    *pIsElementClosed=true;
}


/** ***************************************************************************
 * @brief Ce Handler est appelé à chaque fois qu'il y a un contenu textuel.
 *        On écrit le contenu textuel dans le fichier texte.
 * @param userData : Non utilisé.
 * @param content : Contenu textuel de la section XML.
 * @param length : Le contenu textuel n'étant pas NULL-Terminated: il faut utiliser length.
 * **************************************************************************** */
void XMLCALL BodyPrintableParser::dataHandler(void *userData, const XML_Char* content, int length)
{
    (void)userData; // pour enlever le warning....
    fichierBody.write(content,length);
}


/** ***************************************************************************
 * @brief Ce Handler est appelé à chaque fois qu'il y a un Commentaire.
 *        On écrit le texte du commentaire dans le fichier texte.
 * @param userData : Non utilisé.
 * @param data : Le texte du commentaire.
 * **************************************************************************** */
void XMLCALL BodyPrintableParser::commentHandler(void *userData, const XML_Char *data)
{
    (void)userData; // pour enlever les warnings....
    fichierBody << "<!-- " << data << " -->";
}
