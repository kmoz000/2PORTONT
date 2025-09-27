#!/bin/bash
#
# TR-069 CWMP Session Simulator
# Simulates various TR-069 client sessions for testing
#

set -e

# Configuration
ACS_URL="${ACS_URL:-http://localhost:7547/acs}"
CPE_IP="${CPE_IP:-192.168.1.1}"
CPE_PORT="${CPE_PORT:-7547}"
LOG_DIR="/tmp/tr069_logs"
VERBOSE="${VERBOSE:-1}"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Logging functions
log_info() {
    if [ "$VERBOSE" -eq 1 ]; then
        echo -e "${BLUE}[INFO]${NC} $1" >&2
    fi
}

log_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1" >&2
}

log_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1" >&2
}

log_error() {
    echo -e "${RED}[ERROR]${NC} $1" >&2
}

# Create log directory
setup_logging() {
    mkdir -p "$LOG_DIR"
    log_info "Log directory: $LOG_DIR"
}

# Generate session ID
generate_session_id() {
    echo $((RANDOM + 10000))
}

# Send SOAP request
send_soap_request() {
    local xml_file="$1"
    local session_id="$2"
    local output_file="$3"
    
    if [ ! -f "$xml_file" ]; then
        log_error "XML file not found: $xml_file"
        return 1
    fi
    
    log_info "Sending SOAP request: $(basename "$xml_file")"
    
    curl -s \
        -H "Content-Type: text/xml; charset=utf-8" \
        -H "SOAPAction: " \
        -d "@$xml_file" \
        "$ACS_URL" \
        > "$output_file" 2>&1
        
    local curl_exit_code=$?
    
    if [ $curl_exit_code -eq 0 ]; then
        log_success "Request sent successfully"
        return 0
    else
        log_error "Request failed with exit code: $curl_exit_code"
        return 1
    fi
}

# Extract SOAP response details
parse_soap_response() {
    local response_file="$1"
    
    if [ ! -f "$response_file" ]; then
        log_error "Response file not found: $response_file"
        return 1
    fi
    
    # Check for SOAP fault
    if grep -q "soap:Fault" "$response_file"; then
        log_error "SOAP Fault detected:"
        grep -A 5 "FaultString" "$response_file" | sed 's/<[^>]*>//g' | tr -d '\n\t '
        echo
        return 1
    fi
    
    # Extract session ID
    local session_id=$(grep -o 'cwmp:ID[^>]*>[^<]*' "$response_file" | sed 's/.*>//')
    if [ -n "$session_id" ]; then
        log_info "Session ID: $session_id"
    fi
    
    return 0
}

# Simulate basic inform session
simulate_basic_inform() {
    log_info "=== Simulating Basic Inform Session ==="
    
    local session_id=$(generate_session_id)
    local examples_dir="../01_basic_sessions"
    local log_prefix="$LOG_DIR/basic_inform_$$"
    
    # Create temporary XML with session ID
    local temp_xml="$log_prefix.xml"
    sed "s/<cwmp:ID[^>]*>[^<]*</<cwmp:ID soap:mustUnderstand=\"1\">$session_id</g" \
        "$examples_dir/02_basic_inform.xml" > "$temp_xml"
    
    # Send inform
    if send_soap_request "$temp_xml" "$session_id" "$log_prefix.response"; then
        if parse_soap_response "$log_prefix.response"; then
            log_success "Basic inform session completed"
            return 0
        fi
    fi
    
    log_error "Basic inform session failed"
    return 1
}

# Simulate value change inform
simulate_value_change_inform() {
    log_info "=== Simulating Value Change Inform Session ==="
    
    local session_id=$(generate_session_id)
    local examples_dir="../02_inform_sessions"
    local log_prefix="$LOG_DIR/value_change_$$"
    
    # Create temporary XML with session ID
    local temp_xml="$log_prefix.xml"
    sed "s/<cwmp:ID[^>]*>[^<]*</<cwmp:ID soap:mustUnderstand=\"1\">$session_id</g" \
        "$examples_dir/04_value_change_inform.xml" > "$temp_xml"
    
    # Send value change inform
    if send_soap_request "$temp_xml" "$session_id" "$log_prefix.response"; then
        if parse_soap_response "$log_prefix.response"; then
            log_success "Value change inform session completed"
            return 0
        fi
    fi
    
    log_error "Value change inform session failed"
    return 1
}

# Simulate RPC method calls
simulate_rpc_methods() {
    log_info "=== Simulating RPC Method Calls ==="
    
    local session_id=$(generate_session_id)
    local examples_dir="../03_rpc_methods"
    local log_prefix="$LOG_DIR/rpc_methods_$$"
    
    # Test GetParameterValues
    log_info "Testing GetParameterValues..."
    local temp_xml="$log_prefix.get_params.xml"
    sed "s/<cwmp:ID[^>]*>[^<]*</<cwmp:ID soap:mustUnderstand=\"1\">$session_id</g" \
        "$examples_dir/03_GetParameterValues.xml" > "$temp_xml"
    
    if send_soap_request "$temp_xml" "$session_id" "$log_prefix.get_params.response"; then
        if parse_soap_response "$log_prefix.get_params.response"; then
            log_success "GetParameterValues test passed"
        else
            log_warning "GetParameterValues test had issues"
        fi
    else
        log_error "GetParameterValues test failed"
    fi
    
    # Test SetParameterValues
    log_info "Testing SetParameterValues..."
    session_id=$((session_id + 1))
    temp_xml="$log_prefix.set_params.xml"
    sed "s/<cwmp:ID[^>]*>[^<]*</<cwmp:ID soap:mustUnderstand=\"1\">$session_id</g" \
        "$examples_dir/04_SetParameterValues.xml" > "$temp_xml"
    
    if send_soap_request "$temp_xml" "$session_id" "$log_prefix.set_params.response"; then
        if parse_soap_response "$log_prefix.set_params.response"; then
            log_success "SetParameterValues test passed"
        else
            log_warning "SetParameterValues test had issues"
        fi
    else
        log_error "SetParameterValues test failed"
    fi
}

# Test ACS connectivity
test_acs_connectivity() {
    log_info "=== Testing ACS Connectivity ==="
    
    if curl -s --connect-timeout 5 "$ACS_URL" > /dev/null 2>&1; then
        log_success "ACS is reachable at $ACS_URL"
        return 0
    else
        log_error "ACS is not reachable at $ACS_URL"
        log_info "Make sure your ACS is running and accessible"
        return 1
    fi
}

# Main function
main() {
    local operation="${1:-all}"
    
    echo "TR-069 CWMP Session Simulator"
    echo "============================="
    echo "ACS URL: $ACS_URL"
    echo "CPE IP: $CPE_IP:$CPE_PORT"
    echo
    
    setup_logging
    
    case "$operation" in
        "connectivity")
            test_acs_connectivity
            ;;
        "basic")
            test_acs_connectivity && simulate_basic_inform
            ;;
        "valuechange")
            test_acs_connectivity && simulate_value_change_inform
            ;;
        "rpc")
            test_acs_connectivity && simulate_rpc_methods
            ;;
        "all"|*)
            if test_acs_connectivity; then
                simulate_basic_inform
                sleep 2
                simulate_value_change_inform
                sleep 2
                simulate_rpc_methods
                log_success "All simulation tests completed"
            else
                log_error "ACS connectivity test failed, skipping other tests"
                exit 1
            fi
            ;;
    esac
}

# Usage information
usage() {
    echo "Usage: $0 [operation]"
    echo
    echo "Operations:"
    echo "  connectivity  - Test ACS connectivity only"
    echo "  basic        - Simulate basic inform session"
    echo "  valuechange  - Simulate value change inform"
    echo "  rpc          - Simulate RPC method calls"
    echo "  all          - Run all simulations (default)"
    echo
    echo "Environment variables:"
    echo "  ACS_URL      - ACS server URL (default: http://localhost:7547/acs)"
    echo "  CPE_IP       - CPE IP address (default: 192.168.1.1)"
    echo "  CPE_PORT     - CPE port (default: 7547)"
    echo "  VERBOSE      - Enable verbose output (default: 1)"
    echo
    echo "Examples:"
    echo "  $0                          # Run all tests"
    echo "  $0 basic                    # Run basic inform only"
    echo "  ACS_URL=http://acs.test.com:8080/cwmp $0 all"
}

# Check for help
if [ "$1" = "-h" ] || [ "$1" = "--help" ]; then
    usage
    exit 0
fi

# Run main function
main "$@"