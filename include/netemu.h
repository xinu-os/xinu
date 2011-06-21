/**
 * @file netemu.h
 *
 * $Id: netemu.h 2126 2009-11-12 07:23:04Z svn $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#ifndef _NETEMU_H_
#define _NETEMU_H_

syscall netemu(struct packet *pkt);
syscall emuCorrupt(struct packet *pkt);
syscall emuDelay(struct packet *pkt);
syscall emuDrop(struct packet *pkt);
syscall emuDuplicate(struct packet *pkt);
syscall emuReorder(struct packet *pkt);
#endif                          /* _NETEMU_H_ */
