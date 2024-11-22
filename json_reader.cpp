#include "json_reader.h"
#include <set>

using namespace json;

void JsonReader::ProcessTransportCatalogue()
{
    Document doc(Load(in_));
    const auto &dict = doc.GetRoot().AsMap();
    FillDataBase(dict.at("base_requests"));
  //  GetResponce(dict.at("stat_requests"));
    GetRenderSettings(dict.at("render_settings"));
}

void JsonReader::FillDataBase(const Node &node)
{
    distances temp_distances;
    for (const auto &arr : node.AsArray())
    {
        const auto &data = arr.AsMap();
        if (data.at("type").AsString() == "Stop")
            FillStops(data, temp_distances);
    }
    db_.AddDistances(std::move(temp_distances));
    for (const auto &arr : node.AsArray())
    {
        const auto &data = arr.AsMap();
        if (data.at("type").AsString() == "Bus")
            FillBuses(data);
    }
}

void JsonReader::FillStops(const Dict &node, distances &temp_distances)
{
    db_.AddStop(node.at("name").AsString(),
                {node.at("latitude").AsDouble(),
                 node.at("longitude").AsDouble()});
    if (node.count("road_distances"))
    {
        const auto &road_distances = node.at("road_distances").AsMap();
        for (const auto &[key, value] : road_distances)
            temp_distances[node.at("name").AsString()][key] = value.AsInt();
    }
}

void JsonReader::FillBuses(const Dict &node)
{
    const auto &stop_array = node.at("stops").AsArray();
    std::vector<std::string_view> stops;
    stops.reserve(stop_array.size() * 2);
    std::for_each(stop_array.cbegin(), stop_array.cend(), [&stops](const auto &el)
                  { stops.push_back(el.AsString()); });
    if (!node.at("is_roundtrip").AsBool())
    {
        std::for_each(std::next(stop_array.crbegin()), stop_array.crend(), [&stops](const auto &el)
                      { stops.push_back(el.AsString()); });
    }
    db_.AddBus(node.at("name").AsString(), std::move(stops));
}

void JsonReader::GetResponce(const Node &node)
{
    using namespace std::string_view_literals;
    out_ << "["sv;
    bool is_first = true;
    for (const auto &req : node.AsArray())
    {
        if (!is_first)
        {
            out_ << ","sv;
        }
        const auto &stat = req.AsMap();
        if (stat.at("type").AsString() == "Stop")
        {
            const auto stop_responce = rh_.GetBusesByStop(stat.at("name").AsString());
            ConstructJson(stop_responce, stat.at("id").AsInt());
        }
        else if (stat.at("type").AsString() == "Bus")
        {
            const auto bus_responce = rh_.GetBusStat(stat.at("name").AsString());
            ConstructJson(bus_responce, stat.at("id").AsInt());
        } else { // stat.at("type").AsString() == "Map"
            const auto map_responce = rh_.RenderMap();
          //  ConstructJson(map_responce, stat.at("id").AsInt());
        }
        is_first = false;
    }
    out_ << "]"sv;
}

void JsonReader::GetRenderSettings(const Node &node)
{
    const auto &settings = node.AsMap();
    renderer_.setWidth(settings.at("width").AsDouble());
    renderer_.setHeight(settings.at("height").AsDouble());
    renderer_.setPadding(settings.at("padding").AsDouble());
    renderer_.setLineWidth(settings.at("line_width").AsDouble());
    renderer_.setStopRadius(settings.at("stop_radius").AsDouble());
    renderer_.setBusLabelFontSize(settings.at("bus_label_font_size").AsInt());
    renderer_.setBusLabelOffset(GetPointFromArray(settings.at("bus_label_offset").AsArray()));
    renderer_.setStopLabelFontSize(settings.at("stop_label_font_size").AsInt());
    renderer_.setStopLabelOffset(GetPointFromArray(settings.at("stop_label_offset").AsArray()));

    const auto &underlayerColor = settings.at("underlayer_color");
    if (underlayerColor.IsString())
        renderer_.setUnderlayerColor(svg::Color{underlayerColor.AsString()});
    else
        renderer_.setUnderlayerColor(GetColorFromArray(underlayerColor));

    renderer_.setUnderlayerWidth(settings.at("underlayer_width").AsDouble());
    renderer_.setColorPalette(std::move(GetPaletteFromArray(settings.at("color_palette"))));

    const auto& stops = db_.GetStops();
    std::vector<geo::Coordinates> coordinates;
    coordinates.reserve(stops.size());
    for_each(stops.begin(), stops.end(), [&](const auto& stop) {
        coordinates.push_back(stop.second->coordinates);
    });
    renderer_.InitProjector(std::move(coordinates));
}

void JsonReader::ConstructJson(const std::optional<std::unordered_set<entity::BusPtr>> &buses, int request_id)
{
    using namespace std::string_view_literals;
    out_ << "{\"request_id\":"sv << request_id << ","sv;
    if (buses == std::nullopt)
    {
        out_ << "\"error_message\":\"not found\"}"sv;
        return;
    }
    out_ << "\"buses\":["sv;
    if (!buses->empty())
    {
        bool is_first = true;
        std::set<entity::BusPtr> tmp_buses{buses->begin(), buses->end()};
        for (const auto &bus : tmp_buses)
        {
            if (!is_first)
            {
                out_ << ","sv;
            }
            out_ << "\""sv;
            out_ << bus;
            out_ << "\""sv;
            is_first = false;
        }
    }
    out_ << "]"sv;
    out_ << "}"sv;
}

void JsonReader::ConstructJson(const std::optional<entity::BusStat> &busstat, int request_id)
{
    using namespace std::string_view_literals;
    out_ << "{\"request_id\":"sv << request_id << ","sv;
    if (!busstat)
    {
        out_ << "\"error_message\":\"not found\""sv;
    }
    else
    {
        out_ << "\"curvature\":"sv << busstat->curvature_ << ","sv;
        out_ << "\"route_length\":"sv << busstat->route_length_ << ","sv;
        out_ << "\"stop_count\":"sv << busstat->stop_count_ << ","sv;
        out_ << "\"unique_stop_count\":"sv << busstat->unique_stop_count_;
    }
    out_ << "}"sv;
}

// void JsonReader::ConstructJson(const svg::Document& document, int request_id) {
//     document.Render(out_);
// }

namespace json
{
    svg::Color GetColorFromArray(const Node &node)
    {
        const auto &array = node.AsArray();
        if (array.size() == 3)
            return svg::Rgb{static_cast<uint8_t>(array[0].AsInt()),
                            static_cast<uint8_t>(array[1].AsInt()),
                            static_cast<uint8_t>(array[2].AsInt())};
        if (array.size() == 4)
            return svg::Rgba{static_cast<uint8_t>(array[0].AsInt()),
                             static_cast<uint8_t>(array[1].AsInt()),
                             static_cast<uint8_t>(array[2].AsInt()),
                             array[3].AsDouble()};
        return svg::NoneColor;
    }

    svg::Point GetPointFromArray(const Node &node)
    {
        return svg::Point{node.AsArray()[0].AsDouble(), node.AsArray()[1].AsDouble()};
    }

    std::vector<svg::Color> GetPaletteFromArray(const Node &node)
    {
        const auto &color_palette = node.AsArray();
        std::vector<svg::Color> palette;
        for (const auto &color : color_palette)
        {
            if (color.IsString())
                palette.push_back(color.AsString());
            else
                palette.push_back(GetColorFromArray(color));
        }
        return palette;
    }
}