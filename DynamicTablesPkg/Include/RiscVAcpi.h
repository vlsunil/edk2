/** @file
  ACPI table definition of VTXFPGA platform.

  Copyright (c) 2017, Linaro, Ltd. All rights reserved.<BR>
  Copyright (c) 2022, Ventana Micro Systems Inc. All Rights Reserved.<BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#ifndef ACPI_H_
#define ACPI_H_

#include <RiscVAcpi.h>

#define VTXFPGA_ACPI_HEADER(Signature, Type, Revision) {              \
    Signature,                          /* UINT32  Signature */       \
    sizeof (Type),                      /* UINT32  Length */          \
    Revision,                           /* UINT8   Revision */        \
    0,                                  /* UINT8   Checksum */        \
    { 'V', 'N', 'T', 'A', 'N', 'A' },   /* UINT8   OemId[6] */        \
    0x205245544942524FULL,              /* UINT64  OemTableId */      \
    1,                                  /* UINT32  OemRevision */     \
    0x4E544E56,                         /* UINT32  CreatorId */       \
    1                                   /* UINT32  CreatorRevision */ \
  }

#define EFI_ACPI_6_6_RINTC                    0x18
#define EFI_ACPI_6_6_IMSIC                    0x19
#define EFI_ACPI_6_6_APLIC                    0x1A
#define EFI_ACPI_6_6_PLIC                     0x1B

#define IMSIC_MMIO_PAGE_SHIFT     12
#define IMSIC_MMIO_PAGE_SZ        (1 << IMSIC_MMIO_PAGE_SHIFT)

#pragma pack (1)

///
/// RISC-V Hart Local Interrupt Controller
///
typedef struct {
  UINT8     Type;
  UINT8     Length;
  UINT8     Version;
  UINT8     Reserved1;
  UINT32    Flags;
  UINT64    HartId;
  UINT32    AcpiProcessorUid;
  UINT32    ExtIntCId;
  UINT64    ImsicBaseAddress;
  UINT32    ImsicSize;
} EFI_ACPI_6_6_RINTC_STRUCTURE;

#define EFI_ACPI_6_6_RISCV_RINTC_STRUCTURE_VERSION  1
#define EFI_ACPI_6_6_RINTC_FLAG_ENABLE        1

///
/// RISC-V Incoming MSI Controller
///
typedef struct {
  UINT8     Type;
  UINT8     Length;
  UINT8     Version;
  UINT8     Reserved1;
  UINT32    Flags;
  UINT16    NumIds;
  UINT16    NumGuestIds;
  UINT8     GuestIndexBits;
  UINT8     HartIndexBits;
  UINT8     GroupIndexBits;
  UINT8     GroupIndexShift;
} EFI_ACPI_6_6_IMSIC_STRUCTURE;

#define EFI_ACPI_6_6_RISCV_IMSIC_STRUCTURE_VERSION  1

///
/// RISC-V Advanced Platform Level Interrupt Controller (APLIC)
///
typedef struct {
  UINT8     Type;
  UINT8     Length;
  UINT8     Version;
  UINT8     AplicId;
  UINT32    Flags;
  UINT8     HwId[8];
  UINT16    NumIdcs;
  UINT16    NumSources;
  UINT32    GsiBase;
  UINT64    AplicAddress;
  UINT32    AplicSize;
} EFI_ACPI_6_6_APLIC_STRUCTURE;

#define EFI_ACPI_6_6_RISCV_APLIC_STRUCTURE_VERSION 1
///
/// RISC-V Platform Level Interrupt Controller (PLIC)
///
typedef struct {
  UINT8     Type;
  UINT8     Length;
  UINT8     Version;
  UINT8     PlicId;
  UINT8     HwId[8];
  UINT16    NumSources;
  UINT16    MaxPriority;
  UINT32    Flags;
  UINT32    PlicSize;
  UINT64    PlicAddress;
  UINT32    GsiBase;
} EFI_ACPI_6_6_PLIC_STRUCTURE;

#define EFI_ACPI_RHCT_TYPE_ISA_NODE 0
#define EFI_ACPI_RHCT_TYPE_CMO_NODE 1
#define EFI_ACPI_RHCT_TYPE_MMU_NODE 2
#define EFI_ACPI_RHCT_TYPE_HART_INFO_NODE 65535
#define EFI_ACPI_6_6_RHCT_FLAG_TIMER_CANNOT_WAKE_CPU 0x1

///
/// RISC-V Hart RHCT Node Header Structure
///
typedef struct {
  UINT16    Type;
  UINT16    Length;
  UINT16    Revision;
} EFI_ACPI_6_6_RISCV_RHCT_NODE;

///
/// RISC-V Hart RHCT ISA Node Structure
///
typedef struct {
  EFI_ACPI_6_6_RISCV_RHCT_NODE Node;
  UINT16    IsaLength;
//  CHAR8     *IsaString;
} EFI_ACPI_6_6_RISCV_RHCT_ISA_NODE;

#define EFI_ACPI_6_6_RISCV_RHCT_ISA_NODE_STRUCTURE_VERSION 1

///
/// RISC-V Hart RHCT CMO Node Structure
///
typedef struct {
  EFI_ACPI_6_6_RISCV_RHCT_NODE Node;
  UINT8     Reserved;
  UINT8     CbomBlockSize;
  UINT8     CbopBlockSize;
  UINT8     CbozBlockSize;
} EFI_ACPI_6_6_RISCV_RHCT_CMO_NODE;

#define EFI_ACPI_6_6_RISCV_RHCT_CMO_NODE_STRUCTURE_VERSION 1

///
/// RISC-V Hart RHCT Hart Info Structure
///
typedef struct {
  EFI_ACPI_6_6_RISCV_RHCT_NODE Node;
  UINT16    NumOffsets;
  UINT32    ACPICpuUid;
  UINT32    Offsets[0];
} EFI_ACPI_6_6_RISCV_RHCT_HART_INFO_NODE;

#define EFI_ACPI_6_6_RISCV_RHCT_HART_INFO_NODE_STRUCTURE_VERSION 1

///
/// RISC-V Hart Capabilities Table (RHCT)
///
typedef struct {
  EFI_ACPI_DESCRIPTION_HEADER    Header;
  UINT32                         Flags;
  UINT64                         TimerFreq;
  UINT32                         NumNodes;
  UINT32                         NodeOffset;
} EFI_ACPI_6_6_RISCV_HART_CAPABILITIES_TABLE;

#define EFI_ACPI_6_6_RISCV_RHCT_TABLE_REVISION 1

///
/// "RNCT" Multiple APIC Description Table
///
#define EFI_ACPI_6_6_RISCV_HART_CAPABILITIES_TABLE_SIGNATURE  SIGNATURE_32('R', 'H', 'C', 'T')

#pragma pack ()

/** Helper macro for CPPC _CPC object initialization. Use of this macro is
    restricted to ASL file and not to TDL file.

    @param [in] DesiredPerfReg      Fastchannel address for desired performance
                                    register.
    @param [in] PerfLimitedReg      Fastchannel address for performance limited
                                    register.
    @param [in] GranularityMHz      Granularity of the performance scale.
    @param [in] HighestPerf         Highest performance in linear scale.
    @param [in] NominalPerf         Nominal performance in linear scale.
    @param [in] LowestNonlinearPerf Lowest non-linear performnce in linear
                                    scale.
    @param [in] LowestPerf          Lowest performance in linear scale.
    @param [in] RefPerf             Reference performance in linear scale.
**/
#define CPPC_PACKAGE_INIT(GranularityMHz,                                      \
  HighestPerf, NominalPerf, LowestNonlinearPerf, LowestPerf, RefPerf)          \
  {                                                                            \
    23,                                 /* NumEntries */                       \
    3,                                  /* Revision */                         \
    HighestPerf,                        /* Highest Performance */              \
    NominalPerf,                        /* Nominal Performance */              \
    LowestNonlinearPerf,                /* Lowest Nonlinear Performance */     \
    LowestPerf,                         /* Lowest Performance */               \
    /* Guaranteed Performance Register */                                      \
    ResourceTemplate () { Register (SystemMemory, 0, 0, 0, 0) },               \
    /* Desired Performance Register */                                         \
    ResourceTemplate () { Register (FFixedHW, 32, 0, 0x00000005, 3) },         \
    /* Minimum Performance Register */                                         \
    ResourceTemplate () { Register (SystemMemory, 0, 0, 0, 0) },               \
    /* Maximum Performance Register */                                         \
    ResourceTemplate () { Register (SystemMemory, 0, 0, 0, 0) },               \
    /* Performance Reduction Tolerance Register */                             \
    ResourceTemplate () { Register (SystemMemory, 0, 0, 0, 0) },               \
    /* Time Window Register */                                                 \
    ResourceTemplate () { Register (SystemMemory, 0, 0, 0, 0) },               \
    /* Counter Wraparound Time */                                              \
    ResourceTemplate () { Register (SystemMemory, 0, 0, 0, 0) },               \
    /* Reference Performance Counter Register */                               \
    ResourceTemplate () { Register (FFixedHW, 64, 0, 0x0000000B, 4) },         \
    /* Delivered Performance Counter Register */                               \
    ResourceTemplate () { Register (FFixedHW, 64, 0, 0x0000000C, 4) },         \
    /* Performance Limited Register */                                         \
    ResourceTemplate () { Register (SystemMemory, 0, 0, 0, 0) },               \
    /* CPPC Enable Register */                                                 \
    ResourceTemplate () { Register (SystemMemory, 0, 0, 0, 0) },               \
    /* Autonomous Selection Enable Register */                                 \
    ResourceTemplate () { Register (SystemMemory, 0, 0, 0, 0) },               \
    /* Autonomous Activity Window Register */                                  \
    ResourceTemplate () { Register (SystemMemory, 0, 0, 0, 0) },               \
    /* Energy Performance Preference Register */                               \
    ResourceTemplate () { Register (SystemMemory, 0, 0, 0, 0) },               \
    RefPerf,                            /* Reference Performance */            \
    (LowestPerf * GranularityMHz),      /* Lowest Frequency */                 \
    (NominalPerf * GranularityMHz),     /* Nominal Frequency */                \
  }

// Power state dependency (_PSD) for CPPC

/** Helper macro to initialize Power state dependancy (_PSD) object required
    for CPPC. Use of this macro is restricted to ASL file and not to TDL file.

    @param [in] Domain              The dependency domain number to which this
                                    P-state entry belongs.
**/
#define PSD_INIT(Domain)                                                       \
  {                                                                            \
    5,              /* Entries */                                              \
    0,              /* Revision */                                             \
    Domain,         /* Domain */                                               \
    0xFD,           /* Coord Type- SW_ANY */                                   \
    1               /* Processors */                                           \
  }

// ACPI OSC Status bits
#define OSC_STS_BIT0_RES              (1U << 0)
#define OSC_STS_FAILURE               (1U << 1)
#define OSC_STS_UNRECOGNIZED_UUID     (1U << 2)
#define OSC_STS_UNRECOGNIZED_REV      (1U << 3)
#define OSC_STS_CAPABILITY_MASKED     (1U << 4)
#define OSC_STS_MASK                  (OSC_STS_BIT0_RES          | \
                                       OSC_STS_FAILURE           | \
                                       OSC_STS_UNRECOGNIZED_UUID | \
                                       OSC_STS_UNRECOGNIZED_REV  | \
                                       OSC_STS_CAPABILITY_MASKED)

// ACPI OSC for Platform-Wide Capability
#define OSC_CAP_CPPC_SUPPORT          (1U << 5)
#define OSC_CAP_CPPC2_SUPPORT         (1U << 6)
#define OSC_CAP_PLAT_COORDINATED_LPI  (1U << 7)
#define OSC_CAP_OS_INITIATED_LPI      (1U << 8)


#endif /* ACPI_H_ */
