#include <iostream>
#include <string>

// #include "input_reader.h"
// #include "stat_reader.h"
#include "json_reader.h"
#include "transport_catalogue.h"

using namespace std;
using namespace Data;
using namespace json;
using namespace Renderer;

int main()
{
    TransportCatalogue db;
    MapRenderer renderer;
    RequestHandler handler(db, renderer);
    JsonReader reader(db, handler, renderer, std::cin, std::cout);
    reader.ProcessTransportCatalogue();
}