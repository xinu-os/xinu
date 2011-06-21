/**
 * @file httpCleanPrompts.c
 * @provides cleanPrompts.
 *
 * $Id: httpCleanPrompts.c 2077 2009-09-24 23:58:54Z mschul $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <stdlib.h>
#include <http.h>
#include <shell.h>
#include <string.h>

/**
 * Clears the prompts from a provided buffer.
 * @param buf buffer to cook prompts out of
 * @param len length of provided buffer
 * @param webptr http device struct if applicable, NULL if generic buffer
 * @return the updated length of the buffer
 **/
int cleanPrompts(device *devptr, char *buf, int len, struct http *webptr)
{
    char *found, *end;
    uint promptlen, lentoend, basepromptlen;
    char *prompt = SHELL_PROMPT;
    char *start = buf;
    bool discovered = FALSE;

    /* Cook out shell prompts from the intermediate buffer */
    if (len > 4 && NULL != webptr
        && !(webptr->flags & HTTP_FLAG_CLEANSED))
    {

        basepromptlen = strnlen(SHELL_PROMPT, MAX_PROMPT_LEN);
        found = strstr(buf, prompt);

        while (NULL != found)
        {
            start = found + basepromptlen;

            /* Shell will either be @hostname or just $ */
            if ('@' == *(start) || '$' == *(start))
            {
                /* Find the next space in the string (end of prompt) */
                end = strstr(start, " ");
                if (NULL != end)
                {
                    discovered = TRUE;

                    /* Calculate length to end of buffer from prompt */
                    lentoend = len - (uint)(found - buf) + 1;

                    /* Calculate the prompt length */
                    promptlen = (uint)(end - found);

                    /* Move stuff after the prompt to where prompt is */
                    memcpy(found, found + promptlen,
                           lentoend - promptlen);

                    /* Subtract from the buffer length */
                    len -= promptlen;

                    /* Zero out the part of the buffer that was moved */
                    bzero(found + (lentoend - promptlen), promptlen);
                }

            }

            /* Attempt to find the next shell prompt */
            found = strstr(start, prompt);
        }

        /* If cleansing structure, set flag */
        if (NULL != webptr)
        {
            httpControl(devptr, HTTP_CTRL_SET_FLAG, HTTP_FLAG_CLEANSED,
                        NULL);
        }

        /* If final page was waiting for shell output to finish */
        if (NULL != webptr && discovered &&
            (webptr->flags & HTTP_FLAG_WAITONSHELL))
        {
            httpControl(devptr, HTTP_CTRL_CLR_FLAG, HTTP_FLAG_WAITONSHELL,
                        NULL);
        }

    }

    return len;
}
