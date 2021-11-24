# Documentation intégrée (Qdoc)



Voir la page officielle de [doxygen ici](https://www.doxygen.nl/manual/index.html)

La fonctionnalité Qdoc de Qt est décrite [ici](https://doc.qt.io/qt-5/qdoc-guide-writing.html) et ici pour [la génération de la doc](https://doc.qt.io/qt-5/qdoc-guide-conf.html)

## Balises utilisées

* `@brief` : oui (voir si on peut remplacer par AUTOBRIEF)

* `@param`  ou `@param[in,out]` : oui. (Ne pas mettre de `:`)

* `@retval TRUE Successfuly done` : Explication des valeurs retournées.

* `@ref doc/readme.md` : lien vers un fichier *markdown*

* `@todo { ... }` 

* `@class` : à voir

* `@warning` `@see {...}` ou ``@attention`: selon les cas...

* `@ingroup` : à voir. Utile semble-t-il.

* Link : `Read the \l {http://doc.qt.io/qt-5/} {Qt Documentation} carefully.`

  

```ini
GENERATE_QHP           = YES
QCH_FILE               = SOAP-Message-Viewer.qch
QHP_NAMESPACE          = sphinkie.SOAPMessageViewer
QHP_VIRTUAL_FOLDER     = doc
QHG_LOCATION           = qhelpgenerator
```

The name of the *.qch* file can be whatever you choose, as is the QHP namespace. 

An important note is that the *qhelpgenerator* tool has to be on your path. I normally add the following setting to my **$PATH** variable :

```ini
QT_TOOL_BIN = $HOME/Qt/5.13.1/gcc_64/bin
export PATH = $PATH:$HOME/.bin:$QT_TOOL_BIN
```



### Mise en page

* Mise en gras : `\b word`

* `\c word`: le mot est mis en police console

  

## Balises non utilisées

* `@var`, `@function`, `@file` : non
* `@author`, `@details` : non


## Sections doxygen



La balise typique de Qdoc est `/*! ... */`  et `//!`



Pour les fonctions:

```c++
/** **************************************
 * @brief Ma function.
 *     Inconvénient: la ligne de *** est incluse avec le texte...
 * *************************************** */ 
```
Pour les variables:
```c++
int cpt;    /**< compteur */
```

Pour les classes: on met une section `@brief` dans ls fichier **header**. et optionllement, on peut mettre une section `@class` dans le fichier cpp.
```c++
/** ***************************************
 * @class Ma classe.
 * ****************************************/ 
```



## Page d'accueil de la doc



```c++
/*! \mainpage My Personal Index Page
 *
 * \section intro_sec Introduction
 *
 * This is the introduction.
 *
 * \section install_sec Installation
 *
 * \subsection step1 Step 1: Opening the box
 *
 * etc...
 */
```



## Formalisme doxygen non supporté par Qt

Pour les fonctions:
```c++
/**************************************//**
 * @brief Ma function.
 *    si JAVADOC_BANNER=YES est positionné
 *****************************************/ 
```

Pour les variables:
```c++
int cpt;    //< compteur
```

https://www.doxygen.nl/manual/commands.html

https://www.sinax.be/blog/software-development/display-doxygen-generated-docs-in-qt-creator.html

http://tvaira.free.fr/projets/activites/activite-documentation-doxygen.html





