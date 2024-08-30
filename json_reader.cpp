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
    for (const auto &arr : node.AsArray())
    {
        const auto &data = arr.AsMap();
        if (data.at("type") == "Stop")
            FillStops(data);
    }
    for (const auto &arr : node.AsArray())
    {
        const auto &data = arr.AsMap();
        if (data.at("type") == "Bus")
            FillBuses(data);
    }
    FillDistances();
}

void JsonReader::FillStops(const Dict &node)
{
    db_.AddBus(node.at("name"),
               {node.at("stops").AsArray().begin(),
                node.at("stops").AsArray().end()});
}

void JsonReader::FillBuses(const Dict &node)
{
    db_.AddStop(node.at("name"),
                {node.at("latitude"),
                 node.at("longitude")});
    db_.AddDistances({node.at("name"), {node.at("road_distances").AsMap().begin(),node.at("road_distances").AsMap().end()}});
}