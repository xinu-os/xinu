/**
 * @file snoopicmp.c
 * @provides snoopicmp
 * 
 * $Id: snoopicmp.c 2020 2009-08-13 17:50:08Z mschul $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <net.h>
#include <stdio.h>
#include <string.h>
#include <interrupt.h>
#include <mailbox.h>
#include <memory.h>
#include <snoop.h>
#include <network.h>
#include <arp.h>
//#include <ip.h>
//#include <icmp.h>
//#include <dhcp.h>
//#include <udp.h>
//#include <dns.h>

/**
 * Print contents of icmp packet 
 * @param icmp the icmp packet
 * @param len length of the icmp packet data and header
 */
/*void snoopicmp(struct icmpPkt *icmp, ushort len)
{
	ulong count = 0;
	ushort temp = 0;

	fprintf(stdout,"ICMP:\t----- ICMP Header -----\n");
	fprintf(stdout,"ICMP:\ttype = 0x%04X ", icmp->type);
	
	switch(icmp->type)
	{
	case ICMP_ECHO_REPLY:
		fprintf(stdout, "(Echo Reply)\n");
		fprintf(stdout, "ICMP:\tcode = 0x%04X\n", icmp->code);
		fprintf(stdout,"ICMP:\tchecksum = 0x%04X ", net2hs(icmp->chksm));
		temp = icmp->chksm;
		icmp->chksm = 0;
		icmp->chksm = ipChecksum((uchar *)icmp, len);
		if(icmp->chksm == temp)
		{ fprintf(stdout, "(correct!)\n"); }
		else
		{ fprintf(stdout, "(Incorrect: Correct = 0x%04X)\n", 
											net2hs(icmp->chksm)); }
		fprintf(stdout,"ICMP:\tidentifier = %d\n",
					net2hs(icmp->id));
		fprintf(stdout,"ICMP:\tsequence = %d\n",
					net2hs(icmp->sequence));
		break;

	case ICMP_DST_UNR:
		fprintf(stdout, "(Destination Unreachable)\n");
		fprintf(stdout, "ICMP:\tcode = 0x%04X ", icmp->code);
		switch(icmp->code)
		{
		case ICMP_NET_UNR:
			fprintf(stdout,"(Net Unreachable)\n");
			break;
		case ICMP_HST_UNR:
			fprintf(stdout,"(Host Unreachable)\n");
			break;
		case ICMP_PROTO_UNR:
			fprintf(stdout,"(Protocol Unreachable)\n");
			break;
		case ICMP_PORT_UNR:
			fprintf(stdout,"(Port Unreachable)\n");
			break;
		case ICMP_FOFF_DFSET:
			fprintf(stdout,"(Framentation needed, DF set)\n");
			break;
		case ICMP_SRCRT_FAIL:
			fprintf(stdout,"(Source Route Failed)\n");
			break;
		default:
			fprintf(stdout,"(Unrecognized Code)\n");
			break;
		}
		fprintf(stdout,"ICMP:\tchecksum = 0x%04X ", net2hs(icmp->chksm));
		temp = icmp->chksm;
		icmp->chksm = 0;
		icmp->chksm = ipChecksum((uchar *)icmp, len);
		if(icmp->chksm == temp)
		{ fprintf(stdout, "(correct!)\n"); }
		else
		{ fprintf(stdout, "(Incorrect: Correct = 0x%04X)\n", 
											net2hs(icmp->chksm)); }
		break;

	case ICMP_SRC_QNCH:
		fprintf(stdout, "(Source Quench)\n");
		fprintf(stdout, "ICMP:\tcode = 0x%04X\n", icmp->code);
		fprintf(stdout,"ICMP:\tchecksum = 0x%04X ", net2hs(icmp->chksm));
		temp = icmp->chksm;
		icmp->chksm = 0;
		icmp->chksm = ipChecksum((uchar *)icmp, len);
		if(icmp->chksm == temp)
		{ fprintf(stdout, "(correct!)\n"); }
		else
		{ fprintf(stdout, "(Incorrect: Correct = 0x%04X)\n", 
											net2hs(icmp->chksm)); }
		break;

	case ICMP_REDIRECT:
		fprintf(stdout, "(Redirect)\n");
		fprintf(stdout, "ICMP:\tcode = 0x%04X ", icmp->code);
		fprintf(stdout, "(Redirect Datagrams ");
		switch(icmp->code)
		{
		case ICMP_RNET:
			fprintf(stdout,"Network)\n");
			break;
		case ICMP_RHST:
			fprintf(stdout,"Host)\n");
			break;
		case ICMP_RTOS_NET:
			fprintf(stdout,"Type of Service and Network)\n");
			break;
		case ICMP_RTOS_HST:
			fprintf(stdout,"Type of Service and Host)\n");
			break;
		default:
			fprintf(stdout,"(Unrecognized Code)\n");
			break;
		}
		fprintf(stdout,"ICMP:\tchecksum = 0x%04X ", net2hs(icmp->chksm));
		temp = icmp->chksm;
		icmp->chksm = 0;
		icmp->chksm = ipChecksum((uchar *)icmp, len);
		if(icmp->chksm == temp)
		{ fprintf(stdout, "(correct!)\n"); }
		else
		{ fprintf(stdout, "(Incorrect: Correct = 0x%04X)", 
											net2hs(icmp->chksm)); }
		ulong gateway;
		uchar gatewayip[IP_ADDR_LEN];
		gateway = (net2hs(icmp->id) << 16) 
					+ net2hs(icmp->sequence);
		long2bytes(gateway, gatewayip);
		fprintf(stdout,"ICMP:\tgateway address = ");
		fprint_ip(gatewayip);
		fprintf(stdout,"\n");
		break;

	case ICMP_ECHO:
		fprintf(stdout, "(Echo)\n");
		fprintf(stdout, "ICMP:\tcode = 0x%04X\n", icmp->code);
		fprintf(stdout,"ICMP:\tchecksum = 0x%04X ", net2hs(icmp->chksm));
		temp = icmp->chksm;
		icmp->chksm = 0;
		icmp->chksm = ipChecksum((uchar *)icmp, len);
		if(icmp->chksm == temp)
		{ fprintf(stdout, "(correct!)\n"); }
		else
		{ fprintf(stdout, "(Incorrect: Correct = 0x%04X)\n", 
											net2hs(icmp->chksm)); }
		fprintf(stdout,"ICMP:\tidentifier = %d\n", 
					net2hs(icmp->id));
		fprintf(stdout,"ICMP:\tsequence = %d\n",
					net2hs(icmp->sequence));
		break;

	case ICMP_TIME_EXCD:
		fprintf(stdout, "(Time Exceeded)\n");
		fprintf(stdout, "ICMP:\tcode = 0x%04X ", icmp->code);
		switch(icmp->code)
		{
		case ICMP_TTL_EXC:
			fprintf(stdout,"(Time to Live Exceeded)\n");
			break;
		case ICMP_FRA_EXC:
			fprintf(stdout,"(Fragment Reassembly Time Exceeded)\n");
			break;
		default:
			fprintf(stdout,"(Unrecognized Code)\n");
			break;
		}
		fprintf(stdout,"ICMP:\tchecksum = 0x%04X ", net2hs(icmp->chksm));
		temp = icmp->chksm;
		icmp->chksm = 0;
		icmp->chksm = ipChecksum((uchar *)icmp, len);
		if(icmp->chksm == temp)
		{ fprintf(stdout, "(correct!)\n"); }
		else
		{ fprintf(stdout, "(Incorrect: Correct = 0x%04X)\n", 
											net2hs(icmp->chksm)); }
		break;

	case ICMP_PARAM_PROB:
		fprintf(stdout, "(Parameter Problem)\n");
		fprintf(stdout, "ICMP:\tcode = 0x%04X ", icmp->code);
		switch(icmp->code)
		{
		case ICMP_SEE_PNTR:
			fprintf(stdout,"(Error)\n");
			break;
		case ICMP_MISSING_OPT:
			fprintf(stdout,"(Missing Option)\n");
			break;
		case ICMP_BAD_LENGTH:
			fprintf(stdout,"(Bad Length)\n");
		default:
			fprintf(stdout,"(Unrecognized Code)\n");
			break;
		}
		fprintf(stdout,"ICMP:\tchecksum = 0x%04X ", net2hs(icmp->chksm));
		temp = icmp->chksm;
		icmp->chksm = 0;
		icmp->chksm = ipChecksum((uchar *)icmp, len);
		if(icmp->chksm == temp)
		{ fprintf(stdout, "(correct!)\n"); }
		else
		{ fprintf(stdout, "(Incorrect: Correct = 0x%04X)\n", 
											net2hs(icmp->chksm)); }
		break;

	case ICMP_TM_STMP:
		fprintf(stdout, "(Timestamp)\n");
		fprintf(stdout, "ICMP:\tcode = 0x%04X\n", icmp->code);
		fprintf(stdout,"ICMP:\tchecksum = 0x%04X ", net2hs(icmp->chksm));
		temp = icmp->chksm;
		icmp->chksm = 0;
		icmp->chksm = ipChecksum((uchar *)icmp, len);
		if(icmp->chksm == temp)
		{ fprintf(stdout, "(correct!)\n"); }
		else
		{ fprintf(stdout, "(Incorrect: Correct = 0x%04X)\n", 
											net2hs(icmp->chksm)); }
		fprintf(stdout,"ICMP:\tidentifier = %d\n", 
					net2hs(icmp->id));
		fprintf(stdout,"ICMP:\tsequence = %d\n",
					net2hs(icmp->sequence));
		count = 0;
		fprintf(stdout,"ICMP:\tOriginate Timestamp = %d\n",
					(icmp->data + count));
		count++;
		fprintf(stdout,"ICMP:\tReceive Timestamp = %d\n",
					(icmp->data + count));
		count++;
		fprintf(stdout,"ICMP:\tTransmit Timestamp = %d\n",
					(icmp->data + count));
		break;

	case ICMP_TM_STMP_REPLY:
		fprintf(stdout, "(Timestamp Reply)\n");
		fprintf(stdout, "ICMP:\tcode = 0x%04X\n", icmp->code);
		fprintf(stdout,"ICMP:\tchecksum = 0x%04X ", net2hs(icmp->chksm));
		temp = icmp->chksm;
		icmp->chksm = 0;
		icmp->chksm = ipChecksum((uchar *)icmp, len);
		if(icmp->chksm == temp)
		{ fprintf(stdout, "(correct!)\n"); }
		else
		{ fprintf(stdout, "(Incorrect: Correct = 0x%04X)\n", 
											net2hs(icmp->chksm)); }
		break;

	case ICMP_INFO_RQST:
		fprintf(stdout, "(Information Request)\n");
		fprintf(stdout, "ICMP:\tcode = 0x%04X\n", icmp->code);
		fprintf(stdout,"ICMP:\tchecksum = 0x%04X ", net2hs(icmp->chksm));
		temp = icmp->chksm;
		icmp->chksm = 0;
		icmp->chksm = ipChecksum((uchar *)icmp, len);
		if(icmp->chksm == temp)
		{ fprintf(stdout, "(correct!)\n"); }
		else
		{ fprintf(stdout, "(Incorrect: Correct = 0x%04X)\n", 
											net2hs(icmp->chksm)); }
		fprintf(stdout,"ICMP:\tidentifier = %d\n", 
					net2hs(icmp->id));
		fprintf(stdout,"ICMP:\tsequence = %d\n",
					net2hs(icmp->sequence));
		break;

	case ICMP_INFO_REPLY:
		fprintf(stdout, "(Information Reply)\n");
		fprintf(stdout, "ICMP:\tcode = 0x%04X\n", icmp->code);
		fprintf(stdout,"ICMP:\tchecksum = 0x%04X ", net2hs(icmp->chksm));
		temp = icmp->chksm;
		icmp->chksm = 0;
		icmp->chksm = ipChecksum((uchar *)icmp, len);
		if(icmp->chksm == temp)
		{ fprintf(stdout, "(correct!)\n"); }
		else
		{ fprintf(stdout, "(Incorrect: Correct = 0x%04X)\n", 
											net2hs(icmp->chksm)); }
		fprintf(stdout,"ICMP:\tidentifier = %d\n", 
					net2hs(icmp->id));
		fprintf(stdout,"ICMP:\tsequence = %d\n",
					net2hs(icmp->sequence));
		break;

	case ICMP_TRACEROUTE:
		fprintf(stdout, "(Traceroute)\n");
		fprintf(stdout, "ICMP:\tcode = 0x%04X ", icmp->code);
		switch(icmp->code)
		{
		case OP_FORWARD:
			fprintf(stdout,"(Forward)\n");
			break;
		case OP_FAILED:
			fprintf(stdout,"(Failed)\n");
			break;
		default:
			fprintf(stdout,"(Unrecognized Code)\n");
			break;
		}
		fprintf(stdout,"ICMP:\tchecksum = 0x%04X ", net2hs(icmp->chksm));
		temp = icmp->chksm;
		icmp->chksm = 0;
		icmp->chksm = ipChecksum((uchar *)icmp, len);
		if(icmp->chksm == temp)
		{ fprintf(stdout, "(correct!)\n"); }
		else
		{ fprintf(stdout, "(Incorrect: Correct = 0x%04X)\n", 
											net2hs(icmp->chksm)); }
		break;

	default:
		fprintf(stdout,"(Unrecognized Type)\n");
		break;
	}
	fprintf(stdout,"ICMP:\n");
}
*/
