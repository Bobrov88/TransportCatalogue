#include "stat_reader.h"
#include <iomanip>
#include <set>
#include <math.h>

using namespace std::string_view_literals;

Request::Req Request::ParseRequest(const std::string_view request)
{
    Req result;
    auto not_space = request.find_first_not_of(' ');
    if (not_space == std::string_view::npos)
        return {};
    auto pos = request.find(' ', not_space);
    result.request = request.substr(not_space, pos - not_space);
    pos = request.find_first_not_of(' ', pos);
    auto last_pos = request.find_last_not_of(' ');
    result.what = request.substr(pos, last_pos - pos + 1);
    return result;
}

void Request::ParseAndPrintStat(const Data::TransportCatalogue &transport_catalogue, std::string_view request,
                                std::ostream &output)
{
    auto parsed = Request::ParseRequest(request);
    output << parsed.request << " "sv << parsed.what << ": "sv;
    if (parsed.request == "Bus"sv)
    {
        if (transport_catalogue.GetBus(parsed.what) == nullptr)
        {
            output << "not found\n"sv;
        }
        else
        {
            // size_t stop_on_route = transport_catalogue.GetStopCount(parsed.what);
            // size_t unique_stop_on_route = transport_catalogue.GetUniqueStopCount(parsed.what);
            // int real_route_length = transport_catalogue.GetRealRouteLength(parsed.what);
            // double route_length = transport_catalogue.GetRouteLength(parsed.what);
            // double curvature = static_cast<double>(real_route_length) / route_length;
            // output << stop_on_route << " stops on route, "sv;
            // output << unique_stop_on_route << " unique stops, "sv;
            // output << real_route_length << " route length, "sv;
            // put_curvature_to_output(curvature, output);
        }
        return;
    }
    if (parsed.request == "Stop"sv)
    {
        auto stop = transport_catalogue.GetStop(parsed.what);
        if (stop == nullptr)
        {
            output << "not found\n"sv;
        }
        else
        {
            auto &bus_route = transport_catalogue.GetBusesAtStop();
            if (bus_route.count(stop->name))
            {
                output << "buses"sv;
                std::set<std::string_view> s;
                for (const auto& bus : bus_route.at(stop->name))
                {
                    s.insert(bus->name);
                }
                for (const auto &bus : s)
                {
                    output << " "sv << bus;
                }
                output << "\n";
            }
            else
            {
                output << "no buses\n";
                return;
            }
        }
    }
}

void put_route_to_output(double value, std::ostream &output)
{
    int precision = 1;
    while (true)
    {
        ++precision;
        if (value < std::pow(10, precision))
            break;
    }
    output << std::fixed << std::setprecision(6 - precision) << value << " route length, "sv;
}

void put_curvature_to_output(double value, std::ostream &output)
{
    output << std::fixed << std::setprecision(5) << value << " curvature\n"sv;
}