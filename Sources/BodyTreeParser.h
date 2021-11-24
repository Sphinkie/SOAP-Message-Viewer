#ifndef BODYTREEPARSER_H
#define BODYTREEPARSER_H

#include <stack>
#include <QTreeWidget>
#include <Expat/include/expat.h>

/** ****************************************************************************
 * @brief La classe \b statique BodyTreeParser analyse le Body XML d'un message,
 *        pour l'afficher sous forme de TreeView repliable dans l'onglet "Tree View".
 * @author David de Lorenzo
 * ***************************************************************************** */
class BodyTreeParser
{
public:
    static int  buildTree(std::string blob, bool hideNamespaces, QTreeWidget* tree_root_item);
    static void XMLCALL startElementHandler(void *userData, const XML_Char *name, const XML_Char **attrs);
    static void XMLCALL endElementHandler(void *userData, const XML_Char *name);
    static void XMLCALL commentHandler(void *userData, const XML_Char *data);
    static void XMLCALL dataHandler(void *userData, const XML_Char *content, int length);

private:
    static void free();

    static const long  MAX_BODY_SIZE = 1024*512;       //!< 512kB : Taille max des Body (il y a une limitation dans Expat)
    static QTreeWidget* treeWidget;                    //!< Le Treeview à remplir
    static std::string  currentLine;
    static QTreeWidgetItem* currentItem;
    static std::stack<QTreeWidgetItem*> itemStack;
    static int          profondeur;
    static bool         elementClosed;
    static bool         keepNamespaces;
    static std::string  errorMessage;

};

#endif // BODYTREEPARSER_H
