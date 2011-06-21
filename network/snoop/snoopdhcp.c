/**
 * @file snoophdcp.c
 * @provides snoopdhcp
 * 
 * $Id: snoopdhcp.c 2020 2009-08-13 17:50:08Z mschul $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <net.h>
#include <stdio.h>
#include <snoop.h>
#include <network.h>
//#include <dhcp.h>


/**
 * Snoop contents of dhcp packet
 * @param dhcp the dhcp packet
 */
/*void snoopdhcp(struct dhcpPkt *dhcp)
{
	bool done = FALSE;
	uchar *opts = NULL;
	ushort len = 0;
	ulong i = 0;
	fprintf(stdout,"DHCP:\t----- DHCP Header -----\n");
	fprintf(stdout,"DHCP:\tOperation = %d ", dhcp->op);
	switch(dhcp->op)
	{
	case DHCP_OP_REQUEST:
		fprintf(stdout,"(REQUEST)\n");
		break;
	case DHCP_OP_REPLY:
		fprintf(stdout,"(REPLY)\n");
		break;
	default:
		fprintf(stdout,"(Unrecognized Operation)\n");
		break;
	}

	fprintf(stdout,"DHCP:\tHardware type = 0x%04x ", dhcp->htype);
	switch(dhcp->htype)
	{
	case ARP_HTYPE_ETHERNET:
		fprintf(stdout, "(Ethernet)\n");
		break;
	default:
		fprintf(stdout, "(Unrecognized Hardware)\n");
		break;
	}
	fprintf(stdout,"DHCP:\tHardware length = %d bytes\n", dhcp->hlen);
	fprintf(stdout,"DHCP:\tHops = %d\n", dhcp->hops);
	fprintf(stdout,"DHCP:\tTransfer id = %d\n", net2hl(dhcp->xid));
	fprintf(stdout,"DHCP:\tSeconds = %d\n", net2hs(dhcp->secs));
	fprintf(stdout,"DHCP:\tFlags = 0x%04x ", net2hs(dhcp->flags));
	switch(net2hs(dhcp->flags))
	{
	case DHCP_BROADCAST:
		fprintf(stdout, "(Broadcast)\n");
		break;
	default:
		fprintf(stdout, "\n");
	}
	fprintf(stdout,"DHCP:\tClient IP Addr = ");
	fprint_iplong(net2hl(dhcp->ciaddr));
	fprintf(stdout, "\n");
	fprintf(stdout,"DHCP:\tYour IP Addr = ");
	fprint_iplong(net2hl(dhcp->yiaddr));
	fprintf(stdout, "\n");
	fprintf(stdout,"DHCP:\tServer IP Addr = ");
	fprint_iplong(net2hl(dhcp->siaddr));
	fprintf(stdout, "\n");
	fprintf(stdout,"DHCP:\tGateway IP Addr = ");
	fprint_iplong(net2hl(dhcp->giaddr));
	fprintf(stdout, "\n");
	fprintf(stdout,"DHCP:\tClient Hardware Addr = ");
	fprint_mac(dhcp->chaddr);
	fprintf(stdout, "\n");
	i = 0;
	fprintf(stdout,"DHCP:\tServer name = ");
	while (NULL != dhcp->sname[i])
	{
		fprintf(stdout,"%c", dhcp->sname[i]);
		i++;
	}
	fprintf(stdout, "\n");
	fprintf(stdout,"DHCP:\tFile = 0x%x\n", dhcp->file);

	opts = dhcp->opts;

	while(!done)
	{
		switch(*opts)
		{
		case DHCP_OPT_END:
			fprintf(stdout, "DHCP:\tEnd Option (255)\n");
			done = TRUE;
			break;

		case DHCP_OPT_PAD:
			break;

		case DHCP_OPT_SUBNET:
			opts++;
			len = *opts;
			if(len != 4)
			{ break; }
			fprintf(stdout,"DHCP:\tSubnet Mask Option (1)\n");
			fprintf(stdout, "\t\tlength = %d\n", len);
			opts++;
			fprintf(stdout, "\t\tsubnet addr = %d", *opts);
			len--;
			while (0 != len)
			{
				opts++;
				fprintf(stdout,".%d", *opts);
				len--;
			}
			fprintf(stdout, "\n");
			break;

		case DHCP_OPT_GATEWAY:
			opts++;
			len = *opts;
			if(0 != (len % 4) || len == 0)
			{ break; }
			fprintf(stdout,"DHCP:\tRouter Option (3)\n");
			fprintf(stdout, "\t\tlength = %d\n", len);
			opts++;
			i = 1;
			fprintf(stdout, "\t\tgateway addr %d = %d", i, *opts);
			len--;
			while (0 != len)
			{
				opts++;
				if(0 == (len % 4))
				{ 
					i++;
					fprintf(stdout, "\n\t\tgateway addr %d = %d", i, *opts);
					opts++;
					len--;
				}
				fprintf(stdout,".%d", *opts);
				len--;
			}
			fprintf(stdout, "\n");
			break;

		case DHCP_OPT_DNS:
			opts++;
			len = *opts;
			if (0 != (len % 4) || 0 == len)
			{ break; }
			fprintf(stdout,"DHCP:\tDomain Name Server Option (6)\n");
			fprintf(stdout, "\t\tlength = %d\n", len);
			opts++;
			i = 1;
			fprintf(stdout, "\t\tDNS addr %d = %d", i, *opts);
			len--;
			while (0 != len)
			{
				opts++;
				if(0 == (len % 4))
				{ 
					i++;
					fprintf(stdout, "\n\t\tDNS addr %d = %d", i, *opts);
					len--;
					opts++;
				}
				fprintf(stdout,".%d", *opts);
				len--;
			}
			fprintf(stdout, "\n");
			break;

		case DHCP_OPT_HNAME:
			opts++;
			len = *opts;
			if (0 == len)
			{ break; }
			fprintf(stdout, "DHCP:\tHost Name Option (12)\n");
			fprintf(stdout, "\t\tlength = %d\n", len);
			opts++;
			fprintf(stdout, "\t\thost name = %c", *opts);
			len--;
			while (0 != len)
			{
				opts++;
				fprintf(stdout,"%c", *opts);
				len--;
			}
			fprintf(stdout, "\n");

		case DHCP_OPT_DOMAIN:
			opts++;
			len = *opts;
			if(0 == len)
			{ break; }
			fprintf(stdout,"DHCP:\tDomain Name Option (15)\n");
			fprintf(stdout, "\t\tlength = %d\n", len);
			opts++;
			fprintf(stdout, "\t\tdomain name = %c", *opts);
			len--;
			while (0 != len)
			{
				opts++;
				fprintf(stdout,"%c", *opts);
				len--;
			}
			fprintf(stdout, "\n");
			break;

		case DHCP_OPT_REQUEST:
			opts++;
			len = *opts;
			if(len != 4)
			{ break; }
			fprintf(stdout,"DHCP:\tRequested IP Address Option (50)\n");
			fprintf(stdout, "\t\tlength = %d\n", len);
			opts++;
			fprintf(stdout, "\t\trequested addr = %d", *opts);
			len--;
			while (0 != len)
			{
				opts++;
				fprintf(stdout,".%d", *opts);
				len--;
			}
			fprintf(stdout, "\n");
			break;

		case DHCP_OPT_LEASE:
			opts++;
			len = *opts;
			if(len != 4)
			{ break; }
			fprintf(stdout,"DHCP:\tAddress Lease Time Option (51)\n");
			fprintf(stdout, "\t\tlength = %d\n", len);
			opts++;
			i+= *opts<<24;
			opts++;
			i+= *opts<<16;
			opts++;
			i+= *opts<<8;
			opts++;
			i+= *opts;
			fprintf(stdout, "\t\tlease time = %u\n", i);
			break;

		case DHCP_OPT_MSGTYPE:
			opts++;
			len = *opts;
			if(len != 1)
			{ break; }
			fprintf(stdout,"DHCP:\tMessage Type Option (53)\n");
			fprintf(stdout, "\t\tlength = %d\n", len);
			fprintf(stdout, "\t\ttype = ");
			opts++;
			switch(*opts)
			{
			case DHCPDISCOVER:
				fprintf(stdout, "DHCPDISCOVER");
				break;
			case DHCPOFFER:
				fprintf(stdout, "DHCPOFFER");
				break;
			case DHCPREQUEST:
				fprintf(stdout, "DHCPREQUEST");
				break;
			case DHCPDECLINE:
				fprintf(stdout, "DHCPDECLINE");
				break;
			case DHCPACK:
				fprintf(stdout, "DHCPACK");
				break;
			case DHCPNAK:
				fprintf(stdout, "DHCPNAK");
				break;
			case DHCPRELEASE:
				fprintf(stdout, "DHCPRELEASE");
				break;
			default:
				fprintf(stdout, "Unrecognized");
				break;
			}
			fprintf(stdout, "\n");
			break;

		case DHCP_OPT_SERVER:
			opts++;
			len = *opts;
			if(len != 4)
			{ break; }
			fprintf(stdout,"DHCP:\tServer Identifier Option (54)\n");
			fprintf(stdout, "\t\tlength = %d\n", len);
			opts++;
			fprintf(stdout, "\t\tserver addr = %d", *opts);
			len--;
			while (0 != len)
			{
				opts++;
				fprintf(stdout,".%d", *opts);
				len--;
			}
			fprintf(stdout, "\n");
			break;
		case DHCP_OPT_PARAMREQ:
			opts++;
			len = *opts;
			if (0 == len)
			{ break; }
			fprintf(stdout, "DHCP:\tParameter Request List Option (55)\n");
			fprintf(stdout, "\t\tlength = %d\n", len);
			fprintf(stdout, "\t\tRequest List:\n");
			while (0 != len)
			{
				opts++;
				switch(*opts)
				{
				case DHCP_OPT_SUBNET:
					fprintf(stdout, "\t\t\tSubnet Option\n");
					break;
				case DHCP_OPT_GATEWAY:
					fprintf(stdout, "\t\t\tGateway Option\n");
					break;
				case DHCP_OPT_DNS:
					fprintf(stdout, "\t\t\tDNS Option\n");
					break;
				case DHCP_OPT_HNAME:
					fprintf(stdout, "\t\t\tHost Name Option\n");
					break;
				case DHCP_OPT_DOMAIN:
					fprintf(stdout, "\t\t\tDomain Name Option\n");
					break;
				case DHCP_OPT_REQUEST:
					fprintf(stdout, "\t\t\tRequest Option\n");
					break;
				case DHCP_OPT_LEASE:
					fprintf(stdout, "\t\t\tLease Option\n");
					break;
				case DHCP_OPT_SERVER:
					fprintf(stdout, "\t\t\tServer Option\n");
					break;
				default:
					fprintf(stdout, "\t\t\tUnrecognized Option %d\n", *opts);
					break;
				}
				len--;
			}
			
		default:
			opts++;
			len = *opts;
			
			opts+=len;
			break;
		}

		opts++;
	}
	fprintf(stdout,"DHCP:\n");
}
*/
