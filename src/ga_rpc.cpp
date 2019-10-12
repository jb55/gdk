#include "ga_rpc.hpp"
#include "../subprojects/gdk_rpc/gdk_rpc.h"
#include <nlohmann/json.hpp>

namespace {
class gdkrpc_json {
public:
    explicit gdkrpc_json(const nlohmann::json& val)
        : gdkrpc_json(val.dump())
    {
    }

    explicit gdkrpc_json(const std::string& str) { GDKRPC_convert_string_to_json(str.c_str(), &m_json); }

    GDKRPC_json* get() { return m_json; }

    ~gdkrpc_json() { GDKRPC_destroy_json(m_json); }

private:
    GDKRPC_json* m_json;
};
} // namespace

namespace ga {
namespace sdk {

    ga_rpc::ga_rpc(const std::string& endpoint, const std::string& network)
        : m_endpoint(endpoint)
    {
        (void)endpoint;

        auto networks = gdkrpc_json(network);
    }

    ga_rpc::~ga_rpc()
    {
        // gdk_rpc cleanup
    }

} // namespace sdk
} // namespace ga
