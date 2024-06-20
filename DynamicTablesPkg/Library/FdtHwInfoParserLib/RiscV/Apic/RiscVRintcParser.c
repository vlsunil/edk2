/** @file
  RISC-V RINTC parser.

  Copyright (c) 2024, Ventana Micro Systems Inc. All rights reserved.<BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent

  @par Reference(s):
  - linux/Documentation/devicetree/bindings/riscv/cpus.yaml
**/

#include "FdtHwInfoParser.h"
#include "CmObjectDescUtility.h"
#include "RiscVAcpi.h"
#include "RiscV/Apic/RiscVRintcParser.h"
#include "RiscV/Apic/RiscVApicDispatcher.h"

#define ACPI_BUILD_EXT_INTC_ID(PlicAplicId, CtxIdcId) \
                    ((PlicAplicId << 24) | (CtxIdcId))

/** List of "compatible" property values for CPU nodes.

  Any other "compatible" value is not supported by this module.
*/
STATIC CONST COMPATIBILITY_STR  CpuCompatibleStr[] = {
  { "riscv" }
};

/** COMPATIBILITY_INFO structure for CPU nodes.
*/
STATIC CONST COMPATIBILITY_INFO  CpuCompatibleInfo = {
  ARRAY_SIZE (CpuCompatibleStr),
  CpuCompatibleStr
};

#if 0
/** List of "compatible" property values for INTC nodes.

  Any other "compatible" value is not supported by this module.
*/
STATIC CONST COMPATIBILITY_STR  IntcCompatibleStr[] = {
  { "riscv,cpu-intc" }
};

/** COMPATIBILITY_INFO structure for INTC nodes.
*/
STATIC CONST COMPATIBILITY_INFO  IntcCompatibleInfo = {
  ARRAY_SIZE (IntcCompatibleStr),
  IntcCompatibleStr
};

#endif
/** List of "compatible" property values for IMSIC node.

  Any other "compatible" value is not supported by this module.
*/
STATIC CONST COMPATIBILITY_STR  ImsicCompatibleStr[] = {
  { "riscv,imsics" }
};

/** COMPATIBILITY_INFO structure for IMSIC node.
*/
STATIC CONST COMPATIBILITY_INFO  ImsicCompatibleInfo = {
  ARRAY_SIZE (ImsicCompatibleStr),
  ImsicCompatibleStr
};

/** List of "compatible" property values for APLIC node.

  Any other "compatible" value is not supported by this module.
*/
STATIC CONST COMPATIBILITY_STR  AplicCompatibleStr[] = {
  { "riscv,aplic" }
};

/** COMPATIBILITY_INFO structure for APLIC node.
*/
STATIC CONST COMPATIBILITY_INFO  AplicCompatibleInfo = {
  ARRAY_SIZE (AplicCompatibleStr),
  AplicCompatibleStr
};

/** List of "compatible" property values for PLIC node.

  Any other "compatible" value is not supported by this module.
*/
STATIC CONST COMPATIBILITY_STR  PlicCompatibleStr[] = {
  { "riscv,plic0" }
};

/** COMPATIBILITY_INFO structure for IMSIC node.
*/
STATIC CONST COMPATIBILITY_INFO  PlicCompatibleInfo = {
  ARRAY_SIZE (PlicCompatibleStr),
  PlicCompatibleStr
};

/** List of "compatible" property values for timer node.

  Any other "compatible" value is not supported by this module.
*/
STATIC CONST COMPATIBILITY_STR  TimerCompatibleStr[] = {
  { "riscv,timer" }
};

/** COMPATIBILITY_INFO structure for timer node.
*/
STATIC CONST COMPATIBILITY_INFO  TimerCompatibleInfo = {
  ARRAY_SIZE (TimerCompatibleStr),
  TimerCompatibleStr
};

/** Get S-mode node in FDT

  FDT will have entries for both M-mode and S-mode devices. We
  need only S-mode APLIC.

  @param [in]  Fdt             Pointer to device tree.
  @param [in]  CompatInfo      Pointer to compatibility info.
  @param [out] TargetNode      Pointer to target Node.
  @param [in]  IsIc            Is Interrupt Controller Node?

  @retval TRUE                 The AplicNode is S-mode APLIC
  @retval FALSE                The AplicNode is not S-mode APLIC
**/
STATIC
EFI_STATUS
FdtGetCompatSmodeNode (
  IN  CONST FDT_HW_INFO_PARSER_HANDLE  FdtParserHandle,
  IN  CONST VOID  *CompatInfo,
  OUT INT32       *TargetNode,
  IN BOOLEAN      IsIc
  )
{
  INT32    Prev, Node, PropLen, i;
  fdt32_t  *Prop;
  VOID                      *Fdt;
  BOOLEAN MsiParentExists;

  Fdt = FdtParserHandle->Fdt;

  for (Prev = 0; ; Prev = Node) {
    Node = fdt_next_node (Fdt, Prev, NULL);
    if (Node < 0) {
      return EFI_NOT_FOUND;
    }

    if (FdtNodeIsCompatible (Fdt, Node, CompatInfo)) {
      if (IsIc) {
        MsiParentExists = FdtNodeHasProperty (Fdt, Node, "msi-parent");
        if (MsiParentExists == FALSE) {
          Prop = (fdt32_t *)fdt_getprop (Fdt, Node, "interrupts-extended", &PropLen);
          if (!Prop || (PropLen < 4)) {
            continue;
          } else {
            PropLen = PropLen / 4;
            for (i = 0; i < PropLen; i++) {
              if (fdt32_to_cpu (Prop[i*2 + 1]) != IRQ_S_EXT) {
                continue;
              } else {
                break;
              }
            }
            if (i == PropLen) {
              continue;
            }
          }
        }
      }
      *TargetNode = Node;
      return EFI_SUCCESS;
    }
  }

  return EFI_NOT_FOUND;
}

/** Create ISA string Info structure

  @param [in]  FdtParserHandle    A handle to the parser instance.
  @param [in]  CpuNode            cpu node.
**/
STATIC
VOID
EFIAPI
CreateIsaStringInfo (
  IN  CONST FDT_HW_INFO_PARSER_HANDLE  FdtParserHandle,
  IN        INT32                      CpuNode
  )
{
  STATIC BOOLEAN            FoundIsa = FALSE;
  CONST UINT64              *Prop;
  INT32                     Len;
  EFI_STATUS                Status;
  CM_RISCV_ISA_STRING_NODE  IsaStringInfo;
  VOID                      *Fdt;

  Fdt = FdtParserHandle->Fdt;
  if (!FoundIsa) {
    Prop = fdt_getprop (Fdt, CpuNode, "riscv,isa", &Len);
    if (!Prop) {
      DEBUG (
        (
         DEBUG_ERROR,
         "%a: Failed to parse cpu node: riscv,isa\n",
         __func__
        )
        );
      ASSERT (0);
      return;
    }

    IsaStringInfo.IsaString = AllocateZeroPool (Len);
    if (IsaStringInfo.IsaString == NULL) {
      ASSERT (0);
      return;
    }

    //  CopyMem (IsaStringInfo.IsaString, (VOID *)Prop, Len);
    Status = AsciiStrCpyS (
               IsaStringInfo.IsaString,
               Len,
               (CHAR8 *)Prop
               );
    IsaStringInfo.Length = Len;
    // Add the CmObj to the Configuration Manager.
    Status = AddSingleCmObj (
               FdtParserHandle,
               CREATE_CM_RISCV_OBJECT_ID (ERiscVObjIsaStringInfo),
               &IsaStringInfo,
               Len + 2,
               NULL
               );
    if (EFI_ERROR (Status)) {
      ASSERT (0);
      return;
    }

    //    FreePool (IsaStringInfo.IsaString);
    FoundIsa = TRUE;
  }
}

/** Get CMO block size

  CMO block size in ACPI table is power of 2 value.

  @param [in]  Val       CBO size.

  @retval Ret            Exponent value when Val is represented in power of 2.
**/
STATIC
UINT32
RhctCmoGetBlockSize (
  UINT32  Val
  )
{
  UINT32  Ret;

  while (Val > 1) {
    Ret++;
    Val >>= 1;
  }

  return Ret;
}

/** Create Timer Info structure.

  Create Timer info structure with time base frequency and flag.

  @param [in]  FdtParserHandle    A handle to the parser instance.
  @param [in]  CpuNode            cpu node.
**/
STATIC
VOID
EFIAPI
CreateTimerInfo (
  IN  CONST FDT_HW_INFO_PARSER_HANDLE  FdtParserHandle,
  IN        INT32                      CpuNode
  )
{
  STATIC BOOLEAN       FoundTimer = FALSE;
  CONST UINT64         *Prop;
  INT32                Len, TimerNode;
  CM_RISCV_TIMER_INFO  TimerInfo;
  EFI_STATUS           Status;
  INT32                CpusNode;
  VOID                 *Fdt;

  Fdt = FdtParserHandle->Fdt;
  // The "cpus" node resides at the root of the DT. Fetch it.
  CpusNode = fdt_path_offset (Fdt, "/cpus");
  if (CpusNode < 0) {
    return;
  }

  if (!FoundTimer) {
    Prop = fdt_getprop (Fdt, CpusNode, "timebase-frequency", &Len);
    if (!Prop) {
      DEBUG (
        (
         DEBUG_ERROR,
         "%a: Failed to parse cpu node:se-frequency \n",
         __func__
        )
        );
      return;
    } else {
      TimerInfo.TimeBaseFrequency = fdt32_to_cpu (ReadUnaligned32 ((const UINT32 *)Prop));
    }

    Status = FdtGetCompatSmodeNode (FdtParserHandle, &TimerCompatibleInfo, &TimerNode, FALSE);
    if (!EFI_ERROR (Status)) {
      Prop = fdt_getprop (Fdt, TimerNode, "riscv,timer-cannot-wake-cpu", &Len);
      if (!Prop) {
        TimerInfo.TimerCannotWakeCpu = 0;
      } else {
        TimerInfo.TimerCannotWakeCpu = 1;
      }
    }

    Status = AddSingleCmObj (
               FdtParserHandle,
               CREATE_CM_RISCV_OBJECT_ID (ERiscVObjTimerInfo),
               &TimerInfo,
               sizeof (CM_RISCV_TIMER_INFO),
               NULL
               );
    if (EFI_ERROR (Status)) {
      ASSERT (0);
      return;
    }

    FoundTimer = TRUE;
  }
}

/** Create CMO info structure if CMO extension present

  Create CMO structure with CBOM, CBOP and CBOZ sizes.

  @param [in]  FdtParserHandle    A handle to the parser instance.
  @param [in]  CpuNode            cpu node.

**/
STATIC
VOID
EFIAPI
CreateCmoInfo (
  IN  CONST FDT_HW_INFO_PARSER_HANDLE  FdtParserHandle,
  IN        INT32                      CpuNode
  )
{
  STATIC BOOLEAN     FoundCmo = FALSE;
  CONST UINT64       *Prop;
  INT32              Len;
  CM_RISCV_CMO_NODE  CmoInfo;
  EFI_STATUS         Status;
  VOID               *Fdt;

  Fdt = FdtParserHandle->Fdt;
  if (!FoundCmo) {
    Prop = fdt_getprop (Fdt, CpuNode, "riscv,cbom-block-size", &Len);
    if (!Prop) {
      DEBUG (
        (
         DEBUG_VERBOSE,
         "%a: Failed to parse cpu node: riscv,cbom-block-size\n",
         __func__
        )
        );
      CmoInfo.CbomBlockSize = 0;
      CmoInfo.CbozBlockSize = 0;
      CmoInfo.CbopBlockSize = 0;
      return;
    } else {
      CmoInfo.CbomBlockSize = RhctCmoGetBlockSize (fdt32_to_cpu (ReadUnaligned32 ((const UINT32 *)Prop)));
    }

    Prop = fdt_getprop (Fdt, CpuNode, "riscv,cboz-block-size", &Len);
    if (!Prop) {
      DEBUG (
        (
         DEBUG_VERBOSE,
         "%a: Failed to parse cpu node: riscv,cboz-block-size\n",
         __func__
        )
        );
      CmoInfo.CbozBlockSize = 0;
    } else {
      CmoInfo.CbozBlockSize = RhctCmoGetBlockSize (fdt32_to_cpu (ReadUnaligned32 ((const UINT32 *)Prop)));
    }

    Prop = fdt_getprop (Fdt, CpuNode, "riscv,cbop-block-size", &Len);
    if (!Prop) {
      DEBUG (
        (
         DEBUG_VERBOSE,
         "%a: Failed to parse cpu node: riscv,cbop-block-size\n",
         __func__
        )
        );
      CmoInfo.CbopBlockSize = 0;
    } else {
      CmoInfo.CbopBlockSize = RhctCmoGetBlockSize (fdt32_to_cpu (ReadUnaligned32 ((const UINT32 *)Prop)));
    }

    // Add the CmObj to the Configuration Manager.
    Status = AddSingleCmObj (
               FdtParserHandle,
               CREATE_CM_RISCV_OBJECT_ID (ERiscVObjCmoInfo),
               &CmoInfo,
               sizeof (CM_RISCV_CMO_NODE),
               NULL
               );
    if (EFI_ERROR (Status)) {
      ASSERT (0);
      return;
    }

    FoundCmo = TRUE;
  }
}

/** Parse a "cpu" node.

  CPU node parser.

  @param [in]  FdtParserHandle    A handle to the parser instance.
  @param [in]  CpuNode            cpu node.
  @param [in]  AddressCells       AddressCells info.
  @param [out] RintcInfo          Pointer to RINTC Info structure.
  @param [in]  ImsicCpuBaseAddr   IMSIC CPU base address.
  @param [in]  ImsicCpuLen        IMSIC CPU page size.
  @param [in]  IsImsic            Is IMSIC?.

  @retval EFI_SUCCESS             The function completed successfully.
  @retval EFI_ABORTED             An error occurred.
  @retval EFI_INVALID_PARAMETER   Invalid parameter.
  @retval EFI_NOT_FOUND           Not found.
  @retval EFI_UNSUPPORTED         Unsupported.
**/
STATIC
EFI_STATUS
EFIAPI
CpuNodeParser (
  IN  CONST FDT_HW_INFO_PARSER_HANDLE  FdtParserHandle,
  IN        INT32                      CpuNode,
  IN        UINT32                     AddressCells,
  OUT       CM_RISCV_RINTC_INFO        *RintcInfo
  )
{
  CONST UINT8    *Data;
  CONST UINT32   *Prop;
  INT32          DataSize, Len;
  STATIC UINT32  ProcUid;
  UINT64         HartId;
  VOID           *Fdt;
  INT32          IntcNode;
  EFI_STATUS     Status;

  if (RintcInfo == NULL) {
    ASSERT (0);
    return EFI_INVALID_PARAMETER;
  }

  Fdt  = FdtParserHandle->Fdt;
  Data = fdt_getprop (Fdt, CpuNode, "reg", &DataSize);
  if ((Data == NULL)                  ||
      ((DataSize != sizeof (UINT32))  &&
       (DataSize != sizeof (UINT64))))
  {
    ASSERT (0);
    return EFI_ABORTED;
  }

  IntcNode = CpuNode;
//  Status = FdtGetNextCompatNodeInBranch (Fdt, CpuNode, "interrupt-controller", &IntcNode);
  //Status = FdtGetNextCompatNodeInBranch (Fdt, CpuNode, &IntcCompatibleInfo, &IntcNode);
  Status = FdtGetNextNamedNodeInBranch (Fdt, CpuNode, "interrupt-controller", &IntcNode);
  if (EFI_ERROR (Status)) {
    ASSERT (0);
    if (Status == EFI_NOT_FOUND) {
      // Should have found the node.
      Status = EFI_ABORTED;
    }
  }

  Prop = fdt_getprop (Fdt, IntcNode, "phandle", &Len);
  if (Prop || (Len > 0)) {
    RintcInfo->IntcPhandle = fdt32_to_cpu (*((UINT32 *)Prop));
  }
  if (AddressCells == 2) {
    HartId = fdt64_to_cpu (*((UINT64 *)Data));
  } else {
    HartId = fdt32_to_cpu (*((UINT32 *)Data));
  }

  RintcInfo->Flags            = EFI_ACPI_6_6_RINTC_FLAG_ENABLE; // REVISIT - check status
  RintcInfo->HartId           = HartId;
  RintcInfo->Version          = 1;
  RintcInfo->AcpiProcessorUid = ProcUid++;
  RintcInfo->ExtIntCId = 0;
  CreateCmoInfo (FdtParserHandle, CpuNode);
  CreateIsaStringInfo (FdtParserHandle, CpuNode);
  CreateTimerInfo (FdtParserHandle, CpuNode);
  return EFI_SUCCESS;
}

/** CPU node parser

  CPU node parser.

  @param [in]  FdtParserHandle    A handle to the parser instance.
  @param [in]  CpusNode           cpus node.
  @param [out] NewRintcCmObjDesc   Pointer to array of RINTC CM objects.
  @param [in]  IsImsic            Is IMSIC Node?.

  @retval EFI_SUCCESS             The function completed successfully.
  @retval EFI_ABORTED             An error occurred.
  @retval EFI_INVALID_PARAMETER   Invalid parameter.
  @retval EFI_NOT_FOUND           Not found.
  @retval EFI_UNSUPPORTED         Unsupported.
**/
STATIC
EFI_STATUS
EFIAPI
CpusNodeParser (
  IN  CONST FDT_HW_INFO_PARSER_HANDLE  FdtParserHandle,
  IN        INT32                      CpusNode,
  OUT       CM_OBJ_DESCRIPTOR          **NewRintcCmObjDesc
  )
{
  EFI_STATUS  Status;
  INT32       CpuNode;
  UINT32      CpuNodeCount;
  INT32       AddressCells;

  UINT32               Index;
  CM_RISCV_RINTC_INFO  *RintcInfoBuffer;
  UINT32               RintcInfoBufferSize;
  VOID                 *Fdt;

  if (NewRintcCmObjDesc == NULL) {
    ASSERT (0);
    return EFI_INVALID_PARAMETER;
  }

  Fdt          = FdtParserHandle->Fdt;
  AddressCells = fdt_address_cells (Fdt, CpusNode);
  if (AddressCells < 0) {
    ASSERT (0);
    return EFI_ABORTED;
  }

  // Count the number of "cpu" nodes under the "cpus" node.
  Status = FdtCountNamedNodeInBranch (Fdt, CpusNode, "cpu", &CpuNodeCount);
  if (EFI_ERROR (Status)) {
    ASSERT (0);
    return Status;
  }

  if (CpuNodeCount == 0) {
    ASSERT (0);
    return EFI_NOT_FOUND;
  }

  RintcInfoBufferSize = CpuNodeCount * sizeof (CM_RISCV_RINTC_INFO);
  RintcInfoBuffer     = AllocateZeroPool (RintcInfoBufferSize);
  if (RintcInfoBuffer == NULL) {
    ASSERT (0);
    return EFI_OUT_OF_RESOURCES;
  }

  CpuNode = CpusNode;
  for (Index = 0; Index < CpuNodeCount; Index++) {
    Status = FdtGetNextNamedNodeInBranch (Fdt, CpusNode, "cpu", &CpuNode);
    if (EFI_ERROR (Status)) {
      ASSERT (0);
      if (Status == EFI_NOT_FOUND) {
        // Should have found the node.
        Status = EFI_ABORTED;
      }

      goto exit_handler;
    }

    // Parse the "cpu" node.
    if (!FdtNodeIsCompatible (Fdt, CpuNode, &CpuCompatibleInfo)) {
      ASSERT (0);
      Status = EFI_UNSUPPORTED;
      goto exit_handler;
    }

    Status = CpuNodeParser (
               FdtParserHandle,
               CpuNode,
               AddressCells,
               &RintcInfoBuffer[Index]
               );
    if (EFI_ERROR (Status)) {
      ASSERT (0);
      goto exit_handler;
    }
  } // for

  Status = CreateCmObjDesc (
             CREATE_CM_RISCV_OBJECT_ID (ERiscVObjRintcInfo),
             CpuNodeCount,
             RintcInfoBuffer,
             RintcInfoBufferSize,
             NewRintcCmObjDesc
             );
  ASSERT_EFI_ERROR (Status);
exit_handler:
  FreePool (RintcInfoBuffer);
  return Status;
}

STATIC
CM_RISCV_RINTC_INFO *RiscVFindRintc(
  IN CM_OBJ_DESCRIPTOR *NewRintcCmObjDesc,
  INT32 Phandle)
{
  CM_RISCV_RINTC_INFO  *RintcInfo;
  INT32 i;

  RintcInfo = (CM_RISCV_RINTC_INFO *)NewRintcCmObjDesc->Data;

  for (i = 0; i < NewRintcCmObjDesc->Count; i++) {
    if (RintcInfo[i].IntcPhandle == Phandle) {
      return &RintcInfo[i];
    }
  }

  return NULL;
}

/** PLIC parser and update RINTC

  @param [in]  FdtParserHandle    A handle to the parser instance.
  @param [in]  PliicNode          Plic Node in FDT.
  @param [out] NewRintcCmObjDesc  Pointer to array of RINTC CM objects.

  @retval EFI_SUCCESS             The function completed successfully.
  @retval EFI_ABORTED             An error occurred.
  @retval EFI_INVALID_PARAMETER   Invalid parameter.
  @retval EFI_NOT_FOUND           Not found.
  @retval EFI_UNSUPPORTED         Unsupported.
**/
STATIC
EFI_STATUS
EFIAPI
PlicRintcInfoParser (
  IN  CONST FDT_HW_INFO_PARSER_HANDLE  FdtParserHandle,
  IN  OUT   CM_OBJ_DESCRIPTOR          *NewRintcCmObjDesc
  )
{
  EFI_STATUS           Status;
  CM_RISCV_RINTC_INFO  *RintcInfo;
  CM_RISCV_PLIC_INFO   PlicInfo;
  UINT32               PlicGsiBase = 0;
  VOID                 *Fdt;
  INT32                Len, LocalCpuId;
  INT32                PlicId = 0, i, j, Phandle;
  CONST UINT32         *Prop, *IntExtProp, *PhandleProp;
  INT32                Prev, PlicNode;

  if (FdtParserHandle == NULL) {
    ASSERT (0);
    return EFI_INVALID_PARAMETER;
  }

  Fdt       = FdtParserHandle->Fdt;
  for (Prev = 0; ; Prev = PlicNode) {
    PlicNode = fdt_next_node (Fdt, Prev, NULL);
    if (PlicNode < 0) {
      return EFI_SUCCESS;
    }

    if (FdtNodeIsCompatible (Fdt, PlicNode, &PlicCompatibleInfo)) {
      IntExtProp = fdt_getprop (Fdt, PlicNode, "interrupts-extended", &Len);
      if (IntExtProp == NULL || Len < 4) {
        ASSERT(0);
        return EFI_INVALID_PARAMETER;
      } else {
        Len = Len / 4;
        for (i = 0, j = 0; i < Len; i += 2, j++) {
          if (fdt32_to_cpu (IntExtProp[i + 1]) == IRQ_S_EXT) {
            Phandle = fdt32_to_cpu (ReadUnaligned32 ((const UINT32 *)IntExtProp + i));
            RintcInfo = RiscVFindRintc(NewRintcCmObjDesc, Phandle);
            if (RintcInfo == NULL) {
              ASSERT(0);
              return EFI_INVALID_PARAMETER;
            }

            LocalCpuId = j / 2;
            /* Update RINTC EXT INTC ID */
            RintcInfo->ExtIntCId = ACPI_BUILD_EXT_INTC_ID(PlicId, 2 * LocalCpuId + 1);
          }
        }

        ZeroMem (&PlicInfo, sizeof (CM_RISCV_PLIC_INFO));
        Prop = fdt_getprop (Fdt, PlicNode, "reg", &Len);
        if (!Prop || (Len % 4 > 0)) {
          ASSERT(0);
          return EFI_INVALID_PARAMETER;
        }

        PlicInfo.PlicAddress = fdt64_to_cpu (ReadUnaligned64 ((const UINT64 *)Prop));
        PlicInfo.PlicSize    = fdt64_to_cpu (ReadUnaligned64 ((const UINT64 *)Prop + 1));
        Prop                 = fdt_getprop (Fdt, PlicNode, "riscv,ndev", &Len);
        if (!Prop) {
          ASSERT(0);
          return EFI_INVALID_PARAMETER;
        }

        PlicInfo.NumSources = fdt32_to_cpu (ReadUnaligned32 ((const UINT32 *)Prop));
        PhandleProp          = fdt_getprop (Fdt, PlicNode, "phandle", &Len);
        if (!PhandleProp) {
          ASSERT (0);
          return EFI_INVALID_PARAMETER;
        }

        PlicInfo.Phandle = fdt32_to_cpu (ReadUnaligned32 ((const UINT32 *)PhandleProp));
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
  }

  return EFI_SUCCESS;
}

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

/** APLIC parser and update RINTC

  @param [in]  FdtParserHandle    A handle to the parser instance.
  @param [in]  ApliicNode         Aplic Node in FDT.
  @param [out] NewRintcCmObjDesc  Pointer to array of RINTC CM objects.

  @retval EFI_SUCCESS             The function completed successfully.
  @retval EFI_ABORTED             An error occurred.
  @retval EFI_INVALID_PARAMETER   Invalid parameter.
  @retval EFI_NOT_FOUND           Not found.
  @retval EFI_UNSUPPORTED         Unsupported.
**/
STATIC
EFI_STATUS
EFIAPI
AplicRintcInfoParser (
  IN  CONST FDT_HW_INFO_PARSER_HANDLE  FdtParserHandle,
  IN  OUT   CM_OBJ_DESCRIPTOR          *NewRintcCmObjDesc
  )
{
  EFI_STATUS           Status;
  CM_RISCV_APLIC_INFO  AplicInfo;
  UINT32               AplicGsiBase = 0;
  VOID                 *Fdt;
  INT32                Len;
  INT32                AplicId = 0, i, Phandle;
  CONST UINT32         *IntExtProp, *PhandleProp;
  CONST UINT64         *Prop;
  CM_RISCV_RINTC_INFO  *RintcInfo = NULL;
  INT32                Prev, AplicNode;

  if (FdtParserHandle == NULL) {
    ASSERT (0);
    return EFI_INVALID_PARAMETER;
  }

  Fdt  = FdtParserHandle->Fdt;
  for (Prev = 0; ; Prev = AplicNode) {
    AplicNode = fdt_next_node (Fdt, Prev, NULL);
    if (AplicNode < 0) {
      return EFI_SUCCESS;
    }

    if (FdtNodeIsCompatible (Fdt, AplicNode, &AplicCompatibleInfo) &&
        IsSmodeAplic(Fdt, AplicNode)) {
      ZeroMem (&AplicInfo, sizeof (CM_RISCV_APLIC_INFO));
      IntExtProp   = fdt_getprop (Fdt, AplicNode, "interrupts-extended", &Len);
      if ((IntExtProp != 0) && ((Len / sizeof (UINT32)) % 2 == 0)) {
        Len = Len / 4;
        AplicInfo.NumIdcs = Len / 2;
        for (i = 0; i < Len; i += 2) {
          Phandle = fdt32_to_cpu (ReadUnaligned32 ((const UINT32 *)IntExtProp + i));
          RintcInfo = RiscVFindRintc(NewRintcCmObjDesc, Phandle);
          if (RintcInfo == NULL) {
            ASSERT (0);
            return EFI_NOT_FOUND;
          }

          /* Update RINTC EXT INTC ID */
          RintcInfo->ExtIntCId = ACPI_BUILD_EXT_INTC_ID(AplicId, i/2);
        }
      }

      Prop = fdt_getprop (Fdt, AplicNode, "reg", &Len);
      if (!Prop || (Len % 4 > 0)) {
        ASSERT (0);
        return EFI_INVALID_PARAMETER;
      }

      AplicInfo.AplicAddress = fdt64_to_cpu (ReadUnaligned64 ((const UINT64 *)Prop));
      AplicInfo.AplicSize    = fdt64_to_cpu (ReadUnaligned64 ((const UINT64 *)Prop + 1));
      Prop                   = fdt_getprop (Fdt, AplicNode, "riscv,num-sources", &Len);
      if (!Prop) {
        ASSERT (0);
        return EFI_INVALID_PARAMETER;
      }

      AplicInfo.NumSources = fdt32_to_cpu (ReadUnaligned32 ((const UINT32 *)Prop));
      PhandleProp          = fdt_getprop (Fdt, AplicNode, "phandle", &Len);
      if (!PhandleProp) {
        ASSERT (0);
        return EFI_INVALID_PARAMETER;
      }

      AplicInfo.Phandle = fdt32_to_cpu (ReadUnaligned32 ((const UINT32 *)PhandleProp));
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

  return EFI_SUCCESS;
}

/** RINTC parser using IMSIC node

  Parse RINTC information using IMSIC.

  @param [in]  FdtParserHandle    A handle to the parser instance.
  @param [in]  ImsicNode          IMSIC Node in FDT.
  @param [out] NewRintcCmObjDesc   Pointer to array of RINTC CM objects.

  @retval EFI_SUCCESS             The function completed successfully.
  @retval EFI_ABORTED             An error occurred.
  @retval EFI_INVALID_PARAMETER   Invalid parameter.
  @retval EFI_NOT_FOUND           Not found.
  @retval EFI_UNSUPPORTED         Unsupported.
**/
STATIC
EFI_STATUS
EFIAPI
ImsicRintcInfoParser (
  IN  CONST FDT_HW_INFO_PARSER_HANDLE  FdtParserHandle,
  IN  OUT   CM_OBJ_DESCRIPTOR          *NewRintcCmObjDesc
  )
{
  EFI_STATUS           Status;
  CM_RISCV_RINTC_INFO  *RintcInfoBuffer;
  CM_RISCV_IMSIC_INFO  ImsicInfo;
  CONST UINT64         *Prop;
  CONST UINT32         *IntExtProp;
  CONST UINT64         *ImsicRegProp;
  INT32                Len, NumPhandle, Phandle;
  INTN                 Idx, Idx1, Idx2 = 0, Limit;
  UINT64               ImsicBaseAddr, ImsicBaseLen;
  UINT64               ImsicCpuBaseAddr, ImsicCpuBaseLen;
  UINTN                NumImsicBase;
  VOID                 *Fdt;
  INT32                Prev, ImsicNode;

  if (NewRintcCmObjDesc == NULL) {
    ASSERT (0);
    return EFI_INVALID_PARAMETER;
  }

  Fdt  = FdtParserHandle->Fdt;
  for (Prev = 0; ; Prev = ImsicNode) {
    ImsicNode = fdt_next_node (Fdt, Prev, NULL);
    if (ImsicNode < 0) {
      return EFI_NOT_FOUND;
    }

    if (FdtNodeIsCompatible (Fdt, ImsicNode, &ImsicCompatibleInfo)) {
      IntExtProp   = fdt_getprop (Fdt, ImsicNode, "interrupts-extended", &Len);
      if ((IntExtProp == 0) || ((Len / sizeof (UINT32)) % 2 != 0)) {
        /* interrupts-extended: <phandle>, <flag> */
        ASSERT(0);
        return EFI_INVALID_PARAMETER;

      }

      if (fdt32_to_cpu (IntExtProp[1]) == IRQ_S_EXT) {
        NumPhandle = (Len / sizeof (UINT32)) / 2;
        if (NumPhandle == 0) {
          ASSERT (0);
          return EFI_NOT_FOUND;
        }

        Prop = fdt_getprop (Fdt, ImsicNode, "riscv,num-ids", &Len);
        if (Prop == 0) {
          ASSERT(0);
          return EFI_INVALID_PARAMETER;
        }

        ImsicInfo.NumIds = fdt32_to_cpu (ReadUnaligned32 ((const UINT32 *)Prop));
        Prop             = fdt_getprop (Fdt, ImsicNode, "riscv,num-guest-ids", &Len);
        if (Prop == 0) {
          ImsicInfo.NumGuestIds = ImsicInfo.NumIds;
        } else {
          ImsicInfo.NumGuestIds = fdt32_to_cpu (ReadUnaligned32 ((const UINT32 *)Prop));
        }

        Prop = fdt_getprop (Fdt, ImsicNode, "riscv,guest-index-bits", &Len);
        if (Prop == 0) {
          ImsicInfo.GuestIndexBits = 0;
        } else {
          ImsicInfo.GuestIndexBits = fdt32_to_cpu (ReadUnaligned32 ((const UINT32 *)Prop));
        }

        Prop = fdt_getprop (Fdt, ImsicNode, "riscv,hart-index-bits", &Len);
        if (Prop == 0) {
          ImsicInfo.HartIndexBits = 0; // update default value later
        } else {
          ImsicInfo.HartIndexBits = fdt32_to_cpu (ReadUnaligned32 ((const UINT32 *)Prop));
        }

        Prop = fdt_getprop (Fdt, ImsicNode, "riscv,group-index-bits", &Len);
        if (Prop == 0) {
          ImsicInfo.GroupIndexBits = 0;
        } else {
          ImsicInfo.GroupIndexBits = fdt32_to_cpu (ReadUnaligned32 ((const UINT32 *)Prop));
        }

        Prop = fdt_getprop (Fdt, ImsicNode, "riscv,group-index-shift", &Len);
        if (Prop == 0) {
          ImsicInfo.GroupIndexShift = IMSIC_MMIO_PAGE_SHIFT * 2;
        } else {
          ImsicInfo.GroupIndexShift = fdt32_to_cpu (ReadUnaligned32 ((const UINT32 *)Prop));
        }

        ImsicInfo.Version   = 1;
        ImsicInfo.Reserved1 = 0;
        ImsicInfo.Flags     = 0;
        Prop                = fdt_getprop (Fdt, ImsicNode, "reg", &Len);
        if ((Prop == 0) || (((Len / sizeof (UINT32)) % 4) != 0)) {
          // address-cells and size-cells are always 2
          DEBUG (
            (
             DEBUG_ERROR,
             "%a: Failed to parse imsic node: reg\n",
             __func__
            )
            );
          return EFI_INVALID_PARAMETER;
        }

        ImsicRegProp = Prop;
        NumImsicBase = (Len / sizeof (UINT32)) / 4;
        if (ImsicInfo.HartIndexBits == 0) {
          Len = NumPhandle;
          while (Len > 0) {
            ImsicInfo.HartIndexBits++;
            Len = Len >> 1;
          }
        }

        Idx2 = 0;
        for (Idx = 0; Idx < NumImsicBase; Idx++) {
          ImsicBaseAddr = fdt64_to_cpu (ReadUnaligned64 ((const UINT64 *)ImsicRegProp + Idx * 2));
          ImsicBaseLen  = fdt64_to_cpu (ReadUnaligned64 ((const UINT64 *)ImsicRegProp + Idx * 2 + 1));
          // Calculate the limit of number of cpu nodes this imsic can handle
          Limit = ImsicBaseLen /  IMSIC_MMIO_PAGE_SZ;
          for (Idx1 = 0; Idx1 < Limit && Idx2 < NumPhandle; Idx1++, Idx2++) {
            Phandle = fdt32_to_cpu (ReadUnaligned32 ((const UINT32 *)IntExtProp + Idx2 * 2));
            RintcInfoBuffer = RiscVFindRintc(NewRintcCmObjDesc, Phandle);
            if (RintcInfoBuffer == NULL) {
              DEBUG ((DEBUG_ERROR, "%a: Failed to find RINTC node\n", __func__));
              return EFI_NOT_FOUND;
            }

            ImsicCpuBaseAddr = ImsicBaseAddr + Idx1 * IMSIC_MMIO_PAGE_SZ;
            ImsicCpuBaseLen  = IMSIC_MMIO_PAGE_SZ;
            RintcInfoBuffer->ImsicBaseAddress = ImsicCpuBaseAddr;
            RintcInfoBuffer->ImsicSize = ImsicCpuBaseLen;
          }
        }

        // Add the CmObj to the Configuration Manager.
        Status = AddSingleCmObj (
                   FdtParserHandle,
                   CREATE_CM_RISCV_OBJECT_ID (ERiscVObjImsicInfo),
                   &ImsicInfo,
                   sizeof (CM_RISCV_IMSIC_INFO),
                   NULL
                   );
        if (EFI_ERROR (Status)) {
          ASSERT (0);
        }

        return Status;
      }
    }
  }

  return Status;
}

/** CM_RISCV_RINTC_INFO and CM_ARCH_IMSIC_INFO parser function.

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
RiscVIntcInfoParser (
  IN  CONST FDT_HW_INFO_PARSER_HANDLE  FdtParserHandle,
  IN        INT32                      FdtBranch
  )
{
  EFI_STATUS         Status;
  CM_OBJ_DESCRIPTOR  *NewCmObjDesc;
  VOID               *Fdt;

  if (FdtParserHandle == NULL) {
    ASSERT (0);
    return EFI_INVALID_PARAMETER;
  }

  Fdt          = FdtParserHandle->Fdt;
  NewCmObjDesc = NULL;

  // Parse the "cpus" nodes and its children "cpu" nodes,
  // and create a CM_OBJ_DESCRIPTOR.
  Status = CpusNodeParser (FdtParserHandle, FdtBranch, &NewCmObjDesc);
  if (EFI_ERROR (Status)) {
    ASSERT (0);
    return Status;
  }

  Status = ImsicRintcInfoParser (FdtParserHandle, NewCmObjDesc);
  if (EFI_ERROR (Status) && (Status != EFI_NOT_FOUND)) {
    ASSERT_EFI_ERROR (Status);
    goto exit_handler;
  }

  Status = AplicRintcInfoParser (FdtParserHandle, NewCmObjDesc);
  if (EFI_ERROR (Status) && (Status != EFI_NOT_FOUND)) {
    ASSERT_EFI_ERROR (Status);
    goto exit_handler;
  }

  Status = PlicRintcInfoParser (FdtParserHandle, NewCmObjDesc);
  if (EFI_ERROR (Status) && (Status != EFI_NOT_FOUND)) {
    ASSERT_EFI_ERROR (Status);
    goto exit_handler;
  }

  // Add all the RINTC CmObjs to the Configuration Manager.
  Status = AddMultipleCmObj (FdtParserHandle, NewCmObjDesc, 0, NULL);
  if (EFI_ERROR (Status)) {
    ASSERT (0);
    goto exit_handler;
  }

exit_handler:
  FreeCmObjDesc (NewCmObjDesc);
  return Status;
}
