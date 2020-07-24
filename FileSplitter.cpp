/* ****************************************************************************
 * FileSplitter découpe un fichier .SCVLOG en plusieurs fichiers de 20.000
 * messages SOAP chacun.
 *
 * Cette classe lit le fichier SVCLOG caractère-par-caractère et n'utilise PAS de parser XML.
 * **************************************************************************** */
#include "FileSplitter.h"
using namespace std;

/* ****************************************************************************
 * Ouverture des fichiers SVCLOG (input) et SPLITTED (output)
 * **************************************************************************** */
void FileSplitter::open(std::string filename)
{
    filenameSVC = filename;

    fichierSVC.open(filename, ios::in | ios::binary);
    if (fichierSVC.fail())
    {
       isGood=false;
       errorMessage.assign("Failed to open ");
       errorMessage.append(filename);
    }
    else
    {
       this->splitNumber=0;
       this->nbMessages=0;
       this->openNewSplittedFile();
    }
}


/* ************************************************************************************************
 *
 *
 * ************************************************************************************************ */
void FileSplitter::split()
{

    QProgressDialog progressBar("Splitting file 1", QString(), 0, MAX_NB_MESSAGES);
    progressBar.setWindowModality(Qt::WindowModal);
    progressBar.setCancelButton(nullptr);       // masque bouton Cancel
    progressBar.setWindowFlag(Qt::WindowContextHelpButtonHint, false); // masque icone "?"

    if (this->isGood)
    {
        char lettre;

        fichierSVC.clear();
        fichierSVC.seekg(0, fichierSVC.beg);

        while (fichierSVC.get(lettre) && isGood)
        {
            fichierSPLITTED << lettre;
            bool finBlock = calculateScore(lettre);          // passe à True quand on atteint une fin de message
            if (finBlock)
            {
                this->nbMessages++;
                progressBar.setValue(this->nbMessages);
            }

            if (nbMessages==MAX_NB_MESSAGES)
            {
                // on a atteint le nombre max de messages dans ce bloc
                // on ferme le fichier SPLITTED et on ouvre le suivant
                this->openNewSplittedFile();
                this->nbMessages=0;
                progressBar.setLabelText("Splitting file "+QString::number(this->splitNumber));
            }
        }
    }
}

/* ************************************************************************************************
 * Referme les fichiers
 * ************************************************************************************************ */
void FileSplitter::close()
{
    fichierSVC.close();
    fichierSPLITTED.close();
}

/* ************************************************************************************************
 * le score atteint son maximum lorsque l'on a détecté une séquence terminatrice "</E2TraceEvent>"
 * ************************************************************************************************ */
bool FileSplitter::calculateScore(char lettre)
{
    // si le score est à 0, on attend la première lettre.
    // si le score est à 1, c'est qu'on a deja la première lettre et on attend la seconde.
    // etc...
    if (lettre==TERMINATOR[score])
    {
        score++;
        if (score==strlen(TERMINATOR)) return true;
    }
    else
    {
        score=0;
    }
    return false;
}

/* ************************************************************************************************
 * Ferme un fichier Splitted et ouvre le suivant
 * ************************************************************************************************ */
void FileSplitter::openNewSplittedFile()
{
    if (this->splitNumber==0)
        // Creation du premier fichier
        this->splitNumber=1;
    else
    {
        // création fichiers suivants
        fichierSPLITTED.close();
        this->splitNumber++;
    }

    if (this->splitNumber>10)
    {
        errorMessage.assign("Splittig stopped (more than 10 files).");
        isGood=false;
    }

    std::string filenameOut = filenameSVC.substr(0, filenameSVC.length()-7);  // on enleve l'extension .svclog
    filenameOut.append("."+to_string(this->splitNumber));                     // on ajoute l'extension .1
    filenameOut.append(".svclog");                                            // on remet l'extension .svclog

    this->fichierSPLITTED.open(filenameOut, ios::out | ios::binary);

    if (fichierSPLITTED.fail())
    {
        errorMessage.assign("Fail to create ");
        errorMessage.append(filenameOut);
        isGood=false;
    }
}


void FileSplitter::cancel()
{
    this->isGood=false;
}
