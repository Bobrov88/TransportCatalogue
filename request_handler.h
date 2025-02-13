#pragma once
#include <iostream>
#include <string_view>
#include <optional>
#include <unordered_set>
#include "transport_catalogue.h"
#include "transport_router.h"
#include "map_renderer.h"

using namespace entity;
using namespace Data;

class RequestHandler
{
public:
    RequestHandler(const TransportCatalogue &db, const Renderer::MapRenderer &renderer) : db_(db),
                                                                                          renderer_(renderer),
                                                                                          router_{TransportRouter{db_}} {}

    std::optional<BusStat> GetBusStat(const std::string_view &bus_name) const;

    std::optional<std::unordered_set<BusPtr>> GetBusesByStop(const std::string_view &stop_name) const;

    svg::Document RenderMap() const;

    std::pair<double, std::optional<std::vector<RouteItems>>> GetRouteItems(const std::string_view &from_stop, const std::string_view &to_stop) const;
    void InitializeRouter(routestats stats);

private:
    const TransportCatalogue &db_;
    const Renderer::MapRenderer &renderer_;
    TransportRouter router_;
};