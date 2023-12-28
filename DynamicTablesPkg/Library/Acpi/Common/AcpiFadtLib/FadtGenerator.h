/** @file

  Copyright (c) 2017 - 2023, Arm Limited. All rights reserved.<BR>
  Copyright (c) 2024, Ventana Micro Systems Inc. All rights reserved.<BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef FADT_GENERATOR_H_
#define FADT_GENERATOR_H_

/** Updates the Architecture specific information in the FADT Table.

  @param [in]  CfgMgrProtocol Pointer to the Configuration Manager
                              Protocol Interface.

  @retval EFI_SUCCESS           Success.
  @retval EFI_INVALID_PARAMETER A parameter is invalid.
  @retval EFI_NOT_FOUND         The required object was not found.
  @retval EFI_BAD_BUFFER_SIZE   The size returned by the Configuration
                                Manager is less than the Object size for the
                                requested object.
**/
EFI_STATUS
EFIAPI
FadtAddArchInfo (
  IN  CONST EDKII_CONFIGURATION_MANAGER_PROTOCOL  *CONST  CfgMgrProtocol,
  OUT EFI_ACPI_6_5_FIXED_ACPI_DESCRIPTION_TABLE           *Fadt
  );

#endif // FADT_GENERATOR_H_
