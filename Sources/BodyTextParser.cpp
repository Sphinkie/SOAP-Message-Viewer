/*
 * Ce parser SAX est basé sur la librarie EXPAT.
 * Elle sert à convertir un blob XML (typiquement le body d'un messaeg SOAP)
 * en un XML contenant des balises de coloration syntaxiques pour un affichage dans QT.
 *
 * CLASSE STATIQUE
 *
 *
 * <!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0//EN" "http://www.w3.org/TR/REC-html40/strict.dtd">
 * <html><head><meta name="qrichtext" content="1" /><style type="text/css"> p, li { white-space: pre-wrap; }
 * </style></head>
 * <body style=" font-family:'Courier New'; font-size:9pt; font-weight:400; font-style:normal;">
 * <p style=" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;">
 * <span style=" color:#0000ff;">&lt;soap:Body&gt;</span></p>
 * <p style=" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;">
 *   <span style=" color:#0000ff;">   &lt;OpenOrReuse </span>
 *   <span style=" color:#ff0000;">xmlns</span>
 *   <span style=" color:#0000ff;">=</span>
 *   <span style=" color:#aa00ff;">&quot;http://www.sgt.eu/VEDA.SOA/Session.v1&quot; </span>
 * </p>
 * <p style=" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;">
 *   <span style=" color:#0000ff;">      &lt;ClientLogin&gt;</span>
 * </p>
 * <p style=" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;">
 *   <span style=" color:#0000ff;">         &lt;HostName&gt;</span>
 *   <span style=" font-weight:600; color:#000000;">00XDSVMAM03200</span>
 *   <span style=" color:#0000ff;">&lt;/HostName&gt;</span>
 * </p>
 * <p style=" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;">
 *   <span style=" color:#0000ff;">      &lt;/ClientLogin&gt;</span>
 * </p>
 * <p style=" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;">
 *   <span style=" color:#0000ff;">   &lt;/OpenOrReuse&gt;</span>
 * </p>
 * <p style=" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;">
 *   <span style=" color:#0000ff;">&lt;/soap:Body&gt;</span>
 * </p>
 * </body>
 * </html>
 *
 */
#include "BodyTextParser.h"

/* Variables statiques de la Classe */
std::string BodyTextParser::formatedBlob;
std::string BodyTextParser::errMessage;
int         BodyTextParser::profondeur;
bool        BodyTextParser::elementClosed;
bool        BodyTextParser::keepNamespaces;


/* ****************************************************************************
 * Effectue le parsing d'un XML sous forme de std::string
 * **************************************************************************** */
int BodyTextParser::parse(std::string blob, bool hideNamespaces)
{
    int blobSize= blob.length()+1;
    char* buffer = new char [blobSize];

    strcpy_s(buffer,blobSize, blob.c_str());
    int errcode = parse(buffer, !hideNamespaces);

    delete[] buffer;
    return errcode;
}

/* ****************************************************************************
 * Effectue le parsing d'un XML sous forme de char*
 * **************************************************************************** */
int BodyTextParser::parse(char* buffer, bool hideNamespaces)
{
    int bufferSize= strlen(buffer)+1;
    if (bufferSize>MAX_BODY_SIZE)
    {
        formatedBlob.assign("Messsage body is too long. <br/> Maximum size allowed is ");
        formatedBlob.append(std::to_string(MAX_BODY_SIZE/1024));
        formatedBlob.append("kB.");
        return XML_ERROR_NO_MEMORY;
    }

    XML_Error errcode  = XML_ERROR_NONE;

    keepNamespaces= !hideNamespaces;
    profondeur    = 0;
    formatedBlob.clear();
    XML_Parser parser = XML_ParserCreate(NULL);

    XML_SetElementHandler(parser, &BodyTextParser::startElementHandler, &BodyTextParser::endElementHandler);
    XML_SetCharacterDataHandler(parser, &BodyTextParser::dataHandler);
    XML_SetCommentHandler(parser,&BodyTextParser::commentHandler);
    XML_Status status = XML_Parse(parser, buffer, bufferSize, true);
    if (status == XML_STATUS_ERROR)
    {
        errcode  = XML_GetErrorCode(parser);
        errMessage.assign("XML error code " + std::to_string(errcode));
        errMessage.append(": " + std::string(XML_ErrorString(errcode)));
        errMessage.append(", at line " +std::to_string(XML_GetCurrentLineNumber(parser)));
    }
    XML_ParserFree(parser);
    return errcode;
}

/* ****************************************************************************
 * Renvoie le XML qui a été formaté par le Parser.
 * C-a-d avec des balises html de coloration syntaxique.
 * **************************************************************************** */
std::string BodyTextParser::getFormatedBlob()
{
    return BodyTextParser::formatedBlob;
}

/* ****************************************************************************
 * Handler (static) appelé à chaque balise ouvrante
 * **************************************************************************** */
void XMLCALL BodyTextParser::startElementHandler(void* userData, const XML_Char* name, const XML_Char **attrs)
{
    (void)userData;

    if (profondeur>0) formatedBlob.append("<br/>");
    formatedBlob.append("<span style='color:#0000ff;");
    formatedBlob.append(" text-indent:20;");                 // fonctionne sur <p>
    // formatedBlob.append(" margin-left:20px;");
    // formatedBlob.append(" Padding: 0 0 0 50px;");
    // formatedBlob.append(std::to_string(profondeur));
    formatedBlob.append("'>");
    for (int i=0; i<profondeur;i++) formatedBlob.append("&nbsp;&nbsp;&nbsp;");
    formatedBlob.append("&lt;");
    formatedBlob.append(name);
    for (int i=0; attrs[i]; i+=2)
    {
        if ((keepNamespaces) || (strncmp(attrs[i],"xmlns",5)))
        {
        formatedBlob.append(" ");
        formatedBlob.append("<span style='color:#ff0000'>");
        formatedBlob.append(attrs[i]);
        formatedBlob.append("</span><span style='color:#aa00ff'>");
        formatedBlob.append("=\"");
        formatedBlob.append(attrs[i+1]);
        formatedBlob.append("\"</span>");
        }
    }
    formatedBlob.append("&gt;</span>");
    profondeur++;
    elementClosed=false;
}

/* ****************************************************************************
 * Ce Handler est appelé à chaque balise fermante.
 * **************************************************************************** */
void XMLCALL BodyTextParser::endElementHandler(void *userData, const XML_Char *name)
{
    (void)userData; // pour enlever le warning....

    profondeur--;
    if(elementClosed)
    {
        formatedBlob.append("<br/>");
        for (int i=0; i<profondeur;i++) formatedBlob.append("&nbsp;&nbsp;&nbsp;");
    }
    formatedBlob.append("<span style='color:#0000ff;'>&lt;/");
    formatedBlob.append(name);
    formatedBlob.append("&gt;</span>");
    elementClosed=true;
}


/* ****************************************************************************
 * Ce Handler est appelé à chaque fois qu'il y a un Content.
 * Note: Content n'est pas NULL-terminated: il faut utiliser length.
 * **************************************************************************** */
void XMLCALL BodyTextParser::dataHandler(void *userData, const XML_Char* content, int length)
{
    (void)userData; // pour enlever le warning....

    formatedBlob.append("<span style='color:#000000'><b>");
    formatedBlob.append(content,length);
    formatedBlob.append("</b></span>");
}

/* ****************************************************************************
 * Ce Handler est appelé à chaque fois qu'il y a un Commentaire.
 * **************************************************************************** */
void XMLCALL BodyTextParser::commentHandler(void *userData, const XML_Char *data)
{
    (void)userData; // pour enlever les warnings....
    formatedBlob.append("<span style='color:#28b463'>&lt;!-- ");
    formatedBlob.append(data,strlen(data));
    formatedBlob.append(" --></span>");
}

