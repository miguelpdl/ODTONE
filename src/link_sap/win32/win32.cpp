//=============================================================================
// Brief   : Win32 API Warpers
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

#include "win32.hpp"
#include <odtone/cast.hpp>
#include <boost/throw_exception.hpp>
#include <boost/system/system_error.hpp>
#include <boost/bind.hpp>
#include <boost/scoped_ptr.hpp>
#include <cstring>

///////////////////////////////////////////////////////////////////////////////
namespace win { namespace detail {

///////////////////////////////////////////////////////////////////////////////
static void wlan_close(void* h)
{
	::WlanCloseHandle(h, nullptr);
}

static void WINAPI wlan_notify_handler(PWLAN_NOTIFICATION_DATA data, void* context)
{
	wlan_register_notification_handler* pcb = reinterpret_cast<wlan_register_notification_handler*>(context);
	wlan_register_notification_handler& cb = *pcb;
	wlan_notification_data nd;

	if (data->NotificationSource == WLAN_NOTIFICATION_SOURCE_NONE) {
		delete pcb;
		return;
	}

	if (!cb)
		return;

	switch (data->NotificationCode) {
	case wlan_notification_acm_connection_start:
	case wlan_notification_acm_connection_complete:
	case wlan_notification_acm_connection_attempt_fail: {
			WLAN_CONNECTION_NOTIFICATION_DATA* dt = reinterpret_cast<WLAN_CONNECTION_NOTIFICATION_DATA*>(data->pData);

			nd.error = dt->wlanReasonCode;
			nd.profile = dt->strProfileName;
			nd.ssid.assign(reinterpret_cast<char*>(dt->dot11Ssid.ucSSID), dt->dot11Ssid.uSSIDLength);
			nd.is_secure = bool(dt->bSecurityEnabled);
		}
		break;
	}
	cb(wlan_notification_type(data->NotificationCode), nd);
}

///////////////////////////////////////////////////////////////////////////////
} /* namespace detail */

using odtone::nullptr;

///////////////////////////////////////////////////////////////////////////////
handle wlan_open()
{
	DWORD res;
	DWORD ver;
	HANDLE h;

	res = ::WlanOpenHandle(WLAN_API_VERSION, nullptr, &ver, &h);
	if (res != ERROR_SUCCESS)
		boost::throw_exception(boost::system::system_error(res,
														   boost::system::get_system_category(),
														   "win::wlan_open"));

	return handle(h, detail::wlan_close);
}

void wlan_register_notification(const handle& h, const wlan_register_notification_handler& handler)
{
	wlan_register_notification_handler* cb = new wlan_register_notification_handler(handler);
	DWORD res;

	res = ::WlanRegisterNotification(h.get(), WLAN_NOTIFICATION_SOURCE_ALL, TRUE,
									 detail::wlan_notify_handler, cb,
									 nullptr, nullptr);
	if (res == ERROR_SUCCESS)
		return;

	delete cb;
	boost::throw_exception(boost::system::system_error(res,
													   boost::system::get_system_category(),
													   "win::wlan_register_notification"));
}

std::string wstring_to_string(wchar_t const* str, size_t len)
{
	std::string tmp;
	int res;

	res = ::WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, str, odtone::truncate_cast<int>(len), nullptr, 0, nullptr, FALSE);
	if (res > 0) {
		tmp.resize(res);
		::WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, str, len, &tmp[0], res, nullptr, FALSE);
		tmp.resize(res - 1);

	} else {
		boost::throw_exception(boost::system::system_error(::GetLastError(),
														   boost::system::get_system_category(),
														   "win::wstring_to_string"));
	}

	return tmp;
}

std::string wstring_to_string(const wchar_t* str)
{
	return wstring_to_string(str, std::wcslen(str));
}

///////////////////////////////////////////////////////////////////////////////
wlan_if_list wlan_enum_interfaces(handle const& h)
{
	WLAN_INTERFACE_INFO_LIST* iflist = nullptr;
	DWORD res;

	res = ::WlanEnumInterfaces(h.get(), nullptr, &iflist);
	if (res == ERROR_SUCCESS)
		return wlan_if_list(iflist, &::WlanFreeMemory);

	boost::throw_exception(boost::system::system_error(res,
													   boost::system::get_system_category(),
													   "win::wlan_if_list::update"));
	return wlan_if_list();
}

///////////////////////////////////////////////////////////////////////////////
} /* namespace win */

// EOF ////////////////////////////////////////////////////////////////////////
