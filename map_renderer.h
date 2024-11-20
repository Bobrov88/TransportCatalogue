#pragma once
#include <algorithm>
#include "svg.h"
#include "geo.h"

namespace Renderer
{
    class MapRenderer
    {
        inline static const double EPSILON = 1e-6;
        static bool IsZero(double value)
        {
            return std::abs(value) < EPSILON;
        }

        class SphereProjector
        {
        public:
            // points_begin и points_end задают начало и конец интервала элементов geo::Coordinates
            template <typename PointInputIt>
            SphereProjector(PointInputIt points_begin, PointInputIt points_end,
                            double max_width, double max_height, double padding)
                : padding_(padding) //
            {
                // Если точки поверхности сферы не заданы, вычислять нечего
                if (points_begin == points_end)
                {
                    return;
                }

                // Находим точки с минимальной и максимальной долготой
                const auto [left_it, right_it] = std::minmax_element(
                    points_begin, points_end,
                    [](auto lhs, auto rhs)
                    { return lhs.lng < rhs.lng; });
                min_lon_ = left_it->lng;
                const double max_lon = right_it->lng;

                // Находим точки с минимальной и максимальной широтой
                const auto [bottom_it, top_it] = std::minmax_element(
                    points_begin, points_end,
                    [](auto lhs, auto rhs)
                    { return lhs.lat < rhs.lat; });
                const double min_lat = bottom_it->lat;
                max_lat_ = top_it->lat;

                // Вычисляем коэффициент масштабирования вдоль координаты x
                std::optional<double> width_zoom;
                if (!IsZero(max_lon - min_lon_))
                {
                    width_zoom = (max_width - 2 * padding) / (max_lon - min_lon_);
                }

                // Вычисляем коэффициент масштабирования вдоль координаты y
                std::optional<double> height_zoom;
                if (!IsZero(max_lat_ - min_lat))
                {
                    height_zoom = (max_height - 2 * padding) / (max_lat_ - min_lat);
                }

                if (width_zoom && height_zoom)
                {
                    // Коэффициенты масштабирования по ширине и высоте ненулевые,
                    // берём минимальный из них
                    zoom_coeff_ = std::min(*width_zoom, *height_zoom);
                }
                else if (width_zoom)
                {
                    // Коэффициент масштабирования по ширине ненулевой, используем его
                    zoom_coeff_ = *width_zoom;
                }
                else if (height_zoom)
                {
                    // Коэффициент масштабирования по высоте ненулевой, используем его
                    zoom_coeff_ = *height_zoom;
                }
            }

            // Проецирует широту и долготу в координаты внутри SVG-изображения
            svg::Point operator()(geo::Coordinates coords) const
            {
                return {
                    (coords.lng - min_lon_) * zoom_coeff_ + padding_,
                    (max_lat_ - coords.lat) * zoom_coeff_ + padding_};
            }

        private:
            double padding_;
            double min_lon_ = 0;
            double max_lat_ = 0;
            double zoom_coeff_ = 0;
        };

        double width_ = 0.0;
        double height_ = 0.0;
        double padding_ = 0.0;
        double line_width_ = 0.0;
        double stop_radius_ = 0.0;
        size_t bus_label_font_size_ = 8;
        svg::Point bus_label_offset_ = {0.0, 0.0};
        size_t stop_label_font_size_ = 8;
        svg::Point stop_label_offset_ = {0.0, 0.0};
        svg::Color underlayer_color_ = svg::NoneColor;
        double underlayer_width_ = 0.0;
        std::vector<svg::Color> color_palette;

    public:
        MapRenderer() = default;

        double getWidth() const { return width_; }
        double getHeight() const { return height_; }
        double getPadding() const { return padding_; }
        double getLineWidth() const { return line_width_; }
        double getStopRadius() const { return stop_radius_; }
        size_t getBusLabelFontSize() const { return bus_label_font_size_; }
        svg::Point getBusLabelOffset() const { return bus_label_offset_; }
        size_t getStopLabelFontSize() const { return stop_label_font_size_; }
        svg::Point getStopLabelOffset() const { return stop_label_offset_; }
        svg::Color getUnderlayerColor() const { return underlayer_color_; }
        double getUnderlayerWidth() const { return underlayer_width_; }
        const std::vector<svg::Color> &getColorPalette() const { return color_palette; }

        void setWidth(double width) { width_ = width; }
        void setHeight(double height) { height_ = height; }
        void setPadding(double padding) { padding_ = padding; }
        void setLineWidth(double lineWidth) { line_width_ = lineWidth; }
        void setStopRadius(double stopRadius) { stop_radius_ = stopRadius; }
        void setBusLabelFontSize(size_t fontSize) { bus_label_font_size_ = fontSize; }
        void setBusLabelOffset(const svg::Point &offset) { bus_label_offset_ = offset; }
        void setStopLabelFontSize(size_t fontSize) { stop_label_font_size_ = fontSize; }
        void setStopLabelOffset(const svg::Point &offset) { stop_label_offset_ = offset; }
        void setUnderlayerColor(const svg::Color &color) { underlayer_color_ = color; }
        void setUnderlayerWidth(double width) { underlayer_width_ = width; }
        void setColorPalette(const std::vector<svg::Color> &palette) { color_palette = palette; }

        const svg::Polyline CreatePolyline(const std::vector<geo::Coordinates> &points) const;
        svg::Text CreateText(const std::string_view text);
        //svg::Circle CreateCircle();
    };
}