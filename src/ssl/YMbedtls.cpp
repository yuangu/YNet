#include "ssl/YMbedtls.h"
#include "YScoket.h"

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

static void my_debug(void *ctx, int level,
	const char *file, int line,
	const char *str)
{
	((void)level);

	fprintf((FILE *)ctx, "%s:%04d: %s", file, line, str);
	fflush((FILE *)ctx);
}


struct mbedtls_context
{
	mbedtls_net_context server_fd;
	mbedtls_entropy_context entropy;
	mbedtls_ctr_drbg_context ctr_drbg;
	mbedtls_ssl_context ssl;
	mbedtls_ssl_config conf;
	mbedtls_x509_crt cacert;
};



int YMbedtls::init(YSocket* socket)
{
	mSocket = socket;
	mContext = new mbedtls_context;

	mbedtls_net_init(&mContext->server_fd);
	mbedtls_ssl_init(&mContext->ssl);
	mbedtls_ssl_config_init(&mContext->conf);
	mbedtls_x509_crt_init(&mContext->cacert);
	mbedtls_ctr_drbg_init(&mContext->ctr_drbg);
	
	mbedtls_entropy_init(&mContext->entropy);

	int ret = mbedtls_ctr_drbg_seed(&mContext->ctr_drbg, mbedtls_entropy_func, &mContext->entropy,
		NULL, 0);

	if (ret != 0)
	{
		printf(" failed\n  ! mbedtls_ctr_drbg_seed returned %d\n", ret);
		return ret;
	}

	//初使化证书
	/*ret = mbedtls_x509_crt_parse(&mContext->cacert, (const unsigned char *)mbedtls_test_cas_pem,
		mbedtls_test_cas_pem_len);
	if (ret != 0)
	{
		printf(" failed\n  ! mbedtls_x509_crt_parse returned %d\n", ret);
		return ret;
	}*/

	//设置默认配置
	int type = socket->isTCP()?MBEDTLS_SSL_TRANSPORT_STREAM:MBEDTLS_SSL_TRANSPORT_DATAGRAM;
	ret = mbedtls_ssl_config_defaults(&mContext->conf,
		MBEDTLS_SSL_IS_CLIENT,
		type,
		MBEDTLS_SSL_PRESET_DEFAULT);
	if (ret != 0)
	{
		printf(" failed\n  ! mbedtls_ssl_config_defaults returned %d\n", ret);
		return ret;
	}

	
	
	return 0;
}


 void YMbedtls::clean()
{
	 mbedtls_net_free(&mContext->server_fd);
	 mbedtls_x509_crt_free(&mContext->cacert);
	 mbedtls_ssl_free(&mContext->ssl);
	 mbedtls_ssl_config_free(&mContext->conf);
	 mbedtls_ctr_drbg_free(&mContext->ctr_drbg);
	 mbedtls_entropy_free(&mContext->entropy);
	 delete mContext;
	 mContext = nullptr;
	
}
 std::string YMbedtls::version()
{
	 char buffer[50] = {0x00};
	 unsigned int version = mbedtls_version_get_number();
	 int size =  _snprintf(buffer, 50, "mbedTLS/%u.%u.%u", version >> 24,
		 (version >> 16) & 0xff, (version >> 8) & 0xff);

	 return std::string(buffer, size);
}

bool  YMbedtls::handshake()
 {
	
	mContext->server_fd.fd = mSocket->fd();



	mbedtls_ssl_conf_authmode(&mContext->conf, MBEDTLS_SSL_VERIFY_OPTIONAL);
	mbedtls_ssl_conf_ca_chain(&mContext->conf, &mContext->cacert, NULL);
	mbedtls_ssl_conf_rng(&mContext->conf, mbedtls_ctr_drbg_random, &mContext->ctr_drbg);
	//mbedtls_ssl_conf_dbg(&mContext->conf, my_debug, stdout);

	auto ret = mbedtls_ssl_setup(&mContext->ssl, &mContext->conf);
	if (ret != 0)
	{
		printf(" failed\n  ! mbedtls_ssl_setup returned %d\n", ret);
		return ret;
	}

	if ((ret = mbedtls_ssl_set_hostname(&mContext->ssl, "www.baidu.com")) != 0)
	{
		printf(" failed\n  ! mbedtls_ssl_set_hostname returned %d\n\n", ret);
		return ret;
	}

	mbedtls_ssl_set_bio(&mContext->ssl, &mContext->server_fd, mbedtls_net_send, mbedtls_net_recv, NULL);

	while ((ret = mbedtls_ssl_handshake(&mContext->ssl)) != 0)
	{
		if (ret != MBEDTLS_ERR_SSL_WANT_READ && ret != MBEDTLS_ERR_SSL_WANT_WRITE)
		{
			printf(" failed\n  ! mbedtls_ssl_handshake returned -0x%x\n\n", -ret);
			return false;
		}
	}
	 return true;
 }

int YMbedtls::send(const char *buffer, size_t size)
{
	return mbedtls_ssl_write(&mContext->ssl, (const unsigned char *)buffer, size);
}

int  YMbedtls::receive(char *buffer, size_t size)
{
	return mbedtls_ssl_read(&mContext->ssl, (unsigned char *)buffer, size);
}