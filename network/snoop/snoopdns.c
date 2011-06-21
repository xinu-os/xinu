/**
 * @file snoopdns.c
 * @provides snoopdns
 * 
 * $Id: snoopdns.c 2020 2009-08-13 17:50:08Z mschul $
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
 * Snoop contents of dns packet
 * @param dns the dns packet
 */
/*void snoopdns(struct dnsPkt *dns)
{
	ulong var;
	uchar *ptr = NULL;
	ushort type = 0;
	ushort rdatalen = 0;
	ulong numbofsect = 0;
	ulong count = 0;
	fprintf(stdout,"DNS:\t----- DNS Header -----\n");
	fprintf(stdout,"DNS\tID = %d\n", net2hs(dns->id));
	fprintf(stdout,"DNS:\tQR = %d ", (dns->opts1 & DNS_QR) >> 7);
	switch((dns->opts1 & DNS_QR) >> 7)
	{
	case DNS_Q:
		fprintf(stdout,"(Query)\n");
		break;
	case DNS_R:
		fprintf(stdout,"(Response)\n");
		break;
	default:
		fprintf(stdout,"(Unrecognized QR)\n");
		break;
	}
	fprintf(stdout,"DNS:\tOpcode = %d ", (dns->opts1 & DNS_OPCODE) >> 3);
	switch((dns->opts1 & DNS_OPCODE) >> 3)
	{
	case DNS_CODE_Q:
		fprintf(stdout, "(Query)\n");
		break;
	case DNS_CODE_IQ:
		fprintf(stdout, "(Inverse Query)\n");
		break;
	case DNS_CODE_STATUS:
		fprintf(stdout, "(Status)\n");
		break;
	default:
		fprintf(stdout, "(Unrecognized Opcode)\n");
		break;
	}
	fprintf(stdout,"DNS:\tAuthoritative Answer = %d\n", 
				(dns->opts1 & DNS_AA) >> 2);
	fprintf(stdout,"DNS:\tTruncation = %d\n", (dns->opts1 & DNS_TC) >> 1);
	fprintf(stdout,"DNS:\tRecursion Desired = %d\n", (dns->opts1 & DNS_RD));
	fprintf(stdout,"DNS:\tRecursion Available = %d\n", 
				(dns->opts2 & DNS_RA) >> 7);
	fprintf(stdout,"DNS:\tResponse Code = %d ", dns->opts2 & DNS_RCODE);
	switch(dns->opts2 & DNS_RCODE)
	{
	case DNS_RESP_NOERR:
		fprintf(stdout, "(No Error)\n");
		break;
	case DNS_RESP_FORMAT:
		fprintf(stdout, "(Format Error)\n");
		break;
	case DNS_RESP_SERV:
		fprintf(stdout, "(Server Error)\n");
		break;
	case DNS_RESP_NAME:
		fprintf(stdout, "(Name Error)\n");
		break;
	case DNS_RESP_NOIMP:
		fprintf(stdout, "(Not Implemented Error)\n");
		break;
	case DNS_RESP_REFUSE:
		fprintf(stdout, "(Refused)\n");
		break;
	default:
		fprintf(stdout, "(Unrecognized Code)\n");
		break;
	}
	fprintf(stdout,"DNS:\tQuestion Entry in Question Section Count = %d\n", 
				net2hs(dns->qdcount));
	fprintf(stdout,"DNS:\tResource Record in Answer Section Count = %d\n", 
				net2hs(dns->ancount));
	fprintf(stdout,"DNS:\tName Server Resource Record Count = %d\n", 
				net2hs(dns->nscount));
	fprintf(stdout,"DNS:\tResource Record in Records Section Count = %d\n", 
				net2hs(dns->arcount));
	
	fprintf(stdout,"DNS:\n");
	ptr = dns->data;
	numbofsect = net2hs(dns->qdcount);
	ulong sectioncount = 1;
	while (0 != numbofsect)
	{
		fprintf(stdout,"DNS:\t----- DNS Question Section %d -----\n", 
						sectioncount);
		fprintf(stdout, "DNS:\tQuestion Name = ");
		while (0 != *ptr)
		{
			
			count = *ptr;
			ptr++;
			while (0 != count)
			{
				fprintf(stdout, "%c", *ptr);
				count--;
				ptr++;
			}
			if (0 != *ptr)
			{ fprintf(stdout,"."); }
			
		}	

		fprintf(stdout, "\n");

		ptr++;
		var = *ptr << 8;
		ptr++;
		var+=*ptr;
		fprintf(stdout, "DNS:\tQuestion Type = %d ", var);

		switch (var)
		{
		case DNS_TYPE_A:
			fprintf(stdout,"(Host Address)\n");
			break;
		case DNS_TYPE_NS:
			fprintf(stdout,"(Name Server)\n");
			break;
		case DNS_TYPE_CNAME:
			fprintf(stdout,"(Canonical Name)\n");
			break;
		case DNS_TYPE_SOA:
			fprintf(stdout,"(Start of Zone)\n");
			break;
		case DNS_TYPE_WKS:
			fprintf(stdout,"(Well Known Service Description)\n");
			break;
		case DNS_TYPE_PTR:
			fprintf(stdout,"(Domain Name Pointer)\n");
			break;
		case DNS_TYPE_HINFO:
			fprintf(stdout,"(Host Information)\n");
			break;
		case DNS_TYPE_MINFO:
			fprintf(stdout,"(Mailbox Mail List)\n");
			break;
		case DNS_TYPE_MX:
			fprintf(stdout,"(Mail Exchange)\n");
			break;
		case DNS_TYPE_TXT:
			fprintf(stdout,"(Text Strings)\n");
			break;
		case DNS_QTYPE_AXFR:
			fprintf(stdout,"(Request Transfer Zone)\n");
			break;
		case DNS_QTYPE_MAILB:
			fprintf(stdout,"(Request Mailbox Related Records)\n");
			break;
		case DNS_QTYPE_ALL:
			fprintf(stdout,"(Reqeust for All Records)\n");
			break;
		default:
			fprintf(stdout,"(Unrecognized Type)\n");
			break;
		}

		ptr++;		
		var = *ptr << 8;
		ptr++;
		var+=*ptr;
		fprintf(stdout, "DNS:\tQuestion Class = %d ", var);

		switch (var)
		{
		case DNS_CLASS_IN:
			fprintf(stdout, "(Internet)\n");
			break;
		case DNS_QCLASS_ALL:
			fprintf(stdout, "(Any Class)\n");
			break;
		default:
			fprintf(stdout, "(Unrecognized Class)\n");
			break;
		}
		numbofsect--;
		sectioncount++;
		fprintf(stdout, "DNS:\n");
		ptr++;
	}

	numbofsect = net2hs(dns->ancount);
	sectioncount = 1;
	while (0 != numbofsect)
	{
		fprintf(stdout,"DNS:\t----- DNS Answer Section %d -----\n", 
						sectioncount);
		fprintf(stdout, "DNS:\tServer Name = ");
		while (0 != *ptr)
		{
			// If name is a pointer then we are done
			if (*ptr & 0xC0)
			{
				var = *ptr & 0x3;
				ptr++;
				var+= *ptr;
				fprintf(stdout,"Pointer = %d", var);
				break;
			}
			
			count = *ptr;
			ptr++;
			while (0 != count)
			{
				fprintf(stdout, "%c", *ptr);
				count--;
				ptr++;
			}
			if (0 != *ptr)
			{ fprintf(stdout,"."); }
			
		}	

		fprintf(stdout, "\n");

		ptr++;
		var = *ptr << 8;
		ptr++;
		var+=*ptr;
		type = var;
		fprintf(stdout, "DNS:\tRecord Type = %d ", var);

		switch (var)
		{
		case DNS_TYPE_A:
			fprintf(stdout,"(Host Address)\n");
			break;
		case DNS_TYPE_NS:
			fprintf(stdout,"(Name Server)\n");
			break;
		case DNS_TYPE_CNAME:
			fprintf(stdout,"(Canonical Name)\n");
			break;
		case DNS_TYPE_SOA:
			fprintf(stdout,"(Start of Zone)\n");
			break;
		case DNS_TYPE_WKS:
			fprintf(stdout,"(Well Known Service Description)\n");
			break;
		case DNS_TYPE_PTR:
			fprintf(stdout,"(Domain Name Pointer)\n");
			break;
		case DNS_TYPE_HINFO:
			fprintf(stdout,"(Host Information)\n");
			break;
		case DNS_TYPE_MINFO:
			fprintf(stdout,"(Mailbox Mail List)\n");
			break;
		case DNS_TYPE_MX:
			fprintf(stdout,"(Mail Exchange)\n");
			break;
		case DNS_TYPE_TXT:
			fprintf(stdout,"(Text Strings)\n");
			break;
		default:
			fprintf(stdout,"(Unrecognized Type)\n");
			break;
		}

		ptr++;		
		var = *ptr << 8;
		ptr++;
		var+=*ptr;
		fprintf(stdout, "DNS:\tRecord Class = %d ", var);

		switch (var)
		{
		case DNS_CLASS_IN:
			fprintf(stdout, "(Internet)\n");
			break;
		case DNS_QCLASS_ALL:
			fprintf(stdout, "(Any Class)\n");
			break;
		default:
			fprintf(stdout, "(Unrecognized Class)\n");
			break;
		}
		
		ptr++;		
		var = *ptr << 24;
		ptr++;
		var+=*ptr << 16;
		ptr++;
		var+=*ptr << 8;
		ptr++;
		var+=*ptr;
		fprintf(stdout, "DNS:\tTTL = %u\n", var);
		
		ptr++;
		var=*ptr << 8;
		ptr++;
		var+=*ptr;
		rdatalen = var;
		fprintf(stdout, "DNS:\tRData length = %d\n", var);

		fprintf(stdout, "DNS:\tRData = ");
		switch(type)
		{
		case DNS_TYPE_A:
			ptr++;		
			var = *ptr << 24;
			ptr++;
			var+=*ptr << 16;
			ptr++;
			var+=*ptr << 8;
			ptr++;
			var+=*ptr;
			fprint_iplong(var);
			fprintf(stdout, "\n");
			break;
		case DNS_TYPE_CNAME:
			ptr++;
			while (0 != *ptr)
			{
				// If name is a pointer then we are done
				if (*ptr & 0xC0)
				{
					var = *ptr & 0x3;
					ptr++;
					var+= *ptr;
					fprintf(stdout,"Pointer = %d", var);
					break;
				}
				
				count = *ptr;
				ptr++;
				while (0 != count)
				{
					fprintf(stdout, "%c", *ptr);
					count--;
					ptr++;
				}
				if (0 != *ptr)
				{ fprintf(stdout,"."); }
				
			}	
			fprintf(stdout, "\n");
			break;
		case DNS_TYPE_PTR:
			ptr++;
			while (0 != *ptr)
			{
				// If name is a pointer then we are done
				if (*ptr & 0xC0)
				{
					var = *ptr & 0x3;
					ptr++;
					var+= *ptr;
					fprintf(stdout,"Pointer = %d", var);
					break;
				}
				
				count = *ptr;
				ptr++;
				while (0 != count)
				{
					fprintf(stdout, "%c", *ptr);
					count--;
					ptr++;
				}
				if (0 != *ptr)
				{ fprintf(stdout,"."); }
				
			}	
			fprintf(stdout, "\n");
			break;
		default:
			fprintf(stdout,"Unrecognized\n");
			ptr+=rdatalen;
			break;
		}
		ptr++;
		sectioncount++;
		fprintf(stdout, "DNS:\n");
		numbofsect--;
	}

	numbofsect = net2hs(dns->nscount);
	sectioncount = 1;
	while (0 != numbofsect)
	{
		fprintf(stdout,"DNS:\t----- DNS Name Server Section %d -----\n", 
						sectioncount);
		fprintf(stdout, "DNS:\tServer Name = ");
		while (0 != *ptr)
		{
			// If name is a pointer then we are done
			if (*ptr & 0xC0)
			{
				var = *ptr & 0x3;
				ptr++;
				var+= *ptr;
				fprintf(stdout,"Pointer = %d", var);
				break;
			}
			
			count = *ptr;
			ptr++;
			while (0 != count)
			{
				fprintf(stdout, "%c", *ptr);
				count--;
				ptr++;
			}
			if (0 != *ptr)
			{ fprintf(stdout,"."); }
			
		}	

		fprintf(stdout, "\n");

		ptr++;
		var = *ptr << 8;
		ptr++;
		var+=*ptr;
		type = var;
		fprintf(stdout, "DNS:\tRecord Type = %d ", var);

		switch (var)
		{
		case DNS_TYPE_A:
			fprintf(stdout,"(Host Address)\n");
			break;
		case DNS_TYPE_NS:
			fprintf(stdout,"(Name Server)\n");
			break;
		case DNS_TYPE_CNAME:
			fprintf(stdout,"(Canonical Name)\n");
			break;
		case DNS_TYPE_SOA:
			fprintf(stdout,"(Start of Zone)\n");
			break;
		case DNS_TYPE_WKS:
			fprintf(stdout,"(Well Known Service Description)\n");
			break;
		case DNS_TYPE_PTR:
			fprintf(stdout,"(Domain Name Pointer)\n");
			break;
		case DNS_TYPE_HINFO:
			fprintf(stdout,"(Host Information)\n");
			break;
		case DNS_TYPE_MINFO:
			fprintf(stdout,"(Mailbox Mail List)\n");
			break;
		case DNS_TYPE_MX:
			fprintf(stdout,"(Mail Exchange)\n");
			break;
		case DNS_TYPE_TXT:
			fprintf(stdout,"(Text Strings)\n");
			break;
		default:
			fprintf(stdout,"(Unrecognized Type)\n");
			break;
		}

		ptr++;		
		var = *ptr << 8;
		ptr++;
		var+=*ptr;
		fprintf(stdout, "DNS:\tRecord Class = %d ", var);

		switch (var)
		{
		case DNS_CLASS_IN:
			fprintf(stdout, "(Internet)\n");
			break;
		case DNS_QCLASS_ALL:
			fprintf(stdout, "(Any Class)\n");
			break;
		default:
			fprintf(stdout, "(Unrecognized Class)\n");
			break;
		}
		
		ptr++;		
		var = *ptr << 24;
		ptr++;
		var+=*ptr << 16;
		ptr++;
		var+=*ptr << 8;
		ptr++;
		var+=*ptr;
		fprintf(stdout, "DNS:\tTTL = %u\n", var);
		
		ptr++;
		var=*ptr << 8;
		ptr++;
		var+=*ptr;
		rdatalen = var;
		fprintf(stdout, "DNS:\tRData length = %d\n", var);

		fprintf(stdout, "DNS:\tRData = ");
		switch(type)
		{
		case DNS_TYPE_A:
			ptr++;		
			var = *ptr << 24;
			ptr++;
			var+=*ptr << 16;
			ptr++;
			var+=*ptr << 8;
			ptr++;
			var+=*ptr;
			fprint_iplong(var);
			fprintf(stdout, "\n");
			break;
		case DNS_TYPE_CNAME:
			ptr++;
			while (0 != *ptr)
			{
				// If name is a pointer then we are done
				if (*ptr & 0xC0)
				{
					var = *ptr & 0x3;
					ptr++;
					var+= *ptr;
					fprintf(stdout,"Pointer = %d", var);
					break;
				}
				
				count = *ptr;
				ptr++;
				while (0 != count)
				{
					fprintf(stdout, "%c", *ptr);
					count--;
					ptr++;
				}
				if (0 != *ptr)
				{ fprintf(stdout,"."); }
				
			}	
			fprintf(stdout, "\n");
			break;
		case DNS_TYPE_PTR:
			ptr++;
			while (0 != *ptr)
			{
				// If name is a pointer then we are done
				if (*ptr & 0xC0)
				{
					var = *ptr & 0x3;
					ptr++;
					var+= *ptr;
					fprintf(stdout,"Pointer = %d", var);
					break;
				}
				
				count = *ptr;
				ptr++;
				while (0 != count)
				{
					fprintf(stdout, "%c", *ptr);
					count--;
					ptr++;
				}
				if (0 != *ptr)
				{ fprintf(stdout,"."); }
				
			}	
			fprintf(stdout, "\n");
			break;
		default:
			fprintf(stdout,"Unrecognized\n");
			ptr+=rdatalen;
			break;
		}
		ptr++;
		sectioncount++;
		fprintf(stdout, "DNS:\n");
		numbofsect--;
	}

	numbofsect = net2hs(dns->arcount);
	while (0 != numbofsect)
	{
		numbofsect--;
	}
}
*/
