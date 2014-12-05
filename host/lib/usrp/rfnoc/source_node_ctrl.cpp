//
// Copyright 2014 Ettus Research LLC
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

#include "utils.hpp"
#include <uhd/usrp/rfnoc/source_node_ctrl.hpp>
#include <uhd/usrp/rfnoc/sink_node_ctrl.hpp>

using namespace uhd::rfnoc;

size_t source_node_ctrl::connect_downstream(
        node_ctrl_base::sptr downstream_node,
        size_t port,
        const uhd::device_addr_t &args
) {
    boost::mutex::scoped_lock lock(_output_mutex);
    port = _request_output_port(port, args);
    _register_downstream_node(downstream_node, port);
    return port;
}

size_t source_node_ctrl::_request_output_port(
        const size_t suggested_port,
        const uhd::device_addr_t &
) const {
    return utils::node_map_find_first_free(_downstream_nodes, suggested_port);
}

void source_node_ctrl::_register_downstream_node(
        node_ctrl_base::sptr downstream_node,
        size_t port
) {
    // Do all the checks:
    if (port == ANY_PORT) {
        throw uhd::type_error("Invalid input port number.");
    }
    if (_downstream_nodes.count(port)) {
        throw uhd::runtime_error(str(boost::format("On node %s, output port %d is already connected.") % unique_id() % port));
    }
    if (not boost::dynamic_pointer_cast<sink_node_ctrl>(downstream_node)) {
        throw uhd::type_error("Attempting to register a non-sink block as downstream.");
    }
    // Alles klar, Herr Kommissar :)

    _downstream_nodes[port] = boost::weak_ptr<node_ctrl_base>(downstream_node);
}

