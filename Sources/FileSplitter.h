#ifndef FILESPLITTER_H
#define FILESPLITTER_H

#include <string>
#include <fstream>
#include <iostream>
#include <QProgressDialog>

/** *******************************************************************************
 * @brief La classe FileSplitter ouvre un fichier .SCVLOG et le découpe en morceaux
 *        plus petits de 20.000 messages SOAP chacun.
 *        Ceci est utile quand le fichier SVCLOG est tellement gros qu'il ne peut
 *        pas être ouvert par les outils d'analyse.
 *        Les nouveaux fichierx XML sont nommés ???.
 *
 * La classe transforme un fichier SVCLOG en un fichier XML.
 * Elle lit le fichier SVCLOG caractère-par-caractère et n'utilise PAS de parser XML.
 *
 * @author David de Lorenzo
 * ********************************************************************************/
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
    std::ifstream fichierSVC;            ///< Fichier en lecture
    std::ofstream fichierSPLITTED;       ///< Fichier en écriture
    std::string   filenameSVC;           ///< Nom du fichier
    std::string   errorMessage;
    bool          isGood     = true;
    unsigned int  splitNumber= 0;
    unsigned int  score      = 0;
    unsigned int  nbMessages = 0;
    const char*   TERMINATOR = "</E2ETraceEvent>";

};

#endif // FILESPLITTER_H
