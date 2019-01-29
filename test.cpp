#include <stdio.h>
#include "YDNSResolver.h"

int main() {
	YDNSResolver resolver;
	std::vector<std::string> ret;
	resolver.lookupByName(std::string("www.google.com"), ret);
	return 0;
}