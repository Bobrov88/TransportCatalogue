#include "stat_reader.h"
#include <iomanip>

using namespace std::string_view_literals;


Request ParseRequest(const std::string_view request)
{
    Request result;
    auto not_space = request.find_first_not_of(' ');
    if (not_space != std::string_view::npos)
        return {};
    auto pos = request.find(' ');
    result.request = request.substr(not_space, pos - not_space);
    pos = request.find_first_not_of(' ');
    auto last_pos = request.find_last_not_of(' ');
    result.what = request.substr(pos, last_pos - pos);
    return result;
}

void ParseAndPrintStat(const TransportCatalogue &transport_catalogue, std::string_view request,
                       std::ostream &output)
{
    auto parsed = ParseRequest(request);
    output << parsed.request << ": "sv;
    if (transport_catalogue.get_bus(parsed_request) == nullptr)
    {
        output << "not found\n"sv;
        return;
    }
    size_t stop_on_route = transport_catalogue.get_stop_count(parsed_request);
    size_t unique_stop_on_route = transport_catalogue.get_unique_stop_count(parsed_request);
    double route_length = transport_catalogue.get_route_length(parsed_request);
    output << stop_on_route << " stops on route, "sv;
    output << unique_stop_on_route << " unique stops, "sv;
    output << std::fixed << route_length << " route length";
    output << "\n"sv;
}
