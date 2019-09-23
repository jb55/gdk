#include <string>

namespace ga {
namespace sdk {

    class ga_rpc final {
    public:
        ~ga_rpc();

        explicit ga_rpc(const std::string& endpoint, const std::string& network);

        void connect();

    private:
        std::string m_endpoint;
    };

} // namespace sdk
} // namespace ga
