#pragma once

#include <fstream>
#include <iostream>
#include <cstring>


/** ****************************************************************************
 * @brief La classe FileRepair ouvre un fichier .SCVLOG dont des messages sont tronqués
 *        au début ou à la fin, et essaye de le réparer.
 *        Un nouveau fichier XML est généré (avec l'extension \c ".repaired.xml" ),
 *        et contenant le flux réparé.
 *
 * La classe transforme un fichier SVCLOG en un fichier XML.
 * Elle lit le fichier SVCLOG caractère-par-caractère et n'utilise PAS de parser XML.
 *
 * @author David de Lorenzo
 * **************************************************************************** */
class FileRepair
{
public:
    bool openSVCfile(std::string filename);
    bool isSvcLog(const char* filename);
    void repair();
	void close();

private:
	bool replaceGtLt(char lettre);
    bool replaceiNil(char lettre);
    bool replaceXsiNil(char lettre);
    void trackOpeningTags();
	void trackClosingTags();
	void closeOpenedTags();
    bool removeBalise(std::string name);
	void removeLastTag();

    std::ifstream fichierSVC;
    std::ofstream fichierXML;

    struct Balise
	{
        std::string  TagName  = "";
        Balise* Previous = nullptr;
	} ;
    Balise* lastBalise = nullptr;
    std::streamoff lastTagPosition = 0;

};

