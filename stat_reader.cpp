#include "stat_reader.h"
#include "input_reader.cpp"

void ParseAndPrintStat(const TransportCatalogue &transport_catalogue, std::string_view request,
                       std::ostream &output)
{
    using namespace std::string_view_literals;
    auto parsed_request = Split(request, ' ');
    output << "Bus "sv;
    output << parsed_request[1] << ": "sv;
    size_t stop_on_route = transport_catalogue.get_stop_count(parsed_request[1]);
    size_t unique_stop_on_route = transport_catalogue.get_unique_stop_count(parsed_request[1]);
    double route_length = transport_catalogue.get_route_length(parsed_request[1]);
    output << stop_on_route << " stop on route, "sv;
    output << unique_stop_on_route << " unique stop on route, "sv;
    output << std::setprecision(2) << route_length << " route length";
}