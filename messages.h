#ifndef MESSAGES_H
#define MESSAGES_H

#include <iostream>      // pour cout
#include <string>
#include <vector>
#include <fstream>
#include <stdlib.h>     // pour strcpy_s

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

    struct Message
    {
        std::string host;
        std::string module;
        std::string action;
        std::string direction;
        std::string activityId;
        std::string messageTime;
        std::string wsdl;
        char*       body=nullptr;
        long        bodyStartPosition=NULL;
        long        bodyEndPosition=NULL;
        bool        isRequest=false;
        bool        isServiceLevel=false;
    };

private:
    static std::string computerName;
    static std::string logDate;
    static std::string fileName;

};

#endif // MESSAGES_H


/* *********************************************************************************
 *
 * (RR: Balise présente dans Request+Response)
 * (**: les balises utilisées)
 *
 * Pour les messages de type REQUEST, on a des informations dans les balises:
 *
 * <System> contient:
 * RR     <TimeCreated SystemTime="2020-06-22T14:31:17.9755137Z" />
 * RR **  <Correlation ActivityID="{dee19e7b-5fba-4adc-8197-2aee3c2e6557}" />
 * RR     <Computer> 00XDSVMAM03200 </Computer>
 * <HttpRequest> contient:
 * R- **  <Host>172.18.146.32</Host>
 * <MessageLogTraceRecord> contient:
 * RR **  <MessageLogTraceRecord Time="2020-06-22T14:31:17.9755137+00:00" >
 * RR **  <MessageLogTraceRecord Source="TransportReceive" >
 * R- **  <To> http://00xdsvmam03200.paris.tv5monde.org/VEDA.SOA/Session.svc?wsdl </To>
 * RR **  <Action> http://www.sgt.eu/VEDA.SOA/Session.v1/Session/OpenOrReuse </Action>
 * <Non utilisé>:
 * R-     <SOAPAction> "http://www.sgt.eu/VEDA.SOA/Session.v1/Session/OpenOrReuse" </SOAPAction>
 *
 * Pour les messages de type RESPONSE:
 *
 * <System> contient:
 * RR     <TimeCreated SystemTime="2020-06-22T14:31:17.9755137Z" />
 * RR **  <Correlation ActivityID="{dee19e7b-5fba-4adc-8197-2aee3c2e6557}" />
 * RR     <Computer> 00XDSVMAM03200 </Computer>
 * <MessageLogTraceRecord> contient:
 * RR **  <MessageLogTraceRecord Time="2020-06-22T14:31:17.9755137+00:00">
 * RR **  <MessageLogTraceRecord Source="TransportSend" >
 * RR **  <Action> http://www.sgt.eu/VEDA.SOA/Session.v1/Session/OpenOrReuseResponse </Action>
 * <Non utilisé>:
 * -R     <ActivityId> dee19e7b-5fba-4adc-8197-2aee3c2e6557 </ActivityId>
 *
 * *************************************************************************************** */
