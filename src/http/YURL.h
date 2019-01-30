#pragma once

#include <string>

class YURL
{
public:
	YURL();
	YURL(const std::string& url);
	~YURL();

    bool parse(const std::string& url);
    
    const std::string& getScheme() const { return scheme_; }
    const std::string& getHost() const { return host_; }
    const std::string& getPort() const { return port_; }
    const std::string& getPath() const { return path_; }
    const std::string& getQuery() const { return query_; }
    const std::string& getFragment() const { return fragment_; }
    
private:
    bool parse_host_port(const std::string& hostport, std::string& host, std::string& port);

private:
    std::string         scheme_;
    std::string         host_;
    std::string         port_;
	std::string         path_;
    std::string         query_;
    std::string         fragment_;
};

