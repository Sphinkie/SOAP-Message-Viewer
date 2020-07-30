#pragma once

#include <fstream>
#include <iostream>
#include <cstring>

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

