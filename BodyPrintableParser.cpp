#include "BodyPrintableParser.h"

std::ofstream BodyPrintableParser::fichierBody;

/* ****************************************************************************
 * Constructeur: ouvre le fichier XML
 * **************************************************************************** */
BodyPrintableParser::BodyPrintableParser(std::string filename)
{
    fichierBody.open(filename);
    isGood = fichierBody.good();
}


/* ****************************************************************************
 * Destructeur: referme le fichier XML
 * **************************************************************************** */
BodyPrintableParser::~BodyPrintableParser()
{
    fichierBody.close();
}


/* ****************************************************************************
 * Fonction principale pour déclencher le parsing par Expat
 * Retourne un code d'erreur (0 si tout s'est bien passé)
 * **************************************************************************** */
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



/* ****************************************************************************
 * Handler appelé à chaque balise ouvrante
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

/* ****************************************************************************
 * Ce Handler est appelé à chaque balise fermante.
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


/* ****************************************************************************
 * Ce Handler est appelé à chaque fois qu'il y a un Content.
 * Note: Content n'est pas NULL-terminated: il faut utiliser length.
 * **************************************************************************** */
void XMLCALL BodyPrintableParser::dataHandler(void *userData, const XML_Char* content, int length)
{
    (void)userData; // pour enlever le warning....
    fichierBody.write(content,length);
}

/* ****************************************************************************
 * Ce Handler est appelé à chaque fois qu'il y a un Commentaire.
 * **************************************************************************** */
void XMLCALL BodyPrintableParser::commentHandler(void *userData, const XML_Char *data)
{
    (void)userData; // pour enlever les warnings....
    fichierBody << "<!-- " << data << " -->";
}
