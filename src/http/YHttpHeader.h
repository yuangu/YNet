
#pragma once
#include "http/httpdefs.h"

class YHttpHeader
{
public:
    virtual ~YHttpHeader() {}
    void addHeader(std::string name, std::string value);
    void addHeader(std::string name, uint32_t value);
    bool hasHeader(const std::string &name) const;
    const std::string& getHeader(const std::string &name) const;
    std::string buildHeader(const std::string &method, const std::string &url, const std::string &ver);
    std::string buildHeader(int status_code, const std::string &desc, const std::string &ver);
    bool hasBody() const { return has_body_; }
    virtual void reset();
    HeaderVector& getHeaders() { return header_vec_; }
    
protected:
    void processHeader();
    void processHeader(int status_code);
    
protected:
    HeaderVector            header_vec_;
    bool                    is_chunked_ = false;
    bool                    has_content_length_ = false;
    bool                    has_body_ = false;
    size_t                  content_length_ = 0;
};

