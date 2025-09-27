/*
 * Basic TR-069 CWMP Session Implementation Example
 * 
 * This file demonstrates how to implement basic TR-069 client sessions
 * using the existing CWMP client infrastructure.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

// Include CWMP client headers (relative to cwmpClient directory)
#include "../cwmp_rpc.h"
#include "../prmt_igd.h"
#include "../cwmpc_utility.h"

/*
 * Example 1: Create and send a basic Inform message
 */
int example_basic_inform(struct soap *soap, const char *acs_url)
{
    int type;
    void *data;
    unsigned int event_code = EC_PERIODIC; // Periodic inform
    char *opt = NULL;
    int result;

    printf("=== Basic Inform Example ===\n");

    // Create Inform message
    result = cwmp_CreateInform(soap, &type, &data, event_code, opt);
    if (result != 0) {
        printf("Error: Failed to create Inform message\n");
        return -1;
    }

    printf("Created Inform message with event code: PERIODIC\n");

    // Send the message to ACS
    result = cwmp_process_send(soap, acs_url, NULL, type, data);
    if (result != 0) {
        printf("Error: Failed to send Inform message\n");
        return -1;
    }

    printf("Inform message sent successfully\n");

    // Wait for and process response
    result = cwmp_process_recv(soap, &type, &data);
    if (result != 0) {
        printf("Error: Failed to receive response\n");
        return -1;
    }

    printf("Received InformResponse from ACS\n");
    return 0;
}

/*
 * Example 2: Handle a complete basic session
 */
int example_basic_session(const char *acs_url)
{
    struct soap soap;
    struct cwmp_userdata *user;
    int result = 0;

    printf("=== Basic Session Example ===\n");

    // Initialize SOAP context
    soap_init(&soap);
    soap_set_mode(&soap, SOAP_C_UTFSTRING);

    // Initialize CWMP user data
    user = cwmp_init_userdata();
    if (!user) {
        printf("Error: Failed to initialize user data\n");
        soap_cleanup(&soap);
        return -1;
    }

    // Set initial session parameters
    user->ID = 1001; // Session ID
    user->MaxEnvelopes = 1;
    user->EventCode = EC_PERIODIC;

    printf("Initialized session with ID: %u\n", user->ID);

    // Send basic inform
    result = example_basic_inform(&soap, acs_url);
    if (result != 0) {
        printf("Basic inform failed\n");
        goto cleanup;
    }

    printf("Basic session completed successfully\n");

cleanup:
    // Cleanup resources
    cwmp_free_userdata(user);
    soap_cleanup(&soap);
    return result;
}

/*
 * Example 3: Session with multiple events
 */
int example_multi_event_session(const char *acs_url)
{
    struct soap soap;
    int type;
    void *data;
    unsigned int events = EC_BOOT | EC_PERIODIC; // Multiple events
    int result;

    printf("=== Multi-Event Session Example ===\n");

    soap_init(&soap);
    soap_set_mode(&soap, SOAP_C_UTFSTRING);

    // Create Inform with multiple events
    result = cwmp_CreateInform(&soap, &type, &data, events, NULL);
    if (result == 0) {
        printf("Created Inform with BOOT and PERIODIC events\n");
        
        result = cwmp_process_send(&soap, acs_url, NULL, type, data);
        if (result == 0) {
            printf("Multi-event Inform sent successfully\n");
        }
    }

    soap_cleanup(&soap);
    return result;
}

/*
 * Example 4: Session timeout handling
 */
int example_session_with_timeout(const char *acs_url, int timeout_seconds)
{
    struct soap soap;
    time_t start_time, current_time;
    int result = 0;

    printf("=== Session with Timeout Example ===\n");

    soap_init(&soap);
    soap_set_mode(&soap, SOAP_C_UTFSTRING);
    
    // Set socket timeout
    soap.connect_timeout = timeout_seconds;
    soap.send_timeout = timeout_seconds;
    soap.recv_timeout = timeout_seconds;

    start_time = time(NULL);
    printf("Starting session with %d second timeout\n", timeout_seconds);

    // Attempt basic inform with timeout
    result = example_basic_inform(&soap, acs_url);
    
    current_time = time(NULL);
    printf("Session duration: %ld seconds\n", current_time - start_time);

    if (result != 0) {
        if (current_time - start_time >= timeout_seconds) {
            printf("Session timed out\n");
        } else {
            printf("Session failed for other reason\n");
        }
    }

    soap_cleanup(&soap);
    return result;
}

/*
 * Example usage and test function
 */
int main(int argc, char *argv[])
{
    const char *acs_url = "http://localhost:8080/acs"; // Default ACS URL
    
    if (argc > 1) {
        acs_url = argv[1];
    }

    printf("TR-069 Basic Session Examples\n");
    printf("ACS URL: %s\n\n", acs_url);

    // Run examples
    printf("1. Running basic inform example...\n");
    example_basic_inform(NULL, acs_url); // Note: In real usage, need proper soap context

    printf("\n2. Running basic session example...\n");
    example_basic_session(acs_url);

    printf("\n3. Running multi-event session example...\n");
    example_multi_event_session(acs_url);

    printf("\n4. Running session with timeout example...\n");
    example_session_with_timeout(acs_url, 30);

    printf("\nAll examples completed.\n");
    return 0;
}

/*
 * Compilation instructions:
 * 
 * cd /path/to/cwmpClient
 * gcc -I../include -I../gsoaplib -o basic_session_example \
 *     examples/01_basic_sessions/basic_session.c \
 *     cwmpc_utility.c prmt_igd.c main.c \
 *     -L../lib -lcwmp -lssl -lcrypto -lpthread
 */