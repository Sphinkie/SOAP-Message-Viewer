# Documentation intégrée

L'application de documentation utilisée est [doxygen](https://www.doxygen.nl).

Voir la page officielle des [commandes doxygen ici](https://www.doxygen.nl/manual/index.html)

On utilise des balises compatibles avec Qt, pour avoir la description brève qui apparait dans l'IDE lorsque l'on survole une fonction.
Par contre, on n'utilise pas la fonctionnalité [Qdoc de Qt](https://doc.qt.io/qt-5/qdoc-guide-writing.html) dont le rendu visuel n'est pas aussi bien que doxygen.

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
* `\l {http://doc.qt.io/qt-5/} {Qt Documentation}` : ajout d'un lien de type url   **A tester**
 

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
```

Pour les classes: on met une section `@brief` dans le fichier **header**. Optionellement, on peut mettre une section `@class` dans le fichier cpp.
```c++
/** ***************************************
 * @class Ma classe.
 * ****************************************/ 
```

Note: Les balises typiques de Qdoc sont `/*! ... */`  et `//! ...`


## Balises à rajouter dans doxyfile

A compléter
