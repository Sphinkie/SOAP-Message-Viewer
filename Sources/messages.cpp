/* ****************************************************************************
 * Classe pour gérer la liste des messages SOAP échangés.
 * Tous les membres de la classe sont statiques: Pas besoin d'isntancier la classe.
 * **************************************************************************** */
#include "messages.h"
using namespace std;

/* ****************************************************************************
 * Variables statiques
 * **************************************************************************** */
static std::vector <Messages::Message*> messageList;
static Messages::Message* currentMessage;

std::string        Messages::computerName;
std::string        Messages::logDate;
std::string        Messages::fileName;


/* ****************************************************************************
 * Ajout d'un nouveau message à la liste
 * **************************************************************************** */
void Messages::append()
{
    // On cree un nouveau message
    currentMessage= new Message;
    // on met son adresse dans le vecteur
    messageList.push_back(currentMessage);
}

/* ****************************************************************************
 * Attention, setAction change la string passée en paramètre pour ne laisser que le dernier item.
 * action est du type "http://www.sgt.eu/VEDA.SOA/Session.v1/Session/OpenOrReuse".
 *   On enregistre dans le message courant : "Session/OpenOrReuse".
 *   On retourne "OpenOrReuse".
 * **************************************************************************** */
void Messages::setAction(std::string* action)
{
    size_t pos2 = action->rfind('/');              // position du dernier '/'
    size_t pos1 = action->rfind('/',pos2-1);       // position de l'avant-dernier '/'
    currentMessage->module=action->substr(pos1+1,pos2-pos1-1); // mémorise "Session"
    currentMessage->action=action->substr(pos2+1);             // mémorise "OpenOrReuse"
    action->assign(action->substr(pos2+1));        // modifie la chaine en "OpenOrReuse"

}
/* ****************************************************************************
 * Contient généralement une adresse IP.
 * **************************************************************************** */
void Messages::setHost(std::string host)
{
    currentMessage->host = host;
}
/* ****************************************************************************
 * ActivityID est généralement un UUID du type "{dee19e7b-5fba-4adc-8197-2aee3c2e6557}"
 * **************************************************************************** */
void Messages::setActivityID(std::string id)
{
    currentMessage->activityId=id;
}
/* ****************************************************************************
 * Time est du type "2020-06-22T14:31:17.9755137+00:00". On ne garde que "14:31:17.975"
 * **************************************************************************** */
void Messages::setMessageTime(std::string messagetime)
{
    currentMessage->messageTime=messagetime.substr(11,12);
}
/* ****************************************************************************
 * Source est du type "TransportReceive" ou "TransportSend"
 * ou "ServiceLevelReceiveRequest" ou "ServiceLevelSendReply"
 * **************************************************************************** */
void Messages::setMessageSource(std::string source)
{
    currentMessage->direction=source;
    currentMessage->isRequest=       // Test ultra-basique mais rapide
            ((source[9]=='R') ||     // = "TransportReceive"
             (source[12]=='R'));     // = "ServiceLevelReceiveRequest"  
    currentMessage->isServiceLevel=
            (source[0]=='S');        // = "ServiceLevelReceiveRequest" ou "ServiceLevelSendReply"
}
/* ****************************************************************************
 * WSDL est du type "http://00xdsvmam03200.paris.tv5monde.org/VEDA.SOA/Session.svc?wsdl"
 * Présent pour les messages de type REQUEST, uniquement
 * **************************************************************************** */
void Messages::setWsdl(std::string wsdl)
{
    currentMessage->wsdl=wsdl;
}
/* ****************************************************************************
 * Pointeurs dans le fichier pour retrouver le contenu du Body.
 * (Byte index)
 * **************************************************************************** */
void Messages::setCurrentBodyStartPosition(long position)
{
    currentMessage->bodyStartPosition=position;
}
void Messages::setCurrentBodyEndPosition(long position)
{
    currentMessage->bodyEndPosition=position;
}
/* ****************************************************************************
 * Retourne le numéro dans la liste du message courant.
 * **************************************************************************** */
std::string Messages::getCurrentMessageIndex()
{
    return std::to_string(messageList.size());
}
/* ****************************************************************************
 * Retourne le message de la liste portant le numéro demandé.
 * **************************************************************************** */
Messages::Message* Messages::getMessage(std::string number)
{
    size_t num = stoi(number);
    return messageList[num];
}
Messages::Message* Messages::getMessage(long number)
{
    return messageList[number];
}
/* ****************************************************************************
 * Cette fonction remplit le champ body dans la liste des messages.
 * **************************************************************************** */
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
/* ****************************************************************************
 * On regarde si le Body demandé contient la substring
 * **************************************************************************** */
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

/* ****************************************************************************
 * Renvoie True si le message courant est de type Session
 * **************************************************************************** */
bool Messages::isSession()
{
    return(currentMessage->module=="Session");
}

/* ****************************************************************************
 * Renvoie True si le message courant est de type Request
 * **************************************************************************** */
bool Messages::isCurrentMessageRequest()
{
    return(currentMessage->isRequest);
}

/* ****************************************************************************
 * Renvoie True si le message courant est de type Transport
 * **************************************************************************** */
bool Messages::isCurrentMessageServiceLevel()
{
    return(currentMessage->isServiceLevel);
}

/* ****************************************************************************
 * Renvoie True si le message courant se termine par Fault ou fault
 * **************************************************************************** */
bool Messages::isCurrentMessageFaulty()
{
    int offset = currentMessage->action.length()-5;

    if (offset <0) return false;  // moins de 5 caractères
    if ((currentMessage->action).compare(offset, 5,"Fault") == 0) return true; // se termine par Fault
    if ((currentMessage->action).compare(offset, 5,"fault") == 0) return true; // se termine par fault
    return false;
}

/* ****************************************************************************
 * Renvoie le MessageTime du message courant
 * **************************************************************************** */
std::string Messages::getCurrentMessageTime()
{
    return(currentMessage->messageTime);
}

/* ****************************************************************************
 * Renvoie l'index du prochain Message ayant cet ActivityId, ou -1 si pas trouvé.
 * on commence à startIndex (non-inclus): mettre -1 pour inclure la ligne 0.
 * on s'arrete à endingIndex (-1=last)
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

/* ****************************************************************************
 * Renvoie l'index d'un précédent Message ayant cet ActivityId,
 * ou -1 si pas trouvé.
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


/* ****************************************************************************
 * Initialise toutes les infos du parsing.
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

