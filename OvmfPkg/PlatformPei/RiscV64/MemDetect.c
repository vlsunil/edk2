/** @file
  Memory Detection for Virtual Machines.

  Copyright (c) 2021, Hewlett Packard Enterprise Development LP. All rights reserved.<BR>
  Copyright (c) 2006 - 2014, Intel Corporation. All rights reserved.<BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent

Module Name:

  MemDetect.c

**/

//
// The package level header files this module uses
//
#include <PiPei.h>

//
// The Library classes this module consumes
//
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/HobLib.h>
#include <Library/IoLib.h>
#include <Library/PcdLib.h>
#include <Library/PeimEntryPoint.h>
#include <Library/ResourcePublicationLib.h>
#include <Library/BaseRiscVSbiLib.h>

#include <libfdt.h>

#include <Guid/FdtHob.h>

#include "Platform.h"

STATIC EFI_PHYSICAL_ADDRESS  SystemMemoryBase;
STATIC UINT64                SystemMemorySize;
STATIC EFI_PHYSICAL_ADDRESS  MmodeResvBase;
STATIC UINT64                MmodeResvSize;

/**
  Publish PEI core memory.

  @return EFI_SUCCESS     The PEIM initialized successfully.

**/
EFI_STATUS
PublishPeiMemory (
  VOID
  )
{
  EFI_RISCV_FIRMWARE_CONTEXT  *FirmwareContext;
  EFI_PHYSICAL_ADDRESS        MemoryBase;
  CONST UINT64                *RegProp;
  CONST CHAR8                 *Type;
  EFI_STATUS                  Status;
  UINT64                      CurBase, CurSize;
  UINT64                      NewBase = 0, NewSize = 0;
  UINT64                      MemorySize;
  INT32                       Node, Prev;
  INT32                       Len;
  VOID                        *FdtPointer;

  FirmwareContext = NULL;
  GetFirmwareContextPointer (&FirmwareContext);

  if (FirmwareContext == NULL) {
    DEBUG ((DEBUG_ERROR, "%a: Firmware Context is NULL\n", __FUNCTION__));
    return EFI_UNSUPPORTED;
  }

  FdtPointer = (VOID *)FirmwareContext->FlattenedDeviceTree;
  if (FdtPointer == NULL) {
    DEBUG ((DEBUG_ERROR, "%a: Invalid FDT pointer\n", __FUNCTION__));
    return EFI_UNSUPPORTED;
  }

  // Look for the lowest memory node
  for (Prev = 0; ; Prev = Node) {
    Node = fdt_next_node (FdtPointer, Prev, NULL);
    if (Node < 0) {
      break;
    }

    // Check for memory node
    Type = fdt_getprop (FdtPointer, Node, "device_type", &Len);
    if (Type && (AsciiStrnCmp (Type, "memory", Len) == 0)) {
      // Get the 'reg' property of this node. For now, we will assume
      // two 8 byte quantities for base and size, respectively.
      RegProp = fdt_getprop (FdtPointer, Node, "reg", &Len);
      if ((RegProp != 0) && (Len == (2 * sizeof (UINT64)))) {
        CurBase = fdt64_to_cpu (ReadUnaligned64 (RegProp));
        CurSize = fdt64_to_cpu (ReadUnaligned64 (RegProp + 1));

        DEBUG ((
          DEBUG_INFO,
          "%a: System RAM @ 0x%lx - 0x%lx\n",
          __FUNCTION__,
          CurBase,
          CurBase + CurSize - 1
          ));

        if ((NewBase > CurBase) || (NewBase == 0)) {
          NewBase = CurBase;
          NewSize = CurSize;
        }
      } else {
        DEBUG ((
          DEBUG_ERROR,
          "%a: Failed to parse FDT memory node\n",
          __FUNCTION__
          ));
      }
    }
  }

  SystemMemoryBase = NewBase;
  SystemMemorySize = NewSize;

  /* try to locate the reserved memory opensbi node */
  Node = fdt_path_offset (FdtPointer, "/reserved-memory/mmode_resv0");
  if (Node >= 0) {
    RegProp = fdt_getprop (FdtPointer, Node, "reg", &Len);
    if ((RegProp != 0) && (Len == (2 * sizeof (UINT64)))) {
      NewBase = fdt64_to_cpu (ReadUnaligned64 (RegProp));
      NewSize = fdt64_to_cpu (ReadUnaligned64 (RegProp + 1));
      DEBUG ((
        DEBUG_INFO,
        "%a: M-mode Base = 0x%lx, M-mode Size = 0x%lx\n",
        __FUNCTION__,
        NewBase,
        NewSize
        ));
      MmodeResvBase =  NewBase;
      MmodeResvSize = NewSize;
    }
  }

  DEBUG ((
    DEBUG_INFO,
    "%a: SystemMemoryBase:0x%x SystemMemorySize:%x\n",
    __FUNCTION__,
    SystemMemoryBase,
    SystemMemorySize
    ));

  //
  // Initial 16MB needs to be reserved
  //
  MemoryBase = SystemMemoryBase + SIZE_16MB;
  MemorySize = SystemMemorySize - SIZE_16MB;

  //
  // Publish this memory to the PEI Core
  //
  Status = PublishSystemMemory (MemoryBase, MemorySize);
  ASSERT_EFI_ERROR (Status);

  return Status;
}

/**
  Publish system RAM and reserve memory regions.

**/
VOID
InitializeRamRegions (
  VOID
  )
{
  /*
   * M-mode FW can be loaded anywhere in memory but should not overlap
   * with the EDK2. This can happen if some other boot code loads the
   * M-mode firmware.
   *
   * The M-mode firmware memory should be marked as reserved memory
   * so that OS doesn't use it.
   */
  DEBUG ((
    DEBUG_INFO,
    "%a: M-mode FW Memory Start:0x%lx End:0x%lx\n",
    __FUNCTION__,
    MmodeResvBase,
    MmodeResvBase + MmodeResvSize
    ));
  AddReservedMemoryBaseSizeHob (MmodeResvBase, MmodeResvSize);

  if (MmodeResvBase > SystemMemoryBase) {
    DEBUG ((
      DEBUG_INFO,
      "%a: Free Memory Start:0x%lx End:0x%lx\n",
      __FUNCTION__,
      SystemMemoryBase,
      MmodeResvBase
      ));
    AddMemoryRangeHob (SystemMemoryBase, MmodeResvBase);
  }

  DEBUG ((
    DEBUG_INFO,
    "%a: Free Memory Start:0x%lx End:0x%lx\n",
    __FUNCTION__,
    MmodeResvBase + MmodeResvSize,
    SystemMemoryBase + SystemMemorySize
    ));
  AddMemoryRangeHob (
    MmodeResvBase + MmodeResvSize,
    SystemMemoryBase + SystemMemorySize
    );
}
