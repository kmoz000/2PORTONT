/*
 * TR-069 CWMP Session Client Example
 * 
 * Comprehensive example demonstrating how to create a TR-069 client
 * that can handle various session types and RPC methods.
 * 
 * This example integrates with the existing CWMP client infrastructure
 * and provides a reference implementation for custom TR-069 clients.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <getopt.h>
#include <signal.h>

// CWMP Client includes (adjust paths as needed)
#include "../../cwmp_rpc.h" 
#include "../../prmt_igd.h"
#include "../../cwmpc_utility.h"

#define MAX_URL_LENGTH 256
#define MAX_BUFFER_SIZE 4096
#define DEFAULT_ACS_URL "http://localhost:7547/acs"
#define DEFAULT_TIMEOUT 30

// Global variables
static volatile int g_running = 1;
static int g_verbose = 0;

// Session types
typedef enum {
    SESSION_BOOTSTRAP = 0,
    SESSION_BOOT,
    SESSION_PERIODIC,
    SESSION_VALUE_CHANGE,
    SESSION_CONNECTION_REQUEST,
    SESSION_TRANSFER_COMPLETE
} session_type_t;

// Session configuration
typedef struct {
    char acs_url[MAX_URL_LENGTH];
    char username[64];
    char password[64];
    int timeout;
    int max_envelopes;
    session_type_t session_type;
    unsigned int event_code;
    char command_key[64];
} session_config_t;

// Function prototypes
static void print_usage(const char *program_name);
static void signal_handler(int signum);
static int init_session_config(session_config_t *config);
static int create_soap_context(struct soap **soap);
static void cleanup_soap_context(struct soap *soap);
static int execute_inform_session(session_config_t *config);
static int execute_rpc_session(session_config_t *config, const char *rpc_method);
static int handle_inform_response(struct soap *soap, session_config_t *config);
static void log_message(const char *level, const char *format, ...);

/*
 * Signal handler for graceful shutdown
 */
static void signal_handler(int signum) {
    log_message("INFO", "Received signal %d, shutting down gracefully", signum);
    g_running = 0;
}

/*
 * Logging function
 */
static void log_message(const char *level, const char *format, ...) {
    va_list args;
    time_t now;
    char timestamp[64];
    
    if (!g_verbose && strcmp(level, "DEBUG") == 0) {
        return;
    }
    
    time(&now);
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", localtime(&now));
    
    printf("[%s] %s: ", timestamp, level);
    
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
    
    printf("\n");
    fflush(stdout);
}

/*
 * Initialize session configuration with defaults
 */
static int init_session_config(session_config_t *config) {
    if (!config) {
        return -1;
    }
    
    memset(config, 0, sizeof(session_config_t));
    
    strncpy(config->acs_url, DEFAULT_ACS_URL, sizeof(config->acs_url) - 1);
    config->timeout = DEFAULT_TIMEOUT;
    config->max_envelopes = 1;
    config->session_type = SESSION_PERIODIC;
    config->event_code = EC_PERIODIC;
    
    return 0;
}

/*
 * Create and initialize SOAP context
 */
static int create_soap_context(struct soap **soap) {
    if (!soap) {
        return -1;
    }
    
    *soap = (struct soap *)malloc(sizeof(struct soap));
    if (!*soap) {
        log_message("ERROR", "Failed to allocate SOAP context");
        return -1;
    }
    
    soap_init(*soap);
    soap_set_mode(*soap, SOAP_C_UTFSTRING);
    
    // Set timeouts
    (*soap)->connect_timeout = DEFAULT_TIMEOUT;
    (*soap)->send_timeout = DEFAULT_TIMEOUT;
    (*soap)->recv_timeout = DEFAULT_TIMEOUT;
    
    log_message("DEBUG", "SOAP context created and initialized");
    return 0;
}

/*
 * Cleanup SOAP context
 */
static void cleanup_soap_context(struct soap *soap) {
    if (soap) {
        soap_cleanup(soap);
        free(soap);
        log_message("DEBUG", "SOAP context cleaned up");
    }
}

/*
 * Execute an Inform session
 */
static int execute_inform_session(session_config_t *config) {
    struct soap *soap = NULL;
    struct cwmp_userdata *user = NULL;
    int type;
    void *data;
    int result = -1;
    
    log_message("INFO", "Starting Inform session to %s", config->acs_url);
    
    // Create SOAP context
    if (create_soap_context(&soap) != 0) {
        log_message("ERROR", "Failed to create SOAP context");
        return -1;
    }
    
    // Initialize user data
    user = cwmp_init_userdata();
    if (!user) {
        log_message("ERROR", "Failed to initialize user data");
        goto cleanup;
    }
    
    // Configure user data
    user->ID = (unsigned int)(time(NULL) % 100000);
    user->MaxEnvelopes = config->max_envelopes;
    user->EventCode = config->event_code;
    
    log_message("INFO", "Session ID: %u, Event Code: 0x%08X", user->ID, user->EventCode);
    
    // Create Inform message
    result = cwmp_CreateInform(soap, &type, &data, config->event_code, config->command_key);
    if (result != 0) {
        log_message("ERROR", "Failed to create Inform message");
        goto cleanup;
    }
    
    log_message("DEBUG", "Inform message created successfully");
    
    // Send Inform message
    result = cwmp_process_send(soap, config->acs_url, NULL, type, data);
    if (result != 0) {
        log_message("ERROR", "Failed to send Inform message");
        goto cleanup;
    }
    
    log_message("INFO", "Inform message sent successfully");
    
    // Receive and process response
    result = cwmp_process_recv(soap, &type, &data);
    if (result != 0) {
        log_message("ERROR", "Failed to receive response");
        goto cleanup;
    }
    
    log_message("INFO", "Received response from ACS");
    
    // Handle the response
    result = handle_inform_response(soap, config);
    if (result == 0) {
        log_message("INFO", "Inform session completed successfully");
    } else {
        log_message("ERROR", "Inform session failed");
    }
    
cleanup:
    if (user) {
        cwmp_free_userdata(user);
    }
    cleanup_soap_context(soap);
    
    return result;
}

/*
 * Handle Inform response and any follow-up RPC calls
 */
static int handle_inform_response(struct soap *soap, session_config_t *config) {
    // This is a simplified response handler
    // In a real implementation, you would parse the response
    // and handle any RPC method calls from the ACS
    
    log_message("DEBUG", "Processing Inform response");
    
    // Check for any pending RPC calls from ACS
    // This would involve parsing the SOAP response and
    // dispatching to appropriate handlers
    
    return 0;
}

/*
 * Execute RPC method session
 */
static int execute_rpc_session(session_config_t *config, const char *rpc_method) {
    log_message("INFO", "Executing RPC method: %s", rpc_method);
    
    if (strcmp(rpc_method, "GetRPCMethods") == 0) {
        // Implement GetRPCMethods
        struct soap *soap = NULL;
        int type;
        void *data;
        int result;
        
        if (create_soap_context(&soap) != 0) {
            return -1;
        }
        
        result = cwmp_CreateGetRPCMethods(soap, &type, &data);
        if (result == 0) {
            result = cwmp_process_send(soap, config->acs_url, NULL, type, data);
            if (result == 0) {
                result = cwmp_process_recv(soap, &type, &data);
                log_message("INFO", "GetRPCMethods completed");
            }
        }
        
        cleanup_soap_context(soap);
        return result;
    }
    
    // Add more RPC method implementations here
    log_message("WARNING", "RPC method %s not implemented", rpc_method);
    return -1;
}

/*
 * Print usage information
 */
static void print_usage(const char *program_name) {
    printf("Usage: %s [OPTIONS]\n", program_name);
    printf("\n");
    printf("TR-069 CWMP Session Client Example\n");
    printf("\n");
    printf("Options:\n");
    printf("  -u, --url URL         ACS URL (default: %s)\n", DEFAULT_ACS_URL);
    printf("  -U, --username USER   Username for authentication\n");
    printf("  -P, --password PASS   Password for authentication\n");
    printf("  -t, --timeout SEC     Connection timeout in seconds (default: %d)\n", DEFAULT_TIMEOUT);
    printf("  -s, --session TYPE    Session type (bootstrap|boot|periodic|valuechange)\n");
    printf("  -e, --event CODE      Event code (hex, e.g., 0x04)\n");
    printf("  -k, --key KEY         Command key\n");
    printf("  -r, --rpc METHOD      Execute RPC method\n");
    printf("  -v, --verbose         Enable verbose output\n");
    printf("  -h, --help            Show this help message\n");
    printf("\n");
    printf("Examples:\n");
    printf("  %s --session periodic --url http://acs.example.com:7547/cwmp\n", program_name);
    printf("  %s --rpc GetRPCMethods --verbose\n", program_name);
    printf("  %s --session valuechange --event 0x10 --key \"Config_001\"\n", program_name);
    printf("\n");
}

/*
 * Main function
 */
int main(int argc, char *argv[]) {
    session_config_t config;
    const char *rpc_method = NULL;
    int result = 0;
    
    // Set up signal handlers
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);
    
    // Initialize configuration
    if (init_session_config(&config) != 0) {
        log_message("ERROR", "Failed to initialize configuration");
        return 1;
    }
    
    // Parse command line options
    static struct option long_options[] = {
        {"url", required_argument, 0, 'u'},
        {"username", required_argument, 0, 'U'},
        {"password", required_argument, 0, 'P'},
        {"timeout", required_argument, 0, 't'},
        {"session", required_argument, 0, 's'},
        {"event", required_argument, 0, 'e'},
        {"key", required_argument, 0, 'k'},
        {"rpc", required_argument, 0, 'r'},
        {"verbose", no_argument, 0, 'v'},
        {"help", no_argument, 0, 'h'},
        {0, 0, 0, 0}
    };
    
    int c;
    while ((c = getopt_long(argc, argv, "u:U:P:t:s:e:k:r:vh", long_options, NULL)) != -1) {
        switch (c) {
            case 'u':
                strncpy(config.acs_url, optarg, sizeof(config.acs_url) - 1);
                break;
            case 'U':
                strncpy(config.username, optarg, sizeof(config.username) - 1);
                break;
            case 'P':
                strncpy(config.password, optarg, sizeof(config.password) - 1);
                break;
            case 't':
                config.timeout = atoi(optarg);
                break;
            case 's':
                if (strcmp(optarg, "bootstrap") == 0) {
                    config.session_type = SESSION_BOOTSTRAP;
                    config.event_code = EC_BOOTSTRAP;
                } else if (strcmp(optarg, "boot") == 0) {
                    config.session_type = SESSION_BOOT;
                    config.event_code = EC_BOOT;
                } else if (strcmp(optarg, "periodic") == 0) {
                    config.session_type = SESSION_PERIODIC;
                    config.event_code = EC_PERIODIC;
                } else if (strcmp(optarg, "valuechange") == 0) {
                    config.session_type = SESSION_VALUE_CHANGE;
                    config.event_code = EC_VALUECHANGE;
                } else {
                    log_message("ERROR", "Unknown session type: %s", optarg);
                    return 1;
                }
                break;
            case 'e':
                config.event_code = (unsigned int)strtoul(optarg, NULL, 0);
                break;
            case 'k':
                strncpy(config.command_key, optarg, sizeof(config.command_key) - 1);
                break;
            case 'r':
                rpc_method = optarg;
                break;
            case 'v':
                g_verbose = 1;
                break;
            case 'h':
                print_usage(argv[0]);
                return 0;
            default:
                print_usage(argv[0]);
                return 1;
        }
    }
    
    log_message("INFO", "TR-069 CWMP Session Client starting");
    log_message("INFO", "ACS URL: %s", config.acs_url);
    
    // Execute the requested operation
    if (rpc_method) {
        result = execute_rpc_session(&config, rpc_method);
    } else {
        result = execute_inform_session(&config);
    }
    
    if (result == 0) {
        log_message("INFO", "Session completed successfully");
    } else {
        log_message("ERROR", "Session failed with code: %d", result);
    }
    
    log_message("INFO", "TR-069 CWMP Session Client shutting down");
    return result;
}

/*
 * Compilation instructions:
 * 
 * cd /path/to/cwmpClient
 * gcc -I../include -I../gsoaplib -DHAVE_CONFIG_H \
 *     -o tr069_session_client examples/utils/tr069_session_client.c \
 *     cwmpc_utility.c prmt_igd.c \
 *     -L../lib -lcwmp -lssl -lcrypto -lpthread
 */