#pragma once
#include "json.h"
#include "request_handler.h"

using namespace Data;

namespace json
{
    class JsonReader
    {
        using distances = std::unordered_map<std::string_view, std::map<std::string, int>>;
        TransportCatalogue &db_;
        std::istream &in_;
        RequestHandler &rh_;
        std::ostream &out_;
        void FillDataBase(const Node &node);
        void FillStops(const Dict &node, distances &temp_distances);
        void FillBuses(const Dict &node);
        void GetResponce(const Node &node);
        void ConstructJson(const std::unordered_set<entity::BusPtr> &buses, int request_id);
        void ConstructJson(const std::optional<entity::BusStat> &busstat, int reqeust_id);

    public:
        JsonReader(TransportCatalogue &db, std::istream &in, RequestHandler &rh, std::ostream &out) : db_(db), in_(in), rh_(rh), out_(out) {}
        void ProcessTransportCatalogue();
    };
}