#include "Common.h"

/** ***********************************************************************************************
 * @brief Fonction case-sentitive de comparaison des derniers caractères de deux chaines.
 * @attention Pas de blindage: str1 et str2 doivent être plus longues que len.
 * @param str1 : Chaine de caractères
 * @param str2 : Chaine de caractères
 * @param len : Nombre de caractères finaux comparés
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
