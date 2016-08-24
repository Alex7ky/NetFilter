#include "mylib.h"
static struct nf_hook_ops nf_in;
static struct nf_hook_ops nf_out;

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Buin Buin");
MODULE_DESCRIPTION("Testing drop pack module");

static unsigned int hook(unsigned int hooknum,
		struct sk_buff **skb,
		const struct net_device *in,
		const struct net_device *out,
		int (*okfn)(struct sk_buff *))
{
	return NF_DROP;
}

int init_module(void)
{
	nf_in.hook = hook;
	nf_in.pf = PF_INET;
	nf_in.hooknum = NF_INET_PRE_ROUTING;
	nf_in.priority = NF_IP_PRI_FIRST;

	nf_out.hook = hook;
	nf_out.pf = PF_INET;
	nf_out.hooknum = NF_INET_PRE_ROUTING;
	nf_out.priority = NF_IP_PRI_FIRST;

	nf_register_hook(&nf_in);
	nf_register_hook(&nf_out);
	pr_info("init_module loaded\n");
	return 0;
}

void cleanup_module(void)
{
	nf_unregister_hook(&nf_in);
	nf_unregister_hook(&nf_out);
	pr_info("cleanup_module\n");
}