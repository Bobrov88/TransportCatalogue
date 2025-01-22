#include "json_reader.h"
#include "transport_catalogue.h"
#include "transport_router.h"

using namespace std;
using namespace Data;
using namespace json;
using namespace Renderer;

int main()
{
    TransportCatalogue db;
    MapRenderer renderer;
    RequestHandler handler(db, renderer);
    JsonReader reader(db,
                      handler,
                      renderer,
                      std::cin,
                      std::cout);
    reader.Process();
}