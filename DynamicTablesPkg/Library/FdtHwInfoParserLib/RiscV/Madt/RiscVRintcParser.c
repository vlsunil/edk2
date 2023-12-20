/** @file
  RISC-V RINTC parser.

  Copyright (c) 2021 - 2022, Arm Limited. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

  @par Reference(s):
  - linux/Documentation/devicetree/bindings/riscv/cpus.yaml
**/

#include <RiscVNameSpaceObjects.h>
#include "FdtHwInfoParser.h"
#include "CmObjectDescUtility.h"
#include "RiscVAcpi.h"
#include "RiscV/Madt/RiscVRintcParser.h"
#include "RiscV/Madt/RiscVMadtDispatcher.h"

/** List of "compatible" property values for CPU nodes.

  Any other "compatible" value is not supported by this module.
*/
STATIC CONST COMPATIBILITY_STR  CpuCompatibleStr[] = {
  { "riscv"     }
};

/** COMPATIBILITY_INFO structure for CPU nodes.
*/
STATIC CONST COMPATIBILITY_INFO  CpuCompatibleInfo = {
  ARRAY_SIZE (CpuCompatibleStr),
  CpuCompatibleStr
};

/** List of "compatible" property values for IMSIC node.

  Any other "compatible" value is not supported by this module.
*/
STATIC CONST COMPATIBILITY_STR  ImsicCompatibleStr[] = {
  { "riscv,imsics"     }
};

/** COMPATIBILITY_INFO structure for IMSIC node.
*/
STATIC CONST COMPATIBILITY_INFO  ImsicCompatibleInfo = {
  ARRAY_SIZE (ImsicCompatibleStr),
  ImsicCompatibleStr
};

/** List of "compatible" property values for timer node.

  Any other "compatible" value is not supported by this module.
*/
STATIC CONST COMPATIBILITY_STR  TimerCompatibleStr[] = {
  { "riscv,timer"     }
};

/** COMPATIBILITY_INFO structure for timer node.
*/
STATIC CONST COMPATIBILITY_INFO  TimerCompatibleInfo = {
  ARRAY_SIZE (TimerCompatibleStr),
  TimerCompatibleStr
};

STATIC
EFI_STATUS
FdtGetCompatSmodeNode (
  CONST VOID               *Fdt,
  IN  CONST VOID   *CompatInfo,
  OUT INT32        *TargetNode,
  IN BOOLEAN IsIc
  )
{
  INT32 Prev, Node, PropLen;
  fdt32_t *Prop;

  for (Prev = 0; ; Prev = Node) {
    Node = fdt_next_node (Fdt, Prev, NULL);
    if (Node < 0) {
      return EFI_NOT_FOUND;
    }

    if (FdtNodeIsCompatible (Fdt, Node, CompatInfo)) {
      if (IsIc == TRUE) {
        Prop = (fdt32_t *) fdt_getprop (Fdt, Node, "interrupts-extended", &PropLen);      
        if (!Prop || PropLen < 4) {
          continue;
        }
  
        if (fdt32_to_cpu(Prop[1]) != IRQ_S_EXT)
          continue;
      }
     
      *TargetNode = Node;
      return EFI_SUCCESS;
    }
  }

  return EFI_NOT_FOUND;
}

STATIC
VOID
EFIAPI
CreateIsaStringInfo (
  IN  CONST FDT_HW_INFO_PARSER_HANDLE  FdtParserHandle,
  IN        INT32             CpuNode)
{
  STATIC BOOLEAN FoundIsa = FALSE;
  CONST UINT64                          *Prop;
  INT32                                 Len;
  EFI_STATUS  Status;
  CM_RISCV_ISA_STRING_NODE  IsaStringInfo;
  VOID              *Fdt;

  Fdt          = FdtParserHandle->Fdt;
  if (FoundIsa == FALSE) {
    Prop = fdt_getprop (Fdt, CpuNode, "riscv,isa", &Len);
    if (!Prop) {
      DEBUG ((
        DEBUG_ERROR,
        "%a: Failed to parse cpu node: riscv,isa\n",
        __func__
        ));
      ASSERT (0);
      return;
    }

    IsaStringInfo.IsaString     = AllocateZeroPool (Len);
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

STATIC
UINT32
RhctCmoGetBlockSize (UINT32 Val)
{
  UINT32 Ret = 0;

  while (Val > 1) {
    Ret++;
    Val >>= 1;
  }

  return Ret;
}

STATIC
VOID
EFIAPI
CreateTimerInfo (
  IN  CONST FDT_HW_INFO_PARSER_HANDLE  FdtParserHandle,
  IN        INT32             CpuNode)
{
  STATIC BOOLEAN FoundTimer = FALSE;
  CONST UINT64                          *Prop;
  INT32                                 Len, TimerNode;
  CM_RISCV_TIMER_INFO  TimerInfo;
  EFI_STATUS  Status;
  INT32 CpusNode;
  VOID              *Fdt;

  Fdt          = FdtParserHandle->Fdt;
  // The "cpus" node resides at the root of the DT. Fetch it.
  CpusNode = fdt_path_offset (Fdt, "/cpus");
  if (CpusNode < 0) {
    return;
  }

  if (FoundTimer == FALSE) {
    Prop = fdt_getprop (Fdt, CpusNode, "timebase-frequency", &Len);
    if (!Prop) {
      DEBUG ((
        DEBUG_INFO,
        "%a: Failed to parse cpu node:se-frequency \n",
        __func__
        ));
      return;
    } else {
      TimerInfo.TimeBaseFrequency = fdt32_to_cpu (ReadUnaligned32 ((const UINT32 *)Prop));
    }

    Status = FdtGetCompatSmodeNode (Fdt, &TimerCompatibleInfo, &TimerNode, FALSE);
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

STATIC
VOID
EFIAPI
CreateCmoInfo (
  IN  CONST FDT_HW_INFO_PARSER_HANDLE  FdtParserHandle,
  IN        INT32             CpuNode)
{
  STATIC BOOLEAN FoundCmo = FALSE;
  CONST UINT64                          *Prop;
  INT32                                 Len;
  CM_RISCV_CMO_NODE  CmoInfo;
  EFI_STATUS  Status;
  VOID              *Fdt;

  Fdt          = FdtParserHandle->Fdt;
  if (FoundCmo == FALSE) {
    Prop = fdt_getprop (Fdt, CpuNode, "riscv,cbom-block-size", &Len);
    if (!Prop) {
      DEBUG ((
        DEBUG_VERBOSE,
        "%a: Failed to parse cpu node: riscv,cbom-block-size\n",
        __func__
        ));
      CmoInfo.CbomBlockSize = 0;
      CmoInfo.CbozBlockSize = 0;
      CmoInfo.CbopBlockSize = 0;
      return;
    } else {
      CmoInfo.CbomBlockSize = RhctCmoGetBlockSize (fdt32_to_cpu (ReadUnaligned32 ((const UINT32 *)Prop)));
    }

    Prop = fdt_getprop (Fdt, CpuNode, "riscv,cboz-block-size", &Len);
    if (!Prop) {
      DEBUG ((
        DEBUG_VERBOSE,
        "%a: Failed to parse cpu node: riscv,cboz-block-size\n",
        __func__
        ));
      CmoInfo.CbozBlockSize = 0;
    } else {
      CmoInfo.CbozBlockSize = RhctCmoGetBlockSize (fdt32_to_cpu (ReadUnaligned32 ((const UINT32 *)Prop)));
    }

    Prop = fdt_getprop (Fdt, CpuNode, "riscv,cbop-block-size", &Len);
    if (!Prop) {
      DEBUG ((
        DEBUG_VERBOSE,
        "%a: Failed to parse cpu node: riscv,cbop-block-size\n",
        __func__
        ));
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

  @param [in]  Fdt              Pointer to a Flattened Device Tree (Fdt).
  @param [in]  CpuNode          Offset of a cpu node.
  @param [in]  AddressCells     Number of address cells used for the reg
                                property.
  @param [out] RintcInfo         CM_RISCV_RINTC_INFO structure to populate.

  @retval EFI_SUCCESS             The function completed successfully.
  @retval EFI_ABORTED             An error occurred.
  @retval EFI_INVALID_PARAMETER   Invalid parameter.
  @retval EFI_UNSUPPORTED         Unsupported.
**/
STATIC
EFI_STATUS
EFIAPI
CpuNodeParser (
  IN  CONST FDT_HW_INFO_PARSER_HANDLE  FdtParserHandle,
  IN        INT32             CpuNode,
  IN        UINT32            AddressCells,
  OUT       CM_RISCV_RINTC_INFO  *RintcInfo,
  IN UINT64 ImsicCpuBaseAddr,
  IN UINT64 ImsicCpuLen,
  IN BOOLEAN                  IsImsic   
  )
{
  CONST UINT8  *Data;
  INT32        DataSize;
  STATIC UINT32       ProcUid = 0;
  UINT64       HartId = 0;
  VOID *Fdt;

  if (RintcInfo == NULL) {
    ASSERT (0);
    return EFI_INVALID_PARAMETER;
  }

  Fdt          = FdtParserHandle->Fdt;
  Data = fdt_getprop (Fdt, CpuNode, "reg", &DataSize);
  if ((Data == NULL)                  ||
      ((DataSize != sizeof (UINT32))  &&
       (DataSize != sizeof (UINT64))))
  {
    ASSERT (0);
    return EFI_ABORTED;
  }

  if (AddressCells == 2) {
    HartId         = fdt64_to_cpu (*((UINT64 *)Data));
  } else {
    HartId = fdt32_to_cpu (*((UINT32 *)Data));
  }

  RintcInfo->Flags            = EFI_ACPI_6_6_RINTC_FLAG_ENABLE; // REVISIT - check status
  RintcInfo->HartId           = HartId;
  RintcInfo->Version          = 1;
  RintcInfo->AcpiProcessorUid = ProcUid++;
  if (IsImsic == 1) {
    RintcInfo->ExtIntCId = 0;
  } else {
    RintcInfo->ExtIntCId = 0;
  }
  RintcInfo->ImsicBaseAddress = ImsicCpuBaseAddr;
  RintcInfo->ImsicSize = ImsicCpuLen;

  // Presence of ISA will be checked while creating RHCT
  CreateIsaStringInfo(FdtParserHandle, CpuNode);
  CreateCmoInfo(FdtParserHandle, CpuNode);
  CreateTimerInfo(FdtParserHandle, CpuNode);

  return EFI_SUCCESS;
}

/** Parse a "cpus" node and its children "cpu" nodes.

  Create as many CM_RISCV_RINTC_INFO structures as "cpu" nodes.

  @param [in]  Fdt              Pointer to a Flattened Device Tree (Fdt).
  @param [in]  CpusNode         Offset of a cpus node.
  @param [out] NewRintcmObjDesc  If success, CM_OBJ_DESCRIPTOR containing
                                all the created CM_RISCV_RINTC_INFO.

  @retval EFI_SUCCESS             The function completed successfully.
  @retval EFI_ABORTED             An error occurred.
  @retval EFI_INVALID_PARAMETER   Invalid parameter.
  @retval EFI_UNSUPPORTED         Unsupported.
**/
STATIC
EFI_STATUS
EFIAPI
CpusNodeParser (
  IN  CONST FDT_HW_INFO_PARSER_HANDLE  FdtParserHandle,
  IN        INT32              CpusNode,
  OUT       CM_OBJ_DESCRIPTOR  **NewRintcmObjDesc,
  IN BOOLEAN                   IsImsic
  )
{
  EFI_STATUS  Status;
  INT32       CpuNode;
  UINT32      CpuNodeCount;
  INT32       AddressCells;

  UINT32            Index;
  CM_RISCV_RINTC_INFO  *RintcInfoBuffer;
  UINT32            RintcInfoBufferSize;
  VOID *Fdt;

  if (NewRintcmObjDesc == NULL) {
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
               &RintcInfoBuffer[Index],
               0, 0,
               IsImsic
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
             NewRintcmObjDesc
             );
  ASSERT_EFI_ERROR (Status);

exit_handler:
  FreePool (RintcInfoBuffer);
  return Status;
}

STATIC
EFI_STATUS
EFIAPI
ImsicRintcInfoParser (
  IN  CONST FDT_HW_INFO_PARSER_HANDLE  FdtParserHandle,
  IN        INT32              ImsicNode,
  OUT       CM_OBJ_DESCRIPTOR  **NewRintcmObjDesc
  )
{
  EFI_STATUS  Status;
  CM_RISCV_RINTC_INFO  *RintcInfoBuffer;
  CM_RISCV_IMSIC_INFO  ImsicInfo;
  UINT32            RintcInfoBufferSize;
  CONST UINT64      *Prop, *IntExtProp, *ImsicRegProp;
  INT32                                 Len, NumpHandle, CpuNode, pHandle;
  INTN                                  Idx, Idx1, Idx2, Limit;
  UINT64                                ImsicBaseAddr, ImsicBaseLen;
  UINT64                                ImsicCpuBaseAddr, ImsicCpuBaseLen;
  UINTN                                 NumImsicBase;
  VOID *Fdt;

  if (NewRintcmObjDesc == NULL) {
    ASSERT (0);
    return EFI_INVALID_PARAMETER;
  }

  Fdt          = FdtParserHandle->Fdt;
  Prop = fdt_getprop (Fdt, ImsicNode, "riscv,num-ids", &Len);
  if (!Prop) {
    DEBUG ((
      DEBUG_ERROR,
      "%a: Invalid num-ids\n",
      __func__
      ));
    return 0;
  }
  ImsicInfo.NumIds = fdt32_to_cpu (ReadUnaligned32 ((const UINT32 *)Prop));
  Prop = fdt_getprop (Fdt, ImsicNode, "riscv,num-guest-ids", &Len);
  if (!Prop) {
    ImsicInfo.NumGuestIds = 0;
  } else {
    ImsicInfo.NumGuestIds = fdt32_to_cpu (ReadUnaligned32 ((const UINT32 *)Prop));
  }
  Prop = fdt_getprop (Fdt, ImsicNode, "riscv,guest-index-bits", &Len);
  if (!Prop) {
    ImsicInfo.GuestIndexBits = 0;
  } else {
    ImsicInfo.GuestIndexBits = fdt32_to_cpu (ReadUnaligned32 ((const UINT32 *)Prop));
  }
  Prop = fdt_getprop (Fdt, ImsicNode, "riscv,hart-index-bits", &Len);
  if (!Prop) {
    ImsicInfo.HartIndexBits = 0; // update default value later
  } else {
    ImsicInfo.HartIndexBits = fdt32_to_cpu (ReadUnaligned32 ((const UINT32 *)Prop));
  }
  Prop = fdt_getprop (Fdt, ImsicNode, "riscv,group-index-bits", &Len);
  if (!Prop) {
    ImsicInfo.GroupIndexBits = 0;
  } else {
    ImsicInfo.GroupIndexBits = fdt32_to_cpu (ReadUnaligned32 ((const UINT32 *)Prop));
  }
  Prop = fdt_getprop (Fdt, ImsicNode, "riscv,group-index-shift", &Len);
  if (!Prop) {
    ImsicInfo.GroupIndexShift = IMSIC_MMIO_PAGE_SHIFT * 2;
  } else {
    ImsicInfo.GroupIndexShift = fdt32_to_cpu (ReadUnaligned32 ((const UINT32 *)Prop));
  }
  ImsicInfo.Version = 1;
  ImsicInfo.Reserved1 = 0;
  ImsicInfo.Flags = 0;
  Prop = fdt_getprop (Fdt, ImsicNode, "reg", &Len);
  if (!Prop || ((Len / sizeof (UINT32)) % 4) ) {
    // address-cells and size-cells are always 2
    DEBUG ((
      DEBUG_ERROR,
      "%a: Failed to parse ismic node: reg\n",
      __func__
      ));
    return EFI_INVALID_PARAMETER;
  }

  ImsicRegProp = Prop;
  NumImsicBase = (Len / sizeof (UINT32)) / 4;
  IntExtProp = fdt_getprop (Fdt, ImsicNode, "interrupts-extended", &Len);
  if (IntExtProp == 0 || ((Len / sizeof (UINT32)) % 2)) {
    /* interrupts-extended: <phandle flag>, <phandle flag> */
    DEBUG ((
      DEBUG_ERROR,
      "%a: Failed to parse ismic node: interrupts-extended\n",
      __func__
      ));
    return EFI_INVALID_PARAMETER;
  }

  NumpHandle = (Len / sizeof (UINT32)) / 2;
  if (NumpHandle == 0) {
    ASSERT (0);
    return EFI_NOT_FOUND;
  }

  // Allocate memory for NumpHandle CM_RISCV_RINTC_INFO structures.
  RintcInfoBufferSize = NumpHandle * sizeof (CM_RISCV_RINTC_INFO);
  RintcInfoBuffer     = AllocateZeroPool (RintcInfoBufferSize);
  if (RintcInfoBuffer == NULL) {
    ASSERT (0);
    return EFI_OUT_OF_RESOURCES;
  }

  if (ImsicInfo.HartIndexBits == 0) {
    Len = NumpHandle;
    while (Len) {
      ImsicInfo.HartIndexBits++;
      Len = Len >> 1;
    }
  }
  Idx2 = 0;
  for (Idx = 0; Idx < NumImsicBase; Idx++) {
    ImsicBaseAddr = fdt64_to_cpu (ReadUnaligned64 ((const UINT64 *)ImsicRegProp + Idx * 2));
    ImsicBaseLen = fdt64_to_cpu (ReadUnaligned64 ((const UINT64 *)ImsicRegProp + Idx * 2 + 1));
    // Calculate the limit of number of cpu nodes this imsic can handle
    Limit = ImsicBaseLen / ((1 << ImsicInfo.HartIndexBits) * IMSIC_MMIO_PAGE_SZ);
    for (Idx1 = 0; Idx1 < Limit && Idx2 < NumpHandle; Idx1++, Idx2++) {
      pHandle = fdt32_to_cpu (ReadUnaligned32 ((const UINT32 *)IntExtProp + Idx2 * 2));
      CpuNode = fdt_node_offset_by_phandle (Fdt, pHandle);
      if (CpuNode < 0) {
        DEBUG ((
          DEBUG_ERROR,
          "%a: Failed to locate CPU intc phandle: %X\n",
          __func__,
          pHandle
          ));
        return 0;
      }
      CpuNode = fdt_parent_offset (Fdt, CpuNode);
      ASSERT (CpuNode >= 0);
      // Parse the "cpu" node.
      if (!FdtNodeIsCompatible (Fdt, CpuNode, &CpuCompatibleInfo)) {
        ASSERT (0);
        Status = EFI_UNSUPPORTED;
        goto exit_handler;
      }

      ImsicCpuBaseAddr = ImsicBaseAddr + Idx1 * ((1 << ImsicInfo.HartIndexBits) * IMSIC_MMIO_PAGE_SZ);
      ImsicCpuBaseLen = (UINT32)((1 << ImsicInfo.HartIndexBits) * IMSIC_MMIO_PAGE_SZ);
      Status = CpuNodeParser (
                 FdtParserHandle,
                 CpuNode,
                 NumpHandle,
                 &RintcInfoBuffer[Idx2],
                 ImsicCpuBaseAddr,
                 ImsicCpuBaseLen,
                 1
                 );
      if (EFI_ERROR (Status)) {
        ASSERT (0);
        goto exit_handler;
      }
    }
  }

  Status = CreateCmObjDesc (
             CREATE_CM_RISCV_OBJECT_ID (ERiscVObjRintcInfo),
             NumpHandle,
             RintcInfoBuffer,
             RintcInfoBufferSize,
             NewRintcmObjDesc
             );
  ASSERT_EFI_ERROR (Status);

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
        return Status;
      }
exit_handler:
  FreePool (RintcInfoBuffer);
  return Status;
}

/** CM_RISCV_RINTC_INFO and CM_RISCV_IMSIC_INFO parser function.

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
RiscVRintcInfoParser (
  IN  CONST FDT_HW_INFO_PARSER_HANDLE  FdtParserHandle,
  IN        INT32                      FdtBranch
  )
{
  EFI_STATUS         Status;
  INT32              IntcNode, ImsicNode;
  CM_OBJ_DESCRIPTOR  *NewCmObjDesc;
  VOID               *Fdt;

  if (FdtParserHandle == NULL) {
    ASSERT (0);
    return EFI_INVALID_PARAMETER;
  }

  Fdt          = FdtParserHandle->Fdt;
  NewCmObjDesc = NULL;

  Status = FdtGetCompatSmodeNode (Fdt, &ImsicCompatibleInfo, &ImsicNode, TRUE);
  if (EFI_ERROR (Status) && (Status != EFI_NOT_FOUND)) {
    Status = EFI_ABORTED;
    return Status;
  } else if (Status != EFI_NOT_FOUND) {
    Status = ImsicRintcInfoParser(FdtParserHandle, ImsicNode, &NewCmObjDesc);    
  } else {
    // The FdtBranch points to the Cpus Node.
    // Get the interrupt-controller node associated to the "cpus" node.
    Status = FdtGetIntcParentNode (Fdt, FdtBranch, &IntcNode);
    if (EFI_ERROR (Status)) {
      ASSERT (0);
      if (Status == EFI_NOT_FOUND) {
        // Should have found the node.
        Status = EFI_ABORTED;
      }

      return Status;
    }

    // Parse the "cpus" nodes and its children "cpu" nodes,
    // and create a CM_OBJ_DESCRIPTOR.
    Status = CpusNodeParser (FdtParserHandle, FdtBranch, &NewCmObjDesc, 0);
    if (EFI_ERROR (Status)) {
      ASSERT (0);
      return Status;
    }
  }


  // Add all the CmObjs to the Configuration Manager.
  Status = AddMultipleCmObj (FdtParserHandle, NewCmObjDesc, 0, NULL);
  if (EFI_ERROR (Status)) {
    ASSERT (0);
    goto exit_handler;
  }

exit_handler:
  FreeCmObjDesc (NewCmObjDesc);
  return Status;
}
