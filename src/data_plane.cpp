#include <data_plane.h>

data_plane::data_plane(control_plane &control_plane) : _control_plane(control_plane) {}

void data_plane::handle_uplink(uint32_t dp_teid, Packet &&packet){
    std::shared_ptr<bearer> _bearer = _control_plane.find_bearer_by_dp_teid(dp_teid);
    if(_bearer == nullptr) return;

    std::shared_ptr<last_packet> _last_packet = _control_plane.find_bearer_ul_rate_limiter_by_dp_teid(dp_teid);
    if(_last_packet == nullptr) return;

    if(_bearer->get_ul_rate_limit() == 0){
        forward_packet_to_apn(_bearer->get_pdn_connection()->get_apn_gw(), std::move(packet));
    }
    else if(_last_packet->last_packet_size == 0 && _last_packet->last_timestamp == 0){
        _last_packet->last_packet_size = packet.size();
        _last_packet->last_timestamp = clock();
        forward_packet_to_apn(_bearer->get_pdn_connection()->get_apn_gw(), std::move(packet));
    }
    else{
        double period = (double)(clock() - _last_packet->last_timestamp) / CLOCKS_PER_SEC;
        if(_last_packet->last_packet_size / period < _bearer->get_ul_rate_limit()){
            _last_packet->last_packet_size = packet.size();
            _last_packet->last_timestamp = clock();
            forward_packet_to_apn(_bearer->get_pdn_connection()->get_apn_gw(), std::move(packet));
        }
    }
}
void data_plane::handle_downlink(const boost::asio::ip::address_v4 &ue_ip, Packet &&packet){
    std::shared_ptr<pdn_connection> pdn = _control_plane.find_pdn_by_ip_address(ue_ip);
    if(pdn == nullptr) return;

    std::shared_ptr<bearer> _bearer = pdn->get_default_bearer();
    if(_bearer == nullptr) return;

    std::shared_ptr<last_packet> _last_packet = _control_plane.find_bearer_dl_rate_limiter_by_dp_teid(_bearer->get_dp_teid());
    if(_last_packet == nullptr) return;

    if(_bearer->get_dl_rate_limit() == 0){
        forward_packet_to_sgw(pdn->get_sgw_address(), _bearer->get_sgw_dp_teid(), std::move(packet));
    }
    else if(_last_packet->last_packet_size == 0 && _last_packet->last_timestamp == 0){
        _last_packet->last_packet_size = packet.size();
        _last_packet->last_timestamp = clock();
        forward_packet_to_sgw(pdn->get_sgw_address(), _bearer->get_sgw_dp_teid(), std::move(packet));
    }
    else{
        double period = (double)(clock() - _last_packet->last_timestamp) / CLOCKS_PER_SEC;
        if(_last_packet->last_packet_size / period < _bearer->get_dl_rate_limit()){
            _last_packet->last_packet_size = packet.size();
            _last_packet->last_timestamp = clock();
            forward_packet_to_sgw(pdn->get_sgw_address(), _bearer->get_sgw_dp_teid(), std::move(packet));
        }
    }
}
