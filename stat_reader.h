#pragma once

#include <iosfwd>
#include <string_view>
#include <vector>
#include "transport_catalogue.h"
#include "input_reader.h"

struct Request {
    std::string_view request;
    std::string_view what;
};

Request ParseRequest(const std::string_view request);

void ParseAndPrintStat(const TransportCatalogue& tansport_catalogue, std::string_view request,
                       std::ostream& output);
