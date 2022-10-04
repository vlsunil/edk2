/** @file
  RISC-V SEC phase module for Qemu Virt.

  Copyright (c) 2008 - 2015, Intel Corporation. All rights reserved.<BR>
  Copyright (c) 2022, Ventana Micro Systems Inc. All rights reserved.<BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "SecMain.h"

EFI_STATUS
EFIAPI
TemporaryRamMigration (
  IN CONST EFI_PEI_SERVICES  **PeiServices,
  IN EFI_PHYSICAL_ADDRESS    TemporaryMemoryBase,
  IN EFI_PHYSICAL_ADDRESS    PermanentMemoryBase,
  IN UINTN                   CopySize
  );

EFI_STATUS
EFIAPI
TemporaryRamDone (
  VOID
  );

STATIC EFI_PEI_TEMPORARY_RAM_SUPPORT_PPI  mTemporaryRamSupportPpi = {
  TemporaryRamMigration
};

STATIC EFI_PEI_TEMPORARY_RAM_DONE_PPI  mTemporaryRamDonePpi = {
  TemporaryRamDone
};

STATIC EFI_PEI_PPI_DESCRIPTOR  mPrivateDispatchTable[] = {
  {
    EFI_PEI_PPI_DESCRIPTOR_PPI,
    &gEfiTemporaryRamSupportPpiGuid,
    &mTemporaryRamSupportPpi
  },
  {
    (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
    &gEfiTemporaryRamDonePpiGuid,
    &mTemporaryRamDonePpi
  },
};

/** Temporary RAM migration function.

  This function migrates the data from temporary RAM to permanent
  memory.

  @param[in]  PeiServices           PEI service
  @param[in]  TemporaryMemoryBase   Temporary memory base address
  @param[in]  PermanentMemoryBase   Permanent memory base address
  @param[in]  CopySize              Size to copy

**/
EFI_STATUS
EFIAPI
TemporaryRamMigration (
  IN CONST EFI_PEI_SERVICES  **PeiServices,
  IN EFI_PHYSICAL_ADDRESS    TemporaryMemoryBase,
  IN EFI_PHYSICAL_ADDRESS    PermanentMemoryBase,
  IN UINTN                   CopySize
  )
{
  VOID                        *OldHeap;
  VOID                        *NewHeap;
  VOID                        *OldStack;
  VOID                        *NewStack;
  EFI_RISCV_FIRMWARE_CONTEXT  *FirmwareContext;

  DEBUG ((
    DEBUG_INFO,
    "%a: Temp Mem Base:0x%Lx, Permanent Mem Base:0x%Lx, CopySize:0x%Lx\n",
    __FUNCTION__,
    TemporaryMemoryBase,
    PermanentMemoryBase,
    (UINT64)CopySize
    ));

  OldHeap = (VOID *)(UINTN)TemporaryMemoryBase;
  NewHeap = (VOID *)((UINTN)PermanentMemoryBase + (CopySize >> 1));

  OldStack = (VOID *)((UINTN)TemporaryMemoryBase + (CopySize >> 1));
  NewStack = (VOID *)(UINTN)PermanentMemoryBase;

  CopyMem (NewHeap, OldHeap, CopySize >> 1);   // Migrate Heap
  CopyMem (NewStack, OldStack, CopySize >> 1); // Migrate Stack

  //
  // Reset firmware context pointer
  //
  GetFirmwareContextPointer (&FirmwareContext);
  FirmwareContext = (VOID *)FirmwareContext + (unsigned long)((UINTN)NewStack - (UINTN)OldStack);
  SetFirmwareContextPointer (FirmwareContext);

  DEBUG ((DEBUG_INFO, "%a: Firmware Context is relocated to 0x%x\n", __FUNCTION__, FirmwareContext));

  register UINTN  a0 asm ("a0") = (UINTN)((UINTN)NewStack - (UINTN)OldStack);

  asm volatile ("add sp, sp, a0"::"r"(a0):);
  return EFI_SUCCESS;
}

/** Temprary RAM done function.

**/
EFI_STATUS EFIAPI
TemporaryRamDone (
  VOID
  )
{
  DEBUG ((DEBUG_INFO, "%a: 2nd time PEI core, temporary ram done.\n", __FUNCTION__));
  return EFI_SUCCESS;
}

/** Return platform SEC PPI before PEI Core

  @param[in,out]  ThisPpiList   Pointer to retrieve EFI_PEI_PPI_DESCRIPTOR.

**/
STATIC EFI_STATUS
GetPlatformPrePeiCorePpiDescriptor (
  IN OUT EFI_PEI_PPI_DESCRIPTOR  **ThisPpiList
  )
{
  *ThisPpiList = mPrivateDispatchTable;
  return EFI_SUCCESS;
}

/**
  Locates the main boot firmware volume.

  @param[in,out]  BootFv  On input, the base of the BootFv
                          On output, the decompressed main firmware volume

  @retval EFI_SUCCESS    The main firmware volume was located and decompressed
  @retval EFI_NOT_FOUND  The main firmware volume was not found

**/
EFI_STATUS
FindMainFv (
  IN OUT  EFI_FIRMWARE_VOLUME_HEADER  **BootFv
  )
{
  EFI_FIRMWARE_VOLUME_HEADER  *Fv;
  UINTN                       Distance;

  ASSERT (((UINTN)*BootFv & EFI_PAGE_MASK) == 0);

  Fv       = *BootFv;
  Distance = (UINTN)(*BootFv)->FvLength;
  do {
    Fv        = (EFI_FIRMWARE_VOLUME_HEADER *)((UINT8 *)Fv + EFI_PAGE_SIZE);
    Distance += EFI_PAGE_SIZE;
    if (Distance > SIZE_32MB) {
      return EFI_NOT_FOUND;
    }

    if (Fv->Signature != EFI_FVH_SIGNATURE) {
      continue;
    }

    if ((UINTN)Fv->FvLength < Distance) {
      continue;
    }

    *BootFv = Fv;
    return EFI_SUCCESS;
  } while (TRUE);
}

/**
  Locates the compressed main firmware volume and decompresses it.

  @param[in,out]  Fv            On input, the firmware volume to search
                                On output, the decompressed BOOT/PEI FV

  @retval EFI_SUCCESS           The file and section was found
  @retval EFI_NOT_FOUND         The file and section was not found
  @retval EFI_VOLUME_CORRUPTED  The firmware volume was corrupted

**/
EFI_STATUS
DecompressMemFvs (
  IN OUT EFI_FIRMWARE_VOLUME_HEADER  **Fv
  )
{
  EFI_STATUS                  Status;
  EFI_GUID_DEFINED_SECTION    *Section;
  UINT32                      OutputBufferSize;
  UINT32                      ScratchBufferSize;
  UINT16                      SectionAttribute;
  UINT32                      AuthenticationStatus;
  VOID                        *OutputBuffer;
  VOID                        *ScratchBuffer;
  EFI_COMMON_SECTION_HEADER   *FvSection;
  EFI_FIRMWARE_VOLUME_HEADER  *PeiMemFv;
  EFI_FIRMWARE_VOLUME_HEADER  *DxeMemFv;
  UINT32                      FvHeaderSize;
  UINT32                      FvSectionSize;

  FvSection = (EFI_COMMON_SECTION_HEADER *)NULL;

  Status = FindFfsFileAndSection (
             *Fv,
             EFI_FV_FILETYPE_FIRMWARE_VOLUME_IMAGE,
             EFI_SECTION_GUID_DEFINED,
             (EFI_COMMON_SECTION_HEADER **)&Section
             );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Unable to find GUID defined section\n"));
    return Status;
  }

  Status = ExtractGuidedSectionGetInfo (
             Section,
             &OutputBufferSize,
             &ScratchBufferSize,
             &SectionAttribute
             );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Unable to GetInfo for GUIDed section\n"));
    return Status;
  }

  OutputBuffer  = (VOID *)((UINT8 *)(UINTN)PcdGet32 (PcdOvmfDxeMemFvBase) + SIZE_1MB);
  ScratchBuffer = ALIGN_POINTER ((UINT8 *)OutputBuffer + OutputBufferSize, SIZE_1MB);

  Status = ExtractGuidedSectionDecode (
             Section,
             &OutputBuffer,
             ScratchBuffer,
             &AuthenticationStatus
             );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Error during GUID section decode\n"));
    return Status;
  }

  Status = FindFfsSectionInstance (
             OutputBuffer,
             OutputBufferSize,
             EFI_SECTION_FIRMWARE_VOLUME_IMAGE,
             0,
             &FvSection
             );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Unable to find PEI FV section\n"));
    return Status;
  }

  ASSERT (
    SECTION_SIZE (FvSection) ==
    (PcdGet32 (PcdOvmfPeiMemFvSize) + sizeof (*FvSection))
    );
  ASSERT (FvSection->Type == EFI_SECTION_FIRMWARE_VOLUME_IMAGE);

  PeiMemFv = (EFI_FIRMWARE_VOLUME_HEADER *)(UINTN)PcdGet32 (PcdOvmfPeiMemFvBase);
  CopyMem (PeiMemFv, (VOID *)(FvSection + 1), PcdGet32 (PcdOvmfPeiMemFvSize));

  if (PeiMemFv->Signature != EFI_FVH_SIGNATURE) {
    DEBUG ((DEBUG_ERROR, "Extracted FV at %p does not have FV header signature\n", PeiMemFv));
    CpuDeadLoop ();
    return EFI_VOLUME_CORRUPTED;
  }

  Status = FindFfsSectionInstance (
             OutputBuffer,
             OutputBufferSize,
             EFI_SECTION_FIRMWARE_VOLUME_IMAGE,
             1,
             &FvSection
             );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Unable to find DXE FV section\n"));
    return Status;
  }

  ASSERT (FvSection->Type == EFI_SECTION_FIRMWARE_VOLUME_IMAGE);

  if (IS_SECTION2 (FvSection)) {
    FvSectionSize = SECTION2_SIZE (FvSection);
    FvHeaderSize  = sizeof (EFI_COMMON_SECTION_HEADER2);
  } else {
    FvSectionSize = SECTION_SIZE (FvSection);
    FvHeaderSize  = sizeof (EFI_COMMON_SECTION_HEADER);
  }

  ASSERT (FvSectionSize == (PcdGet32 (PcdOvmfDxeMemFvSize) + FvHeaderSize));

  DxeMemFv = (EFI_FIRMWARE_VOLUME_HEADER *)(UINTN)PcdGet32 (PcdOvmfDxeMemFvBase);
  CopyMem (DxeMemFv, (VOID *)((UINTN)FvSection + FvHeaderSize), PcdGet32 (PcdOvmfDxeMemFvSize));

  if (DxeMemFv->Signature != EFI_FVH_SIGNATURE) {
    DEBUG ((DEBUG_ERROR, "Extracted FV at %p does not have FV header signature\n", DxeMemFv));
    CpuDeadLoop ();
    return EFI_VOLUME_CORRUPTED;
  }

  *Fv = PeiMemFv;
  return EFI_SUCCESS;
}

/**
  Locates the PEI Core entry point address

  @param[in,out]  Fv                 The firmware volume to search
  @param[out]     PeiCoreEntryPoint  The entry point of the PEI Core image

  @retval EFI_SUCCESS           The file and section was found
  @retval EFI_NOT_FOUND         The file and section was not found
  @retval EFI_VOLUME_CORRUPTED  The firmware volume was corrupted

**/
VOID
FindPeiCoreImageBase (
  IN OUT  EFI_FIRMWARE_VOLUME_HEADER  **BootFv,
  OUT  EFI_PHYSICAL_ADDRESS           *PeiCoreImageBase
  )
{
  *PeiCoreImageBase = 0;

  FindMainFv (BootFv);

  DecompressMemFvs (BootFv);

  FindPeiCoreImageBaseInFv (*BootFv, PeiCoreImageBase);
}

/**
  Find core image base.

**/
EFI_STATUS
FindImageBase (
  IN  EFI_FIRMWARE_VOLUME_HEADER  *BootFirmwareVolumePtr,
  OUT EFI_PHYSICAL_ADDRESS        *SecCoreImageBase
  )
{
  EFI_PHYSICAL_ADDRESS       CurrentAddress;
  EFI_PHYSICAL_ADDRESS       EndOfFirmwareVolume;
  EFI_FFS_FILE_HEADER        *File;
  UINT32                     Size;
  EFI_PHYSICAL_ADDRESS       EndOfFile;
  EFI_COMMON_SECTION_HEADER  *Section;
  EFI_PHYSICAL_ADDRESS       EndOfSection;

  *SecCoreImageBase = 0;

  CurrentAddress      = (EFI_PHYSICAL_ADDRESS)(UINTN)BootFirmwareVolumePtr;
  EndOfFirmwareVolume = CurrentAddress + BootFirmwareVolumePtr->FvLength;

  //
  // Loop through the FFS files in the Boot Firmware Volume
  //
  for (EndOfFile = CurrentAddress + BootFirmwareVolumePtr->HeaderLength; ; ) {
    CurrentAddress = (EndOfFile + 7) & 0xfffffffffffffff8ULL;
    if (CurrentAddress > EndOfFirmwareVolume) {
      return EFI_NOT_FOUND;
    }

    File = (EFI_FFS_FILE_HEADER *)(UINTN)CurrentAddress;
    Size = FFS_FILE_SIZE (File);
    if (Size < sizeof (*File)) {
      return EFI_NOT_FOUND;
    }

    EndOfFile = CurrentAddress + Size;
    if (EndOfFile > EndOfFirmwareVolume) {
      return EFI_NOT_FOUND;
    }

    //
    // Look for SEC Core
    //
    if (File->Type != EFI_FV_FILETYPE_SECURITY_CORE) {
      continue;
    }

    //
    // Loop through the FFS file sections within the FFS file
    //
    EndOfSection = (EFI_PHYSICAL_ADDRESS)(UINTN)(File + 1);
    for ( ; ;) {
      CurrentAddress = (EndOfSection + 3) & 0xfffffffffffffffcULL;
      Section        = (EFI_COMMON_SECTION_HEADER *)(UINTN)CurrentAddress;

      Size = SECTION_SIZE (Section);
      if (Size < sizeof (*Section)) {
        return EFI_NOT_FOUND;
      }

      EndOfSection = CurrentAddress + Size;
      if (EndOfSection > EndOfFile) {
        return EFI_NOT_FOUND;
      }

      //
      // Look for executable sections
      //
      if ((Section->Type == EFI_SECTION_PE32) || (Section->Type == EFI_SECTION_TE)) {
        if (File->Type == EFI_FV_FILETYPE_SECURITY_CORE) {
          *SecCoreImageBase = (PHYSICAL_ADDRESS)(UINTN)(Section + 1);
        }

        break;
      }
    }

    //
    // SEC Core image found
    //
    if (*SecCoreImageBase != 0) {
      return EFI_SUCCESS;
    }
  }
}

/*
  Find and return Pei Core entry point.

  It also find SEC and PEI Core file debug information. It will report them if
  remote debug is enabled.

**/
VOID
FindAndReportEntryPoints (
  IN  EFI_FIRMWARE_VOLUME_HEADER  **BootFirmwareVolumePtr,
  OUT EFI_PEI_CORE_ENTRY_POINT    *PeiCoreEntryPoint
  )
{
  EFI_STATUS                    Status;
  EFI_PHYSICAL_ADDRESS          SecCoreImageBase;
  EFI_PHYSICAL_ADDRESS          PeiCoreImageBase;
  PE_COFF_LOADER_IMAGE_CONTEXT  ImageContext;

  //
  // Find SEC Core and PEI Core image base
  //
  Status = FindImageBase (*BootFirmwareVolumePtr, &SecCoreImageBase);
  ASSERT_EFI_ERROR (Status);

  FindPeiCoreImageBase (BootFirmwareVolumePtr, &PeiCoreImageBase);

  ZeroMem ((VOID *)&ImageContext, sizeof (PE_COFF_LOADER_IMAGE_CONTEXT));
  //
  // Report SEC Core debug information when remote debug is enabled
  //
  ImageContext.ImageAddress = SecCoreImageBase;
  ImageContext.PdbPointer   = PeCoffLoaderGetPdbPointer ((VOID *)(UINTN)ImageContext.ImageAddress);
  PeCoffLoaderRelocateImageExtraAction (&ImageContext);

  //
  // Report PEI Core debug information when remote debug is enabled
  //
  ImageContext.ImageAddress = (EFI_PHYSICAL_ADDRESS)(UINTN)PeiCoreImageBase;
  ImageContext.PdbPointer   = PeCoffLoaderGetPdbPointer ((VOID *)(UINTN)ImageContext.ImageAddress);
  PeCoffLoaderRelocateImageExtraAction (&ImageContext);

  //
  // Find PEI Core entry point
  //
  Status = PeCoffLoaderGetEntryPoint ((VOID *)(UINTN)PeiCoreImageBase, (VOID **)PeiCoreEntryPoint);
  if (EFI_ERROR (Status)) {
    *PeiCoreEntryPoint = 0;
  }

  return;
}

/**

  Entry point to the C language phase of SEC. After the SEC assembly
  code has initialized some temporary memory and set up the stack,
  the control is transferred to this function.


  @param[in]  BootHartId         Hardware thread ID of boot hart.
  @param[in]  DeviceTreeAddress  Pointer to Device Tree (DTB)
**/
VOID
NORETURN
EFIAPI
SecStartup (
  IN  UINTN  BootHartId,
  IN  VOID   *DeviceTreeAddress
  )
{
  EFI_RISCV_FIRMWARE_CONTEXT  FirmwareContext;
  EFI_FIRMWARE_VOLUME_HEADER  *BootFv;
  EFI_PEI_CORE_ENTRY_POINT    PeiCoreEntryPoint;
  EFI_PEI_PPI_DESCRIPTOR      *PpiList;
  EFI_SEC_PEI_HAND_OFF        SecCoreData;
  EFI_STATUS                  Status;

  //
  // Report Status Code to indicate entering SEC core
  //
  DEBUG ((
    DEBUG_INFO,
    "%a() BootHartId: 0x%x, DeviceTreeAddress=0x%x\n",
    __FUNCTION__,
    BootHartId,
    DeviceTreeAddress
    ));

  //
  // Process all libraries constructor function linked to SecCore.
  //
  ProcessLibraryConstructorList ();

  BootFv = (EFI_FIRMWARE_VOLUME_HEADER *)FixedPcdGet32 (PcdOvmfFdBaseAddress);

  ASSERT (BootFv != NULL);
  SecCoreData.DataSize               = (UINT16)sizeof (EFI_SEC_PEI_HAND_OFF);
  SecCoreData.BootFirmwareVolumeBase = BootFv;
  SecCoreData.BootFirmwareVolumeSize = (UINTN)BootFv->FvLength;
  SecCoreData.TemporaryRamBase       = (VOID *)(UINT64)FixedPcdGet32 (PcdOvmfSecPeiTempRamBase);
  SecCoreData.TemporaryRamSize       = (UINTN)FixedPcdGet32 (PcdOvmfSecPeiTempRamSize);
  SecCoreData.PeiTemporaryRamBase    = SecCoreData.TemporaryRamBase;
  SecCoreData.PeiTemporaryRamSize    = SecCoreData.TemporaryRamSize >> 1;
  SecCoreData.StackBase              = (UINT8 *)SecCoreData.TemporaryRamBase + (SecCoreData.TemporaryRamSize >> 1);
  SecCoreData.StackSize              = SecCoreData.TemporaryRamSize >> 1;

  DEBUG ((
    DEBUG_INFO,
    "%a() BFV Base: 0x%x, BFV Size: 0x%x, TempRAM Base: 0x%x, TempRAM Size: 0x%x, PeiTempRamBase: 0x%x, PeiTempRamSize: 0x%x, StackBase: 0x%x, StackSize: 0x%x\n",
    __FUNCTION__,
    SecCoreData.BootFirmwareVolumeBase,
    SecCoreData.BootFirmwareVolumeSize,
    SecCoreData.TemporaryRamBase,
    SecCoreData.TemporaryRamSize,
    SecCoreData.PeiTemporaryRamBase,
    SecCoreData.PeiTemporaryRamSize,
    SecCoreData.StackBase,
    SecCoreData.StackSize
    ));

  FindAndReportEntryPoints (
    &BootFv,
    &PeiCoreEntryPoint
    );
  if (PeiCoreEntryPoint == NULL) {
    CpuDeadLoop ();
  }

  SecCoreData.BootFirmwareVolumeBase = BootFv;
  SecCoreData.BootFirmwareVolumeSize = (UINTN)BootFv->FvLength;

  Status = GetPlatformPrePeiCorePpiDescriptor (&PpiList);
  if (EFI_ERROR (Status)) {
    PpiList = NULL;
  }

  FirmwareContext.BootHartId          = BootHartId;
  FirmwareContext.FlattenedDeviceTree = (UINT64)DeviceTreeAddress;
  SetFirmwareContextPointer (&FirmwareContext);

  //
  // Transfer the control to the PEI core
  //
  ASSERT (PeiCoreEntryPoint != NULL);
  (*PeiCoreEntryPoint)(&SecCoreData, PpiList);

  //
  // Should not come here.
  //
  UNREACHABLE ();
}
