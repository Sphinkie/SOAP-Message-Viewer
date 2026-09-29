/* expat_config.h : configuration minimale pour compiler Expat directement dans le projet (Windows).
 * Normalement généré par le CMake d'Expat. Valeurs par défaut de la distribution Expat 2.4.1. */

#ifndef EXPAT_CONFIG_H
#define EXPAT_CONFIG_H

/* 1234 = little endian (x86, x64, ARM64 Windows) */
#define BYTEORDER 1234

/* Taille du contexte conservé autour des erreurs de parsing (utilisé par XML_GetInputContext) */
#define XML_CONTEXT_BYTES 1024

/* Support des DTD et des namespaces */
#define XML_DTD 1
#define XML_NS 1

#endif /* EXPAT_CONFIG_H */
