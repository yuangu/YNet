#include <stdio.h>
#include "YDNSResolver.h"

int main() {
	YDNSResolver resolver;
	resolver.lookupByName(std::string("www.baidu.com"));
	return 0;
}