//=============================================================================
// Brief   : Link SAP Linux Main Entry Point
// Authors : Bruno Santos <bsantos@av.it.pt>
//
//
// Copyright (C) 2009 Universidade Aveiro - Instituto de Telecomunicacoes Polo Aveiro
//
// This file is part of ODTONE - Open Dot Twenty One.
//
// This software is distributed under a license. The full license
// agreement can be found in the file LICENSE in this distribution.
// This software may not be copied, modified, sold or distributed
// other than expressed in the named license agreement.
//
// This software is distributed without any warranty.
//=============================================================================

///////////////////////////////////////////////////////////////////////////////
#include "../link_sap.hpp"
#include "../interface/ethernet.hpp"
#include "../interface/if_802_11.hpp"
#include <odtone/debug.hpp>
#include <boost/bind.hpp>
#include <boost/thread.hpp>
#include "rtnetlink.hpp"
#include <iostream>

// This file defines a macro that contains the path to the default
// configuration file
#ifndef LINK_SAP_CONFIG
#define LINK_SAP_CONFIG "link_sap.conf"
#endif

///////////////////////////////////////////////////////////////////////////////
int main(int argc, char** argv)
{
	odtone::setup_crash_handler();

	try {
          odtone::mih::config cfg(argc, argv, LINK_SAP_CONFIG);
          std::cout << LINK_SAP_CONFIG << std::endl;

		std::cout	<< "cfg: port=" << cfg.get<ushort>(odtone::mih::kConf_Port)
					<< " mihf.ip=\"" << cfg.get<std::string>(odtone::mih::kConf_MIHF_Ip)
					<< " mihf.local_port=\"" << cfg.get<ushort>(odtone::mih::kConf_MIHF_Local_Port)
					<< "\"\n";

		boost::asio::io_service ios;
		link_sap ls(cfg, ios);
		boost::thread io(boost::bind(&boost::asio::io_service::run, &ios));

		rtnetlink nl(rtnetlink::link        |
					 rtnetlink::notify      |
					 rtnetlink::tc          |
					 rtnetlink::ipv4_ifaddr |
					 rtnetlink::ipv6_ifaddr |
					 rtnetlink::ipv6_ifinfo |
					 rtnetlink::ipv6_prefix);
		rtnetlink::message msg;

		for (;;) {
			nl.recv(msg);

			do {
				std::cout << "\ngot msg: " << msg.type() << std::endl;

				if (rtnetlink::if_link::is(msg)) {
					rtnetlink::if_link lnk(msg);
					interface* it = nullptr;

					std::cout << "if_type: " << lnk.type() << std::endl;
					std::cout << "if_index: " << lnk.index() << std::endl << std::hex;
					std::cout << "if_flags: " << lnk.flags() << std::endl << std::dec;

					if (lnk.has_address())
						std::cout << "if_address: " << lnk.address() << std::endl;
					if (lnk.has_bcast_addr())
						std::cout << "if_broadcast: " << lnk.bcast_addr() << std::endl;
					if (lnk.has_name())
						std::cout << "if_name: " << lnk.name() << std::endl;
					if (lnk.has_mtu())
						std::cout << "if_mtu: " << lnk.mtu() << std::endl;
					if (lnk.has_lnk_type())
						std::cout << "if_link_type: " << lnk.link_type() << std::endl;
					if (lnk.has_qdisc())
						std::cout << "if_qdisc: " << lnk.qdisc() << std::endl;

					switch (lnk.type()) {
					case rtnetlink::if_link::ethernet:
						it = new interface_ethernet(lnk.index(), lnk.name(),
													odtone::mih::mac_addr(lnk.address().get(), lnk.address().size()));
						break;

					case rtnetlink::if_link::ieee802_11:
						it = new if_802_11(lnk.index(), lnk.name(),
										   odtone::mih::mac_addr(lnk.address().get(), lnk.address().size()));
						break;

					default:
						continue;
					}

					if (lnk.flags() & rtnetlink::if_link::up)
						it->up(true);
					else
						it->up(false);

					ios.dispatch(boost::bind(&link_sap::update, &ls, it));

				} else if (rtnetlink::if_addr::is(msg)) {
					rtnetlink::if_addr addr(msg);

					std::cout << "ifa_family: " << uint(addr.family()) << std::endl;
					std::cout << "ifa_prefixlen: " << uint(addr.prefixlen()) << std::endl;
					std::cout << "ifa_scope: " << uint(addr.scope()) << std::endl << std::hex;
					std::cout << "ifa_flags: " << uint(addr.flags()) << std::endl << std::dec;
					std::cout << "ifa_index: " << addr.index() << std::endl;

					if (addr.has_address())
						std::cout << "ifa_address: " << addr.address() << std::endl;
					if (addr.has_local())
						std::cout << "ifa_local: " << addr.local() << std::endl;
					if (addr.has_label())
						std::cout << "ifa_label: " << addr.label() << std::endl;
					if (addr.has_broadcast())
						std::cout << "ifa_broadcast: " << addr.broadcast() << std::endl;
					if (addr.has_anycast())
						std::cout << "ifa_anycast: " << addr.anycast() << std::endl;
				}

			} while (msg.next());
		}

	} catch(std::exception& e) {
		std::cerr << "exception: " << e.what() << std::endl;

	} catch(const char* str) {
		std::cerr << "exception: " << str << std::endl;
	}
}

// EOF ////////////////////////////////////////////////////////////////////////
