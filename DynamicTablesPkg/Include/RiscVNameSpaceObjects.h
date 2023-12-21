/** @file

  Copyright (c) 2023, Ventana Micro Systems Inc. All rights reserved.<BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent

  @par Glossary:
    - Cm or CM   - Configuration Manager
    - Obj or OBJ - Object
    - Std or STD - Standard
**/

#ifndef RISCV_NAMESPACE_OBJECTS_H_
#define RISCV_NAMESPACE_OBJECTS_H_

#include <AmlCpcInfo.h>
#include <StandardNameSpaceObjects.h>

#pragma pack(1)

/** The ERISCV_OBJECT_ID enum describes the Object IDs
    in the RISCV Namespace
*/
typedef enum RiscVObjectID {
  ERiscVObjReserved,                                             ///<  0 - Reserved
  ERiscVObjCpuInfo,                                              ///<  2 - CPU Info
  ERiscVObjPowerManagementProfileInfo,                           ///<  3 - Power Management Profile Info
  ERiscVObjRintcInfo,                                            ///<  4 - RINTC Info
  ERiscVObjImsicInfo,                                            ///<  5 - IMSIC Info
  ERiscVObjAplicInfo,                                            ///<  6 - APLIC Frame Info
  ERiscVObjPlicInfo,                                             ///<  7 - PLIC Info
  ERiscVObjIsaStringInfo,                                        ///<  8 - Pcc Subspace Type 5 Info
  ERiscVObjCmoInfo,                                              ///<  9 - Pcc Subspace Type 5 Info
  ERiscVObjTimerInfo,                                            ///< 10 - Pcc Subspace Type 5 Info
  ERiscVObjSerialConsolePortInfo,                                ///< 11 - Serial Console Port Info
  ERiscVObjSerialDebugPortInfo,                                  ///< 12 - Serial Debug Port Info
  ERiscVObjPciConfigSpaceInfo,                                   ///< 13 - PCI Configuration Space Info
  ERiscVObjHypervisorVendorIdentity,                             ///< 14 - Hypervisor Vendor Id
  ERiscVObjFixedFeatureFlags,                                    ///< 15 - Fixed feature flags for FADT
  ERiscVObjProcHierarchyInfo,                                    ///< 16 - Processor Hierarchy Info
  ERiscVObjCmRef,                                                ///< 17 - CM Object Reference
  ERiscVObjSerialPortInfo,                                       ///< 18 - Generic Serial Port Info
  ERiscVObjLpiInfo,                                              ///< 19 - Lpi Info
  ERiscVObjPciAddressMapInfo,                                    ///< 20 - Pci Address Map Info
  ERiscVObjPciInterruptMapInfo,                                  ///< 21 - Pci Interrupt Map Info
  ERiscVObjCpcInfo,                                              ///< 22 - Continuous Performance Control Info
  ERiscVObjMax
} ERISCV_OBJECT_ID;

/** A structure that describes the
    RISCV Boot Architecture flags.

    ID: ERiscVObjBootArchInfo
*/
typedef struct CmRiscVBootArchInfo {
  /** This is the RISCV_BOOT_ARCH flags field of the FADT Table
      described in the ACPI Table Specification.
  */
  UINT16    BootArchFlags;
} CM_RISCV_BOOT_ARCH_INFO;

/** A structure that describes the
    Power Management Profile Information for the Platform.

    ID: ERiscVObjPowerManagementProfileInfo
*/
typedef struct CmRiscVPowerManagementProfileInfo {
  /** This is the Preferred_PM_Profile field of the FADT Table
      described in the ACPI Specification
  */
  UINT8    PowerManagementProfile;
} CM_RISCV_POWER_MANAGEMENT_PROFILE_INFO;

/** A structure that describes the
    RINTC for the Platform.

    ID: ERiscVObjRintcInfo
*/
typedef struct CmRiscVRintcInfo {
  /// Version
  UINT8             Version;

  /// Reserved1
  UINT8             Reserved1;

  /** The flags field as described by the RINTC structure
      in the ACPI Specification.
  */
  UINT32             Flags;

  // Hart ID
  UINT64             HartId;

  /** The ACPI Processor UID. This must match the
      _UID of the CPU Device object information described
      in the DSDT/SSDT for the CPU.
  */
  UINT32             AcpiProcessorUid;

  // External Interrupt Controller ID
  UINT32             ExtIntCId;

  // IMSIC Base address
  UINT64             ImsicBaseAddress;

  // IMSIC Size
  UINT32             ImsicSize;

  /** Optional field: Reference Token for the Cpc info of this processor.
      i.e. a token referencing a CM_RISCV_CPC_INFO object.
  */
  CM_OBJECT_TOKEN    CpcToken;

  /** Optional field: Reference Token for the Embedded Trace device info for
      this processing element.
      i.e. a token referencing a CM_RISCV_ET_INFO object.
  */
  CM_OBJECT_TOKEN    EtToken;
} CM_RISCV_RINTC_INFO;

/** A structure that describes the
    IMSIC information for the Platform.

    ID: ERiscVObjImsicInfo
*/
typedef struct CmRiscVImsicInfo {
  /// Version
  UINT8             Version;

  /// Reserved1
  UINT8             Reserved1;

  /** The flags field as described by the IMSIC structure
      in the ACPI Specification.
  */
  UINT32             Flags;

  // Number of S-mode Interrupt Identities
  UINT16             NumIds;

  // Number of guest mode Interrupt Identities
  UINT16             NumGuestIds;

  // Guest Index Bits
  UINT8             GuestIndexBits;

  // Hart Index Bits
  UINT8             HartIndexBits;

  // Group Index Bits
  UINT8             GroupIndexBits;

  // Group Index Shift
  UINT8             GroupIndexShift;
} CM_RISCV_IMSIC_INFO;

/** A structure that describes the
    APLIC information for the Platform.

    ID: ERiscVObjAplicInfo
*/
typedef struct CmRiscVAplicInfo {
  /// Version
  UINT8             Version;

  /// APLIC ID
  UINT8             AplicId;

  /** The flags field as described by the APLIC structure
      in the ACPI Specification.
  */
  UINT32             Flags;

  /// Hardware ID
  UINT8             HwId[8];

  // Number of IDCs
  UINT16             NumIdcs;

  // Number of Interrupt Sources
  UINT16             NumSources;

  /// GSI Base
  UINT32             GsiBase;

  /// APLIC Address
  UINT64             AplicAddress;

  /// APLIC size
  UINT32             AplicSize;
} CM_RISCV_APLIC_INFO;

/** A structure that describes the
    PLIC information for the Platform.

    ID: ERiscVObjPlicInfo
*/
typedef struct CmRiscVPlicInfo {
  /// Version
  UINT8             Version;

  /// PLIC ID
  UINT8             PlicId;

  /// Hardware ID
  UINT8             HwId[8];

  // Number of Interrupt Sources
  UINT16             NumSources;

  // Max Priority
  UINT16             MaxPriority;

  /** The flags field as described by the PLIC structure
      in the ACPI Specification.
  */
  UINT32             Flags;

  /// PLIC Size
  UINT32             PlicSize;

  /// PLIC Address
  UINT64             PlicAddress;

  /// GSI Base
  UINT32             GsiBase;
} CM_RISCV_PLIC_INFO;

/** A structure that describes the
    ISA string for the Platform.

    ID: ERiscVObjIsaStringInfo
*/
typedef struct CmRiscVIsaStringInfo {

  UINT16 Length;

  CHAR8 *IsaString;

} CM_RISCV_ISA_STRING_NODE;

/** A structure that describes the
    CMO for the Platform.

    ID: ERiscVObjCmoInfo
*/
typedef struct CmRiscVCmoInfo {
  /// CbomBlockSize
  UINT8             CbomBlockSize;

  /// CbopBlockSize
  UINT8             CbopBlockSize;

  /// CbozBlockSize
  UINT8             CbozBlockSize;
} CM_RISCV_CMO_NODE;

/** A structure that describes the
    Timer for the Platform.

    ID: ERiscVObjTimerInfo
*/
typedef struct CmRiscVTimerInfo {
  UINT8             TimerCannotWakeCpu;

  UINT64              TimeBaseFrequency;

} CM_RISCV_TIMER_INFO;

/** A structure that describes the
    Serial Port information for the Platform.

    ID: ERiscVObjSerialConsolePortInfo or
        ERiscVObjSerialDebugPortInfo or
        ERiscVObjSerialPortInfo
*/
typedef struct CmRiscVSerialPortInfo {
  /// The physical base address for the serial port
  UINT64    BaseAddress;

  /// The serial port interrupt
  UINT32    Interrupt;

  /// The serial port baud rate
  UINT64    BaudRate;

  /// The serial port clock
  UINT32    Clock;

  /// Serial Port subtype
  UINT16    PortSubtype;

  /// The Base address length
  UINT64    BaseAddressLength;

  /// The access size
  UINT8     AccessSize;
} CM_RISCV_SERIAL_PORT_INFO;

/** A structure that describes the
    PCI Configuration Space information for the Platform.

    ID: ERiscVObjPciConfigSpaceInfo
*/
typedef struct CmRiscVPciConfigSpaceInfo {
  /// The physical base address for the PCI segment
  UINT64             BaseAddress;

  /// The PCI segment group number
  UINT16             PciSegmentGroupNumber;

  /// The start bus number
  UINT8              StartBusNumber;

  /// The end bus number
  UINT8              EndBusNumber;

  /// Optional field: Reference Token for address mapping.
  /// Token identifying a CM_RISCV_OBJ_REF structure.
  CM_OBJECT_TOKEN    AddressMapToken;

  /// Optional field: Reference Token for interrupt mapping.
  /// Token identifying a CM_RISCV_OBJ_REF structure.
  CM_OBJECT_TOKEN    InterruptMapToken;
} CM_RISCV_PCI_CONFIG_SPACE_INFO;

/** A structure that describes the
    Hypervisor Vendor ID information for the Platform.

    ID: ERiscVObjHypervisorVendorIdentity
*/
typedef struct CmRiscVHypervisorVendorId {
  /// The hypervisor Vendor ID
  UINT64    HypervisorVendorId;
} CM_RISCV_HYPERVISOR_VENDOR_ID;

/** A structure that describes the
    Fixed feature flags for the Platform.

    ID: ERiscVObjFixedFeatureFlags
*/
typedef struct CmRiscVFixedFeatureFlags {
  /// The Fixed feature flags
  UINT32    Flags;
} CM_RISCV_FIXED_FEATURE_FLAGS;

/** A structure that describes the RiscV
    Generic Interrupts.
*/
typedef struct CmRiscVGenericInterrupt {
  /// Interrupt number
  UINT32    Interrupt;

  /// Flags
  /// BIT0: 0: Interrupt is Level triggered
  ///       1: Interrupt is Edge triggered
  /// BIT1: 0: Interrupt is Active high
  ///       1: Interrupt is Active low
  UINT32    Flags;
} CM_RISCV_GENERIC_INTERRUPT;

/** A structure that describes the Processor Hierarchy Node (Type 0) in PPTT

    ID: ERiscVObjProcHierarchyInfo
*/
typedef struct CmRiscVProcHierarchyInfo {
  /// A unique token used to identify this object
  CM_OBJECT_TOKEN    Token;
  /// Processor structure flags (ACPI 6.3 - January 2019, PPTT, Table 5-155)
  UINT32             Flags;
  /// Token for the parent CM_RISCV_PROC_HIERARCHY_INFO object in the processor
  /// topology. A value of CM_NULL_TOKEN means this node has no parent.
  CM_OBJECT_TOKEN    ParentToken;
  /// Token of the associated CM_RISCV_GICC_INFO object which has the
  /// corresponding ACPI Processor ID. A value of CM_NULL_TOKEN means this
  /// node represents a group of associated processors and it does not have an
  /// associated GIC CPU interface.
  CM_OBJECT_TOKEN    RintcToken;
  /// Number of resources private to this Node
  UINT32             NoOfPrivateResources;
  /// Token of the array which contains references to the resources private to
  /// this CM_RISCV_PROC_HIERARCHY_INFO instance. This field is ignored if
  /// the NoOfPrivateResources is 0, in which case it is recommended to set
  /// this field to CM_NULL_TOKEN.
  CM_OBJECT_TOKEN    PrivateResourcesArrayToken;
  /// Optional field: Reference Token for the Lpi state of this processor.
  /// Token identifying a CM_RISCV_OBJ_REF structure, itself referencing
  /// CM_RISCV_LPI_INFO objects.
  CM_OBJECT_TOKEN    LpiToken;
  /// Set to TRUE if UID should override index for name and _UID
  /// for processor container nodes and name of processors.
  /// This should be consistently set for containers or processors to avoid
  /// duplicate values
  BOOLEAN            OverrideNameUidEnabled;
  /// If OverrideNameUidEnabled is TRUE then this value will be used for name of
  /// processors and processor containers.
  UINT16             OverrideName;
  /// If OverrideNameUidEnabled is TRUE then this value will be used for
  /// the UID of processor containers.
  UINT32             OverrideUid;
} CM_RISCV_PROC_HIERARCHY_INFO;

/** A structure that describes a reference to another Configuration Manager
    object.

    This is useful for creating an array of reference tokens. The framework
    can then query the configuration manager for these arrays using the
    object ID ERiscVObjCmRef.

    This can be used is to represent one-to-many relationships between objects.

    ID: ERiscVObjCmRef
*/
typedef struct CmRiscVObjRef {
  /// Token of the CM object being referenced
  CM_OBJECT_TOKEN    ReferenceToken;
} CM_RISCV_OBJ_REF;

/** A structure that describes the Lpi information.

  The Low Power Idle states are described in DSDT/SSDT and associated
  to cpus/clusters in the cpu topology.

  ID: ERiscVObjLpiInfo
*/
typedef struct CmRiscVLpiInfo {
  /** Minimum Residency. Time in microseconds after which a
      state becomes more energy efficient than any shallower state.
  */
  UINT32                                    MinResidency;

  /** Worst case time in microseconds from a wake interrupt
      being asserted to the return to a running state
  */
  UINT32                                    WorstCaseWakeLatency;

  /** Flags.
  */
  UINT32                                    Flags;

  /** Architecture specific context loss flags.
  */
  UINT32                                    ArchFlags;

  /** Residency counter frequency in cycles-per-second (Hz).
  */
  UINT32                                    ResCntFreq;

  /** Every shallower power state in the parent is also enabled.
  */
  UINT32                                    EnableParentState;

  /** The EntryMethod _LPI field can be described as an integer
      or in a Register resource data descriptor.

  If IsInteger is TRUE, the IntegerEntryMethod field is used.
  If IsInteger is FALSE, the RegisterEntryMethod field is used.
  */
  BOOLEAN                                   IsInteger;

  /** EntryMethod described as an Integer.
  */
  UINT64                                    IntegerEntryMethod;

  /** EntryMethod described as a EFI_ACPI_GENERIC_REGISTER_DESCRIPTOR.
  */
  EFI_ACPI_6_3_GENERIC_ADDRESS_STRUCTURE    RegisterEntryMethod;

  /** Residency counter register.
  */
  EFI_ACPI_6_3_GENERIC_ADDRESS_STRUCTURE    ResidencyCounterRegister;

  /** Usage counter register.
  */
  EFI_ACPI_6_3_GENERIC_ADDRESS_STRUCTURE    UsageCounterRegister;

  /** String representing the Lpi state
  */
  CHAR8                                     StateName[16];
} CM_RISCV_LPI_INFO;

/** A structure that describes a PCI Address Map.

  The memory-ranges used by the PCI bus are described by this object.

  ID: ERiscVObjPciAddressMapInfo
*/
typedef struct CmRiscVPciAddressMapInfo {
  /** Pci address space code

  Available values are:
   - 0: Configuration Space
   - 1: I/O Space
   - 2: 32-bit-address Memory Space
   - 3: 64-bit-address Memory Space
  */
  UINT8     SpaceCode;

  /// PCI address
  UINT64    PciAddress;

  /// Cpu address
  UINT64    CpuAddress;

  /// Address size
  UINT64    AddressSize;
} CM_RISCV_PCI_ADDRESS_MAP_INFO;

/** A structure that describes a PCI Interrupt Map.

  The legacy PCI interrupts used by PCI devices are described by this object.

  Cf Devicetree Specification - Release v0.3
  s2.4.3 "Interrupt Nexus Properties"

  ID: ERiscVObjPciInterruptMapInfo
*/
typedef struct CmRiscVPciInterruptMapInfo {
  /// Pci Bus.
  /// Value on 8 bits (max 255).
  UINT8    PciBus;

  /// Pci Device.
  /// Value on 5 bits (max 31).
  UINT8    PciDevice;

  /** PCI interrupt

  ACPI bindings are used:
  Cf. ACPI 6.4, s6.2.13 _PRT (PCI Routing Table):
      "0-INTA, 1-INTB, 2-INTC, 3-INTD"

  Device-tree bindings are shifted by 1:
      "INTA=1, INTB=2, INTC=3, INTD=4"
  */
  UINT8                       PciInterrupt;

  /** Interrupt controller interrupt.

  Cf Devicetree Specification - Release v0.3
  s2.4.3 "Interrupt Nexus Properties": "parent interrupt specifier"
  */
  CM_RISCV_GENERIC_INTERRUPT    IntcInterrupt;
} CM_RISCV_PCI_INTERRUPT_MAP_INFO;

/** A structure that describes the Cpc information.

  Continuous Performance Control is described in DSDT/SSDT and associated
  to cpus/clusters in the cpu topology.

  Unsupported Optional registers should be encoded with NULL resource
  Register {(SystemMemory, 0, 0, 0, 0)}

  For values that support Integer or Buffer, integer will be used
  if buffer is NULL resource.
  If resource is not NULL then Integer must be 0

  Cf. ACPI 6.4, s8.4.7.1 _CPC (Continuous Performance Control)

  ID: ERiscVObjCpcInfo
*/
typedef AML_CPC_INFO CM_RISCV_CPC_INFO;

#pragma pack()

#endif // RISCV_NAMESPACE_OBJECTS_H_
