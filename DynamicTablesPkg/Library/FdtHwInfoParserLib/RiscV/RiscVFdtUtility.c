/** @file
  RISC-V Flattened device tree utility.

  Copyright (c) 2024, Ventana Micro Systems Inc. All rights reserved.<BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent

  @par Reference(s):
  - Device tree Specification - Release v0.3
**/

#include <FdtHwInfoParserInclude.h>
#include "FdtUtility.h"

/** Get the interrupt Id of an interrupt described in a fdt.

  @param [in]  Data   Pointer to the first cell of an "interrupts" property.

  @retval  The interrupt id.
**/
UINT32
EFIAPI
FdtGetInterruptId (
  UINT32 CONST  *Data
  )
{
  ASSERT (Data != NULL);

  return fdt32_to_cpu (Data[RISCV_IRQ_NUMBER_OFFSET]);
}

/** Get the ACPI interrupt flags of an interrupt described in a fdt.


  @param [in]  Data   Pointer to the first cell of an "interrupts" property.

  @retval  The interrupt flags (for ACPI).
**/
UINT32
EFIAPI
FdtGetInterruptFlags (
  UINT32 CONST  *Data
  )
{
  UINT32  IrqFlags;
  UINT32  AcpiIrqFlags;

  ASSERT (Data != NULL);

  IrqFlags = fdt32_to_cpu (Data[RISCV_IRQ_FLAGS_OFFSET]);

  AcpiIrqFlags  = DT_IRQ_IS_EDGE_TRIGGERED (IrqFlags) ? BIT0 : 0;
  AcpiIrqFlags |= DT_IRQ_IS_ACTIVE_LOW (IrqFlags) ? BIT1 : 0;

  return AcpiIrqFlags;
}
