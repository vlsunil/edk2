#include <Library/DebugLib.h>
#include <Library/PL011UartLib.h>
#include <ConfigurationManagerObject.h>
#include <ConfigurationManagerHelper.h>
#include <Protocol/ConfigurationManagerProtocol.h>

/** Initialize the PL011/SBSA UART with the parameters obtained from
    the Configuration Manager.

  @param [in]  SerialPortInfo Pointer to the Serial Port Information.

  @retval EFI_SUCCESS           Success.
  @retval EFI_INVALID_PARAMETER The parameters for serial port initialization
                                are invalid.
**/
EFI_STATUS
SetupDebugUart (
  IN  CONST CM_STD_SERIAL_PORT_INFO  *CONST  SerialPortInfo
  )
{
  EFI_STATUS          Status;
  UINT64              BaudRate;
  UINT32              ReceiveFifoDepth;
  EFI_PARITY_TYPE     Parity;
  UINT8               DataBits;
  EFI_STOP_BITS_TYPE  StopBits;

  ASSERT (SerialPortInfo != NULL);

  // Initialize the Serial Debug UART
  DEBUG ((DEBUG_INFO, "Initializing Serial Debug UART...\n"));
  ReceiveFifoDepth = 0; // Use the default value for FIFO depth
  Parity           = (EFI_PARITY_TYPE)FixedPcdGet8 (PcdUartDefaultParity);
  DataBits         = FixedPcdGet8 (PcdUartDefaultDataBits);
  StopBits         = (EFI_STOP_BITS_TYPE)FixedPcdGet8 (PcdUartDefaultStopBits);

  BaudRate = SerialPortInfo->BaudRate;
  Status   = PL011UartInitializePort (
               (UINTN)SerialPortInfo->BaseAddress,
               SerialPortInfo->Clock,
               &BaudRate,
               &ReceiveFifoDepth,
               &Parity,
               &DataBits,
               &StopBits
               );

  ASSERT_EFI_ERROR (Status);
  return Status;
}

