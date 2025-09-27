# Basic TR-069 CWMP Client Sessions

This directory contains basic examples of TR-069 client-to-ACS communication sessions.

## Files

- `01_empty_session.xml` - Minimal Inform/InformResponse session
- `02_basic_inform.xml` - Basic Inform with device info
- `03_session_termination.xml` - Proper session termination
- `basic_session.c` - C implementation of basic session handling
- `session_client.sh` - Shell script to simulate basic sessions

## Session Flow

1. **CPE sends Inform** - Initial message with event codes and device info
2. **ACS sends InformResponse** - Acknowledgment with MaxEnvelopes
3. **ACS may send RPC requests** - Parameter operations, diagnostics, etc.
4. **CPE responds to RPC** - Execute operations and send responses
5. **Session termination** - Either side can end with empty HTTP response

## Key Concepts

- **Session ID**: Unique identifier for the session in SOAP header
- **MaxEnvelopes**: Number of messages that can be sent before receiving response
- **HoldRequests**: ACS can pause CPE from initiating new sessions
- **NoMoreRequests**: Indicates last message in session