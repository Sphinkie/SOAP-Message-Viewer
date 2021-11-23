# Documentation intégrée (Qdoc)

L'application de documentation utilisée est [doxygen](https://www.doxygen.nl).

Voir la page officielle de [doxygen ici](https://www.doxygen.nl/manual/index.html)

La fonctionnalité Qdoc de Qt est décrite [ici](https://doc.qt.io/qt-5/qdoc-guide-writing.html) et ici pour [la génération de la doc](https://doc.qt.io/qt-5/qdoc-guide-conf.html)

On utilise des balises compatibles avec Qt, pour avoir la description brève qui apparait dans l'IDE lorsque l'on survole une fonction.

## Balises utilisées

* `@brief` : Brève description de la fonction (fichier `.cpp`) ou de la classe (fichier `.h`).
* `@param myParam`  ou `@param[in,out] myParam` : Description des paramètres
* `@return` : Description de la valeur retournée
* autres:
  * `@retval TRUE Successfuly done` : Explication des valeurs retournées.
  * `@author ...` : Auteur du code (optionnelement pour les classes).
  * `@sa UneClasse` ou `@sa UneFonction()` : Lien de type **See Also**
  * `@ref doc/readme.md` : lien vers un fichier *markdown*
  * `@todo  ... `  : **compatible Qt ???**
  * `@warning ...` ou `@attention ...` : Ajout d'un texte mis en évidence dans la description.
  * `@ingroup` : à voir. Utile semble-t-il.


### Mise en page

* `\b word` : le mot est mis en **gras**
* `\e word` : le mot est mis en *italique*
* `\c word` : le mot est mis en police console
* `\l {http://doc.qt.io/qt-5/} {Qt Documentation}` : ajout d'un lien de type url
 

## Balises non utilisées

* `@var`, `@function`, `@file`, `@details`


## Sections doxygen

Pour les fonctions:

```c++
/** **************************************
 * @brief Ma function.
 * @sa Autrefonction() # Pour éviter que la ligne de *** soit affichée par Qt.
 * *************************************** */ 
```
Pour les variables:
```c++
int cpt;    /**< compteur */
int i1;      ///< index
int i2;      //!< index
``

Pour les classes: on met une section `@brief` dans le fichier **header**. Optionellement, on peut mettre une section `@class` dans le fichier cpp.
```c++
/** ***************************************
 * @class Ma classe.
 * ****************************************/ 
```
Note: La balise typique de Qdoc est `/*! ... */`  et `//!`


## Balises à rajouter dans doxyfile

A compléter
