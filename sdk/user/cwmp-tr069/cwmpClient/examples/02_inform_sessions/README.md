# TR-069 Inform Session Examples

This directory contains examples of different types of Inform sessions based on various event codes.

## Files

- `01_bootstrap_inform.xml` - Bootstrap event (first connection)
- `02_boot_inform.xml` - Boot event (device restart)
- `03_periodic_inform.xml` - Periodic event (scheduled check-in)
- `04_value_change_inform.xml` - Value change event
- `05_connection_request_inform.xml` - Connection request event
- `06_transfer_complete_inform.xml` - File transfer completion
- `07_diagnostics_complete_inform.xml` - Diagnostic completion
- `08_vendor_specific_inform.xml` - Vendor-specific events
- `inform_examples.c` - C implementation of inform handling
- `test_inform.sh` - Shell script to test inform scenarios

## TR-069 Event Codes

| Code | Event | Description |
|------|-------|-------------|
| 0 | BOOTSTRAP | First time connection after factory reset |
| 1 | BOOT | Device boot/restart |
| 2 | PERIODIC | Scheduled periodic inform |
| 3 | SCHEDULED | Scheduled inform via ScheduleInform RPC |
| 4 | VALUE CHANGE | Parameter value changed |
| 5 | KICKED | Kicked by ACS via kick mechanism |
| 6 | CONNECTION REQUEST | ACS initiated connection |
| 7 | TRANSFER COMPLETE | Download/Upload completed |
| 8 | DIAGNOSTICS COMPLETE | Diagnostic test completed |
| M Reboot | REBOOT | Method-initiated reboot |
| M Download | DOWNLOAD | Method-initiated download |
| X CT-COM | VENDOR | Vendor-specific events |

## Key Parameters in Inform

- **DeviceId**: Manufacturer, OUI, ProductClass, SerialNumber
- **Event**: Array of event codes with command keys
- **MaxEnvelopes**: Max messages before waiting for response
- **CurrentTime**: Device's current time
- **RetryCount**: Number of retry attempts
- **ParameterList**: Device parameters to report