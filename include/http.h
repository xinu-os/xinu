/**
 * @file http.h
 *
 * $Id: http.h 2076 2009-09-24 23:05:39Z brylow $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */
#ifndef _HTTP_H_
#define _HTTP_H_

#include <stdarg.h>
#include <stddef.h>

#include <device.h>
#include <network.h>
#include <semaphore.h>

#define HTTP_LOCAL_PORT 80

/* N sizes for strnlen calls */
#define HTTP_STR_SM  256
#define HTTP_STR_MD  512
#define HTTP_STR_LG  1024
#define HTTP_STR_XL  16536

/* Page layout variables */
#define HTTP_LINKS_PER_LINE 5

/* HTTP device states */
#define HTTP_STATE_FREE     0
#define HTTP_STATE_ALLOC    1

/* HTTP method types */
#define HTTP_METHOD_NOALLOW -2
#define HTTP_METHOD_GET      0
#define HTTP_METHOD_POST     1
#define HTTP_METHOD_HEAD     2

/* HTTP method types */
#define HTTP_VERSION_10     0
#define HTTP_VERSION_11     1

/* Body page types */
#define NO_BODY         0
#define CONFIG_PAGE     1
#define FLASHOS_PAGE    2

/* Config Variables */
#define HTTP_NCONFIG_VARS 4*NETHER + 1  /**<variables on config page    */

/* Structure buffer sizes */
#define HTTP_OBLEN      HTTP_WBLEN + 8  /**<output buffer length        */
#define HTTP_RBLEN      4096            /**<http read buffer length     */
#define HTTP_WBLEN      1024            /**<http write buffer length    */
#define HTTP_MAX_HDRS   19              /**<maximum headers in request  */
#define HTTP_MAX_CHUNK  5               /**<max size of chunk info      */

/* HTTP error response codes */
#define HTTP_ERR_BADREQ		400     /**<Client sent malformed request   */
#define HTTP_ERR_AUTHREQ	401     /**<Authorization reqired           */
#define HTTP_ERR_FORBID		403     /**<Forbidden                       */
#define HTTP_ERR_NOTFND		404     /**<File not found                  */
#define HTTP_ERR_METHOD		405     /**<Method not allowed              */
#define HTTP_ERR_INTSERV	500     /**<Internal Server Error           */
#define HTTP_ERR_NOTIMP		501     /**<Not Implemented                 */
#define HTTP_ERR_BADVERS	505     /**<HTTP version not supported      */

/* httpControl() functions */
#define HTTP_CTRL_SET_FLAG  0
#define HTTP_CTRL_CLR_FLAG  1

/* HTTP flags */
#define HTTP_FLAG_CONCLOSE      0x00000001
#define HTTP_FLAG_RQSTEND       0x00000002
#define HTTP_FLAG_CHUNKED       0x00000004
#define HTTP_FLAG_WAITONSHELL   0x00000008
#define HTTP_FLAG_AWAITINGRQST  0x00000010
#define HTTP_FLAG_ENDREADOUTPUT 0x00000020
#define HTTP_FLAG_CLEANSED      0x00000040
#define HTTP_FLAG_CLEARWOUT     0x00000080
#define HTTP_FLAG_FLUSHWOUT     0x00000100


/**
 * Check that the http device id is between HTTP0 and HTTP0+NHTTP
 *
 * @param f device id number to verify
 */
#define isbadhttp(f)  ( !(HTTP0 <= (f) && (f) < (HTTP0 + NHTTP)) )


/**
 * Defines what an entry in http command table looks like.
 */
struct httpcmd
{
    char *command;              /**< command to run on shell            */
    char *url;                  /**< url of command                     */
    char *urlname;              /**< text to display with url link      */
    bool inwebshell;            /**< TRUE if command is run in webshell */
};
extern const struct httpcmd httpcmdtab[];
                                    /**< table of shell cmds over http  */
extern ulong nhttpcmd;              /**< number of commands in table    */
extern semaphore maxhttp;           /**< counter for HTTP threads       */
extern semaphore activeXWeb;        /**< on/off status of webserver     */

/* HTTP device structure */
struct http
{
    uchar state;                /**< HTTP_STATE_* as denoted above      */

    /* Pointers to associated structures */
    device *phw;                /**< hardware device structure          */

    /* Shell interaction fields */
    char out[HTTP_OBLEN];       /**< output buffer                      */

    /* TCP interaction fields */
    char rin[HTTP_RBLEN];       /**< read input buffer                  */
    uint rstart;                /**< index of first char in buffer      */
    uint rcount;                /**< number of characters in buffer     */

    char wout[HTTP_WBLEN + 1];  /**< intermediate write output buffer   */
    uint wstart;                /**< index of first char in buffer      */
    uint wcount;                /**< number of characters in buffer     */

    /* Device values determined with each request */
    semaphore closeall;         /**< wait on before closing all devices */

    int flags;                  /**< Control flags for above bools      */
    uint keepalive;             /**< seconds to keep connection active  */

    /* Header fields and associated values */
    int contentlen;             /**< content length at end of request   */
    int boundarylen;            /**< boundary len for content separator */
    int hdrcount;               /**< number of headers read in request  */
    int hdrend[HTTP_MAX_HDRS];  /**< end of each header in rin          */
    char *content;              /**< content at end of HTTP request     */
    char *boundary;             /**< string used as content boundary    */

    /* NVRAM lookup character pointers */
    char *hostname_str;
    char *domainname_str;
    char *lan_ip_str;
    char *subnet_mask_str;
    char *gate_ip_str;
};


/* Table of http devices */
extern struct http httptab[];


/* Device functions */
devcall httpInit(device *);
devcall httpOpen(device *, va_list);
devcall httpClose(device *);
devcall httpRead(device *, void *, uint);
devcall httpWrite(device *, void *, uint);
devcall httpGetc(device *);
devcall httpPutc(device *, char);
devcall httpControl(device *, int, long, long);
thread httpServerKickStart(int);

/* Helper functions */
int httpAlloc(void);
int httpErrorResponse(device *, short);
int httpFree(device *);
int httpReadRqst(device *);
int httpReadHdrs(struct http *);
int validMethod(char *, int);
int validVersion(char *, int);
int validURI(char *, int);
int postConfig(struct http *);
int setupConfig(struct http *);
int outputConfig(device *, int);
int postFlashOS(struct http *);
void outputFlashOS(device *, int);
void httpHtmlBeginPage(device *);
void httpHtmlNavigation(device *);
void httpHtmlBeginContent(device *);
void httpFlushWBuffer(device *);
int cleanPrompts(device *, char *, int, struct http *);

#endif                          /* _HTTP_H_ */
