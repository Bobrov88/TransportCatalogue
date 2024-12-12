// #include <iostream>
// #include <string>

// // #include "input_reader.h"
// // #include "stat_reader.h"
// #include "json_reader.h"
// #include "transport_catalogue.h"

// using namespace std;
// using namespace Data;
// using namespace json;
// using namespace Renderer;

// int main()
// {
//     TransportCatalogue db;
//     MapRenderer renderer;
//     RequestHandler handler(db, renderer);
//     JsonReader reader(db, handler, renderer, std::cin, std::cout);
//     reader.ProcessTransportCatalogue();
// }

#include "json_builder.h"
#include <iostream>

using namespace std;

int main()
{
    try
    {
        json::Print(
            json::Document{
                json::Builder{}
                    .StartDict()
                    .Key("key1"s)
                    .Value(123)
                    .Key("key2"s)
                    .Value("value2"s)
                    .Key("key3"s)
                    .StartArray()
                    .Value(456)
                    .StartDict()
                    .EndDict()
                    .StartDict()
                    .Key(""s)
                    .Value(nullptr)
                    .EndDict()
                    .Value(""s)
                    .EndArray()
                    .EndDict()
                    .Build()},
            cout);
        cout << endl;
    }
    catch (std::exception &e)
    {
        std::cout << "1 " << e.what() << endl;
    }
    try
    {
        json::Print(
            json::Document{
                json::Builder{}
                    .Value("just a string"s)
                    .Build()},
            cout);
        cout << endl;
    }
    catch (std::exception &e)
    {
        std::cout << "2 " << e.what() << endl;
    }
}