#include "src/network_parameters.hpp"
#include "src/session.hpp"
#include <assert.h>
#include <nlohmann/json.hpp>
#include <stdio.h>
#include <stdlib.h>

// #define CHECK_OK assert(ret)

void test_receive_addresses(ga::sdk::session& session) {
    std::string a1 = session.get_receive_address(nlohmann::json{})["address"];
    std::string a2 = session.get_receive_address(nlohmann::json{})["address"];

    assert(a1 != a2);

    printf("addr1: %s\naddr2: %s\n", a1.c_str(), a2.c_str());
}

void test_get_transactions(ga::sdk::session& session) {
    nlohmann::json details;
    auto ret = session.get_transactions(details);
    auto tx = ret.size() > 0 ? ret[0] : ret;

    printf("transactions (%ld): \n%s", ret.size(), tx.dump().c_str());
}

void test_get_balance(ga::sdk::session& session) {
    nlohmann::json balance_details;
    balance_details["num_confs"] = 0;
    auto res = session.get_balance(balance_details);

    assert(res["btc"] >= 0);
}


void test_get_fee_estimates(ga::sdk::session& session) {
    auto res = session.get_fee_estimates();
    auto fees = res["fees"];

    assert(fees.size() > 0);
    assert(fees[0].get<double>() >= 0);

    printf("estimates %s\n", res.dump().c_str());
}


int main()
{
    nlohmann::json init_config;
    init_config["datadir"] = ".";

    const char *mnemonic = getenv("BITCOIN_MNEMONIC");
    const char *network = getenv("GDK_NETWORK");
    const char *url = getenv("GDK_NETWORK_URL");

    if (mnemonic == nullptr)
        mnemonic = "abandon abandon abandon abandon abandon abandon abandon abandon abandon abandon abandon about";

    if (network == nullptr)
        network = "electrum-testnet";

    if (url == nullptr)
        url = "electrum2.hodlister.co:50002";

    std::string state_dir = "/tmp/gdk-" + std::string(network);

    nlohmann::json net_params;
    net_params["log_level"] = "debug";
    net_params["use_tor"] = false;
    net_params["state_dir"] = state_dir;
    net_params["url"] = url;
    net_params["name"] = network;
    net_params["validate_electrum_domain"] = false;

    printf("testing with network(%s) url(%s) state_dir(%s)\n",
           network, url, state_dir.c_str());

    ga::sdk::init(init_config);
    {
        ga::sdk::session session;

        session.connect(net_params);
        session.login(mnemonic, "");

        test_receive_addresses(session);
        test_get_transactions(session);
        test_get_balance(session);
        test_get_fee_estimates(session);
    }

    return 0;
}

