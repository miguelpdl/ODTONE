//=============================================================================
// Brief   : MIH QOS Types
// Authors : Bruno Santos <bsantos@av.it.pt>
//------------------------------------------------------------------------------
// ODTONE - Open Dot Twenty One
//
// Copyright (C) 2009-2011 Universidade Aveiro
// Copyright (C) 2009-2011 Instituto de Telecomunicações - Pólo Aveiro
//
// This software is distributed under a license. The full license
// agreement can be found in the file LICENSE in this distribution.
// This software may not be copied, modified, sold or distributed
// other than expressed in the named license agreement.
//
// This software is distributed without any warranty.
//==============================================================================

#ifndef ODTONE_MIH_TYPES_QOS__HPP_
#define ODTONE_MIH_TYPES_QOS__HPP_

///////////////////////////////////////////////////////////////////////////////
#include <odtone/mih/types/base.hpp>

///////////////////////////////////////////////////////////////////////////////
namespace odtone { namespace mih {

///////////////////////////////////////////////////////////////////////////////
/**
 * Define NUM_COS_TYPES data type.
 */
typedef uint8 num_cos_types;

/**
 * Define COS_ID data type.
 */
typedef uint8 cos_id;

///////////////////////////////////////////////////////////////////////////////
/**
 * Define PK_DELAY data type.
 */
struct pk_delay {
	cos_id cos;
	uint16 value;


	template<class ArchiveT>
	void serialize(ArchiveT& ar)
	{
		ar & cos;
		ar & value;
	}
};

///////////////////////////////////////////////////////////////////////////////
/**
 * Define MIN_PK_TX_DELAY data type.
 */
struct min_pk_tx_delay : pk_delay { };

/**
 * Define LIST(MIN_PK_TX_DELAY) data type.
 */
typedef std::vector<min_pk_tx_delay> min_pk_tx_delay_list;

///////////////////////////////////////////////////////////////////////////////
/**
 * Define AVG_PK_TX_DELAY data type.
 */
struct avg_pk_tx_delay : pk_delay { };

/**
 * Define LIST(AVG_PK_TX_DELAY) data type.
 */
typedef std::vector<avg_pk_tx_delay> avg_pk_tx_delay_list;

///////////////////////////////////////////////////////////////////////////////
/**
 * Define MAX_PK_TX_DELAY data type.
 */
struct max_pk_tx_delay : pk_delay { };

/**
 * Define LIST(MAX_PK_TX_DELAY) data type.
 */
typedef std::vector<max_pk_tx_delay> max_pk_tx_delay_list;

///////////////////////////////////////////////////////////////////////////////
/**
 * Define PK_DELAY_JITTER data type.
 */
struct pk_delay_jitter : pk_delay { };

/**
 * Define LIST(PK_DELAY_JITTER) data type.
 */
typedef std::vector<pk_delay_jitter> pk_delay_jitter_list;

///////////////////////////////////////////////////////////////////////////////
/**
 * Define PK_LOSS_RATE data type.
 */
struct pk_loss_rate : pk_delay { };

/**
 * Define LIST(PK_LOSS_RATE) data type.
 */
typedef std::vector<pk_loss_rate> pk_loss_rate_list;

///////////////////////////////////////////////////////////////////////////////
/**
 * Define QOS_PARAM_VAL data type.
 */
typedef boost::variant<num_cos_types,
					   min_pk_tx_delay_list,
					   avg_pk_tx_delay_list,
					   max_pk_tx_delay_list,
					   pk_delay_jitter_list,
					   pk_loss_rate_list> qos_param_val;

///////////////////////////////////////////////////////////////////////////////
/**
 * Define QOS_LIST data type.
 */
struct qos_list {
	template<class ArchiveT>
	void serialize(ArchiveT& ar)
	{
		ar & cos;
		ar & min_pk_delay_lst;
		ar & avg_pk_tx_delay_lst;
		ar & max_pk_tx_delay_lst;
		ar & pk_delay_jitter_lst;
		ar & pk_loss_rate_lst;
	}

	num_cos_types        cos;
	min_pk_tx_delay_list min_pk_delay_lst;
	avg_pk_tx_delay_list avg_pk_tx_delay_lst;
	max_pk_tx_delay_list max_pk_tx_delay_lst;
	pk_delay_jitter_list pk_delay_jitter_lst;
	pk_loss_rate_list    pk_loss_rate_lst;
};

///////////////////////////////////////////////////////////////////////////////
} /* namespace mih */ } /*namespace odtone */

// EOF ////////////////////////////////////////////////////////////////////////
#endif /* ODTONE_MIH_TYPES_QOS__HPP_ */
