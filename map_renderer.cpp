#include "map_renderer.h"

namespace Renderer
{
    const svg::Polyline MapRenderer::CreatePolyline(const std::vector<geo::Coordinates> &points, size_t color_palette_order) const
    {
        svg::Polyline polyline;

        if (!points.empty())
        {
            for (const auto &point : points)
            {
                const svg::Point &screen_coord = projector(point);
                polyline.AddPoint({screen_coord.x, screen_coord.y});
            }
            polyline.SetFillColor(svg::NoneColor)
                .SetStrokeColor(color_palette_[(color_palette_order) % color_palette_.size()])
                .SetStrokeLineCap(svg::StrokeLineCap::ROUND)
                .SetStrokeLineJoin(svg::StrokeLineJoin::ROUND)
                .SetStrokeWidth(line_width_);
        }
        return polyline;
    }

    const svg::Text MapRenderer::CreateBusName(const std::string &route_name, const geo::Coordinates coordinates, size_t color_palette_order) const
    {
        svg::Text text;
        if (!route_name.empty())
        {
            const svg::Point &point = projector(coordinates);
            text.SetPosition(point)
                .SetOffset(getBusLabelOffset())
                .SetFillColor(color_palette_[(color_palette_order) % color_palette_.size()])
                .SetFontSize(static_cast<uint32_t>(getBusLabelFontSize()))
                .SetFontFamily("Verdana")
                .SetFontWeight("bold")
                .SetData(route_name);
        }
        return text;
    }

    const svg::Text MapRenderer::CreateStrokeUnderBusName(const std::string &route_name, const geo::Coordinates coordinates) const
    {
        svg::Text text;
        if (!route_name.empty())
        {
            const svg::Point &point = projector(coordinates);
            text.SetPosition(point)
                .SetOffset(getBusLabelOffset())
                .SetFontSize(static_cast<uint32_t>(getBusLabelFontSize()))
                .SetFontFamily("Verdana")
                .SetFontWeight("bold")
                .SetData(route_name)
                .SetFillColor(getUnderlayerColor())
                .SetStrokeColor(getUnderlayerColor())
                .SetStrokeWidth(getUnderlayerWidth())
                .SetStrokeLineCap(svg::StrokeLineCap::ROUND)
                .SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);
        }
        return text;
    }

    const svg::Circle MapRenderer::CreateCircle(const geo::Coordinates coordinates) const
    {
        svg::Circle circle;
        const svg::Point &point = projector(coordinates);
        circle.SetCenter(point)
            .SetRadius(getStopRadius())
            .SetFillColor("white");
        return circle;
    }

    const svg::Text MapRenderer::CreateStopName(const std::string &route_name, const geo::Coordinates coordinates) const
    {
        svg::Text text;
        if (!route_name.empty())
        {
            const svg::Point &point = projector(coordinates);
            text.SetPosition(point)
                .SetOffset(getStopLabelOffset())
                .SetFontSize(static_cast<uint32_t>(getStopLabelFontSize()))
                .SetFontFamily("Verdana")
                .SetData(route_name)
                .SetFillColor("black");
        }
        return text;
    }

    const svg::Text MapRenderer::CreateStrokeUnderStopName(const std::string &route_name, const geo::Coordinates coordinates) const
    {
        svg::Text text;
        if (!route_name.empty())
        {
            const svg::Point &point = projector(coordinates);
            text.SetPosition(point)
                .SetOffset(getStopLabelOffset())
                .SetFontSize(static_cast<uint32_t>(getStopLabelFontSize()))
                .SetFontFamily("Verdana")
                .SetData(route_name)
                .SetFillColor(getUnderlayerColor())
                .SetStrokeColor(getUnderlayerColor())
                .SetStrokeWidth(getUnderlayerWidth())
                .SetStrokeLineCap(svg::StrokeLineCap::ROUND)
                .SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);
        }
        return text;
    }

    void MapRenderer::InitProjector(std::vector<geo::Coordinates> &&points)
    {
        projector = std::move(SphereProjector{points.begin(), points.end(), width_, height_, padding_});
    }
}