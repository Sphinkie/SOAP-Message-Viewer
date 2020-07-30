#include "mainwindow.h"

#include <QApplication>
#include <QStyleFactory>
#include <QVersionNumber>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

//    QApplication::setStyle(QStyleFactory::create("Fusion"));

    // Donne la liste des styles disponibles (ils sont tres semblables)
    // QStringList z = QStyleFactory::keys();
    // "windowsvista"   // menu blanc - barre de titre blanche - pas de relief
    // "Windows"        // menu gris  - barre de titre blanche - relief accentué
    // "Fusion"         // menu gris  - barre de titre blanche - boutons arrondis

    MainWindow w;
    w.show();

    return a.exec();
}


