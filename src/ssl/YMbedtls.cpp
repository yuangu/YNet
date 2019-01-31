#include "ssl/YMbedtls.h"
#include <stdio.h>

#include <mbedtls/version.h>
#if MBEDTLS_VERSION_NUMBER >= 0x02040000
#include <mbedtls/net_sockets.h>
#else
#include <mbedtls/net.h>
#endif
#include <mbedtls/ssl.h>
#include <mbedtls/certs.h>
#include <mbedtls/x509.h>

#include <mbedtls/error.h>
#include <mbedtls/entropy.h>
#include <mbedtls/ctr_drbg.h>
#include <mbedtls/sha256.h>

int init()
{
	return 0;
}


 void clean()
{

}
 std::string version()
{
	 char buffer[50] = {0x00};
	 unsigned int version = mbedtls_version_get_number();
	 int size =  _snprintf(buffer, 50, "mbedTLS/%u.%u.%u", version >> 24,
		 (version >> 16) & 0xff, (version >> 8) & 0xff);

	 return std::string(buffer, size);
}