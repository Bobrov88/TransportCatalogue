#pragma once

#include <iosfwd>
#include <string_view>
#include <vector>
#include "transport_catalogue.h"
#include "input_reader.h"

namespace Request
{
    struct Req
    {
        std::string_view request;
        std::string_view what;
    };

    Req ParseRequest(const std::string_view request);

    void ParseAndPrintStat(const Data::TransportCatalogue &tansport_catalogue, std::string_view request,
                           std::ostream &output);
}

void put_route_to_output(double value, std::ostream &output);