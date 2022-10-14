/*
 * Copyright (c) 2022 Addiva Elektronik AB
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#define MODULE main

#include <zephyr/zephyr.h>
#include <app_event_manager.h>
#include <caf/events/module_state_event.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(MODULE);

void main(void)
{
	LOG_INF("Starting");

	// Start CAF and signal that main have completed the system setup
	if (app_event_manager_init()) {
		LOG_ERR("Application Event Manager not initialized");
	} else {
		module_set_state(MODULE_STATE_READY);
	}

	LOG_INF("Done");
}
