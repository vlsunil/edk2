/** @file
  RISC-V APLIC Parser.

  Copyright (c) 2024, Ventana Micro Systems Inc. All rights reserved.<BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent

  @par Reference(s):
  - TBD
**/

#include "CmObjectDescUtility.h"
#include "FdtHwInfoParser.h"
#include "RiscV/Apic/RiscVApicDispatcher.h"
#include "RiscV/Apic/RiscVPlicAplicParser.h"

/** Check if it is S-mode APLIC

  FDT will have entries for both M-mode and S-mode APLIC. We
  need only S-mode APLIC.

  @param [in]  Fdt             Pointer to device tree.
  @param [in]  AplicNode       Node with APLIC compatible property.

  @retval TRUE                 The AplicNode is S-mode APLIC
  @retval FALSE                The AplicNode is not S-mode APLIC
**/
STATIC
BOOLEAN
IsSmodeAplic (
  IN VOID   *Fdt,
  IN INT32  AplicNode
  )
{
  fdt32_t  *IrqProp;
  fdt32_t  *MsiProp;
  INT32    Len, ImsicNode;

  IrqProp = (fdt32_t *)fdt_getprop (Fdt, AplicNode, "interrupts-extended", &Len);
  if ((IrqProp > 0) && (Len >= 4) &&
      (fdt32_to_cpu (IrqProp[1]) == IRQ_S_EXT))
  {
    return TRUE;
  }

  MsiProp = (fdt32_t *)fdt_getprop (Fdt, AplicNode, "msi-parent", &Len);
  if (MsiProp && (Len >= sizeof (fdt32_t))) {
    ImsicNode = fdt_node_offset_by_phandle (Fdt, fdt32_to_cpu (*MsiProp));
    if (ImsicNode < 0) {
      return FALSE;
    }

    IrqProp = (fdt32_t *)fdt_getprop (Fdt, ImsicNode, "interrupts-extended", &Len);
    if ((IrqProp > 0) && (Len >= 4) &&
        (fdt32_to_cpu (IrqProp[1]) == IRQ_S_EXT))
    {
      return TRUE;
    }
  }

  return FALSE;
}

/** CM_RISCV_APLIC_INFO and CM_RISCV_PLIC_INFO parser function.

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
RiscVAplicPlicInfoParser (
  IN  CONST FDT_HW_INFO_PARSER_HANDLE  FdtParserHandle,
  IN        INT32                      FdtBranch
  )
{
  EFI_STATUS           Status;
  CM_RISCV_APLIC_INFO  AplicInfo;
  CM_RISCV_PLIC_INFO   PlicInfo;
  UINT32               AplicGsiBase, PlicGsiBase;
  INT32                AplicNode, PlicNode;
  VOID                 *Fdt;
  INT32                Len, Node, Prev;
  INT32                AplicId, PlicId;
  CONST UINT64         *Prop;

  if (FdtParserHandle == NULL) {
    ASSERT (0);
    return EFI_INVALID_PARAMETER;
  }

  Fdt       = FdtParserHandle->Fdt;
  PlicNode  = 0;
  AplicNode = 0;

  for (Prev = 0; ; Prev = Node) {
    Node = fdt_next_node (Fdt, Prev, NULL);
    if (Node < 0) {
      break;
    }

    AplicNode = Node;

    // Check for aplic node
    if (fdt_node_check_compatible (Fdt, AplicNode, "riscv,aplic") == 0) {
      if (IsSmodeAplic (Fdt, AplicNode)) {
        ZeroMem (&AplicInfo, sizeof (CM_RISCV_APLIC_INFO));
        Prop = fdt_getprop (Fdt, AplicNode, "reg", &Len);
        if (!Prop || (Len % 4 > 0)) {
          DEBUG (
            (
             DEBUG_ERROR,
             "%a: Failed to parse aplic node: reg\n",
             __func__
            )
            );
          return 0;
        }

        AplicInfo.AplicAddress = fdt64_to_cpu (ReadUnaligned64 ((const UINT64 *)Prop));
        AplicInfo.AplicSize    = fdt64_to_cpu (ReadUnaligned64 ((const UINT64 *)Prop + 1));
        Prop                   = fdt_getprop (Fdt, AplicNode, "riscv,num-sources", &Len);
        if (!Prop) {
          DEBUG (
            (
             DEBUG_ERROR,
             "%a: Failed to parse aplic node: riscv,num-sources\n",
             __func__
            )
            );
          return 0;
        }

        AplicInfo.NumSources = fdt32_to_cpu (ReadUnaligned32 ((const UINT32 *)Prop));
        AplicInfo.GsiBase    = AplicGsiBase;
        AplicInfo.Version    = 1;
        AplicInfo.AplicId    = AplicId++;
        AplicGsiBase        += AplicInfo.NumSources;

        // Add the CmObj to the Configuration Manager.
        Status = AddSingleCmObj (
                   FdtParserHandle,
                   CREATE_CM_RISCV_OBJECT_ID (ERiscVObjAplicInfo),
                   &AplicInfo,
                   sizeof (CM_RISCV_APLIC_INFO),
                   NULL
                   );
        if (EFI_ERROR (Status)) {
          ASSERT (0);
          return Status;
        }
      }
    }

    PlicNode = Node;
    // Check for plic node
    if (fdt_node_check_compatible (Fdt, PlicNode, "riscv,plic0") == 0) {
      Prop = fdt_getprop (Fdt, PlicNode, "interrupts-extended", &Len);
      if (Prop && (Len >= 4)) {
        ZeroMem (&PlicInfo, sizeof (CM_RISCV_PLIC_INFO));
        Prop = fdt_getprop (Fdt, PlicNode, "reg", &Len);
        if (!Prop || (Len % 4 > 0)) {
          DEBUG (
            (
             DEBUG_ERROR,
             "%a: Failed to parse plic node: reg\n",
             __func__
            )
            );
          return 0;
        }

        PlicInfo.PlicAddress = fdt64_to_cpu (ReadUnaligned64 ((const UINT64 *)Prop));
        PlicInfo.PlicSize    = fdt64_to_cpu (ReadUnaligned64 ((const UINT64 *)Prop + 1));
        Prop                 = fdt_getprop (Fdt, PlicNode, "riscv,ndev", &Len);
        if (!Prop) {
          DEBUG (
            (
             DEBUG_ERROR,
             "%a: Failed to parse aplic node: riscv,num-sources\n",
             __func__
            )
            );
          return 0;
        }

        PlicInfo.NumSources = fdt32_to_cpu (ReadUnaligned32 ((const UINT32 *)Prop));
        PlicInfo.GsiBase    = PlicGsiBase;
        PlicInfo.Version    = 1;
        PlicInfo.PlicId     = PlicId++;
        PlicGsiBase        += PlicInfo.NumSources;

        // Add the CmObj to the Configuration Manager.
        Status = AddSingleCmObj (
                   FdtParserHandle,
                   CREATE_CM_RISCV_OBJECT_ID (ERiscVObjPlicInfo),
                   &PlicInfo,
                   sizeof (CM_RISCV_PLIC_INFO),
                   NULL
                   );
        if (EFI_ERROR (Status)) {
          ASSERT (0);
          return Status;
        }
      }
    }
  } // for

  return Status;
}
