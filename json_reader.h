#pragma once
#include <queue>
#include "json.h"
#include "request_handler.h"
#include "map_renderer.h"

using namespace Data;
using namespace Renderer;

namespace json
{
    class JsonReader
    {
        using distances = std::unordered_map<std::string_view, std::map<std::string, int>>;
        TransportCatalogue &db_;
        RequestHandler &rh_;
        MapRenderer &renderer_;
        std::istream &in_;
        std::ostream &out_;

        void FillDataBase(const Node &node);
        void FillStops(const Dict &node, distances &temp_distances);
        void FillBuses(const Dict &node);
        void GetResponce(const Node &node);
        void GetRenderSettings(const Node &node);
        void ConstructJson(const std::optional<std::unordered_set<entity::BusPtr>> &buses, int request_id);
        void ConstructJson(const std::optional<entity::BusStat> &busstat, int reqeust_id);
        void ConstructJson(const svg::Document &document, int request_id);

    public:
        JsonReader(TransportCatalogue &db, RequestHandler &rh, MapRenderer &renderer, std::istream &in, std::ostream &out) : db_(db), rh_(rh), renderer_(renderer), in_(in), out_(out) {}
        void ProcessTransportCatalogue();
    };

    svg::Color GetColorFromArray(const Node &node);
    svg::Point GetPointFromArray(const Node &node);
    std::vector<svg::Color> GetPaletteFromArray(const Node &node);
}
