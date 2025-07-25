#include <bearer.h>

#include <pdn_connection.h>

bearer::bearer(uint32_t dp_teid, pdn_connection &pdn) : _dp_teid(dp_teid), _pdn(pdn) {}

uint32_t bearer::get_sgw_dp_teid() const { return _sgw_dp_teid; }

void bearer::set_sgw_dp_teid(uint32_t sgw_cp_teid) { _sgw_dp_teid = sgw_cp_teid; }

uint32_t bearer::get_dp_teid() const { return _dp_teid; }

std::shared_ptr<pdn_connection> bearer::get_pdn_connection() const { return _pdn.shared_from_this(); }

uint32_t bearer::get_ul_rate_limit() const { return _ul_rate_limit; }

void bearer::set_ul_rate_limit(uint32_t ul_rate_limit) { _ul_rate_limit = ul_rate_limit; }

uint32_t bearer::get_dl_rate_limit() const { return _dl_rate_limit; }

void bearer::set_dl_rate_limit(uint32_t dl_rate_limit) { _dl_rate_limit = dl_rate_limit; }
