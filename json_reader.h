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
        std::istream &in_;
        RequestHandler &rh_;
        std::ostream &out_;
        MapRenderer &renderer_;

        void FillDataBase(const Node &node);
        void FillStops(const Dict &node, distances &temp_distances);
        void FillBuses(const Dict &node);
        void GetResponce(const Node &node);
        void GetRenderSettings(const Node &node);
        void ConstructJson(const std::optional<std::unordered_set<entity::BusPtr>> &buses, int request_id);
        void ConstructJson(const std::optional<entity::BusStat> &busstat, int reqeust_id);

    public:
        JsonReader(TransportCatalogue &db, std::istream &in, RequestHandler &rh, std::ostream &out, MapRenderer &renderer) : db_(db), in_(in), rh_(rh), out_(out), renderer_(renderer) {}
        void ProcessTransportCatalogue();
    };

    svg::Color GetColorFromArray(const Node &node);
    svg::Point GetPointFromArray(const Node &node);
    std::queue<svg::Color> GetPaletteFromArray(const Node &node);
}
