#include "json_reader.h"
#include <cassert>

using namespace json;

void JsonReader::ProcessTransportCatalogue()
{
    Document doc(std::move(Load(in_)));
    const auto &dict = doc.GetRoot().AsMap();
    FillDataBase(dict.at("base_requests"));
    GetResponce(dict.at("stat_requests"));
}

void JsonReader::FillDataBase(const Node &node)
{
    distances temp_distances;
    for (const auto &arr : node.AsArray())
    {
        const auto &data = arr.AsMap();
        if (data.at("type") == "Stop")
            FillStops(data, temp_distances);
            assert(db_.GetStop(data.at("name").AsString()));
    }
    for (const auto &arr : node.AsArray())
    {
        const auto &data = arr.AsMap();
        if (data.at("type") == "Bus")
            FillBuses(data);
    }
    db_.AddDistances(temp_distances);
}

void JsonReader::FillStops(const Dict &node, distances &temp_distances)
{
    db_.AddStop(node.at("name").AsString(),
                {node.at("latitude").AsDouble(),
                 node.at("longitude").AsDouble()});
    if (node.count("road_distances"))
    {
        const auto &road_distances = node.at("road_distances").AsMap();
        for (const auto &[key, value] : road_distances)
            temp_distances[node.at("name").AsString()][key] = value.AsInt();
    }
    assert(db_.GetStop(node.at("name").AsString()));
}

void JsonReader::FillBuses(const Dict &node)
{
    const auto &stop_array = node.at("stops").AsArray();
    std::vector<std::string_view> stops;
    stops.reserve(stop_array.size() * 2);
    std::transform(stop_array.cbegin(), stop_array.cend(), stops.begin(),
                   [&stops](const auto &el)
                   { return el.AsString(); });
    if (!node.at("is_roundtrip").AsBool())
    {
        auto end = stops.end();
        std::transform(std::next(stop_array.crbegin()), stop_array.crend(), end,
                       [&stops](const auto &el)
                       { return el.AsString(); });
    }
    db_.AddBus(node.at("name").AsString(), std::move(stops));
}

void JsonReader::GetResponce(const Node &node)
{
    using namespace std::string_view_literals;
    out_ << "["sv;
    bool is_first = true;
    for (const auto &req : node.AsArray())
    {
        if (!is_first)
        {
            out_ << ","sv;
            is_first = false;
        }
        const auto &stat = req.AsMap();
        if (stat.at("type").AsString() == "Stop")
        {
            const auto stop_responce = rh_.GetBusesByStop(stat.at("name").AsString());
            ConstructJson(stop_responce, stat.at("id").AsInt());
        }
        if (stat.at("type").AsString() == "Bus")
        {
            const auto bus_responce = rh_.GetBusStat(stat.at("name").AsString());
            ConstructJson(bus_responce, stat.at("id").AsInt());
        }
    }
    out_ << "]"sv;
}

void JsonReader::ConstructJson(const std::unordered_set<entity::BusPtr> &buses, int request_id)
{
    using namespace std::string_view_literals;
    out_ << "{\"request_id\":"sv << request_id << ","sv;
    if (buses.empty())
    {
        out_ << "\"error_message\":\"not found\""sv;
    }
    else
    {
        out_ << "\"buses\":["sv;
        bool is_first = true;
        for (const auto &bus : buses)
        {
            if (!is_first)
            {
                out_ << ","sv;
                is_first = false;
            }
            out_ << "\""sv;
            out_ << bus;
            out_ << "\""sv;
        }
        out_ << "]"sv;
    }
    out_ << "}"sv;
}

void JsonReader::ConstructJson(const std::optional<entity::BusStat> &busstat, int request_id)
{
    using namespace std::string_view_literals;
    out_ << "{\"request_id\":"sv << request_id << ","sv;
    if (!busstat)
    {
        out_ << "\"error_message\":\"not found\""sv;
    }
    else
    {
        out_ << "\"curvature\":"sv << busstat->curvature_ << ","sv;
        out_ << "\"route_length\":"sv << busstat->route_length_ << ","sv;
        out_ << "\"stop_count\":"sv << busstat->stop_count_ << ","sv;
        out_ << "\"unique_stop_count\":"sv << busstat->unique_stop_count_;
    }
    out_ << "}"sv;
}