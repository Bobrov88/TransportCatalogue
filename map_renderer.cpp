#include "map_renderer.h"

namespace Renderer
{
    const svg::Polyline MapRenderer::CreatePolyline(const std::vector<geo::Coordinates> &points) const
    {
        svg::Polyline polyline;

        if (!points.empty())
        {
            for (const auto &point : points)
            {
                const svg::Point screen_coord = projector(point);
                polyline.AddPoint({screen_coord.x, screen_coord.y});
            }
            polyline.SetFillColor(svg::NoneColor);
            polyline.SetStrokeColor(color_palette_[(color_palette_order_++) % color_palette_.size()]);
            polyline.SetStrokeLineCap(svg::StrokeLineCap::ROUND);
            polyline.SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);
            polyline.SetStrokeWidth(line_width_);
        }
        return polyline;
    }

     void MapRenderer::InitProjector(std::vector<geo::Coordinates>&& points) {
        projector = std::move(SphereProjector{points.begin(), points.end(), width_, height_, padding_});
     }
}