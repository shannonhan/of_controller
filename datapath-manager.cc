/* Copyright 2008 (C) Nicira, Inc.
 *
 * This file is part of NOX.
 *
 * NOX is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * NOX is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with NOX.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "datapath-manager.hh"

#include <inttypes.h>
#include <algorithm>
#include <utility>
#include <boost/bind.hpp>
#include <boost/foreach.hpp>
#include <boost/make_shared.hpp>
#include <boost/pool/object_pool.hpp>
#include <boost/ref.hpp>
#include <boost/thread/locks.hpp>
#include <boost/timer.hpp>

#include "assert.hh"
#include "openflow-1.0.hh"

namespace vigil
{
namespace openflow
{


Datapath_manager::Datapath_manager()
{
}

void
Datapath_manager::configure()
{
    v1::ofp_msg::init();
    v1::ofp_stats_request::init();
    v1::ofp_stats_reply::init();
    v1::ofp_vendor_stats_request::init();
    v1::ofp_vendor_stats_reply::init();
    v1::ofp_action::init();
    v1::ofp_action_vendor::init();
    v1::ofp_queue_prop::init();
    v1::ofp_vendor::init();
}

void
Datapath_manager::register_default_events()
{
}

void
Datapath_manager::register_default_handlers()
{

}

Disposition
Datapath_manager::handle_shutdown()
{
    //const Shutdown_event& se = assert_cast<const Shutdown_event&>(e);
    BOOST_FOREACH(auto conn, connecting_dps)
    {
        conn->close();
    }
    BOOST_FOREACH(auto conn, connected_dps)
    {
        conn.second->close();
    }
    return CONTINUE;
}

Disposition
Datapath_manager::handle_new_connection(boost::shared_ptr<Connection> conn)
{
    boost::shared_ptr<Openflow_datapath> dp(new Openflow_datapath(*this));
    dp->set_connection(conn);
    boost::lock_guard<boost::mutex> lock(dp_mutex);
    connecting_dps.insert(dp);
    return CONTINUE;
}

Disposition
Datapath_manager::handle_datapath_join(boost::shared_ptr<Openflow_datapath> dp)
{
    boost::lock_guard<boost::mutex> lock(dp_mutex);
    connecting_dps.erase(dp);
    connected_dps[dp->id()] = dp;
    return CONTINUE;
}

Disposition
Datapath_manager::handle_datapath_leave(boost::shared_ptr<Openflow_datapath> dp) 
{
    boost::lock_guard<boost::mutex> lock(dp_mutex);
    connected_dps.erase(dp->id());
    return CONTINUE;
}

Disposition
Datapath_manager::handle_echo_request(boost::shared_ptr<Openflow_datapath> dp)
{
    //auto req = assert_cast<const v1::ofp_echo_request*>(msg);
    //v1::ofp_echo_reply rep(*req);
    //dp.send(&rep);
    //return STOP;
}

} // namespace vigil
} // namespace vigil

