
#include "http/YHttpHeader.h"
#include "YStrHelper.h"
#include <sstream>
#include <cstdlib>
#include <string>
#include <sstream>

void YHttpHeader::addHeader(std::string name, std::string value)
{
	if (name.empty()) return;

    if (YStrHelper::isEqual(name, ContentLength)) {
        has_content_length_ = true;
        content_length_ = atoi(value.c_str());
    } else if (YStrHelper::isEqual(name, TransferEncoding)) {
        is_chunked_ = YStrHelper::isEqual(Chunked, value);
    }
    header_vec_.emplace_back(std::move(name), std::move(value));
}

void YHttpHeader::addHeader(std::string name, uint32_t value)
{
    std::stringstream ss;
    ss << value;
    addHeader(std::move(name), ss.str());
}

bool YHttpHeader::hasHeader(const std::string &name) const
{
    for (auto const &kv : header_vec_) {
        if (YStrHelper::isEqual(kv.first, name)) {
            return true;
        }
    }
    return false;
}

const std::string& YHttpHeader::getHeader(const std::string &name) const
{
    for (auto const &kv : header_vec_) {
        if (YStrHelper::isEqual(kv.first, name)) {
            return kv.second;
        }
    }
    return EmptyString;
}

void YHttpHeader::processHeader()
{
    has_body_ = is_chunked_ || (has_content_length_ && content_length_ > 0);
}

void YHttpHeader::processHeader(int status_code)
{
    processHeader();
    has_body_ = has_body_ || !((100 <= status_code && status_code <= 199) ||
                               204 == status_code || 304 == status_code);
}

std::string YHttpHeader::buildHeader(const std::string &method, const std::string &url, const std::string &ver)
{
    processHeader();
    std::string req = method + " " + url + " " + (!ver.empty()?ver:VersionHTTP1_1);
    req += "\r\n";
    for (auto &kv : header_vec_) {
        req += kv.first + ": " + kv.second + "\r\n";
    }
    req += "\r\n";
    return req;
}

std::string YHttpHeader::buildHeader(int status_code, const std::string &desc, const std::string &ver)
{
    processHeader(status_code);
    std::string rsp = (!ver.empty()?ver:VersionHTTP1_1) + " " + std::to_string(status_code);
    if (!desc.empty()) {
        rsp += " " + desc;
    }
    rsp += "\r\n";
    for (auto &kv : header_vec_) {
        rsp += kv.first + ": " + kv.second + "\r\n";
    }
    rsp += "\r\n";
    return rsp;
}

void YHttpHeader::reset()
{
    header_vec_.clear();
    has_content_length_ = false;
    content_length_ = 0;
    is_chunked_ = false;
    has_body_ = false;
}
