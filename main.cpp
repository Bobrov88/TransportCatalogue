#include <iostream>
#include <string>

// #include "input_reader.h"
// #include "stat_reader.h"
#include "json_reader.h"
#include "transport_catalogue.h"

using namespace std;
using namespace Data;
using namespace json;
using namespace renderer;

int main()
{
    TransportCatalogue db;
    RequestHandler handler(db, MapRenderer{});
    JsonReader reader(db, std::cin, handler, std::cout);
    reader.ProcessTransportCatalogue();
}