//=============================================================================
// Brief   : Generic Network Interface
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

#include <odtone/debug.hpp>
#include <odtone/sap/nif/interface.hpp>

///////////////////////////////////////////////////////////////////////////////
namespace odtone { namespace sap { namespace nif {

///////////////////////////////////////////////////////////////////////////////
namespace detail {

///////////////////////////////////////////////////////////////////////////////
static void interface_disposer(interface* p)
{
	delete p;
}

///////////////////////////////////////////////////////////////////////////////
} /* namespace detail */

///////////////////////////////////////////////////////////////////////////////
interface::interface(if_id const& id, odtone::mih::link_type type)
	: _id(id), _type(type)
{
}

interface::~interface()
{
}

boost::logic::tribool interface::up(const boost::logic::tribool& tb)
{
	boost::logic::tribool prev = _up;

	_up = tb;
	return prev;
}

void interface::name(const string& name)
{
	_name = name;
}

///////////////////////////////////////////////////////////////////////////////
interface_map::interface_map()
{
}

interface_map::~interface_map()
{
	ODTONE_ASSERT(_map.empty());
}

std::pair<interface_map::iterator, bool> interface_map::insert(interface& i)
{
	return _map.insert_unique(i);
}

void interface_map::erase(const_iterator i)
{
	_map.erase_and_dispose(i, detail::interface_disposer);
}

void interface_map::erase(if_id const& id)
{
	_map.erase_and_dispose(id, compare(), detail::interface_disposer);
}

interface_map::iterator interface_map::find(if_id const& id)
{
	return _map.find(id, compare());
}

interface_map::const_iterator interface_map::find(if_id const& id) const
{
	return _map.find(id, compare());
}

interface_map::iterator interface_map::begin()
{
	return _map.begin();
}

interface_map::iterator interface_map::end()
{
	return _map.end();
}

interface_map::const_iterator interface_map::begin() const
{
	return _map.begin();
}

interface_map::const_iterator interface_map::end() const
{
	return _map.end();
}

///////////////////////////////////////////////////////////////////////////////
} /* namespace nif */ } /* namespace sap */ } /* namespace odtone */

// EOF ////////////////////////////////////////////////////////////////////////
