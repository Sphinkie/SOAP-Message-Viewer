#ifndef COMMON_H
#define COMMON_H

#include <string>

/** ****************************************************************************
 * @brief Classe pour des fonctions statiques généralistes.
 * @author David de Lorenzo
 * *****************************************************************************/
class Common
{
public:
    static bool endsWith(const char* str1, const char* str2, int len);
};

#endif // COMMON_H
