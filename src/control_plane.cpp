#include <control_plane.h>

std::shared_ptr<pdn_connection> control_plane::find_pdn_by_cp_teid(uint32_t cp_teid) const {
    if(_pdns.contains(cp_teid)) return _pdns.at(cp_teid);
    else return nullptr;
}

std::shared_ptr<pdn_connection> control_plane::find_pdn_by_ip_address(const boost::asio::ip::address_v4 &ip) const {
    if(_pdns_by_ue_ip_addr.contains(ip)) return _pdns_by_ue_ip_addr.at(ip); 
    else return nullptr;
}

std::shared_ptr<bearer> control_plane::find_bearer_by_dp_teid(uint32_t dp_teid) const {
    if(_bearers.contains(dp_teid)) return _bearers.at(dp_teid); 
    else return nullptr;
}

std::shared_ptr<last_packet> control_plane::find_bearer_ul_rate_limiter_by_dp_teid(uint32_t dp_teid) const {
    if(_ul_rate_limiter.contains(dp_teid)) return _ul_rate_limiter.at(dp_teid); 
    else return nullptr;
}

std::shared_ptr<last_packet> control_plane::find_bearer_dl_rate_limiter_by_dp_teid(uint32_t dp_teid) const {
    if(_dl_rate_limiter.contains(dp_teid)) return _dl_rate_limiter.at(dp_teid); 
    else return nullptr;
}

std::shared_ptr<pdn_connection> control_plane::create_pdn_connection(const std::string &apn, boost::asio::ip::address_v4 sgw_addr, uint32_t sgw_cp_teid) { 
    srand(time(0));
    boost::asio::ip::address_v4 apn_gw = _apns[apn];

    uint32_t cp_teid = rand();
    while(_pdns.contains(cp_teid) || cp_teid == sgw_cp_teid) {cp_teid = rand();}
    //Initial IP is 10.0.0.2 shifted according to the number of allocated IPs
    uint32_t uint_ue_ip = (10 << 24) + 2 + _pdns_by_ue_ip_addr.size();
    boost::asio::ip::address_v4 ue_ip{boost::asio::ip::make_address_v4(uint_ue_ip)};

    while(_pdns_by_ue_ip_addr.contains(ue_ip)){
        ++uint_ue_ip;
        ue_ip = boost::asio::ip::make_address_v4(uint_ue_ip);
    }
    
    if(uint_ue_ip > (10 << 24) + (255 << 16) + (255 << 8) + (254)){/*TROUBLE! Overflow with IPs*/}

    std::shared_ptr<pdn_connection> pdn = pdn_connection::create(cp_teid, apn_gw, ue_ip);
    pdn->set_sgw_cp_teid(sgw_cp_teid);
    pdn->set_sgw_addr(sgw_addr);

    _pdns[cp_teid] = pdn;
    _pdns_by_ue_ip_addr[ue_ip] = pdn;

    return pdn;
}

void control_plane::delete_pdn_connection(uint32_t cp_teid){
    boost::asio::ip::address_v4 ue_ip = _pdns[cp_teid]->get_ue_ip_addr();
    _pdns.erase(cp_teid);
    _pdns_by_ue_ip_addr.erase(ue_ip);
}

std::shared_ptr<bearer> control_plane::create_bearer(const std::shared_ptr<pdn_connection> &pdn, uint32_t sgw_teid, uint32_t ul_rate_limit, uint32_t dl_rate_limit){ 
    srand(time(0));
    uint32_t dp_teid = rand();
    while(_bearers.contains(dp_teid) || dp_teid == sgw_teid) {dp_teid = rand();}
    std::shared_ptr<bearer> _bearer(new bearer{dp_teid, *pdn.get()});
    _bearer->set_sgw_dp_teid(sgw_teid);
    _bearer->set_ul_rate_limit(ul_rate_limit);
    _bearer->set_dl_rate_limit(dl_rate_limit);
    pdn->add_bearer(_bearer);
    _bearers[dp_teid] = _bearer;
    _ul_rate_limiter[dp_teid] = std::make_shared<last_packet>();
    _dl_rate_limiter[dp_teid] = std::make_shared<last_packet>();

    return _bearer;
}

void control_plane::delete_bearer(uint32_t dp_teid){
    std::shared_ptr<pdn_connection> pdn = _bearers[dp_teid]->get_pdn_connection();
    _bearers.erase(dp_teid);
    _ul_rate_limiter.erase(dp_teid);
    _dl_rate_limiter.erase(dp_teid);
    pdn->remove_bearer(dp_teid);
}

void control_plane::add_apn(std::string apn_name, boost::asio::ip::address_v4 apn_gateway){ _apns[apn_name] = apn_gateway;}              