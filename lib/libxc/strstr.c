/**
 * @file strstr.c
 * @provides strstr.
 *
 * $Id: strstr.c 2020 2009-08-13 17:50:08Z mschul $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

/** 
 * Returns a pointer to the location in a string at which a particular
 * string appears.
 * @param cs string to search
 * @param ct string to locate
 * @return the pointer in the string, NULL if string not found
 */
char *strstr(const char *cs, const char *ct)
{
    char *cq;
    char *cr;

    for (; *cs != '\0'; cs++)
    {
        if (*cs == *ct)
        {
            cq = (char *)cs;
            cr = (char *)ct;
            while ((*cq != '\0') && (*cr != '\0'))
            {
                if (*cq != *cr)
                {
                    break;
                }
                cq++;
                cr++;
            }
            if ('\0' == *cr)
            {
                return (char *)cs;
            }
        }
    }
    return 0;
}
