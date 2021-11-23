#ifndef BODYTABLEPARSER_H
#define BODYTABLEPARSER_H

#include <stack>
#include <QTreeWidget>
#include <Expat/include/expat.h>

/** ****************************************************************************
 * @brief La classe BodyTableParser analyse le Body XML d'un message, pour l'afficher
 *        sous forme de tableau dans l'onglet "Table View".
 * @author David de Lorenzo
 * ***************************************************************************** */
class BodyTableParser
{
public:
    static int  buildTable(std::string blob, bool hideNamespaces, QTreeWidget* tree_root_item);
    static void XMLCALL startElementHandler(void *userData, const XML_Char *name, const XML_Char **attrs);
    static void XMLCALL endElementHandler(void *userData, const XML_Char *name);
    static void XMLCALL commentHandler(void *userData, const XML_Char *data);
    static void XMLCALL dataHandler(void *userData, const XML_Char *content, int length);

private:
    static void free();

    static const long  MAX_BODY_SIZE = 1024*512;       ///< 512kB : Taille max des Body (Le buffer de Expat est limité)
    static QTreeWidget* treeWidget;
    static QTreeWidgetItem* currentItem;
    static std::stack<QTreeWidgetItem*> itemStack;
    static int          profondeur;
    static bool         keepNamespaces;
    static std::string  errorMessage;
    static QFont        FontGras;


};

#endif // BODYTABLEPARSER_H
