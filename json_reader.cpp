#include "json_reader.h"

using namespace json;

void JsonReader::FillTransportCatalogue()
{
    Document doc(std::move(Load(in_)));
    const auto &dict = doc.GetRoot().AsMap();
    FillDataBase(dict.at("base_request"));
}

void JsonReader::FillDataBase(const Node &node)
{
    distances temp_distances;
    for (const auto &arr : node.AsArray())
    {
        const auto &data = arr.AsMap();
        if (data.at("type") == "Stop")
            FillStops(data, temp_distances);
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
        std::transform(std::next(stop_array.crbegin()), stop_array.crend(), stops.end(),
                       [&stops](const auto &el)
                       { return el.AsString(); });
    }
    db_.AddBus(node.at("name").AsString(), std::move(stops));
}