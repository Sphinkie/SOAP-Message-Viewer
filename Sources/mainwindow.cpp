#include "mainwindow.h"
#include "ui_mainwindow.h"


/** ************************************************************************************************
 * @brief Cette fonction définit la fenêtre principale.
 * @param parent
 * *************************************************************************************************/
MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    // ui->verticalLayout_1->setSizeConstraint(1200);
    // QObject::connect(m_boutonDialogue, SIGNAL(clicked()), this, SLOT(addListItem()));
    // QFont FontRegular("Candara", 10, QFont::Medium);
    FontGras.setBold(true);
    FontItal.setItalic(true);
    arrowRightIcon=QIcon(":/resources/arrowRight-40.png");
    arrowLeftIcon=QIcon(":/resources/arrowLeft-40.png");
    ui->w_MessageTable->setColumnWidth(0,160);   // on élargit la colonne "tag"
}


/** ************************************************************************************************
 * @brief Ajoute un item à la liste des messages.
 * @param texte
 * *************************************************************************************************/
void MainWindow::addListItem(std::string texte)
{
    // Ajoute le texte à la liste
    ui->w_MessageList->addItem(QString::fromStdString(texte));
    // on récupere l'index du dernier item
    int lastIndex = ui->w_MessageList->count();
    QListWidgetItem* w = ui->w_MessageList->item(lastIndex-1);

    if (Messages::isSession())
        w->setForeground(QColorConstants::Svg::darkgray);
    else
        w->setFont(FontGras);

    if (Messages::isCurrentMessageFaulty())
        w->setForeground(QColorConstants::Svg::crimson);

    if (Messages::isCurrentMessageServiceLevel())
        w->setForeground(QColorConstants::Svg::cornflowerblue);

    if (Messages::isCurrentMessageRequest())
        w->setIcon(arrowRightIcon);
    else
        w->setIcon(arrowLeftIcon);
}


/* ************************************************************************************************
 * Menu: OPEN SVCLOG FILE
 * ************************************************************************************************ */
void MainWindow::on_actionOpen_triggered()
{
    QString     fichier  = QFileDialog::getOpenFileName(this, "Open an SVCLOG file", QString(), "SVC logs (*.svclog)");
    std::string filename = fichier.toStdString();
    if (filename == "") return;
    QString     shortFilename = fichier.section('/',-1);

    // initialisations
    FileParser  xparser;
    ui->w_MessageList->clear();                 // reset de la listView
    Messages::init(filename);                   // reset de la liste des messages
    this->on_w_ResetFilterButton_clicked();     // reset de la recherche
    xparser.open(filename, this);               // initialisation du FileParser
    ui->statusbar->showMessage("Parsing "+shortFilename+" ...");

    // Parsing
    int result = xparser.parse(false);  // SVCLOG contient uniquement des LF

    switch (result)
    {
    case XML_ERROR_ABORTED:
        {
        QString nombre = QString::number(FileParser::MAX_MESSAGES);
        QMessageBox::warning(this, "SOAP Message Viewer", "The SVCLOG file contains more than "+nombre+" messages.<br/> Please consider splitting the file.");
        ui->statusbar->showMessage("Only the first "+nombre+" messages are displayed.");
        break;
        }
    case XML_ERROR_NO_ELEMENTS:
        QMessageBox::warning(this, "SOAP Message Viewer", "The parsing of the file has failed !");
        ui->statusbar->showMessage("");
        break;
    case XML_ERROR_NONE:
    default:
        QMessageBox::information(this, "SOAP Message Viewer", "File successfully parsed.");
        ui->w_JumpButton->setEnabled(true);
        // On se positionne sur le dernier élément
        if (ui->w_MessageList->count()>0) ui->w_MessageList->setCurrentRow(0);
        ui->statusbar->showMessage(shortFilename+" : Dialog captured by "+QString::fromStdString(Messages::getComputerName())+" on "+QString::fromStdString(Messages::getLogDate()));
        break;
    }
    xparser.close();
}

/* ************************************************************************************************
 * Menu: IMPORT XML FILE
 * ************************************************************************************************ */
void MainWindow::on_actionImport_triggered()
{
    QString     fichier  = QFileDialog::getOpenFileName(this, "Open an XML file", QString(), "XML files (*.xml)");
    if (fichier == "") return;
    QString     shortFilename = fichier.section('/',-1);
    std::string filename      = fichier.toStdString();

    // initialisations
    FileParser  xparser;
    ui->statusbar->showMessage("Parsing "+shortFilename+"...");
    ui->w_MessageList->clear();                 // reset de la listView
    Messages::init(filename);                   // reset de la liste des messages
    this->on_w_ResetFilterButton_clicked();     // reset de la recherche
    xparser.open(filename, this);               // init du File parser

    // Parsing
    int result = xparser.parse(true);  // les fichiers XML contiennent des CR-LF
    if (result == XML_ERROR_NO_ELEMENTS)
        QMessageBox::warning(this, "SOAP Message Viewer", "The parsing of the file has failed !");
    else
    {
        QMessageBox::information(this, "SOAP Message Viewer", "File successfully parsed.");
        ui->w_JumpButton->setEnabled(true);
        // On se positionne sur le dernier élément
        if (ui->w_MessageList->count()>0) ui->w_MessageList->setCurrentRow(0);
        ui->statusbar->showMessage(shortFilename+" : Dialog captured by "+QString::fromStdString(Messages::getComputerName())+" on "+QString::fromStdString(Messages::getLogDate()));
    }
    xparser.close();
}


/* ************************************************************************************************
 * Appelé chaque fois que l'on clique sur un des messages de la liste.
 * ************************************************************************************************ */
void MainWindow::on_w_MessageList_itemSelectionChanged()
{
    if (ui->w_MessageList->count()==0) return;

    // On recupère le numéro du message.
    int numero = ui->w_MessageList->currentRow();
    // On recupère le pointeur sur le message.
    Messages::Message* msg = Messages::getMessage(numero);

    // On affiche les informations détaillées dans la vue de droite
    ui->line_Time->setText(QString::fromStdString(msg->messageTime));
    ui->line_Host->setText(QString::fromStdString(msg->host));
    ui->line_WSDL->setText(QString::fromStdString(msg->wsdl));
    ui->line_Action->setText(QString::fromStdString(msg->module) + " → " + QString::fromStdString(msg->action));
    ui->line_Source->setText(QString::fromStdString(msg->direction));
    this->displayMessageHeader (msg->isRequest);

    // On récupère le Body dans le fichier
    Messages::getMessageBody(numero);
    // On affiche le contenu du Body en fonction de l'onglet
    this->displayMessageBody(msg->body, ui->tabWidget->currentIndex());
}


/* ************************************************************************************************
 * Affiche certains champs en fontion du type de message REQUEST ou RESPONSE
 * ************************************************************************************************ */
void MainWindow::displayMessageHeader(bool isRequest)
{
    if (isRequest)
    {
        ui->w_MessageTitle->setText("REQUEST MESSAGE BODY");
        // Grisage
        ui->label_WSDL->setEnabled(true);
        ui->label_Host->setEnabled(true);
    }
    else
    {
        ui->w_MessageTitle->setText("RESPONSE MESSAGE BODY");
        // Dégrisage
        ui->label_WSDL->setEnabled(false);
        ui->label_Host->setEnabled(false);
    }
}

/* ************************************************************************************************
 * En cas de double-clic sur un item, on le colorie (marquage manuel)
 * ************************************************************************************************ */
void MainWindow::on_w_MessageList_itemDoubleClicked(QListWidgetItem *item)
{
    item->setBackground(QColorConstants::Svg::lavenderblush);
}

/* ************************************************************************************************
 * Appelé quand on clique sur le bouton JUMP
 * ************************************************************************************************ */
void MainWindow::on_w_JumpButton_clicked()
{
    int foundIndex;
    int index;

    if (ui->w_MessageList->count()==0) return;

    // On recupère le numéro du message.
    if (this->hiddenCorrelatedItem==NOT_FOUND)
    {
        // cas normal: on commence la recherche à partir de l'item sélectionné
        index = ui->w_MessageList->currentRow();
    }
    else
    {
        // sinon, le dernier correlated item trouvé etait caché, et on reprend la recherche à partir de lui.
        index = this->hiddenCorrelatedItem;

    }
    Messages::Message* msg = Messages::getMessage(index);

    if (false)
    {
        // on utilise l'algo de recherche circulaire court
        foundIndex = this->circleFastCorrelatedJump(index, msg);
    }
    else
    {
        // on utilise l'algo de recherche sur la totalité des messages
        foundIndex = this->circleGlobalCorrelatedJump(index, msg);
    }

    if (foundIndex == NOT_FOUND)
    {
        QMessageBox::warning(this, "SOAP Message Viewer", "No correlated message found");
        this->hiddenCorrelatedItem=NOT_FOUND;
    }
    else if (ui->w_MessageList->item(foundIndex)->isHidden())
    {
        QMessageBox::warning(this, "SOAP Message Viewer", "The next correlated message is filtered.");
        this->hiddenCorrelatedItem=foundIndex;
    }
    else
    {
        ui->w_MessageList->setCurrentRow(foundIndex);
        this->hiddenCorrelatedItem=NOT_FOUND;
    }
}

/* ************************************************************************************************
 * circleGlobalCorrelatedJump boucle entre les messages ayant un même Activity Id,
 * On recherche sur l'ensemble des messages de la liste.
 * ************************************************************************************************ */
int MainWindow::circleGlobalCorrelatedJump(int startingPoint, Messages::Message* msg)
{
    int responseIndex;
    int endingPoint = startingPoint;
    responseIndex = Messages::getNextCorrelatedMessageIndex(startingPoint, msg->activityId);
    if (responseIndex==NOT_FOUND)
        // on a atteint la fin sans rien trouver: on recommence du debut
        responseIndex = Messages::getNextCorrelatedMessageIndex(-1, msg->activityId, endingPoint);
    return  responseIndex;
}

/* ************************************************************************************************
 * circleFastCorrelatedJump boucle entre les messages ayant un même Activity Id,
 * En partant de l'hypothèse que on commence par  "TransportSend" et finit par "TransportReceive".
 * On ne cherche pas au dela de ces messages.
 * ************************************************************************************************ */
int MainWindow::circleFastCorrelatedJump(int startingPoint, Messages::Message* msg)
{
    int responseIndex;

    if (!msg->isServiceLevel)
    {
        if (msg->isRequest)
            this->searchDown=true;   // "TransportSend"
        else
            this->searchDown=false;  // "TransportReceive"
    }

    if (this->searchDown)
        responseIndex = Messages::getNextCorrelatedMessageIndex(startingPoint, msg->activityId);
    else
        responseIndex = Messages::getPreviousCorrelatedMessageIndex(startingPoint, msg->activityId);
   return  responseIndex;
}



/* ************************************************************************************************
 * Gestion du menu "Hide Namespaces"
 * ************************************************************************************************ */
void MainWindow::on_actionHideNS_triggered()
{
    if (ui->w_MessageList->count()==0) return;
    // On recupère le pointeur sur le message.
    int index   = ui->tabWidget->currentIndex();         // on récupère l'index de l'onglet (tab)
    int numero  = ui->w_MessageList->currentRow();       // On recupère le numéro du message.
    Messages::Message* msg = Messages::getMessage(numero);          // On recupère le pointeur sur le message.
    this->displayMessageBody(msg->body, index);                     // On affiche la page
}


/* ************************************************************************************************
 * Gestion du menu "Hide Service Level"
 * ************************************************************************************************ */
void MainWindow::on_actionHideService_triggered()
{
    for (int i=0; i<ui->w_MessageList->count()-1; i++)
    {
        if (Messages::getMessage(i)->isServiceLevel)
            ui->w_MessageList->item(i)->setHidden(ui->actionHideService->isChecked());
    }
}


/* ************************************************************************************************
 * Filtre la liste des messages selon le contenu du titre des items
 * ************************************************************************************************ */
void MainWindow::on_w_SearchActionButton_clicked()
{
    QString recherche = ui->w_SearchedText->text();
    if (!recherche.isEmpty())
    {
        for (int i=0; i<ui->w_MessageList->count()-1; i++)
        {
            QString chaine = ui->w_MessageList->item(i)->text();
            if (chaine.contains(recherche,Qt::CaseInsensitive))
                ui->w_MessageList->item(i)->setHidden(false);
            else
                ui->w_MessageList->item(i)->setHidden(true);
        }
    }
}

/* ************************************************************************************************
 * Efface le filtre et ré-affiche la liste des messages complete.
 * ************************************************************************************************ */
void MainWindow::on_w_ResetFilterButton_clicked()
{
    ui->w_SearchedText->clear();                        // Efface la zone de texte
    ui->w_SearchNextButton->setEnabled(false);          // on grise le bouton NEXT
    for (int i=0; i<ui->w_MessageList->count()-1; i++)
    {
        if (!Messages::getMessage(i)->isServiceLevel)
        {
            // on affiche l'item (sauf les messages de service, qu'on laisse tels quels)
            ui->w_MessageList->item(i)->setHidden(false);
        }
        ui->w_MessageList->item(i)->setBackground(QColorConstants::Transparent);    // enleve les coloriages
    }
}

/* ************************************************************************************************
 * Colorie tous les items dont le body contient le texte recherché.
 * ************************************************************************************************ */
void MainWindow::on_w_SearchBodyButton_clicked()
{
    std::string recherche = ui->w_SearchedText->text().toStdString();
    messagesSearchList.clear();

    // on parcourt la liste des messages (éventuellement filtrée)
    if (!recherche.empty())
    {
        for (int i=0; i<ui->w_MessageList->count()-1; i++)
        {
            // on ignore les items cachés
            if (!ui->w_MessageList->item(i)->isHidden())
            {
                // on recherche la chaine dans le body
                if (Messages::containsMessageBody(i, recherche))
                {
                    // si trouvé, on colorie l'item
                    ui->w_MessageList->item(i)->setBackground(QColorConstants::Svg::palegreen);
                    messagesSearchList.push_back(i);
                }
                else
                {
                    ui->w_MessageList->item(i)->setBackground(QColorConstants::Transparent);
                }
            }
        }
    }
    // Si on a trouvé des items
    if (!messagesSearchList.isEmpty())
    {
        ui->w_SearchNextButton->setEnabled(true);                       // on degrise le bouton NEXT
        ui->w_MessageList->setCurrentRow(messagesSearchList.first());   // on se positionne sur le premier
    }
}


/* ************************************************************************************************
 * Menu: REPAIR SVCLOG FILE
 * ************************************************************************************************ */
void MainWindow::on_actionRepair_triggered()
{
    QMessageBox::information(this, "SOAP Message Viewer", explicationRepair);
    QString fichier = QFileDialog::getOpenFileName(this, "Select the file to repair", QString(), "SVC logs (*.svclog)");
    if (fichier.isEmpty())
    {
        QMessageBox::warning(this, "SOAP Message Viewer", "Repair cancelled.");
        return;
    }

    int reponse = QMessageBox::question(this, "SOAP Message Viewer", "Ready to start repairing the file? \n\nThis can be LONG (2-3 minutes...)\n", QMessageBox::Yes|QMessageBox::No);
    if (reponse == QMessageBox::No)
    {
        QMessageBox::warning(this, "SOAP Message Viewer", "Repair cancelled.");
    }
    else
    {
        bool success;
        FileRepair reparation;
        success = reparation.openSVCfile(fichier.toStdString());
        if (success)
        {
            reparation.repair();
            reparation.close();
            QMessageBox::information(this, "Repair", "Done !");
        }
        else
        {
            QMessageBox::critical(this, "Repair", "Failed to open\n"+fichier);
        }
    }
}


/* ************************************************************************************************
 * Menu: CONVERT TO 'FASTER SVCLOG' FILE
 * ************************************************************************************************ */
void MainWindow::on_actionConvert_triggered()
{
    int errcode;
    QMessageBox::information(this, "SOAP Message Viewer", explicationConvert);
    QString fichier = QFileDialog::getOpenFileName(this, "Select the file to convert", QString(), "SVC logs (*.svclog)");

    if (fichier.isEmpty())
    {
        QMessageBox::warning(this, "SOAP Message Viewer", "Conversion cancelled.");
        return;
    }

    FileConverter fileConverter;
    fileConverter.open(fichier.toStdString());
    if (!fileConverter.ready())
    {
        QMessageBox::critical(this, "Conversion", QString::fromStdString(fileConverter.getErrorMessage()));
    }
    errcode = fileConverter.convert();
    fileConverter.close();
    if (errcode==XML_ERROR_NONE)
    {
        QMessageBox::information(this, "Conversion", "Conversion done !");
    }
    else
    {
        QMessageBox::critical(this, "Conversion failed!",  QString::fromStdString(fileConverter.getErrorMessage()));
    }
}

/* ************************************************************************************************
 * Menu: QUIT
 * ************************************************************************************************ */
void MainWindow::on_actionQuit_triggered()
{
    qApp->quit();
}

/* ************************************************************************************************
 * Menu: About
 * ************************************************************************************************ */
void MainWindow::on_actionAbout_triggered()
{
    QMessageBox::about(this, "SOAP Message Viewer", messageAbout);
}

/* ************************************************************************************************
 * Menu: Infos
 * ************************************************************************************************ */
void MainWindow::on_actionInfos_triggered()
{
    QMessageBox::about(this, "SOAP Message Viewer", messageInfos);
}


/* ************************************************************************************************
 * Destructeur
 * ************************************************************************************************ */
MainWindow::~MainWindow()
{
    delete ui;
}


/* ************************************************************************************************
 * Bouton NEXT.
 * On appelle "searchedList" le vecteur contenant les index des elements trouvés lors de la recherche.
 * ************************************************************************************************ */
void MainWindow::on_w_SearchNextButton_clicked()
{
    bool found = false;
    bool endReached = false;
    int  searchListIndex = 0;                               // index pour parcourir le vecteur
    int  selectedRow = ui->w_MessageList->currentRow();     // position actuelle du curseur dans la listView
    int  currentRow  = messagesSearchList.first();          // position du premier element de la SearchedList

    // si la position courante est au delà du dernier élément du vecteur: on reboucle...
    if (selectedRow >= messagesSearchList.last())
        ui->w_MessageList->setCurrentRow(messagesSearchList.first());
    // sinon, on parcourt la searchedList
    else
    {
        do
        {
            currentRow = messagesSearchList[searchListIndex];
            // on a trouvé, si un element du vecteur est > à la position actuelle du curseur
            if ( currentRow > selectedRow)
                found = true;
            // Sauf s'il est masqué
            if (ui->w_MessageList->item(currentRow)->isHidden())
                found = false;
            // on arrete quand on a atteint le dernier élément du vecteur
            endReached = (searchListIndex >= messagesSearchList.count()-1);
            if (!found) searchListIndex++;
        } while (!found && !endReached);

        if (found)
            ui->w_MessageList->setCurrentRow(currentRow);
        else if (endReached)
            ui->w_MessageList->setCurrentRow(messagesSearchList.first());
    }

}

/* ************************************************************************************************
 * Appelé quand on change d'onglet dans la TabView
 * ************************************************************************************************ */
void MainWindow::on_tabWidget_currentChanged(int index)
{
    if (ui->w_MessageList->count()==0) return;

    // On recupère le pointeur sur le message.
    Messages::Message* msg = Messages::getMessage(ui->w_MessageList->currentRow());
    // On a deja chargé le body en mémoire, lors du SelectItem
    this->displayMessageBody(msg->body, index);
}


/* ************************************************************************************************
 * Affichage du Body dans l'onglet actif
 * ************************************************************************************************ */
void MainWindow::displayMessageBody(char* body, int tabIndex)
{
    QTreeWidget* tree;

    switch (tabIndex)
    {
    case 0:
        // Affichage du Body dans le TAB TEXT VIEW
        BodyTextParser::parse(body, ui->actionHideNS->isChecked());   // On ajoute la coloration syntaxique
        ui->w_MessageDump->setText(QString::fromStdString(BodyTextParser::getFormatedBlob()));
        break;
    case 1:
        // Affichage du Body dans le TAB TREE VIEW
        tree = ui->w_MessageTree;
        tree->setColumnCount(1);
        tree->setHeaderHidden(true);
        BodyTreeParser::buildTree(body, ui->actionHideNS->isChecked(), tree);
        break;
    case 2:
        // Affichage du Body dans le TAB TABLE VIEW
        tree = ui->w_MessageTable;
        tree->setColumnCount(2);
        tree->setHeaderHidden(false);
        BodyTableParser::buildTable(body, ui->actionHideNS->isChecked(), tree);
        break;
    }
}

/* ************************************************************************************************
 * Gestion du menu "Split SVCLOG file"
 * ************************************************************************************************ */
void MainWindow::on_actionSplit_triggered()
{
    QMessageBox::information(this, "SOAP Message Viewer", explicationSplitting);
    QString fichier = QFileDialog::getOpenFileName(this, "Select the file to split", QString(), "SVC logs (*.svclog)");

    if (fichier.isEmpty())
    {
        QMessageBox::warning(this, "SOAP Message Viewer", "File splitting cancelled.");
        return;
    }

    FileSplitter fileSplitter;
    fileSplitter.open(fichier.toStdString());
    fileSplitter.split();
    fileSplitter.close();
    if (fileSplitter.good())
    {
        QMessageBox::information(this, "File splitting", "Splitting done !");
    }
    else
    {
        QMessageBox::critical(this, "Splitting failed!",  QString::fromStdString(fileSplitter.getErrorMessage()));
    }
}

/* ************************************************************************************************
 * Marque la ligne avec un fond coloré (semblable au double-clic, mais avec une couleur différente).
 * ************************************************************************************************ */
void MainWindow::on_actionMark_triggered()
{
    if (ui->w_MessageList->currentItem())
        ui->w_MessageList->currentItem()->setBackground(QColorConstants::Svg::thistle);
}

/** ***********************************************************************************************
 * @brief Enregistre le Body du message courant dans un fichier XML.
 * ************************************************************************************************ */
void MainWindow::on_actionSave_triggered()
{
    std::string filename;
    Messages::Message* msg;

    if (ui->w_MessageList->count()>0)
    {
        int index = ui->w_MessageList->currentRow();
        msg = Messages::getMessage(index);
        filename.assign(msg->module + "_" + msg->action + "_" + std::to_string(index+1) + ".xml");
        BodyPrintableParser bodySaver(filename);
        bodySaver.saveBody(msg->body);
        QMessageBox::information(this, "SOAP Message Viewer", "Body saved as: "+QString::fromStdString(filename));
    }
}
