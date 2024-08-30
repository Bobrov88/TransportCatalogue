#include "request_handler.h"

std::optional<BusStat> RequestHandler::GetBusStat(const std::string_view &bus_name) const { return {}; }

const std::unordered_set<BusPtr> *RequestHandler::GetBusesByStop(const std::string_view &stop_name) const { return {}; }
