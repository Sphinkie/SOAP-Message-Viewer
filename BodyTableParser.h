#ifndef BODYTABLEPARSER_H
#define BODYTABLEPARSER_H

#include <stack>
#include <QTreeWidget>
#include <Expat/include/expat.h>

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

    static const long  MAX_BODY_SIZE = 1024*28;         // le body doit tenir dans un buffer de 32kB max (Expat)
    static QTreeWidget* treeWidget;
    static QTreeWidgetItem* currentItem;
    static std::stack<QTreeWidgetItem*> itemStack;
    static int          profondeur;
    static bool         keepNamespaces;
    static std::string  errorMessage;
    static QFont        FontGras;


};

#endif // BODYTABLEPARSER_H
