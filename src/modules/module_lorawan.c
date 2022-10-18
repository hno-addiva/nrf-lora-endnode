/*
 * Class A LoRaWAN sample application
 *
 * Copyright (c) 2020 Manivannan Sadhasivam <mani@kernel.org>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#define MODULE lora_sensor

#include <zephyr/zephyr.h>
#include <zephyr/kernel.h>
#include <caf/events/module_state_event.h>
#include <zephyr/logging/log.h>
#include <zephyr/settings/settings.h>
#include <zephyr/device.h>
#include <zephyr/lorawan/lorawan.h>

LOG_MODULE_REGISTER(MODULE, LOG_LEVEL_DBG);

// Module state
enum {
	M_STATE_UNINITIALIZED = 0,
	M_STATE_READY
} module_state = {
	M_STATE_UNINITIALIZED
};

/* Customize based on network configuration */
static uint8_t dev_eui[] =		{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
static uint8_t join_eui[] = 	{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
static uint8_t app_key[] = 		{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
static uint32_t dev_nonce = 0;

/*
 * Work Queue
 */
static K_THREAD_STACK_DEFINE(stack_area, 2048);
static struct k_work_q work_q;

#define work_submit(name) \
	k_work_submit_to_queue(&work_q, &name##_work.work)
#define WORK_INIT(fn) \
	.work = Z_WORK_INITIALIZER(fn)

/*
 * send lora message
 */

struct send_context {
		struct k_work work;
		char data[16];
		uint8_t len;
};

static void send_task(struct k_work *work)
{
	struct send_context *ctx = CONTAINER_OF(work, struct send_context, work);
	LOG_HEXDUMP_DBG(ctx->data, ctx->len, "Message");

	// TODO: support unconfirmed messages, should be default.
	int ret = lorawan_send(2, ctx->data, ctx->len, LORAWAN_MSG_CONFIRMED);

	/*
	 * Warning: The stack may return -EAGAIN if the provided data
	 * length exceeds the maximum possible one for the region and
	 * datarate.
	 */
	if (ret < 0) {
		LOG_ERR("lorawan_send failed: %d", ret);
		return;
	}

	LOG_INF("Data sent!");
}

static struct send_context send_hello_work = {
	WORK_INIT(send_task),
	.data = "Hello World",
	.len = 11,
};

/*
 * Join LoRaWAN network
 */

struct join_context {
	struct k_work work;
	// The join parameters are global, noting unique here
};

static int is_empty(void *data, size_t size)
{
	uint8_t *p = data;
	for (int i = 0; i < size; i++)
		if (*p++)
			return 0;
	return 1;
}

static void join_task(struct k_work *work)
{
	struct join_context __unused *ctx = CONTAINER_OF(work, struct join_context, work);
	struct lorawan_join_config join_cfg;

	if (is_empty(dev_eui, sizeof(dev_eui))) {
		LOG_ERR("dev_eui blank");
		return;
	}

	if (is_empty(app_key, sizeof(app_key))) {
		LOG_ERR("app_key blank");
		return;
	}

	// TODO: Make these settings
	join_cfg.mode = LORAWAN_ACT_OTAA;
	join_cfg.dev_eui = dev_eui;
	join_cfg.otaa.join_eui = join_eui;
	join_cfg.otaa.app_key = app_key;
	join_cfg.otaa.nwk_key = app_key;
	join_cfg.otaa.dev_nonce = dev_nonce;

	LOG_INF("Joining network over OTAA");
	int ret = lorawan_join(&join_cfg);
	// TODO: Save dev_nonce from the LoRa layer somehow.
	if (ret < 0) {
		LOG_ERR("lorawan_join_network failed: %d", ret);
		return;
	}
}

static struct join_context send_join_work = {
	WORK_INIT(join_task),
};

/*
 * LoRaWAN callbacks
 */

static void dl_callback(uint8_t port, bool data_pending,
			int16_t rssi, int8_t snr,
			uint8_t len, const uint8_t *data)
{
	LOG_INF("Port %d, Pending %d, RSSI %ddB, SNR %ddBm", port, data_pending, rssi, snr);
	if (data) {
		LOG_HEXDUMP_INF(data, len, "Payload: ");
	}
}

static void lorwan_datarate_changed(enum lorawan_datarate dr)
{
	uint8_t unused, max_size;

	lorawan_get_payload_sizes(&unused, &max_size);
	LOG_INF("New Datarate: DR_%d, Max Payload %d", dr, max_size);
}

static void initialize_lora(void)
{
	const struct device *lora_dev;
	int ret;

	struct lorawan_downlink_cb downlink_cb = {
		.port = LW_RECV_PORT_ANY,
		.cb = dl_callback
	};

	lora_dev = DEVICE_DT_GET(DT_ALIAS(lora0));
	if (!device_is_ready(lora_dev)) {
		LOG_ERR("%s: device not ready.", lora_dev->name);
		return;
	}

	ret = lorawan_start();
	if (ret < 0) {
		LOG_ERR("lorawan_start failed: %d", ret);
		return;
	}

	lorawan_register_downlink_callback(&downlink_cb);
	lorawan_register_dr_changed_callback(lorwan_datarate_changed);

	work_submit(send_join);
}

/*
 * Settings
 */

// Wrapper function with common processing per setting
static int get_setting(const char *name, const char *key, void *data, int len, settings_read_cb read_cb, void *cb_arg)
{
	const char *next;
	if (settings_name_steq(name, key, &next) && !next) {
		(void)read_cb(cb_arg, &name, sizeof(name));
		LOG_HEXDUMP_INF(name, sizeof(name), "Setting " STRINGIFY(name));
		return 0;
	}
	return -1;
}

static int m_settings_set(const char *name, size_t len, settings_read_cb read_cb, void *cb_arg)
{
	#define SETTING(setting) (get_setting(name, STRINGIFY(setting), &setting, sizeof(setting), read_cb, cb_arg) == 0)
	if (SETTING(dev_eui))
		return 0;
	if (SETTING(join_eui))
		return 0;
	if (SETTING(app_key))
		return 0;
	if (SETTING(dev_nonce))
		return 0;
	return -1;
}

int m_settings_commit(void)
{
	// Warning: Initial commit is early during startup while main is initializing.
	if (module_state == M_STATE_UNINITIALIZED)
		return 0;

	LOG_INF("Settings committed");
	// Do what is needed to apply/activate new settings
	return -1;
}

// _get is for runtime settings. Not used (NULL)
// _export is for settings_save() of runtime settings. Not used (NULL)
SETTINGS_STATIC_HANDLER_DEFINE(MODULE, STRINGIFY(MODULE), NULL, m_settings_set, m_settings_commit, NULL);

/*
 * CAF Module
 */

static void module_lora_initialize(void)
{
	LOG_DBG("initializing");

	// Start our work queue, with a meaningful name
	#define K_WORK_MODULE_NAME \
		&(struct k_work_queue_config){STRINGIFY(MODULE)}
	k_work_queue_start(&work_q, stack_area, K_THREAD_STACK_SIZEOF(stack_area),
					CONFIG_SYSTEM_WORKQUEUE_PRIORITY, K_WORK_MODULE_NAME);
					 // TODO: Assign proper thread priority
	
	initialize_lora();

	module_set_state(MODULE_STATE_READY);
	LOG_DBG("initialized");
}

static bool handle_lora_module_state_event(const struct module_state_event *evt)
{
	// Wait for dependent modules to initialize
	if (check_state(evt, MODULE_ID(main), MODULE_STATE_READY)) {
		// Initialize this module
		module_lora_initialize();
	}
	return false;
}

static bool app_event_handler(const struct app_event_header *aeh)
{
	if (is_module_state_event(aeh)) {
		return handle_lora_module_state_event(cast_module_state_event(aeh));
	}

	/* Event not handled but subscribed. */
	__ASSERT_NO_MSG(false);

	return false;
}

APP_EVENT_LISTENER(MODULE, app_event_handler);
APP_EVENT_SUBSCRIBE(MODULE, module_state_event);

/*
 * Shell integration
 */

#if CONFIG_SHELL
#include <zephyr/shell/shell.h>

static int sh_lora_hello(const struct shell *shell, size_t argc, char **argv)
{
	work_submit(send_hello);
	return 0;
}

static int sh_lora_join(const struct shell *shell, size_t argc, char **argv)
{
	work_submit(send_join);
	return 0;
}

SHELL_STATIC_SUBCMD_SET_CREATE(module_shell,
	SHELL_CMD_ARG(hello, NULL, "Send Hello World", sh_lora_hello, 0, 0),
	SHELL_CMD_ARG(join, NULL, "Join LoRaWAN Network", sh_lora_join, 0, 0),
#if 0
	SHELL_CMD_ARG(dev_eui, NULL, "Get or set Dev EUI", sh_dev_eui, 0, 0),
	SHELL_CMD_ARG(join_eui, NULL, "Get or set Join EUI", sh_join_eui, 0, 0),
	SHELL_CMD_ARG(app_KEY, NULL, "Get or set App KEY", sh_app_key, 0, 0),
	SHELL_CMD_ARG(join, NULL, "(re)join LoRaWAN network.", sh_join_network, 0, 0),
#endif
	SHELL_SUBCMD_SET_END
);

SHELL_CMD_REGISTER(MODULE, &module_shell, "LoRaWAN commands", NULL);

#endif /* CONFIG_SHELL*/