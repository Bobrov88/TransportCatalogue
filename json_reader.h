#pragma once
#include "json.h"
#include "request_handler.h"

using namespace Data;

namespace json
{
    class JsonReader
    {
        using distances = std::unordered_map<std::string_view, std::map<std::string, int>>;
        Data::TransportCatalogue &db_;
        std::istream &in_;
        void FillDataBase(const Node &node);
        void FillStops(const Dict &node, distances &temp_distances);
        void FillBuses(const Dict &node);

    public:
        JsonReader(TransportCatalogue &db, std::istream &in) : db_(db), in_(in) {}
        void FillTransportCatalogue();
    };
}