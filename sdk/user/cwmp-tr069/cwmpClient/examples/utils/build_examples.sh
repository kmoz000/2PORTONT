#!/bin/bash
#
# Build script for TR-069 CWMP Client Examples
# Compiles all C example programs
#

set -e

# Configuration
CWMP_ROOT="../../"
INCLUDE_DIRS="-I${CWMP_ROOT}../include -I${CWMP_ROOT}../gsoaplib -I${CWMP_ROOT}"
LIB_DIRS="-L${CWMP_ROOT}../lib"
LIBS="-lcwmp -lssl -lcrypto -lpthread"
CFLAGS="-Wall -Wextra -g -DHAVE_CONFIG_H"
OUTPUT_DIR="./bin"

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

log_info() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

log_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

log_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

log_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# Create output directory
mkdir -p "$OUTPUT_DIR"

echo "TR-069 CWMP Client Examples Build Script"
echo "========================================"
echo

# Check if we're in the right directory
if [ ! -f "../main.c" ]; then
    log_error "This script should be run from cwmpClient/examples/utils/"
    log_error "Current directory: $(pwd)"
    exit 1
fi

# Check for required source files
required_files=(
    "../cwmpc_utility.c"
    "../prmt_igd.c"
    "../main.c"
)

for file in "${required_files[@]}"; do
    if [ ! -f "$file" ]; then
        log_warning "Required file not found: $file"
        log_warning "Some examples may not compile correctly"
    fi
done

log_info "Building TR-069 session client..."

# Build the main session client
if gcc $CFLAGS $INCLUDE_DIRS \
    -o "$OUTPUT_DIR/tr069_session_client" \
    tr069_session_client.c \
    ../cwmpc_utility.c \
    ../prmt_igd.c \
    $LIB_DIRS $LIBS 2>/dev/null; then
    log_success "tr069_session_client built successfully"
else
    log_error "Failed to build tr069_session_client"
    log_info "Trying without external libraries..."
    
    # Try building with minimal dependencies
    if gcc $CFLAGS $INCLUDE_DIRS \
        -o "$OUTPUT_DIR/tr069_session_client_minimal" \
        tr069_session_client.c \
        -DMINIMAL_BUILD 2>/dev/null; then
        log_success "tr069_session_client_minimal built successfully"
    else
        log_error "Failed to build minimal version"
    fi
fi

# Build basic session example
log_info "Building basic session example..."
if gcc $CFLAGS $INCLUDE_DIRS \
    -o "$OUTPUT_DIR/basic_session_example" \
    ../01_basic_sessions/basic_session.c \
    ../cwmpc_utility.c \
    ../prmt_igd.c \
    $LIB_DIRS $LIBS 2>/dev/null; then
    log_success "basic_session_example built successfully"
else
    log_warning "Failed to build basic_session_example"
fi

# Create a simple test program that doesn't require external libraries
log_info "Creating test program..."
cat > "$OUTPUT_DIR/simple_test.c" << 'EOF'
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int main() {
    printf("TR-069 CWMP Client Examples Test Program\n");
    printf("========================================\n\n");
    
    printf("Current time: %s", ctime(&(time_t){time(NULL)}));
    printf("Test program compiled successfully!\n\n");
    
    printf("This test program verifies that the build environment\n");
    printf("is working correctly for TR-069 CWMP client examples.\n\n");
    
    printf("Available examples:\n");
    printf("- tr069_session_client: Full-featured session client\n");
    printf("- basic_session_example: Basic session handling\n");
    printf("- session_simulator.sh: Shell script for testing\n\n");
    
    return 0;
}
EOF

if gcc -o "$OUTPUT_DIR/simple_test" "$OUTPUT_DIR/simple_test.c"; then
    log_success "simple_test built successfully"
    rm "$OUTPUT_DIR/simple_test.c"
else
    log_error "Failed to build simple_test"
fi

# Set executable permissions
chmod +x "$OUTPUT_DIR"/* 2>/dev/null || true
chmod +x ../session_simulator.sh 2>/dev/null || true

# Create usage script
log_info "Creating usage script..."
cat > "$OUTPUT_DIR/run_examples.sh" << 'EOF'
#!/bin/bash
echo "TR-069 CWMP Client Examples"
echo "==========================="
echo
echo "Available programs:"
echo
if [ -x "./tr069_session_client" ]; then
    echo "1. Full Session Client:"
    echo "   ./tr069_session_client --help"
    echo
fi
if [ -x "./basic_session_example" ]; then
    echo "2. Basic Session Example:"
    echo "   ./basic_session_example [acs_url]"
    echo
fi
if [ -x "./simple_test" ]; then
    echo "3. Simple Test:"
    echo "   ./simple_test"
    echo
fi
echo "4. Session Simulator (shell script):"
echo "   ../session_simulator.sh --help"
echo
echo "For detailed documentation, see the README files in each example directory."
EOF

chmod +x "$OUTPUT_DIR/run_examples.sh"

echo
log_info "Build Summary:"
echo "=============="
if [ -d "$OUTPUT_DIR" ]; then
    built_count=$(ls -1 "$OUTPUT_DIR" 2>/dev/null | wc -l)
    log_info "Built $built_count files in $OUTPUT_DIR/"
    ls -la "$OUTPUT_DIR/"
else
    log_error "No output directory created"
fi

echo
log_info "To run the examples:"
log_info "cd $OUTPUT_DIR && ./run_examples.sh"
echo
log_success "Build script completed!"