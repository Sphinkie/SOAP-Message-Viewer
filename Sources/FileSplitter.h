#ifndef FILESPLITTER_H
#define FILESPLITTER_H

#include <string>
#include <fstream>
#include <iostream>
#include <QProgressDialog>

class FileSplitter
{

public:
    void  open(std::string filename);
    void  split();
    void  close();
    bool  good() {return isGood;};                    ///< Appeler good() après les fonctions pour savoir si tout s'est bien passé
    std::string getErrorMessage() {return errorMessage;};

    const unsigned int MAX_NB_MESSAGES = 20000;       ///< Nb max de messages par fichier découpé (default: 20000)

private:
    bool calculateScore(char lettre);
    void openNewSplittedFile();
    void cancel();
    std::ifstream fichierSVC;
    std::ofstream fichierSPLITTED;
    std::string   filenameSVC;
    std::string   errorMessage;
    bool          isGood     = true;
    unsigned int  splitNumber= 0;
    unsigned int  score      = 0;
    unsigned int  nbMessages = 0;
    const char*   TERMINATOR = "</E2ETraceEvent>";

};

#endif // FILESPLITTER_H
