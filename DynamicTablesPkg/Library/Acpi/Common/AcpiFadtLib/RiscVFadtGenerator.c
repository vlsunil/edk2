/** @file
  RISC-V FADT Table Helpers

  Copyright (c) 2024, Ventana Micro Systems Inc. All rights reserved.
  SPDX-License-Identifier: BSD-2-Clause-Patent

  @par Reference(s):
  - TBD - ACPI 6.6 Specification

**/

#include <Library/AcpiLib.h>
#include <Library/DebugLib.h>
#include <Protocol/AcpiTable.h>

// Module specific include files.
#include <AcpiTableGenerator.h>
#include <ConfigurationManagerObject.h>
#include <ConfigurationManagerHelper.h>
#include <Library/TableHelperLib.h>
#include <Protocol/ConfigurationManagerProtocol.h>
#include "FadtGenerator.h"

/** Updates the Architecture specific information in the FADT Table.

  @param [in]  CfgMgrProtocol Pointer to the Configuration Manager
                              Protocol Interface.
  @param [out] Fadt           Pointer to the FADT table

  @retval EFI_SUCCESS           Success.
  @retval EFI_NOT_FOUND         The required object was not found.
**/
EFI_STATUS
EFIAPI
FadtAddArchInfo (
  IN  CONST EDKII_CONFIGURATION_MANAGER_PROTOCOL  *CONST  CfgMgrProtocol,
  OUT EFI_ACPI_6_5_FIXED_ACPI_DESCRIPTION_TABLE           *Fadt
  )
{
  return EFI_SUCCESS;
}
