#include "BodyTreeParser.h"

/* ******************************************* */
/* Rappel des variables statiques de la classe */
/* ******************************************* */
int          BodyTreeParser::profondeur;
bool         BodyTreeParser::elementClosed;
bool         BodyTreeParser::keepNamespaces;
std::string  BodyTreeParser::currentLine;
std::string  BodyTreeParser::errorMessage;
QTreeWidget* BodyTreeParser::treeWidget;
QTreeWidgetItem* BodyTreeParser::currentItem;
std::stack<QTreeWidgetItem*> BodyTreeParser::itemStack;


/** *****************************************************************************
 * @brief Effectue le parsing d'un body XML et remplit un TreeView.
 * @param blob : Le flux XML du Body à analyser.
 * @param hideNamespaces : Doit-on afficher ou cacher les attributs \c xmlns.
 * @param treewidget : Le TreeView à remplir.
 * **************************************************************************** */
int BodyTreeParser::buildTree(std::string blob, bool hideNamespaces, QTreeWidget* treewidget)
{
    int bufferSize= blob.length()+1;
    keepNamespaces= !hideNamespaces;
    treeWidget    = treewidget;
    elementClosed = true;
    profondeur    = 0;
    currentLine.clear();
    errorMessage.clear();
    BodyTreeParser::free();

    if (bufferSize>MAX_BODY_SIZE)
    {
        errorMessage.assign("The message body is too long. \nThe maximum size allowed is ");
        errorMessage.append(std::to_string(MAX_BODY_SIZE/1024));
        errorMessage.append("kB.");
        (new QTreeWidgetItem(treeWidget))->setText(0,QString::fromStdString(errorMessage));
        return XML_ERROR_NO_MEMORY;
    }

    XML_Error errcode  = XML_ERROR_NONE;
    XML_Parser parser = XML_ParserCreate(NULL);

    XML_SetElementHandler(parser, &BodyTreeParser::startElementHandler, &BodyTreeParser::endElementHandler);
    XML_SetCharacterDataHandler(parser, &BodyTreeParser::dataHandler);
    XML_SetCommentHandler(parser,&BodyTreeParser::commentHandler);
    XML_Status status = XML_Parse(parser, blob.c_str(), bufferSize, true);
    if (status == XML_STATUS_ERROR)
    {
        errcode  = XML_GetErrorCode(parser);
        errorMessage.assign(XML_ErrorString(errcode));
        errorMessage.append(" at line ");
        errorMessage.append(std::to_string(XML_GetCurrentLineNumber(parser)));
    }
    treewidget->expandAll();

    XML_ParserFree(parser);
    return errcode;
}

/** ***************************************************************************
 * @brief Handler appelé à chaque balise ouvrante.
 *        Ecrit le nom de la balise et les attributs dans le TreeView, et
 *        ajoute un niveau (TreeItem).
 * @param userData : Non utilisé.
 * @param name : Nom de la balise.
 * @param attrs : Tableau des attributs.
 * **************************************************************************** */
void XMLCALL BodyTreeParser::startElementHandler(void* userData, const XML_Char* name, const XML_Char **attrs)
{
    (void)userData;
    // Si la balise précédente n'est pas close, on l'affiche.
    if (!elementClosed)
    {
        currentItem->setText(0, QString::fromStdString(currentLine));
        currentItem->setForeground(0,QColorConstants::Blue);
        currentLine.clear();
    }

    // On construit la nouvelle ligne
    currentLine.assign("<");
    currentLine.append(name);
    for (int i=0; attrs[i]; i+=2)
    {
        if ((keepNamespaces) || (strncmp(attrs[i],"xmlns",5)))
        {
        currentLine.append(" ");
        currentLine.append(attrs[i]);
        currentLine.append("=\"");
        currentLine.append(attrs[i+1]);
        currentLine.append("\"");
        }
    }
    currentLine.append(">");
    if (profondeur==0)
        currentItem = new QTreeWidgetItem(treeWidget);
    else
        currentItem = new QTreeWidgetItem(itemStack.top());
    itemStack.push(currentItem);
    profondeur++;    // On crée et empile un TreeItem
    elementClosed=false;
}


/** ***************************************************************************
 * @brief Ce Handler est appelé à chaque balise fermante.
 *        On ferme le niveau.
 * @param userData : Non utilisé.
 * @param name : Nom de la balise (non utilisé).
 * **************************************************************************** */
void XMLCALL BodyTreeParser::endElementHandler(void *userData, const XML_Char *name)
{
    (void)userData; // pour enlever le warning....

    profondeur--;
    itemStack.pop();

    if(elementClosed)
    {
        // Cas de plusieurs balises fermantes consécutives
        // On cree et on empile un TreeItem pour cette ligne
        if (profondeur==0)
            currentItem = new QTreeWidgetItem(treeWidget);
        else
            currentItem = new QTreeWidgetItem(itemStack.top());
    }
    // On affiche la ligne
    currentLine.append("</");
    currentLine.append(name);
    currentLine.append(">");
    currentItem->setText(0, QString::fromStdString(currentLine));
    currentItem->setForeground(0,QColorConstants::Blue);
    currentLine.clear();

    elementClosed=true;
}


/** ***************************************************************************************
 * @brief Ce Handler est appelé à chaque fois qu'il y a un contenu textuel.
 *        On écrit le contenu textuel dans le TreeView.
 * @param userData : Non utilisé.
 * @param content : Contenu textuel de la section XML.
 * @param length : Le contenu textuel n'étant pas NULL-Terminated: il faut utiliser length.
 * ****************************************************************************************/
void XMLCALL BodyTreeParser::dataHandler(void *userData, const XML_Char* content, int length)
{
    (void)userData; // pour enlever le warning....

    currentLine.append(content,length);
}


/** ***************************************************************************
 * @brief Ce Handler est appelé à chaque fois qu'il y a un Commentaire.
 *        On écrit le texte du commentaire dans le TreeView.
 * @param userData : Non utilisé.
 * @param data : Le texte du commentaire.
 * **************************************************************************** */
void XMLCALL BodyTreeParser::commentHandler(void *userData, const XML_Char *data)
{
    (void)userData; // pour enlever les warnings....
    QTreeWidgetItem* item;
    std::string      line;

    if (profondeur==0)
        item = new QTreeWidgetItem(treeWidget);
    else
        item = new QTreeWidgetItem(itemStack.top());

    line.assign("<!-- ");
    line.append(data,strlen(data));
    line.append(" -->");
    item->setText(0, QString::fromStdString(line));
    item->setForeground(0,QColorConstants::Svg::forestgreen);
}


/** ****************************************************************************
 * @brief Détruit tous les items du TreeView.
 * @link BodyTableParser::buildTable()
 * ***************************************************************************** */
void BodyTreeParser::free()
{
    treeWidget->clear();
    while(!itemStack.empty()) itemStack.pop();
}
