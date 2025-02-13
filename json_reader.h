#pragma once
#include <queue>
#include "json.h"
#include "request_handler.h"
#include "map_renderer.h"
#include "transport_router.h"

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
        void GetRouteSettings(const Node &node);
        json::Node ConstructJson(const std::optional<std::unordered_set<entity::BusPtr>> &buses, int request_id);
        json::Node ConstructJson(const std::optional<entity::BusStat> &busstat, int reqeust_id);
        json::Node ConstructJson(const svg::Document &document, int request_id);
        json::Node ConstructJson(const std::pair<double, std::optional<std::vector<RouteItems>>> &items, int request_id);

    public:
        JsonReader(TransportCatalogue &db,
                   RequestHandler &rh,
                   MapRenderer &renderer,
                   std::istream &in,
                   std::ostream &out) : db_(db),
                                        rh_(rh),
                                        renderer_(renderer),
                                        in_(in),
                                        out_(out) {}
        void Process();
    };

    svg::Color GetColorFromArray(const Node &node);
    svg::Point GetPointFromArray(const Node &node);
    std::vector<svg::Color> GetPaletteFromArray(const Node &node);
    json::Node addItem(WaitingOnStop waiting);
    json::Node addItem(UsingBus bus);
    json::Node NotFoundResponse(int request_id);
}