# Librairie Expat

## Présentation

Access to [The Expat XML Parser reference manual](../../expat/reference.html).

Le projet utilise **Expat 2.4.1**. La librairie n'est pas liée sous forme de DLL : ses sources sont compilées directement avec le projet (lien statique).
Cela évite toute dépendance au compilateur (MinGW ou MSVC) et à l'architecture (32 ou 64 bits), et il n'y a aucune DLL Expat à déployer.

---

## Organisation dans le projet

| Dossier / fichier | Contenu |
| ----------------- | ------- |
| `Expat/src` | Les fichiers `.c` d'Expat (`xmlparse.c`, `xmlrole.c`, `xmltok.c`, `xmltok_impl.c`, `xmltok_ns.c`). |
| `Expat/include` | Les fichiers `.h` d'Expat (publics et internes). |
| `Expat/include/expat_config.h` | Configuration de compilation. **Propre au projet** : ce fichier n'est pas fourni par Expat (il est normalement généré par CMake). À conserver lors d'une mise à jour. |
| `docs/expat` | La documentation html d'Expat, accessible depuis github et doxygen. |

Dans le fichier `.pro`, la compilation d'Expat se fait avec :

```
SOURCES += \
    Expat/src/xmlparse.c \
    Expat/src/xmlrole.c \
    Expat/src/xmltok.c
DEFINES += XML_STATIC

INCLUDEPATH += $$PWD/Expat/include
```

`xmltok_impl.c` et `xmltok_ns.c` ne sont pas compilés séparément : ils sont inclus par `xmltok.c`.

Aucun binaire précompilé d'Expat (DLL ou `.lib`) n'est nécessaire.

---

## Mise à jour d'Expat

Le [site Github de Expat](https://libexpat.github.io/) présente la liste des dernières release de la librairie.

1. Télécharger l'archive des sources (`expat-x.y.z.tar.gz` ou `.zip`) depuis https://github.com/libexpat/libexpat/releases
2. Copier les fichiers `.c` du dossier `lib/` de l'archive dans `Expat/src`.
3. Copier les fichiers `.h` du dossier `lib/` de l'archive dans `Expat/include` (sans écraser `expat_config.h`).
4. Optionnel : copier la documentation html (`doc/`) dans `docs/expat`.
5. Recompiler. Si le compilateur signale une macro de configuration manquante, la reporter dans `expat_config.h`
   (voir le fichier `expat_config.h.cmake` de l'archive pour la liste des options).

---

## Utilisation

Dans les fichiers header qui vont utiliser cette librairie (par exemple *FileParser.h*), inclure la ligne :

```c++
#include <Expat/include/expat.h>
```

Explications sur l'utilisation des méthodes de la librairie : voir https://www.xml.com/pub/1999/09/expat/index.html

On utilise [l'outil de déploiement Qt](https://doc.qt.io/qt-6/windows-deployment.html) (`windeployqt`) pour avoir automatiquement toutes les DLL Qt nécessaires dans le répertoire build.

### Notes

Il existe un wrapper c++ pour Expat : https://github.com/ckane/expatmm
