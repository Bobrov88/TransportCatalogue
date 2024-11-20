#pragma once
#include <iostream>
#include <string_view>
#include <optional>
#include <unordered_set>
#include "transport_catalogue.h"
#include "map_renderer.h"

using namespace entity;
using namespace Data;

class RequestHandler
{
public:
    // MapRenderer понадобится в следующей части итогового проекта
    RequestHandler(const TransportCatalogue &db, const Renderer::MapRenderer &renderer) : db_(db), renderer_(renderer) {}

    // Возвращает информацию о маршруте (запрос Bus)
    std::optional<BusStat> GetBusStat(const std::string_view &bus_name) const;

    // Возвращает маршруты, проходящие через
    const std::optional<std::unordered_set<BusPtr>> GetBusesByStop(const std::string_view &stop_name) const;

    // Этот метод будет нужен в следующей части итогового проекта
    svg::Document RenderMap() const;

private:
    const TransportCatalogue &db_;
    const Renderer::MapRenderer &renderer_;
};