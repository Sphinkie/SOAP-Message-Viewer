#ifndef BODYTEXTPARSER_H
#define BODYTEXTPARSER_H

#include <string>
#include <stdlib.h>   // pour strncpy_s
#include <Expat/include/expat.h>

/** ****************************************************************************
 * @brief La classe \b statique BodyTextParser analyse le Body XML d'un message,
 *        pour l'afficher sous forme de texte simple dans l'onglet "Text View".
 *
 * Elle sert à convertir un blob XML (typiquement le body d'un message SOAP)
 * en une string formatée, contenant des balises de coloration syntaxique, pour un affichage dans QT.
 * Le parser SAX est basé sur la librarie EXPAT.
 *
 * @author David de Lorenzo
 **************************************************************************** */
class BodyTextParser
{
public:
    static int  parse(std::string blob, bool hideNamespaces);
    static int  parse(char* buffer, bool hideNamespaces);
    static std::string  getFormatedBlob();

    // Methodes statiques du Parser
    static void XMLCALL startElementHandler(void *userData, const XML_Char *name, const XML_Char **attrs);
    static void XMLCALL endElementHandler(void *userData, const XML_Char *name);
    static void XMLCALL commentHandler(void *userData, const XML_Char *data);
    static void XMLCALL dataHandler(void *userData, const XML_Char *content, int length);

private:
    static const long  MAX_BODY_SIZE = 1024*512;       ///< 512kB : Taille max des Body (il y a une limitation dans Expat)
    static int         profondeur;
    static bool        elementClosed;
    static bool        keepNamespaces;
    static std::string formatedBlob;                   ///< Le texte à afficher, avec indentation et coloration syntaxique.
    static std::string errMessage;

};

#endif // BODYTEXTPARSER_H


/** ****************************************************************************
 * @class BodyTextParser
 * @details Exemple de formatedBlob:
 *
 \verbatim
 <!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0//EN" "http://www.w3.org/TR/REC-html40/strict.dtd">
 <html>
    <head>
      <meta name="qrichtext" content="1" />
      <style type="text/css"> p, li { white-space: pre-wrap; }</style>
    </head>
    <body style=" font-family:'Courier New'; font-size:9pt; font-weight:400; font-style:normal;">
       <p style=" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;">
          <span style=" color:#0000ff;">&lt;soap:Body&gt;</span>
       </p>
       <p style=" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;">
          <span style=" color:#0000ff;">   &lt;OpenOrReuse </span>
          <span style=" color:#ff0000;">xmlns</span>
          <span style=" color:#0000ff;">=</span>
          <span style=" color:#aa00ff;">&quot;http://www.sgt.eu/VEDA.SOA/Session.v1&quot; </span>
       </p>
       <p style=" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;">
          <span style=" color:#0000ff;">      &lt;ClientLogin&gt;</span>
       </p>
       <p style=" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;">
          <span style=" color:#0000ff;">         &lt;HostName&gt;</span>
          <span style=" font-weight:600; color:#000000;">00XDSVMAM03200</span>
          <span style=" color:#0000ff;">&lt;/HostName&gt;</span>
       </p>
       <p style=" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;">
          <span style=" color:#0000ff;">      &lt;/ClientLogin&gt;</span>
       </p>
       <p style=" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;">
          <span style=" color:#0000ff;">   &lt;/OpenOrReuse&gt;</span>
       </p>
       <p style=" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;">
          <span style=" color:#0000ff;">&lt;/soap:Body&gt;</span>
       </p>
    </body>
 </html>

 \endverbatim
 * ****************************************************************************/
