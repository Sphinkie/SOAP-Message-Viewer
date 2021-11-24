# SOAP Message Viewer

## Présentation

Lorsque deux applicatifs communiquent au protocole SOAP, il est possible d'activer des traces dans le fichier *web.config* grâce à la section `<Diagnostics>`.
On obtient alors deux fichiers XML:

- web_messages.svclog
- web_trace.svclog

L'outil habituel pour visualiser ces logs est *SvcTraceViewer.exe* (*Microsoft Service Trace Viewer*), qui présente l'inconvénient d'être lent, complexe, de nécessiter les deux fichiers avec des horaires concordants.

Le présent outil **SOAP Message Viewer** se focalise sur l'essentiel: le contenu des messages échangés. Il est rapide, simple, et ne nécessite que le fichier **web_messages.svclog**.

![Screenshot](docs/images/Capture.png)

# Installation

L'exécutable est disponible dans la section "Release" de GitHub dans une version _portable_ (cad sans installeur. Il suffit de dézipper le fichier dans le répertoire de destination).
Le programme nécessite le runtime C++ de Microsoft 2015-2019 (téléchargeable via le lien fourni).

# Le coin des développeurs

### Compilation

Pour compiler vous-même ce programme, il faut:
- QtStudio 4.12 (framework **Qt** 5.15).
- La librairie externe **Expat**

(Le fichier `.pro` contient les directives nécessaires à la compilation).

### Link

Certains fichiers doivent être copiés manuellement:

- **version Debug**: copier les fichiers **libexpat.dll** et **libexpatw.dll** de Expat/lib dans le répertoire où le fichier **exe** de debug a été généré par le compilateur.
- **version Release**: copier les fichiers **libexpat.dll** et **libexpatw.dll** de Expat/lib dans le répertoire où le fichier **exe** de release a été généré par le compilateur, et ajouter les DLL du Framework Qt grâce à l'outil **winQtDeploy.exe**.

Une méthode simple est de rajouter une étape "déploiement" dans QtStudio pour faire cette opération.

### Sources

Toutes les informations liées au développement sont dans la [documentation doxygen](https://sphinkie.github.io/SOAP-Message-Viewer/doxygen/html/index.html)..

### La librairie Expat

[**Expat**](https://libexpat.github.io/) est un  parseur XML de type SAX, capable de traiter de gros fichiers rapidement.
Comme la méthode SAX permet de traiter les données du XML au fur et à mesure de leur lecture, la librairie peut extraire des informations utiles du fichier XML, même si celui est tronqué ou abimé à la fin.

Accès à la [procédure](docs/how_to_install_Expat_Parser.md) pour installer la librairie Expat.

## Licence

**QT** est en version OpenSource sous licence LGPL.

**Expat** est un logiciel libre sous licence MIT/X.
