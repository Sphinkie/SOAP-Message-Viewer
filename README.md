# SOAP Message Viewer



## Présentation







## Compilation

Le projet est compilable avec QtStudio 4.12 (framework Qt 5.15).

La librairie externe **Expat** (parser XML SAX capable de traiter de gros fichiers rapidement, même abimés) est aussi nécessaire.

Le fichier .pro contient les directives nécessaires à la compilation.

Pour le fonctionnement de l'exécutable, il faut copier les DLL manuellement:

- **version Debug**: copier les fichiers **libexpat.dll** et **libexpatw.dll** de **Expat/lib** dans le répertoire où le fichier **exe** de debug a été généré par le compilateur.
- **version Release**: copier les fichiers **libexpat.dll** et **libexpatw.dll** de **Expat/lib** dans le répertoire où le fichier **exe** de release a été généré par le compilateur, et ajouter les DLL du framework Qt grace à l'outil **winQtDeploy.exe**.

## Licence

**QT** est en version OpenSource sous licence LGPL.

**Expat** est un logiciel libre sous licence MIT/X. [file://Expat/documentation/reference.html](file://Expat/documentation/reference.html)





