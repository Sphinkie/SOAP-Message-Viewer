#include "BodyTableParser.h"

/* ******************************************* */
/* Rappel des variables statiques de la classe */
/* ******************************************* */
int          BodyTableParser::profondeur;
bool         BodyTableParser::keepNamespaces;
QFont        BodyTableParser::FontGras;
std::string  BodyTableParser::errorMessage;
QTreeWidget* BodyTableParser::treeWidget;
QTreeWidgetItem* BodyTableParser::currentItem;
std::stack<QTreeWidgetItem*> BodyTableParser::itemStack;


/** ********************************************************************************
 * @brief Effectue le parsing d'un body XML et remplit un TreeView à deux colonnes.
 * @param blob : Le flux XML du Body à analyser.
 * @param hideNamespaces : Doit-on afficher ou cacher les attributs \c xmlns.
 * @param treewidget : Le TreeView à remplir.
 * *********************************************************************************/
int BodyTableParser::buildTable(std::string blob, bool hideNamespaces, QTreeWidget* treewidget)
{
    int bufferSize= blob.length()+1;
    keepNamespaces= !hideNamespaces;
    treeWidget    = treewidget;
    profondeur    = 0;
    errorMessage.clear();
    BodyTableParser::free();
    FontGras.setBold(true);

    if (bufferSize>MAX_BODY_SIZE)
    {
        errorMessage.assign("The message body is too long. \nThe maximum size allowed is ");
        errorMessage.append(std::to_string(MAX_BODY_SIZE/1024));
        errorMessage.append("kB.");
        (new QTreeWidgetItem(treeWidget))->setText(0,QString::fromStdString(errorMessage));
        return XML_ERROR_NO_MEMORY;
    }

    XML_Error errcode = XML_ERROR_NONE;
    XML_Parser parser = XML_ParserCreate(NULL);

    XML_SetStartElementHandler(parser, &BodyTableParser::startElementHandler);
    XML_SetEndElementHandler(parser, &BodyTableParser::endElementHandler);
    XML_SetCharacterDataHandler(parser, &BodyTableParser::dataHandler);
    XML_SetCommentHandler(parser,&BodyTableParser::commentHandler);

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
 *        ajoute un niveau.
 * @param userData : Non utilisé.
 * @param name : Nom de la balise.
 * @param attrs : Tableau des attributs.
 * **************************************************************************** */
void XMLCALL BodyTableParser::startElementHandler(void* userData, const XML_Char* name, const XML_Char **attrs)
{
    (void)userData;
    // On crée et empile un TreeItem
    if (profondeur==0)
        currentItem = new QTreeWidgetItem(treeWidget);
    else
        currentItem = new QTreeWidgetItem(itemStack.top());

    itemStack.push(currentItem);
    currentItem->setText(0, QString::fromStdString(name));
    currentItem->setForeground(0,QColorConstants::Blue);

    for (int i=0; attrs[i]; i+=2)
    {
        if ((keepNamespaces) || (strncmp(attrs[i],"xmlns",5)))
        {
            QTreeWidgetItem* itemToWrite;
            QString attrib = attrs[i];
            attrib.append("=\"");
            attrib.append(attrs[i+1]);
            attrib.append("\"");
            if (i==0)
                // On met le premier attribut sur la même ligne que le tag
                itemToWrite=currentItem;
            else
                // On met les attributs suivants sur une nouvelle ligne
                itemToWrite = new QTreeWidgetItem(itemStack.top());
            itemToWrite->setForeground(1,QColorConstants::Svg::firebrick);
            itemToWrite->setText(1,attrib);
        }
    }
    profondeur++;
}

/** ***************************************************************************
 * @brief Ce Handler est appelé à chaque balise fermante.
 *        On ferme le niveau.
 * @param userData : Non utilisé.
 * @param name : Nom de la balise (non utilisé).
 * **************************************************************************** */
void XMLCALL BodyTableParser::endElementHandler(void *userData, const XML_Char *name)
{
    (void)userData; // pour enlever le warning....
    (void)name;

    profondeur--;
    itemStack.pop();
}


/** ***************************************************************************************
 * @brief Ce Handler est appelé à chaque fois qu'il y a un contenu textuel.
 *        On écrit le contenu textuel dans la deuxième colonne.
 * @param userData : Non utilisé.
 * @param content : Contenu textuel de la section XML.
 * @param length : Le contenu textuel n'étant pas NULL-Terminated: il faut utiliser length.
 * ****************************************************************************************/
void XMLCALL BodyTableParser::dataHandler(void *userData, const XML_Char* content, int length)
{
    (void)userData; // pour enlever le warning....

    QTreeWidgetItem* itemToWrite;
    std::string data;
    data.append(content,length);
    if (currentItem->text(1).isEmpty())
    {
        // si la place est libre sur la ligne du tag: on y met le Content
        itemToWrite=currentItem;
    }
    else
    {
        // sinon, on le met sur une nouvelle ligne
        itemToWrite = new QTreeWidgetItem(itemStack.top());
    }
    itemToWrite->setForeground(1,QColorConstants::Black);
    itemToWrite->setText(1, QString::fromStdString(data));
    itemToWrite->setFont(1, FontGras);
}


/** ***************************************************************************
 * @brief Ce Handler est appelé à chaque fois qu'il y a un Commentaire.
 *        On écrit le texte du commentaire dans la deuxième colonne.
 * @param userData : Non utilisé.
 * @param data : Le texte du commentaire.
 * **************************************************************************** */
void XMLCALL BodyTableParser::commentHandler(void *userData, const XML_Char *data)
{
    (void)userData; // pour enlever les warnings....
    QTreeWidgetItem* item;
    std::string      line;

    if (profondeur==0)
        item = new QTreeWidgetItem(treeWidget);
    else
        item = new QTreeWidgetItem(itemStack.top());

    line.append(data,strlen(data));
    item->setText(0, "Comment");
    item->setText(1, QString::fromStdString(line));
    item->setForeground(0,QColorConstants::Svg::forestgreen);
    item->setForeground(1,QColorConstants::Svg::forestgreen);
}


/** ****************************************************************************
 * @brief Détruit tous les items du TreeView.
 * @see BodyTableParser::buildTable()
 * *****************************************************************************/
void BodyTableParser::free()
{
    treeWidget->clear();
    while(!itemStack.empty()) itemStack.pop();
}
