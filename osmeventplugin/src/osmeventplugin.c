/*
 * Copyright (c) 2013 Mellanox Technologies LTD. All rights reserved.
 * Copyright (c) 2008 Voltaire, Inc. All rights reserved.
 * Copyright (c) 2007 The Regents of the University of California.
 *
 * This software is available to you under a choice of one of two
 * licenses.  You may choose to be licensed under the terms of the GNU
 * General Public License (GPL) Version 2, available from the file
 * COPYING in the main directory of this source tree, or the
 * OpenIB.org BSD license below:
 *
 *     Redistribution and use in source and binary forms, with or
 *     without modification, are permitted provided that the following
 *     conditions are met:
 *
 *      - Redistributions of source code must retain the above
 *        copyright notice, this list of conditions and the following
 *        disclaimer.
 *
 *      - Redistributions in binary form must reproduce the above
 *        copyright notice, this list of conditions and the following
 *        disclaimer in the documentation and/or other materials
 *        provided with the distribution.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */

#if HAVE_CONFIG_H
#  include <config.h>
#endif				/* HAVE_CONFIG_H */

#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <dlfcn.h>
#include <stdint.h>
#include <opensm/osm_config.h>
#include <complib/cl_qmap.h>
#include <complib/cl_passivelock.h>
#include <opensm/osm_version.h>
#include <opensm/osm_opensm.h>
#include <opensm/osm_log.h>

/** =========================================================================
 */
typedef struct _plugin
{
	osm_opensm_t *osm;
	// other fields...
} plugin_t;

/** =========================================================================
 * Forward declarations
 */
static int plugin_build_lid_matrices(IN void *context);
static int plugin_ucast_build_fwd_tables(IN void *context);
static void plugin_ucast_dump_tables(IN void *context);
static void plugin_update_sl2vl(void *context, IN osm_physp_t *port, IN uint8_t in_port_num, IN uint8_t out_port_num, IN OUT ib_slvl_table_t *t);
static void plugin_update_vlarb(void *context, IN osm_physp_t *port, IN uint8_t port_num, IN OUT ib_vl_arb_table_t *block, unsigned block_length, unsigned block_num);
static uint8_t plugin_path_sl(IN void *context, IN uint8_t path_sl_hint, IN const ib_net16_t slid, IN const ib_net16_t dlid);
static ib_api_status_t plugin_mcast_build_stree(IN void *context, IN OUT osm_mgrp_box_t *mgb);
static void plugin_destroy_routine_engine(IN void *context);
int routine_engine_setup(osm_routing_engine_t *engine, osm_opensm_t *osm);

/** =========================================================================
 * Implement plugin functions
 */
static void *construct(osm_opensm_t *osm)
{		
	plugin_t * plugin = (plugin_t *) calloc(1, sizeof(plugin_t));
	plugin->osm = osm;

	routing_engine_module_t plugin_routine_engine_module = {
		 "routine_engine_plugin",
		  OSM_ROUTING_ENGINE_TYPE_UNKNOWN,
		  routine_engine_setup,
		  plugin,
	};


	osm_opensm_register_routing_engine(osm, &plugin_routine_engine_module, plugin);
	
	OSM_LOG(&plugin->osm->log, OSM_LOG_INFO, "plugin.construct => routine engine '%s' has been registered with type '%d'\n", 
		plugin_routine_engine_module.name, plugin_routine_engine_module.type);

	return ((void *)plugin);
}

static void report(void *context, osm_epi_event_id_t event_id, void *event_data)
{
	plugin_t *plugin = (plugin_t *) context;
	OSM_LOG(&plugin->osm->log, OSM_LOG_INFO, "plugin.report\n");
}

static void destroy(void *context)
{
	plugin_t *plugin = (plugin_t *) context;
	OSM_LOG(&plugin->osm->log, OSM_LOG_INFO, "plugin.destroy\n");
}

/** =========================================================================
 * Implement routine engine functions
 */

int routine_engine_setup(osm_routing_engine_t *engine, osm_opensm_t *osm)
{
	plugin_t *plugin = (plugin_t *) engine->context;
	OSM_LOG(&plugin->osm->log, OSM_LOG_INFO, "routine_engine_plugin.routine_engine_setup\n");

	engine->build_lid_matrices = plugin_build_lid_matrices;
	engine->ucast_build_fwd_tables = plugin_ucast_build_fwd_tables;
	engine->ucast_dump_tables = plugin_ucast_dump_tables;
	engine->update_sl2vl = plugin_update_sl2vl;
	engine->update_vlarb = plugin_update_vlarb;
	engine->path_sl = plugin_path_sl;
	engine->mcast_build_stree = plugin_mcast_build_stree;
	engine->destroy = plugin_destroy_routine_engine;

	return 0;
}

static int plugin_build_lid_matrices(IN void *context)
{
	plugin_t *plugin = (plugin_t *) context;
	OSM_LOG(&plugin->osm->log, OSM_LOG_ERROR, "routine_engine_plugin.build_lid_matrices\n");
	return 0;
}

static int plugin_ucast_build_fwd_tables(IN void *context)
{
	plugin_t *plugin = (plugin_t *) context;
	OSM_LOG(&plugin->osm->log, OSM_LOG_INFO, "routine_engine_plugin.ucast_build_fwd_tables\n");
	return 0;
}

static void plugin_ucast_dump_tables(IN void *context)
{
	plugin_t *plugin = (plugin_t *) context;
	OSM_LOG(&plugin->osm->log, OSM_LOG_INFO, "routine_engine_plugin.ucast_dump_tables\n");
}

static void plugin_update_sl2vl(void *context,
	IN osm_physp_t *port,
	IN uint8_t in_port_num, IN uint8_t out_port_num,
	IN OUT ib_slvl_table_t *t)
{
	plugin_t *plugin = (plugin_t *) context;
	OSM_LOG(&plugin->osm->log, OSM_LOG_INFO, "routine_engine_plugin.update_sl2vl\n");
}

static void plugin_update_vlarb(void *context,
	IN osm_physp_t *port, IN uint8_t port_num,
	IN OUT ib_vl_arb_table_t *block, unsigned block_length, unsigned block_num)
{
	plugin_t *plugin = (plugin_t *) context;
	OSM_LOG(&plugin->osm->log, OSM_LOG_INFO, "routine_engine_plugin.update_vlarb\n");
}

static uint8_t plugin_path_sl(IN void *context,
	IN uint8_t path_sl_hint, IN const ib_net16_t slid, IN const ib_net16_t dlid)
{
	plugin_t *plugin = (plugin_t *) context;
	OSM_LOG(&plugin->osm->log, OSM_LOG_INFO, "routine_engine_plugin.path_sl\n");
	return 0;
}

static ib_api_status_t plugin_mcast_build_stree(IN void *context, IN OUT osm_mgrp_box_t *mgb)
{
	plugin_t *plugin = (plugin_t *) context;
	OSM_LOG(&plugin->osm->log, OSM_LOG_INFO, "routine_engine_plugin.mcast_build_stree\n");
	return IB_SUCCESS;
}

static void plugin_destroy_routine_engine(IN void *context)
{
	plugin_t *plugin = (plugin_t *) context;
	OSM_LOG(&plugin->osm->log, OSM_LOG_INFO, "routine_engine_plugin.destroy_routine_engine\n");
}

/** =========================================================================
 * Define the object symbol for loading
 */

#if OSM_EVENT_PLUGIN_INTERFACE_VER != 2
#error OpenSM plugin interface version missmatch
#endif

osm_event_plugin_t osm_event_plugin = {
      OSM_VERSION,
      construct,
      destroy,
      report
};
