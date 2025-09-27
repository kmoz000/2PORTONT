# TR-069 RPC Method Examples

This directory contains examples of all major TR-069 RPC methods with request/response pairs.

## Files

- `01_GetRPCMethods.xml` - Discover supported RPC methods
- `02_GetParameterNames.xml` - Get parameter tree structure
- `03_GetParameterValues.xml` - Read parameter values
- `04_SetParameterValues.xml` - Write parameter values
- `05_GetParameterAttributes.xml` - Get parameter attributes
- `06_SetParameterAttributes.xml` - Set parameter attributes
- `07_AddObject.xml` - Create new object instances
- `08_DeleteObject.xml` - Remove object instances
- `09_Reboot.xml` - Restart the device
- `10_Download.xml` - Download firmware/config files
- `11_Upload.xml` - Upload device files to server
- `12_FactoryReset.xml` - Reset to factory defaults
- `rpc_client.c` - C implementation of RPC handling
- `test_rpc.sh` - Shell script to test RPC methods

## TR-069 RPC Methods

### Core Methods (Required)
- **GetRPCMethods**: Query supported RPC methods
- **GetParameterNames**: Get parameter hierarchy
- **GetParameterValues**: Read parameter values
- **SetParameterValues**: Write parameter values
- **GetParameterAttributes**: Get parameter metadata
- **SetParameterAttributes**: Set parameter metadata
- **AddObject**: Create object instances
- **DeleteObject**: Remove object instances
- **Reboot**: Restart device

### Optional Methods
- **Download**: Download files (firmware, config)
- **Upload**: Upload files to server
- **FactoryReset**: Reset to factory defaults
- **GetQueuedTransfers**: List pending transfers
- **ScheduleInform**: Schedule future inform
- **SetVouchers**: Security credential management
- **GetOptions**: Get DHCP options

### Diagnostic Methods
- **GetAllQueuedTransfers**: List all transfers
- **ScheduleDownload**: Schedule future download
- **CancelTransfer**: Cancel pending transfer

## Parameter Types

- **xsd:string**: Text values
- **xsd:int**: 32-bit signed integers
- **xsd:unsignedInt**: 32-bit unsigned integers
- **xsd:boolean**: True/false values
- **xsd:dateTime**: ISO 8601 timestamps
- **xsd:base64**: Binary data (base64 encoded)

## Fault Codes

- **9000**: Method not supported
- **9001**: Request denied
- **9002**: Internal error
- **9003**: Invalid arguments
- **9004**: Resources exceeded
- **9005**: Invalid parameter name
- **9006**: Invalid parameter type
- **9007**: Invalid parameter value
- **9008**: Attempt to set non-writable parameter
- **9009**: Notification request rejected