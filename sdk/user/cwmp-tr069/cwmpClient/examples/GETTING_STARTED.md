# Getting Started with TR-069 CWMP Client Examples

This guide will help you get started with the TR-069 CWMP client session examples.

## Prerequisites

- C compiler (gcc recommended)
- OpenSSL development libraries
- CWMP client libraries (included in this project)
- Basic understanding of TR-069/CWMP protocol

## Quick Start

### 1. Build the Examples

```bash
cd examples/utils
./build_examples.sh
```

This will compile all example programs and create executable files in the `utils/bin/` directory.

### 2. Test Basic Connectivity

```bash
# Test if your ACS is reachable
./session_simulator.sh connectivity

# Run all simulation tests
./session_simulator.sh all
```

### 3. Run Individual Examples

```bash
cd utils/bin

# Run the comprehensive session client
./tr069_session_client --help
./tr069_session_client --session periodic --verbose

# Run basic session example
./basic_session_example http://your-acs-url:7547/cwmp
```

## Directory Structure

```
examples/
├── 01_basic_sessions/          # Basic client-ACS communication
├── 02_inform_sessions/         # Different inform event types
├── 03_rpc_methods/            # RPC method examples
├── 04_download_upload/        # File transfer examples
├── 05_diagnostics/            # Diagnostic operations
├── 06_advanced_scenarios/     # Complex session flows
├── 07_error_handling/         # Error and fault handling
├── 08_configuration/          # Configuration examples
└── utils/                     # Build scripts and tools
```

## Example Usage Scenarios

### Scenario 1: Basic Device Registration

1. **Bootstrap Inform**: Device connects for first time
   ```bash
   ./tr069_session_client --session bootstrap --url http://acs.example.com:7547/cwmp
   ```

2. **Review Bootstrap XML**: See `02_inform_sessions/01_bootstrap_inform.xml`

### Scenario 2: Parameter Management

1. **Get Parameters**: Read device configuration
   ```xml
   <!-- See 03_rpc_methods/03_GetParameterValues.xml -->
   ```

2. **Set Parameters**: Update device settings
   ```xml
   <!-- See 03_rpc_methods/04_SetParameterValues.xml -->
   ```

### Scenario 3: Firmware Update

1. **Download Request**: ACS initiates firmware download
2. **Transfer Progress**: Device reports download status
3. **Transfer Complete**: Device confirms installation

   See complete example in `04_download_upload/01_firmware_download.xml`

### Scenario 4: Diagnostics

1. **Ping Test**: Test network connectivity
2. **Speed Test**: Measure bandwidth
3. **Results Reporting**: Send diagnostic results to ACS

   See examples in `05_diagnostics/` directory

## Configuration

### Environment Variables

```bash
# ACS server configuration
export ACS_URL="http://your-acs-server:7547/cwmp"
export ACS_USERNAME="your-username"
export ACS_PASSWORD="your-password"

# Device configuration  
export CPE_IP="192.168.1.1"
export CPE_PORT="7547"

# Debugging
export VERBOSE=1
```

### Custom Configuration

Edit the configuration files in `08_configuration/` to match your deployment:

- `sample_configurations.xml`: Common configuration scenarios
- Device-specific parameters
- Network settings
- Security configurations

## Testing with Real ACS

### 1. Set up Test Environment

```bash
# Configure your ACS URL
export ACS_URL="http://your-acs:7547/cwmp"

# Test connectivity
curl -v "$ACS_URL"
```

### 2. Run Session Tests

```bash
# Basic inform session
./session_simulator.sh basic

# Value change notification
./session_simulator.sh valuechange

# RPC method testing
./session_simulator.sh rpc
```

### 3. Monitor Logs

```bash
# Check session logs
tail -f /tmp/tr069_logs/*.log

# Enable verbose output
VERBOSE=1 ./session_simulator.sh all
```

## Troubleshooting

### Common Issues

1. **Connection Refused**
   - Check ACS URL and port
   - Verify firewall settings
   - Confirm ACS is running

2. **Authentication Failed**
   - Verify username/password
   - Check certificate configuration
   - Review SSL/TLS settings

3. **SOAP Faults**
   - Review fault codes (9000-9999)
   - Check parameter names
   - Validate parameter values

4. **Compilation Errors**
   - Install development libraries
   - Check include paths
   - Verify library dependencies

### Debug Mode

Enable debug output for detailed troubleshooting:

```bash
# Enable verbose logging
export DEBUG=1
export VERBOSE=1

# Run with debugging
./tr069_session_client --verbose --session periodic
```

### Log Analysis

Check these log locations:
- `/tmp/tr069_logs/`: Session simulator logs
- `/tmp/cwmp.xml`: SOAP message dumps
- System logs: `/var/log/messages` or `/var/log/syslog`

## Best Practices

1. **Start Simple**: Begin with basic inform sessions
2. **Test Incrementally**: Add complexity gradually
3. **Monitor Logs**: Always check logs for issues
4. **Validate XML**: Ensure proper SOAP formatting
5. **Handle Errors**: Implement proper error handling
6. **Security**: Use HTTPS in production
7. **Performance**: Monitor session timing

## Next Steps

1. **Customize Examples**: Modify for your device model
2. **Integrate with Hardware**: Connect to actual device functions
3. **Add Custom RPCs**: Implement vendor-specific methods
4. **Performance Testing**: Test under load conditions
5. **Production Deployment**: Configure for live environment

## Resources

- TR-069 Amendment 6 Specification
- TR-106 Data Model Template
- TR-181 Device:2 Data Model
- CWMP Client Documentation
- ACS Vendor Documentation

## Support

For issues and questions:
1. Check the README files in each example directory
2. Review the XML examples for proper formatting
3. Test with the simulation scripts first
4. Consult the TR-069 specification for protocol details