#include "Common.h"

/* ************************************************************************************************
 * Fonction globale endsWith case-sentitive.
 * Pas de blindage: str1 et str2 doivent etre plus longues que len.
 * ************************************************************************************************ */
bool Common::endsWith(const char* str1, const char* str2, int len)
{
    int len1 = strlen(str1);
    int len2 = strlen(str2);
    int score= 0;
    int i    = 0;

    do
    {
        if (str1[len1-i]==str2[len2-i])
            score++;
        i++;

    } while ((score>0) && (i<len));
    return (score==len);
}
