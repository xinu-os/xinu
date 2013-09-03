/**
 * @file httpHtmlBegin.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stdio.h>

#include <http.h>
#include <shell.h>
#include <string.h>

/**
 * Write the Xinu banner and top of webpage out to the underlying device.
 * @param devptr pointer to the underlying device to write to
 */
void httpHtmlBeginPage(device *devptr)
{
    char *banner, *htmlBegin, *bannerEnd;

    htmlBegin =
        "<html><head><title>Embedded Xinu Web Interface</title></HEAD><BODY>"
        "<table width='800'><tr><td align='center'>"
        "<div style='font-family:courier;font-size:13;font-weight:bold;"
        "color:#aa0000'><pre>";

    banner = SHELL_BANNER_NONVT100;

    bannerEnd = "</pre></div></td></tr>";

    /* Write out the top of the webpage */
    httpWrite(devptr, htmlBegin, strnlen(htmlBegin, HTTP_STR_MD));
    httpWrite(devptr, banner, strnlen(banner, HTTP_STR_LG));
    httpWrite(devptr, bannerEnd, strnlen(bannerEnd, HTTP_STR_SM));
}


/**
 * Write the navigation portion of the Xinu web interface.
 * @param devptr pointer to the underlying device to write to
 */
void httpHtmlNavigation(device *devptr)
{
    int j;
    char *beginNav, *endNav, *navlinks;

    beginNav =
        "<!begin navigation>"
        "<tr><td align='center'>"
        "<div style='font-family:courier;font-size:13'>";

    endNav = "</div></td></tr>" "<!end navigation>";

    /* Write out the beginning of page navigation */
    httpWrite(devptr, beginNav, strnlen(beginNav, HTTP_STR_SM));

    /* Write out the navigation links based on commands in httpcmdtab */
    for (j = 0; j < nhttpcmd; j++)
    {
        if (j % HTTP_LINKS_PER_LINE != 0)
        {
            printf(" | ");
        }
        if (j > 0 && j % HTTP_LINKS_PER_LINE == 0)
        {
            printf("<br>");
        }

        navlinks = "<a href='%s' target='_self' "
            "style='text-decoration:none'>%s</a>";
        printf(navlinks, httpcmdtab[j].url, httpcmdtab[j].urlname);
    }

    /* Write out the end of page navigation */
    httpWrite(devptr, endNav, strnlen(endNav, HTTP_STR_SM));
}


/**
 * Write the HTML navigation portion of the Xinu web interface.
 * @param devptr pointer to the underlying device to write to
 */
void httpHtmlBeginContent(device *devptr)
{
    char *beginContent;

    beginContent =
        "<!begin content>" "<tr><td>" "<br><pre style='width:800'>";

    httpWrite(devptr, beginContent, strnlen(beginContent, HTTP_STR_SM));
}
