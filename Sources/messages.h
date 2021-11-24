#ifndef MESSAGES_H
#define MESSAGES_H

#include <iostream>      // pour cout
#include <string>
#include <vector>
#include <fstream>
#include <stdlib.h>     // pour strcpy_s

/** ****************************************************************************************
 * @brief  La classe \b statique Messages gère la liste des messages SOAP échangés.
 *         Tous les membres de la classe sont statiques: Pas besoin d'instancier la classe.
 * @author David de Lorenzo
 * *****************************************************************************************/
class Messages
{
public:
    struct Message;     // forward declaration

    static void init(std::string filename);
    // Ajout d'un message à la liste
    static void   append();
    static void   setWsdl(std::string wsdl);
    static void   setHost(std::string host);
    static void   setAction(std::string* action);
    static void   setActivityID(std::string id);
    static void   setMessageTime(std::string messagetime);
    static void   setMessageSource(std::string source);
    static void   setCurrentBodyStartPosition(long position);
    static void   setCurrentBodyEndPosition(long position);
    static void   setComputerName(std::string name) {computerName=name;};
    static void   setLogDate(std::string date)      {logDate.assign(date,0,10);};
    // Recupération des infos d'un message
    static Message*    getMessage(std::string number);
    static Message*    getMessage(long number);
    static void        getMessageBody(long number);
    static bool        isSession();
    static bool        isCurrentMessageFaulty();
    static bool        isCurrentMessageRequest();
    static bool        isCurrentMessageServiceLevel();
    static std::string getCurrentMessageTime();
    static std::string getCurrentMessageIndex();
    static int         getNextCorrelatedMessageIndex(int startingIndex, std::string id, int endingIndex=-1);
    static int         getPreviousCorrelatedMessageIndex(int index, std::string id);
    // informations globales
    static std::string getComputerName() {return computerName;};
    static std::string getLogDate()      {return logDate;};
    // recherches
    static bool        containsMessageBody(int index, std::string substring);

    /** Structure descriptive d'un Message SOAP extrait du fichier SVCLOG. */
    struct Message
    {
        std::string host;                     //!< Hostname ou adresse IP de l'emetteur du message.
        std::string module;                   //!< Module WSDL. Exemple: \c "Session".
        std::string action;                   //!< Action WSDL. Exemple: \c "Open".
        std::string direction;                //!< Direction du message (sent = "S" ou received = "R").
        std::string activityId;               //!< Identifiant permettant de relier plusieurs messages entre-eux. Exemple: \c "{dee19e7b-5fba-4adc-8197-2aee3c2e6557}".
        std::string messageTime;              //!< Timestamp du message au format \c "14:31:17.975".
        std::string wsdl;                     //!< URL du WSDL du message. exemple: \c "http://00xdsvmam03200.paris.org/VEDA.SOA/Session.svc?wsdl".
        char*       body=nullptr;             //!< Body du message (renseigné à la demande).
        long        bodyStartPosition=NULL;   //!< Byte index du début du Body dans le fichier SVCLOG.
        long        bodyEndPosition=NULL;     //!< Byte index de la fin du Body dans le fichier SVCLOG.
        bool        isRequest=false;          //!< Indique si le message est de type \b Query ou non.
        bool        isServiceLevel=false;     //!< Indique si le message est de type \b Service ou de type \b Transport.
    };

private:
    static std::string computerName;
    static std::string logDate;
    static std::string fileName;

};

#endif // MESSAGES_H


/** ****************************************************************************************
 * @class Messages
 *
 * @details
 *
 * Pour les messages de type QUERY, on a des informations dans les balises:
 *
 * | Balise | Query | Response | Utilisé |
 * | ------ | ----- | -------- | ------- |
 * |\b System contains: ||||
 * |\<TimeCreated SystemTime="2020-06-22T14:31:17.9755137Z" />|x|x| |
 * |\<Correlation ActivityID="{dee19e7b-5fba-4adc-8197-2aee3c2e6557}" />|x|x|x|
 * |\<Computer> 00XDSVMAM03200 \</Computer>|x|x| |
 * |\b HttpRequest contains: ||||
 * |\<Host> 172.18.146.32 \</Host>|x| |x|
 * |\b MessageLogTraceRecord contains: ||||
 * |\<MessageLogTraceRecord Time="2020-06-22T14:31:17.9755137+00:00" >|x|x|x|
 * |\<MessageLogTraceRecord Source="TransportReceive" >|x|x|x|
 * |\<To> http://00xdsvmam03200.paris.tv5monde.org/VEDA.SOA/Session.svc?wsdl \</To>|x| |x|
 * |\<Action> http://www.sgt.eu/VEDA.SOA/Session.v1/Session/OpenOrReuse \</Action>|x|x|x|
 * |\<SOAPAction> "http://www.sgt.eu/VEDA.SOA/Session.v1/Session/OpenOrReuse" \</SOAPAction>|x| | |
 *
 * Pour les messages de type RESPONSE, on a des informations dans les balises:
 *
 * | Balise | Query | Response | Utilisé |
 * | ------ | ----- | -------- | ------- |
 * |\b System contains: ||||
 * |\<TimeCreated SystemTime="2020-06-22T14:31:17.9755137Z" />|x|x| |
 * |\<Correlation ActivityID="{dee19e7b-5fba-4adc-8197-2aee3c2e6557}" />|x|x|x|
 * |\<Computer> 00XDSVMAM03200 \</Computer>|x|x| |
 * |\b MessageLogTraceRecord contains: ||||
 * |\<MessageLogTraceRecord Time="2020-06-22T14:31:17.9755137+00:00">|x|x|x|
 * |\<MessageLogTraceRecord Source="TransportSend" >|x|x|x|
 * |\<Action> http://www.sgt.eu/VEDA.SOA/Session.v1/Session/OpenOrReuseResponse \</Action>|x|x|x|
 * |\<ActivityId> dee19e7b-5fba-4adc-8197-2aee3c2e6557 \</ActivityId>| |x| |
 *
 * *************************************************************************************** */
