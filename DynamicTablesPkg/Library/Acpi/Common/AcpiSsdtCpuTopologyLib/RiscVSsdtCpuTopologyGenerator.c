#include <AcpiTableGenerator.h>
#include <Library/AmlLib/AmlLib.h>
#include <Library/DebugLib.h>
#include <Protocol/ConfigurationManagerProtocol.h>
#include <ConfigurationManagerObject.h>
#include <ConfigurationManagerHelper.h>

#include "SsdtCpuTopologyGenerator.h"

/** This macro expands to a function that retrieves the GIC
    CPU interface Information from the Configuration Manager.
*/
GET_OBJECT_LIST (
  EObjNameSpaceRiscV,
  ERiscVObjRintcInfo,
  CM_RISCV_RINTC_INFO
  );

/** Create the processor hierarchy AML tree from CM_RISCV_RINTC_INFO
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
CreateTopologyFromApic (
  IN        ACPI_CPU_TOPOLOGY_GENERATOR                   *Generator,
  IN  CONST EDKII_CONFIGURATION_MANAGER_PROTOCOL  *CONST  CfgMgrProtocol,
  IN        AML_OBJECT_NODE_HANDLE                        ScopeNode
  )
{
  EFI_STATUS              Status;
  CM_RISCV_RINTC_INFO        *RintcInfo;
  UINT32                  RintcInfoCount;
  UINT32                  Index;
  AML_OBJECT_NODE_HANDLE  CpuNode;

  ASSERT (Generator != NULL);
  ASSERT (CfgMgrProtocol != NULL);
  ASSERT (ScopeNode != NULL);

  Status = GetERiscVObjRintcInfo (
             CfgMgrProtocol,
             CM_NULL_TOKEN,
             &RintcInfo,
             &RintcInfoCount
             );
  if (EFI_ERROR (Status)) {
    ASSERT (0);
    return Status;
  }

  // For each CM_RISCV_RINTC_INFO object, create an AML node.
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
    // Rintcinfo, create an _CPC method returning them.
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

/** Create and add an Embedded trace device to the Cpu Node.

  @param [in]  Generator              The SSDT Cpu Topology generator.
  @param [in]  CfgMgrProtocol         Pointer to the Configuration Manager
                                      Protocol Interface.
  @param [in]  GicCInfo               Pointer to the CM_ARM_GICC_INFO object
                                      describing the Cpu.
  @param [in]  CpuName                Value used to generate the CPU node name.
  @param [in]  Node                   CPU Node to which the ET device node is
                                      attached.

  @retval EFI_SUCCESS             The function completed successfully.
  @retval EFI_UNSUPPORTED         Feature Unsupported.
  @retval EFI_INVALID_PARAMETER   Invalid parameter.
  @retval EFI_OUT_OF_RESOURCES    Failed to allocate memory.
**/
EFI_STATUS
EFIAPI
CreateAmlEtNode (
  IN  ACPI_CPU_TOPOLOGY_GENERATOR                         *Generator,
  IN  CONST EDKII_CONFIGURATION_MANAGER_PROTOCOL  *CONST  CfgMgrProtocol,
  IN  CM_OBJECT_TOKEN                                     EtToken,
  IN        UINT32                                        CpuName,
  IN  AML_OBJECT_NODE_HANDLE                              *Node
  )
{
  return EFI_UNSUPPORTED;
}

EFI_STATUS
EFIAPI
GetProcUid (
  IN  CONST EDKII_CONFIGURATION_MANAGER_PROTOCOL  *CONST  CfgMgrProtocol,
  IN  CM_OBJECT_TOKEN ApicToken,
  OUT UINT32 *Uid
  )
{
  EFI_STATUS              Status;
  CM_RISCV_RINTC_INFO        *RintcInfo;

  ASSERT (Uid != NULL);
  
  Status = GetERiscVObjRintcInfo (
             CfgMgrProtocol,
             ApicToken,
             &RintcInfo,
             NULL
             );
  if (EFI_ERROR (Status)) {
    ASSERT (0);
    return Status;
  }

  *Uid = RintcInfo->AcpiProcessorUid;
  return EFI_SUCCESS;
}

CM_OBJECT_TOKEN
EFIAPI
GetCpcToken (
  IN  CONST EDKII_CONFIGURATION_MANAGER_PROTOCOL  *CONST  CfgMgrProtocol,
  IN  CM_OBJECT_TOKEN ApicToken
  )
{
  EFI_STATUS              Status;
  CM_RISCV_RINTC_INFO        *RintcInfo;
  
  Status = GetERiscVObjRintcInfo (
             CfgMgrProtocol,
             ApicToken,
             &RintcInfo,
             NULL
             );
  if (EFI_ERROR (Status)) {
    ASSERT (0);
    return CM_NULL_TOKEN;
  }

  return RintcInfo->CpcToken;
}

CM_OBJECT_TOKEN
EFIAPI
GetEtToken (
  IN  CONST EDKII_CONFIGURATION_MANAGER_PROTOCOL  *CONST  CfgMgrProtocol,
  IN  CM_OBJECT_TOKEN ApicToken
  )
{
  return CM_NULL_TOKEN;
}
