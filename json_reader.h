#pragma once
#include <iostream>
#include "json.h"
#include "request_handler.h"

using namespace Data;

namespace json
{
    class JsonReader
    {
        Data::TransportCatalogue &db_;
        std::istream &in_;
        void FillDataBase(const Node &node);
        void FillStops(const Dict &node);
        void FillBuses(const Dict &node);

    public:
        JsonReader(TransportCatalogue &db, std::istream &in) : db_(db), in_(in) {}
        void FillTransportCatalogue();
    };
}