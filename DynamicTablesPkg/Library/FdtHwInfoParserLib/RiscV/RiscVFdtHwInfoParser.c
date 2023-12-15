/** @file
  ARM Flattened Device Tree parser helper.

  Copyright (c) 2021, ARM Limited. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#include "FdtHwInfoParser.h"
#include "Madt/RiscVMadtDispatcher.h"

/** Ordered table of parsers/dispatchers for ARM.

  A parser parses a Device Tree to populate a specific CmObj type. None,
  one or many CmObj can be created by the parser.
  The created CmObj are then handed to the parser's caller through the
  HW_INFO_ADD_OBJECT interface.
  This can also be a dispatcher. I.e. a function that not parsing a
  Device Tree but calling other parsers.
*/
STATIC CONST FDT_HW_INFO_PARSER_FUNC  ArchHwInfoParserTable[] = {
  RiscVMadtDispatcher
};

EFI_STATUS
EFIAPI
ArchDispatcher (
  IN  CONST FDT_HW_INFO_PARSER_HANDLE  FdtParserHandle,
  IN        INT32                      FdtBranch
  )
{
  EFI_STATUS  Status;
  UINT32      Index;

  // Parse ARCH specific
  for (Index = 0; Index < ARRAY_SIZE (ArchHwInfoParserTable); Index++) {
    Status = ArchHwInfoParserTable[Index](
                                      FdtParserHandle,
                                      FdtBranch
                                      );
    if (EFI_ERROR (Status)  &&
        (Status != EFI_NOT_FOUND))
    {
      // If EFI_NOT_FOUND, the parser didn't find information in the DT.
      // Don't trigger an error.
      ASSERT (0);
      return Status;
    }
  } // for

  return EFI_SUCCESS;
}
