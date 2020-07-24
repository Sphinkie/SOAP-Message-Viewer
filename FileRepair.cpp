/* ****************************************************************************
 * Trasnsformation d'un fichier SVCLOG en un fichier XML
 *   Cette classe lit le fichier SVCLOG caractère-par-caractère
 *   et n'utilise PAS de parser XML.
 * **************************************************************************** */

#include "FileRepair.h"

using namespace std;

/* ****************************************************************************
 * Ouverture des fichiers SVCLOG (input) et XML (output)
 * **************************************************************************** */
bool FileRepair::openSVCfile(std::string filename)
{
    std::string filenameOut = filename.substr(0, filename.length()-7);  // on enleve l'extension svclog
    filenameOut.append(".repaired.xml");

	fichierSVC.open(filename, ios::in);
	if (fichierSVC.fail())
    {
       cout << "Failed to open " << filename << endl;
       return false;
    }
	else
        cout << "Reading " << filename << endl;

    fichierXML.open(filenameOut, ios::out);
    cout << "Writing to " << filenameOut << endl;
	return true;
}

/* ****************************************************************************
 * Verification de l'extension du fichier
 * **************************************************************************** */
bool FileRepair::isSvcLog(const char* filename)
{
    char extension[8];

    int lg = strlen(filename);
    int ext_position = lg-7;
    strncpy_s(extension, filename+ext_position,8);
    extension[7]=0;
    return (!strcmp(extension, ".svclog"));  // renvoie 0 s égalité
}

/* ************************************************************************************************
 * Parcourt le fichier SVCLOG et effectue les remplacements nécessaires pour le transformer
 * en un fichier XML lisible.
 * ************************************************************************************************ */
void FileRepair::repair()
{
    fichierXML << "<!-- Repaired WCF log file -->\n";
	fichierXML << "<root>\n";

	if (fichierSVC.is_open())
	{
        // *** PREMIER PASSAGE : on analyse la structure du XML ***
		// on traque toutes les balises ouvrantes qui ne sont pas refermées.
		char lettre;
		while (fichierSVC.get(lettre))
		{
			if (lettre == '<')
			{
                lastTagPosition = fichierSVC.tellg();
                this->trackOpeningTags();
			}
            else if (lettre == '/')  this->trackClosingTags();
		};

        // *** DEUXIEME PASSAGE ***
		// On revient au debut du fichier, pour effectuer la recopie dans le XML.
		fichierSVC.clear();
		fichierSVC.seekg(0, fichierSVC.beg);

		while (fichierSVC.get(lettre))
		{
            bool replacedLetter = false;

            // On remplace certaines séquences de lettres
            replacedLetter = replacedLetter || replaceGtLt(lettre);     // >< par retours à la ligne
            replacedLetter = replacedLetter || replaceiNil(lettre);     // i:nil=
            replacedLetter = replacedLetter || replaceXsiNil(lettre);   // xsi:nil=

            // On recopie chaque lettre d'un fichier à l'autre
            if (!replacedLetter)
			{
                // Sauf quand on atteint la position du dernier tag correct
                if (fichierSVC.tellg() < lastTagPosition)
					fichierXML << lettre;
			}
		}

		// On strappe la derniere balise
        this->removeLastTag();

		// On referme les balises non fermées
        this->closeOpenedTags();
	}
	fichierXML << "\n</root>";
}

/* ************************************************************************************************
 * Referme les fichiers
 * ************************************************************************************************ */
void FileRepair::close()
{
	fichierSVC.close();
	fichierXML.close();
}


/* ************************************************************************************************
 * Ajoute un saut de ligne entre > (GreaterThan) et < (LesserThan).
 * Si c'est un ">", on regarde si le suivant est "<", si oui, on écrit ">\n<"
 * On renvoie True si la lettre a été ecrite dans le XML
 * ************************************************************************************************ */
bool FileRepair::replaceGtLt(char lettre)
{
	char suivante;
	if (lettre == '>')
		{
			fichierSVC.get(suivante);
			if (suivante == '<')
			{
				// si c'est la dernière balise, on droppe la fin
                if (fichierSVC.tellg() >= lastTagPosition)
					fichierXML << ">\n";
				else
					fichierXML << ">\n<";
            }
            else
            {
                fichierXML << lettre << suivante;
            }
            return true;
        }
		else
		{
			return false;
		}
}


/* ************************************************************************************************
 * On enleve les attributs  i:nil="true"
 * Ces attributs posent des problèmes aux parsers XML, et ne servent à rien (?).
 * On renvoie True si la lettre a été ecrite dans le XML
 * ************************************************************************************************ */
bool FileRepair::replaceiNil(char lettre)
{
    char suivantes[16];

    if (lettre == 'i')
    {
        fichierSVC.get(suivantes,12);
        suivantes[12] = '\0';
        if (!strcmp(suivantes, ":nil=\"true\""))
        {
            // on replace la chaine par un vide
            fichierXML << "";
            return true;
        }
        else if (fichierSVC.eof())
        {
            // On a atteint la fin du fichier lors de la lecture des 12 lettres:
            // on ne revient pas en arrière.
            fichierSVC.clear();
            return false;
        }
        else
        {
            // sinon, on fait comme si on n'avait pas lu les lettres suivantes.
            fichierSVC.seekg(-11,ios_base::cur);
            return false;
        }
    }
    else
        return false;
}

/* ************************************************************************************************
 * On enleve les attributs  xsi:nil="true"
 * Ces attributs posent des problèmes aux parsers XML, et ne servent à rien (?).
 * ************************************************************************************************ */
bool FileRepair::replaceXsiNil(char lettre)
{
    char suivantes[16];

    if (lettre == 'x')
    {
        fichierSVC.get(suivantes, 14);
        suivantes[14] = '\0';
        if (!strcmp(suivantes,"si:nil=\"true\""))
        {
            fichierXML << "";
            return true;
        }
        else if (fichierSVC.eof())
        {
            // On fait retomber le bit eof, pour pouvoir lire tellg.
            fichierSVC.clear();
            // On a atteint la fin du fichier: on ne revient pas en arrière.
            return false;
        }
        else
        {
            fichierSVC.seekg(-13,ios_base::cur);
            return false;
        }
    }
    else
        return false;
}

/* ************************************************************************************************
 * Recherche une Balise dans la liste chainée, et la supprime.
 * ************************************************************************************************ */
bool  FileRepair::removeBalise(string name)
{
	// on parcourt la liste des balises
    Balise* currentBalise = lastBalise;
    Balise* childBalise   = nullptr;

	while ((currentBalise->TagName != name) && (currentBalise->Previous != nullptr))
	{
		childBalise = currentBalise;
		currentBalise = currentBalise->Previous;
	};
	// Soit est arrivé à la fin
	if (currentBalise->Previous == nullptr) return false;
	// Soit on a trouvé notre balise
	// if (currentBalise->TagName == name)
	// on relie le child au parent
    Balise* parentBalise = currentBalise->Previous;
	// s'il y a un child, on le relie au Parent
	if (childBalise)
		childBalise->Previous = parentBalise;
	// sinon, c'est que l'on va supprimer la dernière balise.
	else
		// LastBalise devient donc le parent
        lastBalise = parentBalise;
	// on la supprime
	delete currentBalise;
	return true;
}

/* ************************************************************************************************
 * Supprime la dernière balise de la liste chainée.
 * ************************************************************************************************ */
void  FileRepair::removeLastTag()
{
    Balise* parentBalise = lastBalise->Previous;
	// on la supprime
    delete lastBalise;
    lastBalise = parentBalise;
	fichierXML << "<!-- Skipped truncated last line -->";
}

/* ************************************************************************************************
 * On vient de lire '<'
 * On teste si c'est "<balise>" ou "</balise>" ou "<balise/>" ou "<balise attributes"
 * ************************************************************************************************ */
void  FileRepair::trackOpeningTags()
{
    string  buffer = "";
    string  tagName;
    Balise* newBalise;

		// on lit des chars jusqu'à '>'
		char suivante = '<';
		while (suivante != '>' && suivante != ' ')
		{
			fichierSVC.get(suivante);
			buffer += suivante;
		}
		// Cas "/E2ETraceEvent>"
		if (buffer[0] == '/')
		{
			tagName = buffer.substr(1, buffer.length() - 2);
			// Supprimer cette balise
			this->removeBalise(tagName);
		}
		// Cas "E2ETraceEvent/>"
		else if (buffer[buffer.length() - 2] == '/')
		{
			// balise sans contenu. On ne la mémorise pas.
			tagName = "";
		}
		// Cas "E2ETraceEvent>" ou "E2ETraceEvent "
		else
		{
			tagName = buffer.substr(0, buffer.length() - 1);
			//    on stocke la balise.
            newBalise = new Balise;
            newBalise->TagName  = tagName;
            newBalise->Previous = lastBalise;
            lastBalise = newBalise;
		}
}

/* ************************************************************************************************
 * On vient de lire '/'.
 * Si la suivante est "/>", alors on a trouvé une balise fermante, et on peut l'enlever de la liste.
 * ************************************************************************************************ */
void  FileRepair::trackClosingTags()
{
	string  buffer = "";
	char    suivante;
    Balise* parent;

	fichierSVC.get(suivante);
	// Cas "/>"
	if (suivante == '>')
	{
		// Supprimer la dernière balise
        parent = lastBalise->Previous;
        this->removeBalise(lastBalise->TagName);
        lastBalise = parent;
	}
}

/* ************************************************************************************************
 * On ferme les balises qui ne seraient pas fermées.
 * ************************************************************************************************ */
void  FileRepair::closeOpenedTags()
{
    Balise* currentBalise = lastBalise;
	while (currentBalise->Previous != nullptr)
	{
		fichierXML << "\n</" + currentBalise->TagName + ">";
		currentBalise = currentBalise->Previous;
	}
}
