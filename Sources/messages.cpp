/** ****************************************************************************************
 * @class Messages
 *        Cette classe gére la liste des messages SOAP échangés.
 *        Tous les membres de la classe sont statiques: Pas besoin d'instancier la classe.
 * *****************************************************************************************/

#include "messages.h"
using namespace std;

/* ******************************************* */
/* Rappel des variables statiques de la classe */
/* ******************************************* */
static std::vector <Messages::Message*> messageList;
static Messages::Message* currentMessage;

std::string Messages::computerName;
std::string Messages::logDate;
std::string Messages::fileName;


/** ****************************************************************************************
 * @brief Ajout d'un nouveau message à la liste.
 **************************************************************************************** */
void Messages::append()
{
    // On cree un nouveau message
    currentMessage= new Message;
    // on met son adresse dans le vecteur
    messageList.push_back(currentMessage);
}

/** ****************************************************************************************
 * @brief Cette méthode analyse la string fourni et extrait les deux derniers mots significatifs,
 *        pour les enregister dans les champs \b Message.module et \b Message.action.
 *        Exemple "Session" -> "OpenOrReuse".
 * @attention
 *    le paramètre action est modifié pour ne laisser que le dernier item de la string.
 *
 * @param[in,out] action
 *    Une url du type \c "http://www.sgt.eu/VEDA.SOA/Session.v1/Session/OpenOrReuse".
 * *************************************************************************************** */
void Messages::setAction(std::string* action)
{
    size_t pos2 = action->rfind('/');              // position du dernier '/'
    size_t pos1 = action->rfind('/',pos2-1);       // position de l'avant-dernier '/'
    currentMessage->module=action->substr(pos1+1,pos2-pos1-1); // mémorise "Session"
    currentMessage->action=action->substr(pos2+1);             // mémorise "OpenOrReuse"
    action->assign(action->substr(pos2+1));        // modifie la chaine en "OpenOrReuse"
}


/** ****************************************************************************************
 * @brief Mémorise le nom de l'ordinateur émetteur du message.
 *
 * @param host
 *      Contient un hostname ou une adresse IP.
 **************************************************************************************** */
void Messages::setHost(std::string host)
{
    currentMessage->host = host;
}


/** ****************************************************************************************
 * @brief Mémorise l'ID de l'activité. L'activité peut regrouper plusieurs messages,
 *        par exemple une query et sa réponse.
 *
 * @param id L'ActivityID est généralement un UUID du type \c "{dee19e7b-5fba-4adc-8197-2aee3c2e6557}"
 **************************************************************************************** */
void Messages::setActivityID(std::string id)
{
    currentMessage->activityId=id;
}


/** ****************************************************************************************
* @brief Stocke l'heure du message au format \c "14:31:17.975".
* @param messagetime Un timestamp au format \c "2020-06-22T14:31:17.9755137+00:00".
**************************************************************************************** */
void Messages::setMessageTime(std::string messagetime)
{
    currentMessage->messageTime=messagetime.substr(11,12);
}


/** ****************************************************************************************
 * @brief Mémorise le sens du ciculation du message.
 *
 * @param source Les valeurs traitées sont \c TransportReceive ou \c TransportSend
 *               ou \c ServiceLevelReceiveRequest ou \c ServiceLevelSendReply.
 **************************************************************************************** */
void Messages::setMessageSource(std::string source)
{
    currentMessage->direction=source;
    currentMessage->isRequest=       // Test ultra-basique mais rapide
            ((source[9]=='R') ||     // = "TransportReceive"
             (source[12]=='R'));     // = "ServiceLevelReceiveRequest"  
    currentMessage->isServiceLevel=
            (source[0]=='S');        // = "ServiceLevelReceiveRequest" ou "ServiceLevelSendReply"
}


/** ****************************************************************************************
 * @brief Cette méthode mémorise le nom du WSDL.
 *        (Présent pour les messages de type REQUEST, uniquement).
 *
 * @param wsdl : URL du WSDL au format \c "http://00xdsvmam03200.paris.org/VEDA.SOA/Session.svc?wsdl"
 **************************************************************************************** */
void Messages::setWsdl(std::string wsdl)
{
    currentMessage->wsdl=wsdl;
}


/** ****************************************************************************************
 * @brief Mémorise la position du début du Body.
 *
 * @param position Byte index du début du Body dans le fichier SVCLOG.
 **************************************************************************************** */
void Messages::setCurrentBodyStartPosition(long position)
{
    currentMessage->bodyStartPosition=position;
}


/** ****************************************************************************************
 * @brief Mémorise la position de la fin du Body.
 *
 * @param position Byte index de la fin du Body dans le fichier SVCLOG.
 **************************************************************************************** */
void Messages::setCurrentBodyEndPosition(long position)
{
    currentMessage->bodyEndPosition=position;
}


/** ****************************************************************************************
 * @brief Retourne le numéro du message courant dans la liste.
 **************************************************************************************** */
std::string Messages::getCurrentMessageIndex()
{
    return std::to_string(messageList.size());
}


/** ****************************************************************************************
 * @brief Retourne le message de la liste portant le numéro demandé.
 * **************************************************************************************** */
Messages::Message* Messages::getMessage(std::string number)
{
    size_t num = stoi(number);
    return messageList[num];
}

/** ****************************************************************************************
 * @brief Retourne le message de la liste portant le numéro demandé.
 * **************************************************************************************** */
Messages::Message* Messages::getMessage(long number)
{
    return messageList[number];
}


/** ****************************************************************************************
 * @brief Cette fonction lit le Body du message demandé dans le fichier SVCLOG et le mémorise
 *        dans la structure du Message de la liste.
 * **************************************************************************************** */
void Messages::getMessageBody(long number)
{
    // Si on n'a pas le Body en mémoire: on le lit dans le fichier:
    // if (messageList[number]->body.length()==0)
    if (messageList[number]->body==nullptr)
    {
        ifstream fichier;

        // on lit le Body dans le fichier
        long startpos = messageList[number]->bodyStartPosition;
        long endpos   = messageList[number]->bodyEndPosition;
        fichier.open(fileName, ios::in);
        int bodyLength = endpos-startpos;
        if (bodyLength==0) bodyLength=50; // si pas de body, on mettra un message d'information de 50 char.
        char* buffer = new char[bodyLength+1];
        if (startpos==endpos)
        {
            char message[] ="<!-- The body of this message is incomplete -->";
            strcpy_s(buffer,50,message);
        }
        else
        {
            fichier.seekg(startpos);
            fichier.read(buffer, bodyLength);
            streamsize nbBytesRead = fichier.gcount();
            buffer[nbBytesRead]='\0';
            fichier.close();
        }
        // on mémorise le pointeur sur le buffer
        messageList[number]->body = buffer;
    }
}


/** ****************************************************************************************
 * @brief On regarde si le Body demandé contient la substring.
 * @param index : Numéro du message.
 * @param substring : La string à rechercher.
 * @returns \c TRUE si le Body contient la substring.
 * **************************************************************************************** */
bool Messages::containsMessageBody(int index, string substring)
{
    // On commence par lire le body dans le fichier SVCLOG
    Messages::getMessageBody(index);
    // On cherche la substring
    string body(messageList[index]->body);

    if (body.find(substring) != string::npos)
        return true;
    else
        return false;
}

/** ****************************************************************************************
 * @brief Renvoie \c True si le message courant est de type \b Session.
 * **************************************************************************************** */
bool Messages::isSession()
{
    return(currentMessage->module=="Session");
}

/** ****************************************************************************************
 * @brief Renvoie \c True si le message courant est de type \b Request.
 * **************************************************************************************** */
bool Messages::isCurrentMessageRequest()
{
    return(currentMessage->isRequest);
}

/** ****************************************************************************************
 * @brief Renvoie \c True si le message courant est de type \b Service,
 *        et \c False s'il est de type \b Transport.
 * **************************************************************************************** */
bool Messages::isCurrentMessageServiceLevel()
{
    return(currentMessage->isServiceLevel);
}

/** ***************************************************************************
 * @brief Renvoie \c True si le message courant se termine par \c Fault ou \c fault.
 * **************************************************************************************** */
bool Messages::isCurrentMessageFaulty()
{
    int offset = currentMessage->action.length()-5;

    if (offset <0) return false;  // moins de 5 caractères
    if ((currentMessage->action).compare(offset, 5,"Fault") == 0) return true; // se termine par Fault
    if ((currentMessage->action).compare(offset, 5,"fault") == 0) return true; // se termine par fault
    return false;
}

/** ***************************************************************************
 * @brief Renvoie le timestamp du message courant.
 * **************************************************************************** */
std::string Messages::getCurrentMessageTime()
{
    return(currentMessage->messageTime);
}

/** ***************************************************************************
 * @brief Renvoie l'index du prochain Message ayant cet ActivityId.
 * @param startingIndex : Index (non-inclus) de début de la recherche. Mettre -1 pour inclure la ligne 0.
 * @param id : ActivityId à rechercher.
 * @param endingIndex : Index de fin de la recherche (-1=last).
 * @returns Index du prochain Message ayant cet ActivityId, ou -1 si pas trouvé.
 * **************************************************************************** */
int Messages::getNextCorrelatedMessageIndex(int startingIndex, std::string id, int endingIndex)
{
    bool found     = false;
    int  index     = startingIndex;
    int  lastIndex = endingIndex;
    if (endingIndex==-1) lastIndex = messageList.size()-1;
    bool endReached = (startingIndex==lastIndex);

    while (!found && !endReached)
    {
        index++;
        found = !messageList[index]->activityId.compare(id);
        endReached = (index>=lastIndex);
    };
    return found? index : -1;
}

/** ***************************************************************************
 * @brief Renvoie l'index d'un précédent Message ayant cet ActivityId,
 *        ou -1 si pas trouvé.
 * **************************************************************************** */
int Messages::getPreviousCorrelatedMessageIndex(int index, std::string id)
{
    bool found= false;
    bool endReached = (index==0);
    while (!found && !endReached)
    {
        index--;
        found = !messageList[index]->activityId.compare(id);
        endReached = (index<=0);
    };
    return found? index : -1;
}


/** ***************************************************************************
 * @brief Initialise toutes les infos du parsing.
 * @param filename : Le nom du fichier XML
 * **************************************************************************** */
void Messages::init(std::string filename)
{
    for (unsigned int i=0; i>messageList.size()-1; i++)
        delete[] messageList[i]->body;
    messageList.clear();                    // vide le tableau & delete tous ses elements.
    messageList.shrink_to_fit();            // libère la mémoire.
    fileName=filename;                      // pour aller lire les body des messages dans le fichier
    computerName="";
    logDate="";
}

