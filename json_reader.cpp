#include "json_reader.h"
#include "json_builder.h"
#include <set>

using namespace json;

void JsonReader::ProcessTransportCatalogue()
{
    Document doc(Load(in_));
    const auto &dict = doc.GetRoot().AsMap();
    FillDataBase(dict.at("base_requests"));
    GetRenderSettings(dict.at("render_settings"));
    GetResponce(dict.at("stat_requests"));
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
    db_.AddBus(node.at("name").AsString(), std::move(stops), node.at("is_roundtrip").AsBool());
}

void JsonReader::GetResponce(const Node &node)
{
    using namespace std::string_literals;
    std::vector<json::Node> responces;

    for (const auto &req : node.AsArray())
    {
        const auto &stat = req.AsMap();
        if (stat.at("type").AsString() == "Stop")
        {
            const auto stop_responce = rh_.GetBusesByStop(stat.at("name").AsString());
            responces.push_back(ConstructJson(stop_responce, stat.at("id").AsInt()));
        }
        else if (stat.at("type").AsString() == "Bus")
        {
            const auto bus_responce = rh_.GetBusStat(stat.at("name").AsString());
            responces.push_back(ConstructJson(bus_responce, stat.at("id").AsInt()));
        }
        else if (stat.at("type").AsString() == "Map")
        {
            const auto map_responce = rh_.RenderMap();
            responces.push_back(ConstructJson(map_responce, stat.at("id").AsInt()));
        }
    }
    json::Document doc(responces);
    Print(doc, out_);
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

    const auto &stops = db_.GetStops();
    std::vector<geo::Coordinates> coordinates;
    coordinates.reserve(stops.size());
    for_each(stops.begin(), stops.end(), [&](const auto &stop)
             {
        if (stop.second->is_in_route)
        coordinates.push_back(stop.second->coordinates); });
    renderer_.InitProjector(std::move(coordinates));
}

json::Node JsonReader::ConstructJson(const std::optional<std::unordered_set<entity::BusPtr>> &buses, int request_id)
{
    using namespace std::string_literals;

    if (buses == std::nullopt)
        return json::Builder{}.StartDict().Key("request_id"s).Value(request_id).Key("error_message").Value("not found"s).EndDict().Build();

    std::set<entity::BusPtr> tmp_buses{buses->begin(), buses->end()};

    json::Array arr;
    for (const auto &bus : tmp_buses)
    {
        arr.push_back(json::Node(std::string(bus)));
    }
    return json::Builder{}.StartDict().Key("request_id"s).Value(request_id).Key("buses"s).Value(std::move(arr)).EndDict().Build();
}

json::Node JsonReader::ConstructJson(const std::optional<entity::BusStat> &busstat, int request_id)
{
    using namespace std::string_literals;

    if (!busstat)
        return json::Builder{}.StartDict().Key("request_id"s).Value(request_id).Key("error_message"s).Value("not found"s).EndDict().Build();

    return json::Builder{}.StartDict().Key("request_id"s).Value(request_id).Key("curvature"s).Value(busstat->curvature_).Key("route_length"s).Value(busstat->route_length_).Key("stop_count"s).Value(static_cast<int>(busstat->stop_count_)).Key("unique_stop_count"s).Value(static_cast<int>(busstat->unique_stop_count_)).EndDict().Build();
}

json::Node JsonReader::ConstructJson(const svg::Document &document, int request_id)
{
    using namespace std::string_literals;
    std::ostringstream oss;
    document.Render(oss);

    return json::Builder{}.StartDict().Key("request_id"s).Value(request_id).Key("map"s).Value(oss.str()).EndDict().Build();
}

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