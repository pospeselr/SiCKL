#pragma once

struct thistle_error
{
    thistle_error(const char* msg) : error_message(msg) {}
    std::string error_message;
};
typedef thistle_error thistle_error_t;