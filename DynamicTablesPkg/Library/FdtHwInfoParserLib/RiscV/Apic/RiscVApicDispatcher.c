/** @file
  RISC-V Interrupt Controller dispatcher.

  Copyright (c) 2021, ARM Limited. All rights reserved.<BR>
  Copyright (c) 2024, Ventana Micro Systems Inc. All rights reserved.<BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent

  @par Reference(s):
  - linux/Documentation/devicetree/bindings/interrupt-controller/arm,gic.yaml
  - linux/Documentation/devicetree/bindings/interrupt-controller/arm,gic-v3.yaml
**/

#include "FdtHwInfoParser.h"
#include "RiscV/Apic/RiscVPlicAplicParser.h"
#include "RiscV/Apic/RiscVRintcParser.h"
#include "RiscV/Apic/RiscVApicDispatcher.h"

/** MADT dispatcher.


  A parser parses a Device Tree to populate a specific CmObj type. None,
  one or many CmObj can be created by the parser.
  The created CmObj are then handed to the parser's caller through the
  HW_INFO_ADD_OBJECT interface.
  This can also be a dispatcher. I.e. a function that not parsing a
  Device Tree but calling other parsers.

  @param [in]  FdtParserHandle A handle to the parser instance.
  @param [in]  FdtBranch       When searching for DT node name, restrict
                               the search to this Device Tree branch.

  @retval EFI_SUCCESS             The function completed successfully.
  @retval EFI_ABORTED             An error occurred.
  @retval EFI_INVALID_PARAMETER   Invalid parameter.
  @retval EFI_NOT_FOUND           Not found.
  @retval EFI_UNSUPPORTED         Unsupported.
**/
EFI_STATUS
EFIAPI
RiscVApicDispatcher (
  IN  CONST FDT_HW_INFO_PARSER_HANDLE  FdtParserHandle,
  IN        INT32                      FdtBranch
  )
{
  EFI_STATUS  Status;
  VOID        *Fdt;

  if (FdtParserHandle == NULL) {
    ASSERT (0);
    return EFI_INVALID_PARAMETER;
  }

  Fdt = FdtParserHandle->Fdt;

  Status = RiscVRintcInfoParser (FdtParserHandle, -1);
  if (EFI_ERROR (Status)) {
    // EFI_NOT_FOUND is not tolerated at this point.
    ASSERT (0);
    return Status;
  }

  Status = RiscVAplicPlicInfoParser (FdtParserHandle, -1);
  if (EFI_ERROR (Status) && (Status != EFI_NOT_FOUND)) {
    ASSERT (0);
    return Status;
  }

  return EFI_SUCCESS;
}
