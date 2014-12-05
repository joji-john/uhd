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

#ifndef INCLUDED_LIBUHD_NODE_CTRL_BASE_HPP
#define INCLUDED_LIBUHD_NODE_CTRL_BASE_HPP

#include <uhd/types/device_addr.hpp>
#include <uhd/usrp/rfnoc/constants.hpp>
#include <boost/cstdint.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/utility.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <map>
#include <set>

namespace uhd {
    namespace rfnoc {

#define UHD_RFNOC_BLOCK_TRACE() UHD_MSG(status) << "[" << unique_id() << "] "

/*! \brief Abstract base class for streaming nodes.
 *
 */
class UHD_API node_ctrl_base;
class node_ctrl_base : boost::noncopyable, public boost::enable_shared_from_this<node_ctrl_base>
{
public:
    /***********************************************************************
     * Types
     **********************************************************************/
    typedef boost::shared_ptr<node_ctrl_base> sptr;
    typedef boost::weak_ptr<node_ctrl_base> wptr;
    typedef std::map< size_t, wptr > node_map_t;
    typedef std::pair< size_t, wptr > node_map_pair_t;

    /***********************************************************************
     * Node control
     **********************************************************************/
    /*! Initialize the block arguments.
     *
     * This triggers a _post_args_hook().
     */
    void set_args(const uhd::device_addr_t &args);

    //! Returns a unique string that identifies this block.
    virtual std::string unique_id() const;

    /***********************************************************************
     * Connections
     **********************************************************************/
    /*! Clears the list of connected nodes.
     */
    virtual void clear();

    node_map_t list_downstream_nodes() { return _downstream_nodes; };
    node_map_t list_upstream_nodes() { return _upstream_nodes; };

    /*! Find nodes downstream that match a predicate.
     *
     * Uses a non-recursive breadth-first search algorithm.
     * On every branch, the search stops if a block matches.
     * See this example:
     * <pre>
     * A -> B -> C -> C
     * </pre>
     * Say node A searches for nodes of type C. It will only find the
     * first 'C' block, not the second.
     *
     * Returns blocks that are of type T.
     *
     * Search only goes downstream.
     */
    template <typename T>
    UHD_INLINE std::vector< boost::shared_ptr<T> > find_downstream_node()
    {
        return _find_child_node<T>(true);
    }

    /*! Same as find_downstream_node(), but only search upstream.
     */
    template <typename T>
    UHD_INLINE std::vector< boost::shared_ptr<T> > find_upstream_node()
    {
        return _find_child_node<T>(false);
    }

protected:
    /***********************************************************************
     * Structors
     **********************************************************************/
    node_ctrl_base(void) {};
    virtual ~node_ctrl_base() {};

    /***********************************************************************
     * Protected members
     **********************************************************************/

    //! Stores default arguments
    uhd::device_addr_t _args;

    // TODO make these private

    //! List of upstream nodes
    node_map_t _upstream_nodes;

    //! List of downstream nodes
    node_map_t _downstream_nodes;

    /***********************************************************************
     * Hooks
     **********************************************************************/

    /*! This method is called whenever _args is changed.
     *
     * Derive it to update block-specific settings, or to sanity-check
     * the new _args.
     *
     * May throw.
     */
    virtual void _post_args_hook();

    /***********************************************************************
     * Connections
     **********************************************************************/
    /*! Registers another node as downstream of this node, connected to a given port.
     *
     * This implies that this node is a source node, and the downstream node is
     * a sink node.
     * See also uhd::rfnoc::source_node_ctrl::_register_downstream_node().
     */
    virtual void _register_downstream_node(
            node_ctrl_base::sptr downstream_node,
            size_t port
    );

    /*! Registers another node as upstream of this node, connected to a given port.
     *
     * This implies that this node is a sink node, and the upstream node is
     * a source node.
     * See also uhd::rfnoc::sink_node_ctrl::_register_upstream_node().
     */
    virtual void _register_upstream_node(
            node_ctrl_base::sptr upstream_node,
            size_t port
    );

private:
    /*! Implements the search algorithm for find_downstream_node() and
     * find_upstream_node().
     *
     * Depending on \p downstream, "child nodes" are either defined as
     * nodes connected downstream or upstream.
     *
     * \param downstream Set to true if search goes downstream, false for upstream.
     */
    template <typename T>
    std::vector< boost::shared_ptr<T> > _find_child_node(bool downstream);

}; /* class node_ctrl_base */

}} /* namespace uhd::rfnoc */

#include <uhd/usrp/rfnoc/node_ctrl_base.ipp>

#endif /* INCLUDED_LIBUHD_NODE_CTRL_BASE_HPP */
// vim: sw=4 et:
