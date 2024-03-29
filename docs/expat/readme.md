# Librairie Expat

## Présentation

Access to [The Expart XML Parser reference manual](../../expat/reference.html).

---

## Installation Expat XML Parser

Les utilisateurs Windows doivent télécharger le [`package expat_win32` ](https://sourceforge.net/projects/expat/files/expat_win32/), qui inclut à la fois les librairies précompilées, les exécutables, et le code source.

Le [site Github de Expat](https://libexpat.github.io/) présente la liste des dernières release de la librairie.

### Installation avec le fichier EXE

Télécharger le fichier "expat-win32bin-2.2.9.exe" depuis  https://github.com/libexpat/libexpat/releases

Par défaut, cela installe la librairie dans `C:\Program Files (x86)\Expat 2.2.9`

### Installation avec le fichier ZIP

Télécharger le fichier "expat-win32bin-2.2.9.zip" depuis https://github.com/libexpat/libexpat/releases

L'archive contient le même dossier.

## Installation dans un projet Qt

Copier les fichiers dans le répertoire du projet:

* `/Expat/bin` :  copier les fichiers **DLL**: `libexpat.dll` et `libexpatw.dll` depuis `/Bin`.

* `/Expat/include` : copier les fichiers `.h` et `.def` depuis `/source/lib`.

* `/Expat/lib` : copier les fichiers `.lib` depuis `/Bin`.

* `/Expat/src` : copier les fichiers `.c` depuis `/source/lib`.

* `/docs/expat` : En y copiant les fichiers de documentation html de `/Expat/Doc` , on les rend acessibles depuis github et doxygen.


Dans les répertoires de **build** du projet `/Release` et `/Debug` : copier les deux fichiers **dll**.

### Configuration du Compilateur

Dans la config du binder, ajouter la librairie /expad/lib

* **Projet** Clic-droit
  * Sélectioner **add library**
  * Sélectioner **external library**
  * **Fichier de bibliothèque** : `libexpat`
  * **Include Path** : `Expat/include`
  * Décocher: `Mac`, `Linux` et `Use "d" suffix for debug`



## Utilisation

Dans les fichiers header qui vont utiliser cette librairie (par exemple *MyProject_FileParser.h*), inclure la ligne:
```c++
#include <expat/include/expat.h>
```

Explications sur l'utilisation des méthodes de la librairie : voir https://www.xml.com/pub/1999/09/expat/index.html

On utilise  [l'outil de déploiement Qt](https://doc.qt.io/qt-5/windows-deployment.html#the-windows-deployment-too) pour avoir automatiquement toutes les DLL nécessaires dans le répertoire build.



### Notes

Il existe un wrapper c++ pour Expat : https://github.com/ckane/expatmm









