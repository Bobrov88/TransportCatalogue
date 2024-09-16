#include "input_reader.h"

#include <algorithm>
#include <iterator>
#include <map>

/**
 * Парсит строку вида "10.123,  -30.1837" и возвращает пару координат (широта, долгота)
 */
using geo::Coordinates;
using namespace Input;
using namespace std::string_view_literals;

Coordinates ParseCoordinates(std::string_view str)
{
    static const double nan = std::nan("");

    auto not_space = str.find_first_not_of(' ');
    auto comma = str.find(',');

    if (comma == str.npos)
    {
        return {nan, nan};
    }

    auto not_space2 = str.find_first_not_of(' ', comma + 1);

    double lat = std::stod(std::string(str.substr(not_space, comma - not_space)));
    double lng = std::stod(std::string(str.substr(not_space2)));

    return {lat, lng};
}

/**
 * Удаляет пробелы в начале и конце строки
 */
std::string_view Trim(std::string_view string)
{
    const auto start = string.find_first_not_of(' ');
    if (start == string.npos)
    {
        return {};
    }
    return string.substr(start, string.find_last_not_of(' ') + 1 - start);
}

/**
 * Разбивает строку string на n строк, с помощью указанного символа-разделителя delim
 */
std::vector<std::string_view> Split(std::string_view string, char delim)
{
    std::vector<std::string_view> result;

    size_t pos = 0;
    while ((pos = string.find_first_not_of(' ', pos)) < string.length())
    {
        auto delim_pos = string.find(delim, pos);
        if (delim_pos == string.npos)
        {
            delim_pos = string.size();
        }
        if (auto substr = Trim(string.substr(pos, delim_pos - pos)); !substr.empty())
        {
            result.push_back(substr);
        }
        pos = delim_pos + 1;
    }

    return result;
}

/**
 * Парсит маршрут.
 * Для кольцевого маршрута (A>B>C>A) возвращает массив названий остановок [A,B,C,A]
 * Для некольцевого маршрута (A-B-C-D) возвращает массив названий остановок [A,B,C,D,C,B,A]
 */
std::vector<std::string_view> ParseRoute(std::string_view route)
{
    if (route.find('>') != route.npos)
    {
        return Split(route, '>');
    }

    auto stops = Split(route, '-');
    std::vector<std::string_view> results(stops.begin(), stops.end());
    results.insert(results.end(), std::next(stops.rbegin()), stops.rend());

    return results;
}

CommandDescription ParseCommandDescription(std::string_view line)
{
    auto colon_pos = line.find(':');
    if (colon_pos == line.npos)
    {
        return {};
    }

    auto space_pos = line.find(' ');
    if (space_pos >= colon_pos)
    {
        return {};
    }

    auto not_space = line.find_first_not_of(' ', space_pos);
    if (not_space >= colon_pos)
    {
        return {};
    }

    return {std::string(line.substr(0, space_pos)),
            std::string(line.substr(not_space, colon_pos - not_space)),
            std::string(line.substr(colon_pos + 1))};
}

void InputReader::ParseLine(std::string_view line)
{
    auto command_description = ParseCommandDescription(line);
    if (command_description)
    {
        commands_.push_back(std::move(command_description));
    }
}

std::map<std::string, int> ParseDistance(std::vector<std::string_view> distances)
{
    std::map<std::string, int> parsed_distances;
    for (auto &distance : distances)
    {
        auto m_pos = distance.find_first_of('m');
        auto int_distance = std::stoi(std::string(distance.substr(0, m_pos)));
        auto to_pos = distance.find_first_of(" to ");
        auto stop_name = distance.substr(to_pos + 4);
        parsed_distances.emplace(std::move(stop_name), std::move(int_distance));
    }
    return parsed_distances;
}

void InputReader::ApplyCommands([[maybe_unused]] Data::TransportCatalogue &catalogue) const
{
    std::unordered_map<std::string_view, std::map<std::string, int>> temp_distances;
    for (const auto &command : commands_)
    {
        if (command.command == "Stop")
        {
            auto comma1 = std::find(command.description.begin(), command.description.end(), ',');
            auto comma2 = std::find(std::next(comma1), command.description.end(), ',');
            if (comma2 == command.description.end())
            {
                catalogue.AddStop(command.id, std::move(ParseCoordinates(command.description)));
            }
            else
            {
                catalogue.AddStop(command.id, std::move(ParseCoordinates(std::move(std::string(command.description.begin(), comma2)))));

                auto distance = ParseDistance(Split(std::move(std::string(std::next(comma2), command.description.end())), ','));
                temp_distances.emplace(command.id, std::move(distance));
            }
        }
    }

    catalogue.AddDistances(std::move(temp_distances));

    for (const auto &command : commands_)
        if (command.command == "Bus")
            catalogue.AddBus(command.id, ParseRoute(command.description));

    catalogue.AddBusesToStop();
}