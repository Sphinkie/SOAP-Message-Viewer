#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <QFileDialog>
#include <QListWidgetItem>
#include <QVersionNumber>

#include "FileRepair.h"
#include "messages.h"
#include "FileConverter.h"
#include "FileParser.h"
#include "FileSplitter.h"
#include "BodyTextParser.h"
#include "BodyTreeParser.h"
#include "BodyTableParser.h"
#include "BodyPrintableParser.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

/** *******************************************************************************
 * @brief La classe MainWindow gère la fenêtre principale de l'IHM de l'application.
 * @author David de Lorenzo
  **********************************************************************************/
class MainWindow : public QMainWindow
{
    Q_OBJECT


public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void addListItem(std::string texte);

    QVersionNumber version = QVersionNumber::fromString("1.3.0");   //!< Numéro de version logiciel. A mettre aussi à jour dans le fichier .pro et Doxyfile
    const int NOT_FOUND = -1;                                       //!< Constante


private slots:
    // Barre de menu
    void on_actionOpen_triggered();
    void on_actionImport_triggered();
    void on_actionSave_triggered();
    void on_actionMark_triggered();
    void on_actionSplit_triggered();
    void on_actionRepair_triggered();
    void on_actionConvert_triggered();
    void on_actionQuit_triggered();
    void on_actionInfos_triggered();
    void on_actionAbout_triggered();
    void on_actionHideService_triggered();
    void on_actionHideNS_triggered();

    void on_w_JumpButton_clicked();
    void on_w_SearchNextButton_clicked();
    void on_w_SearchActionButton_clicked();
    void on_w_SearchBodyButton_clicked();
    void on_w_ResetFilterButton_clicked();
    void on_w_MessageList_itemSelectionChanged();
    void on_w_MessageList_itemDoubleClicked(QListWidgetItem *item);
    void on_tabWidget_currentChanged(int index);

private:
    void displayMessageHeader(bool isRequest);
    void displayMessageBody(char* body, int tabIndex);
    int circleFastCorrelatedJump(int startingPoint, Messages::Message* msg);
    int circleGlobalCorrelatedJump(int startingPoint, Messages::Message* msg);


    Ui::MainWindow *ui;
    QPushButton    *m_boutonDialogue;
    QFont FontItal;
    QFont FontGras;
    QIcon arrowLeftIcon;
    QIcon arrowRightIcon;
    QVector<int> messagesSearchList;
    bool searchDown = true;                 ///< Sens de la recherche par circleFastSearch (Downwards ou Upwards).
    int  hiddenCorrelatedItem = NOT_FOUND;  ///< Si un item est caché, on le mémorise pour reprendre la recherche à partir de lui.


QString explicationRepair  =
    "Lors de l'enregistrement des traces SOAP avec l'activation des diagnostics, \
    il arrive fréquemment que le fichier <b>web_messages.svclog</b> soit tronqué à la fin au milieu d'une section. \
    <br/> La syntaxe XML n\'est alors pas respectée, et le fichier ne peut pas être ouvert via un éditeur XML standard. \
    <br/> <b>WCF Repair</b> supprime la ligne tronquée, et referme les balises XML qui restaient pendantes. \
    Il encapsule aussi l'ensemble dans une section &lt;root&gt;. \
    <br/> Le fichier réparé <b>web_messages.repaired.xml</b> est alors facilement manipulable et éditable.";

QString explicationConvert  =
    "<h3>Conversion to faster XML format</h3> \
    <p>Le traitement des fichiers SVCLOG est parfois long, car ces fichiers sont verbeux. </p>\
    <p>Les fichiers au format <b>faster</b> (<i>web_messages.faster.xml</i>) contiennent uniquement les informations utiles des messages SOA. \
    Ils sont compatibles avec la présente application, et présentent de meilleurs performances. \
    <br/> Ils restent éditables dans un éditeur XML standard. </p>";

QString explicationSplitting  =
    "<h3>Splitting an SVCLOG file</h3> \
    <p>SVCLOG files can be very huge...</p>\
    <p>If they contain more than 30.000 messages, the application will not be able to display all the messages. <br/> \
    In that case, we recommand to use the <i>Splitting</i> functionnality, which will split the SVCLOG file into several files \
    of 20.000 messages each. </p>";

QString messageInfos =
    "<h2>SOAP Message Viewer</h2> \
    <h3>Usage</h3> \
    <p><b>SOAP Message Viewer</b> allows you to open a SOAP trace file (usually: <i>web_messages.svclog</i>) \
    and see the content of the recorded messages, in a fast and convenient way.</p> \
    <h3>Convert and Repair</h3> \
    <p><b>Convert</b> and <b>Repair</b> are two similar functionalities: <br/><br/> \
       <b>Convert</b> will generate an XML with lesser tags, and faster to process. <br/><br/> \
       <b>Repair</b> is a binary process that will attempt to produce an XML, compatible with usual standards, \
       by discarding truncated elements, removing i:nil and xsi:nil tags, and appending missing tag endings. <br/> \
    </p> \
    <h3>LF and CRLF</h3> \
      <p>The <b>svclog</b> file is supposed to contain 1 line, and eventualy LF [0x0A] byte, if a <b>LineFeed</b> is present. <br/>\
      The <b>XML</b> files may contain CR-LF [0x0D 0x0A] bytes, which is the standard CarriageReturn-LineFeed sequence for Windows.<br/> \
      Your file may not contain <b>both LF and CRLF</b> delimiters, otherwise the application will not be able to parse the XML messages sucessfully.</p>";

QString messageAbout =
      "<h2>SOAP Message Viewer v" + version.toString() + "</h2>" +
      "<p><b>SOAP Message Viewer</b> allows you to open a SOAP trace file <br/> \
       (usually: <i>web_messages.svclog</i>) and to see the content of <br/> \
       the recorded messages, in a fast and convenient way.</p> \
      <p> Written by David de Lorenzo (2020). </p> \
      <p> <a href='http://www.sgt.eu'>http://www.sgt.eu</a></p>  \
      <p> This software uses Qt 5.15 (LGPL licence) <br/> and the Expat Library (MIT licence). </p>";


};

#endif // MAINWINDOW_H
