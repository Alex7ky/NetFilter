#include "mylib.h"
static struct nf_hook_ops nf_in;

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Buin Buin");
MODULE_DESCRIPTION("Testing change port module");

static unsigned int hook(void *priv,
	struct sk_buff *skb, const struct nf_hook_state *state)
{
	static struct iphdr *ip_header;       // ip header struct
	static struct udphdr *udp_header;     // udp header struct
	static struct sk_buff *sock_buff;
	static unsigned int sport, dport;
	static long int udplen;

	sock_buff = skb;
	if (!sock_buff)
		return NF_ACCEPT;
	ip_header = ip_hdr(sock_buff);
	if (!ip_header)
		return NF_ACCEPT;
	if (ip_header->protocol == IPPROTO_UDP) {
		udp_header = udp_hdr(sock_buff);
		sport = htons((unsigned short int) udp_header->source);
		dport = htons((unsigned short int) udp_header->dest);
		pr_info("Source : %d\t Dest : %d\n", sport, dport);
		if (udp_header->dest == ntohs(1338)) {
			pr_info("Old : %d\n", udp_header->check);
			//udp_header->dest = ntohs(1488);

			udplen = skb->len - (ip_hdr(skb)->ihl * 4);
			udp_header->check = 0;
			udp_header->check = csum_tcpudp_magic(ip_header->saddr,
				ip_header->daddr, udplen,
					IPPROTO_UDP,
					csum_partial((char *)udp_header,
					udplen, 0));
			pr_info("New : %d\n", udp_header->check);
		}
		return NF_ACCEPT;
	}
	return NF_ACCEPT;
}

int init_module(void)
{
	nf_in.hook = hook;
	nf_in.pf = PF_INET;
	nf_in.hooknum = NF_INET_PRE_ROUTING;
	nf_in.priority = NF_IP_PRI_FIRST;

	nf_register_hook(&nf_in);
	pr_info("init_module loaded\n");
	return 0;
}

void cleanup_module(void)
{
	nf_unregister_hook(&nf_in);
	pr_info("cleanup_module\n");
}
