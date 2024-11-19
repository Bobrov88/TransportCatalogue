#pragma once
#include <queue>
#include "svg.h"

namespace Renderer
{
    class MapRenderer
    {
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
        std::queue<svg::Color> color_palette;

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
        const std::queue<svg::Color> &getColorPalette() const { return color_palette; }

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
        void setColorPalette(const std::queue<svg::Color> &palette) { color_palette = palette; }
    };
}