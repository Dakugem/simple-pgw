#pragma once

#include <boost/asio/ip/address_v4.hpp>
#include <ctime>

class pdn_connection;

class bearer {
public:
    bearer(uint32_t dp_teid, pdn_connection &pdn);

    [[nodiscard]] uint32_t get_sgw_dp_teid() const;
    void set_sgw_dp_teid(uint32_t sgw_cp_teid);

    [[nodiscard]] uint32_t get_dp_teid() const;

    [[nodiscard]] std::shared_ptr<pdn_connection> get_pdn_connection() const;

    [[nodiscard]] uint32_t get_ul_rate_limit() const;
    void set_ul_rate_limit(uint32_t ul_rate_limit);

    [[nodiscard]] uint32_t get_dl_rate_limit() const;
    void set_dl_rate_limit(uint32_t dl_rate_limit);

private:
    uint32_t _sgw_dp_teid{};
    uint32_t _dp_teid{};
    pdn_connection &_pdn;
    uint32_t _ul_rate_limit{};
    uint32_t _dl_rate_limit{};
};

struct last_packet{
    size_t last_packet_size{};
    clock_t last_timestamp{};
};
