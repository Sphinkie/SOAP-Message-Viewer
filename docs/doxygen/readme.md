# Documentation intégrée

L'application de documentation utilisée est [doxygen](https://www.doxygen.nl).

Voir la page officielle des [commandes doxygen ici](https://www.doxygen.nl/manual/index.html)

On utilise des balises compatibles avec Qt, pour avoir la description brève qui apparait dans l'IDE lorsque l'on survole une fonction.
Par contre, on n'utilise pas la fonctionnalité [Qdoc de Qt](https://doc.qt.io/qt-5/qdoc-guide-writing.html) dont le rendu visuel n'est pas aussi bien que doxygen.

## Balises utilisées

* `@brief` : Brève description de la fonction (fichier `.cpp`) ou de la classe (fichier `.h`).
* `@param myParam`  ou `@param[in,out] myParam` : Description des paramètres
* `@return` : Description de la valeur retournée
* Autres tags:
  * `@retval TRUE Successfuly done` : Explication des valeurs retournées.
  * `@author ...` : Auteur du code (optionnelement pour les classes).
  * `@sa UneClasse` ou `@sa UneFonction()` : Lien de type **See Also**
  * `@ref doc/readme.md` : lien vers un fichier *markdown*
  * `@todo  ... ` : **compatible Qt ???**
  * `@pre  ... ` : Affichage d'un texte concernant les préalables à l'appel d'une fonction.
  * `@warning ...` ou `@attention ...` ou `@remark ...`: Ajout d'un texte mis en évidence dans la description.
  * `@ingroup` : à voir. 


### Mise en page

* `\n` : retour à la ligne
* `\e word` : le mot est mis en *italique*
* `\b word` : le mot est mis en **gras**
* `\c word` : le mot est mis en police console
* `\verbatim .... \endverbatim` : le texte est inclus tel quel dans la documentation 


## Balises non utilisées

* `@var`, `@function`, `@file`, `@details`


## Sections doxygen

Pour les fonctions:
```c++
/** **************************************
 * @brief Ma function.
 * @sa Autrefonction()      # Pour éviter que la ligne de *** soit affichée par Qt.
 * *************************************** */ 
```

Pour les variables:
```c++
int cpt;    /**< compteur */
int i1;     ///< index
int i2;     //!< index
```

Pour les classes: on met une section `@brief` dans le fichier **header**. Optionnellement, on peut mettre une section `@class` ailleurs, pour compléter les informations.
```c++
/** ***************************************
 * @class MyClass
 * @details ....
 * ****************************************/ 
```

Note: Les balises typiques de Qdoc sont `/*! ... */`  et `//! ...`


## Balises à rajouter dans doxyfile

### Balises pour compatibilité Qt

```ini
GENERATE_QHP           = YES
QCH_FILE               = SOAP-Message-Viewer.qch
QHP_NAMESPACE          = sphinkie.SoapMessageViewer
QHP_VIRTUAL_FOLDER     = doc
QHG_LOCATION           = qhelpgenerator
```

Le nom du fichier *.qch* et du *namespace* peuvent être choisis librement. 

Note importante : l'outil *qhelpgenerator* doit être dans le PATH.

```
C:\Qt\5.15.0\msvc2019_64\bin
```
Voir à ce sujet, le site de [synax](https://www.sinax.be/blog/software-development/display-doxygen-generated-docs-in-qt-creator.html).

Affichage des fonctions et variables privées dans la documentation:

```ini
EXTRACT_PRIVATE        = YES
```



## Bibliographie

http://tvaira.free.fr/projets/activites/activite-documentation-doxygen.html



