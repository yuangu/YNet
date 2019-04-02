/* Copyright (c) 2014, Fengping Bao <jamol@live.com>
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#ifndef __HTTPDEFS_H__
#define __HTTPDEFS_H__

#include <string>
#include <vector>
#include <map>
#include <functional>
#include <string.h>
#include <stdlib.h>

#if defined(_MSC_VER)
#define strcasecmp _stricmp
#endif

#define VersionHTTP2_0  "HTTP/2.0"
#define VersionHTTP1_1  "HTTP/1.1"
#define EmptyString  ""
#define UserAgentStr  "ynet/1.0"

#define ContentType "Content-Type"
#define ContentLength "Content-Length"
#define TransferEncoding "Transfer-Encoding"
#define Chunked "chunked"
#define CacheControl "Cache-Control"
#define CookieHeaderName "Cookie"
#define HostHeaderName "Host"
#define Upgrade "Upgrade"
#define UserAgent "User-Agent" 

typedef std::pair<std::string, std::string> KeyValuePair;

typedef std::vector<KeyValuePair> HeaderVector;
typedef std::vector<uint8_t> HttpBody;

struct CaseIgnoreLess
{
    bool operator()(const std::string &lhs, const std::string &rhs) const {
        return strcasecmp(lhs.c_str(), rhs.c_str()) < 0;
    }
};
typedef std::map<std::string, std::string, CaseIgnoreLess> HeaderMap;


#endif
