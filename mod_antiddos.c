#include "anti_ddos_worker.cpp"

#include "httpd.h"
#include "http_config.h"
#include "http_log.h"
#include "http_request.h"

static void module_register_hooks(apr_pool_t *p)
{
    ap_hook_handler(AntiDDoSWorker::PreRequestHook, NULL, NULL, APR_HOOK_FIRST);
	ap_hook_log_transaction(AntiDDoSWorker::PostRequestHook , NULL, NULL, APR_HOOK_MIDDLE);
	//ap_register_output_filter("antiddos_output_filter", AntiDDoSWorker::PostContentHook, NULL, AP_FTYPE_RESOURCE);
    ap_hook_post_config(AntiDDoSWorker::PostConfigHook, NULL, NULL, APR_HOOK_MIDDLE);
}


static const command_rec        antiddos_directives[] =
{
    AP_INIT_TAKE1("AntiDDoSRedisConnectionType", (const char* (*)())Config::SetRedisConnectionType, NULL, RSRC_CONF, "Connection Type of the redis server (unix, tcp)"),
    AP_INIT_TAKE1("AntiDDoSRedisPath", (const char* (*)())Config::SetRedisPath, NULL, RSRC_CONF, "Path of the redis server"),
    AP_INIT_TAKE1("AntiDDoSRedisTimeout", (const char* (*)())Config::SetRedisTimeout, NULL, RSRC_CONF, "Timeout for reply from the redis server (in ms)"),
    AP_INIT_TAKE1("AntiDDoSRedisPort", (const char* (*)())Config::SetRedisPort, NULL, RSRC_CONF, "Path of the redis server"),
    AP_INIT_TAKE1("AntiDDoSConfig", (const char* (*)())Config::SetConfigHook, NULL, RSRC_CONF, "Path of the anti-ddos configuration"),
    AP_INIT_TAKE1("AntiDDoSConfigLocal", (const char* (*)())Config::SetConfigLocalHook, NULL, RSRC_CONF, "Path of the local anti-ddos configuration (overwrite)"),
    AP_INIT_TAKE1("AntiDDoSBlockCommand", (const char* (*)())Config::SetBlockCommandHook, NULL, RSRC_CONF, "Command to execute when ip is blocked"),
    { NULL }
};

/* Dispatch list for API hooks */
module AP_MODULE_DECLARE_DATA antiddos_module = {
    STANDARD20_MODULE_STUFF, 
    NULL,                  /* create per-dir    config structures */
    NULL,                  /* merge  per-dir    config structures */
    NULL,                  /* create per-server config structures */
    NULL,                  /* merge  per-server config structures */
    antiddos_directives,                  /* table of config file commands       */
    module_register_hooks  /* register hooks                      */
};