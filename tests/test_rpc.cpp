#include "src/network_parameters.hpp"
#include "src/session.hpp"
#include <assert.h>
#include <nlohmann/json.hpp>
#include <stdio.h>
#include <stdlib.h>

int main()
{
    nlohmann::json init_config;
    init_config["datadir"] = ".";

    const char *username = getenv("BITCOIN_RPCUSER");
    const char *password = getenv("BITCOIN_RPCPASS");
    const char *wallet = getenv("BITCOIN_RPCWALLET");

    nlohmann::json net_params;
    net_params["log_level"] = "debug";
    net_params["use_tor"] = false;
    net_params["rpc_url"] = "http://localhost:14331";
    net_params["username"] = username? username : "username";
    net_params["password"] = password? password : "password";
    // net_params["proxy"] = "localhost:9050";
    net_params["name"] = "bitcoin-mainnet";
    net_params["wallet"] = wallet? wallet : "";

    ga::sdk::init(init_config);
    {
        nlohmann::json details;
        ga::sdk::session session;

        session.connect(net_params);
        auto ret = session.get_transactions(details);
        printf("transactions (%ld): \n%s", ret.size(), ret.dump().c_str());
    }

    return 0;
}
