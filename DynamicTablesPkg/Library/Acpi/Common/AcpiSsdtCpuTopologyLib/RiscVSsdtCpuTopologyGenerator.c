/** @file
  RISC-V SSDT Cpu Topology Table Generator Helpers.

  Copyright (c) 2024, Ventana Micro Systems Inc. All rights reserved.<BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent

  @par Reference(s):
    - TBD

  @par Glossary:
**/

#include <Library/AcpiLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Protocol/AcpiTable.h>

// Module specific include files.
#include <AcpiTableGenerator.h>
#include <ConfigurationManagerObject.h>
#include <ConfigurationManagerHelper.h>
#include <Library/AcpiHelperLib.h>
#include <Library/TableHelperLib.h>
#include <Library/AmlLib/AmlLib.h>
#include <Protocol/ConfigurationManagerProtocol.h>

#include "SsdtCpuTopologyGenerator.h"

/** This macro expands to a function that retrieves the RINTC
    CPU interface Information from the Configuration Manager.
*/
GET_OBJECT_LIST (
  EObjNameSpaceArch,
  EArchObjRintcInfo,
  CM_ARCH_RINTC_INFO
  );

/** Create the processor hierarchy AML tree from CM_ARCH_RINTC_INFO
    CM objects.

  A processor container is by extension any non-leave device in the cpu topology.

  @param [in] Generator        The SSDT Cpu Topology generator.
  @param [in] CfgMgrProtocol   Pointer to the Configuration Manager
                               Protocol Interface.
  @param [in] ScopeNode        Scope node handle ('\_SB' scope).

  @retval EFI_SUCCESS             Success.
  @retval EFI_INVALID_PARAMETER   Invalid parameter.
  @retval EFI_OUT_OF_RESOURCES    Failed to allocate memory.
**/
EFI_STATUS
EFIAPI
CreateTopologyFromIntC (
  IN        ACPI_CPU_TOPOLOGY_GENERATOR                   *Generator,
  IN  CONST EDKII_CONFIGURATION_MANAGER_PROTOCOL  *CONST  CfgMgrProtocol,
  IN        AML_OBJECT_NODE_HANDLE                        ScopeNode
  )
{
  EFI_STATUS              Status;
  CM_ARCH_RINTC_INFO      *RintcInfo;
  UINT32                  RintcInfoCount;
  UINT32                  Index;
  AML_OBJECT_NODE_HANDLE  CpuNode;

  ASSERT (Generator != NULL);
  ASSERT (CfgMgrProtocol != NULL);
  ASSERT (ScopeNode != NULL);

  Status = GetEArchObjRintcInfo (
             CfgMgrProtocol,
             CM_NULL_TOKEN,
             &RintcInfo,
             &RintcInfoCount
             );
  if (EFI_ERROR (Status)) {
    ASSERT (0);
    return Status;
  }

  // For each CM_ARCH_RINTC_INFO object, create an AML node.
  for (Index = 0; Index < RintcInfoCount; Index++) {
    Status = CreateAmlCpu (
               Generator,
               ScopeNode,
               RintcInfo[Index].AcpiProcessorUid,
               Index,
               &CpuNode
               );
    if (EFI_ERROR (Status)) {
      ASSERT (0);
      break;
    }

    // If a CPC info is associated with the
    // GicCinfo, create an _CPC method returning them.
    if (RintcInfo[Index].CpcToken != CM_NULL_TOKEN) {
      Status = CreateAmlCpcNode (Generator, CfgMgrProtocol, RintcInfo[Index].CpcToken, CpuNode);
      if (EFI_ERROR (Status)) {
        ASSERT_EFI_ERROR (Status);
        break;
      }
    }

    if (RintcInfo[Index].EtToken != CM_NULL_TOKEN) {
      Status = CreateAmlEtNode (
                 Generator,
                 CfgMgrProtocol,
                 RintcInfo[Index].AcpiProcessorUid,
                 RintcInfo[Index].EtToken,
                 Index,
                 CpuNode
                 );
      if (EFI_ERROR (Status)) {
        ASSERT_EFI_ERROR (Status);
        return Status;
      }
    }
  } // for

  return Status;
}

/** Get UID, CpcToken and EtToken from local interrupt controller structure.

  @param [in]  CfgMgrProtocol    Pointer to the Configuration Manager
                                 Protocol Interface.
  @param [in]  IntCToken         Unique Local INTC token to find the correct
                                 INTC info structure.
  @param [out] AcpiProcessorUid  UID of the CPU.
  @param [out] CpcToken          CpcToken of the CPU / local INTC.
  @param [out] EtToken           EtToken of the CPU / local INTC.

  @retval EFI_SUCCESS             Success.
  @retval EFI_NOT_FOUND           INTC structure not found.
**/
EFI_STATUS
EFIAPI
GetIntCUidTokens (
  IN  CONST EDKII_CONFIGURATION_MANAGER_PROTOCOL  *CONST  CfgMgrProtocol,
  IN  CM_OBJECT_TOKEN                                     IntCToken,
  OUT UINT32                                              *AcpiProcessorUid,
  OUT CM_OBJECT_TOKEN                                     *CpcToken,
  OUT CM_OBJECT_TOKEN                                     *EtToken
  )
{
  EFI_STATUS          Status;
  CM_ARCH_RINTC_INFO  *RintcInfo;

  Status = GetEArchObjRintcInfo (
             CfgMgrProtocol,
             IntCToken,
             &RintcInfo,
             NULL
             );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  if (AcpiProcessorUid != NULL) {
    *AcpiProcessorUid = RintcInfo->AcpiProcessorUid;
  }

  if (CpcToken != NULL) {
    *CpcToken = RintcInfo->CpcToken;
  }

  if (EtToken != NULL) {
    *EtToken = RintcInfo->EtToken;
  }

  return Status;
}
