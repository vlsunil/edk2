/** @file

  Copyright (c) 2017 - 2022, Arm Limited. All rights reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent

  @par Glossary:
    - Cm or CM   - Configuration Manager
    - Obj or OBJ - Object
    - Std or STD - Standard
**/

#ifndef STANDARD_NAMESPACE_OBJECTS_H_
#define STANDARD_NAMESPACE_OBJECTS_H_

#include <AcpiTableGenerator.h>
#include <SmbiosTableGenerator.h>

#pragma pack(1)

/** A macro defining a reserved zero/NULL token value that
    does not identify any object.
*/
#define CM_NULL_TOKEN  0

/** A reference token that the Configuration Manager can use
    to identify a Configuration Manager object.

  This can be used to differentiate between instances of
  objects of the same types. The identification scheme is
  implementation defined and is defined by the Configuration
  Manager.

  Typically the token is used to identify a specific instance
  from a set of objects in a call to the GetObject()/SetObject(),
  implemented by the Configuration Manager protocol.

  Note: The token value 0 is reserved for a NULL token and does
        not identify any object.
**/
typedef UINTN CM_OBJECT_TOKEN;

/** The ESTD_OBJECT_ID enum describes the Object IDs
    in the Standard Namespace.
*/
typedef enum StdObjectID {
  EStdObjCfgMgrInfo = 0x00000000,     ///<  0 - Configuration Manager Info
  EStdObjAcpiTableList,               ///<  1 - ACPI table Info List
  EStdObjSmbiosTableList,             ///<  2 - SMBIOS table Info List
  EStdObjBootArchInfo,                ///<  3 - Boot Architecture Info
  EStdObjPowerManagementProfileInfo,  ///<  4 - Power Management Profile Info
  EStdObjSerialConsolePortInfo,       ///<  5 - Serial Console Port Info
  EStdObjSerialDebugPortInfo,         ///<  6 - Serial Debug Port Info
  EStdObjPciConfigSpaceInfo,          ///<  7 - PCI Configuration Space Info
  EStdObjHypervisorVendorIdentity,    ///<  8 - Hypervisor Vendor Id
  EStdObjFixedFeatureFlags,           ///<  9 - Fixed feature flags for FADT
  EStdObjProcHierarchyInfo,                                    ///< 27 - Processor Hierarchy Info
  EStdObjSerialPortInfo,              ///< 10 - Generic Serial Port Info
  EStdObjLpiInfo,                     ///< 11 - Lpi Info
  EStdObjPciAddressMapInfo,           ///< 12 - Pci Address Map Info
  EStdObjPciInterruptMapInfo,         ///< 13 - Pci Interrupt Map Info
  EStdObjCpcInfo,                     ///< 14 - Continuous Performance Control Info
  EStdObjCmRef,                                                ///< 30 - CM Object Reference
  EStdObjMax
} ESTD_OBJECT_ID;

/** A structure that describes the Configuration Manager Information.
*/
typedef struct CmStdObjConfigurationManagerInfo {
  /// The Configuration Manager Revision.
  UINT32    Revision;

  /** The OEM ID. This information is used to
      populate the ACPI table header information.
  */
  UINT8     OemId[6];
} CM_STD_OBJ_CONFIGURATION_MANAGER_INFO;

/** A structure used to describe the ACPI table generators to be invoked.

  The AcpiTableData member of this structure may be used to directly provide
  the binary ACPI table data which is required by the following standard
  generators:
    - RAW
    - DSDT
    - SSDT

  Providing the ACPI table data is optional and depends on the generator
  that is being invoked. If unused, set AcpiTableData to NULL.
*/
typedef struct CmAStdObjAcpiTableInfo {
  /// The signature of the ACPI Table to be installed
  UINT32                         AcpiTableSignature;

  /// The ACPI table revision
  UINT8                          AcpiTableRevision;

  /// The ACPI Table Generator ID
  ACPI_TABLE_GENERATOR_ID        TableGeneratorId;

  /// Optional pointer to the ACPI table data
  EFI_ACPI_DESCRIPTION_HEADER    *AcpiTableData;

  /// An OEM-supplied string that the OEM uses to identify the particular
  /// data table. This field is particularly useful when defining a definition
  /// block to distinguish definition block functions. The OEM assigns each
  /// dissimilar table a new OEM Table ID.
  /// This field could be constructed using the SIGNATURE_64() macro.
  ///   e.g. SIGNATURE_64 ('A','R','M','H','G','T','D','T')
  /// Note: If this field is not populated (has value of Zero), then the
  /// Generators shall populate this information using part of the
  /// CM_STD_OBJ_CONFIGURATION_MANAGER_INFO.OemId field and the
  /// ACPI table signature.
  UINT64    OemTableId;

  /// An OEM-supplied revision number. Larger numbers are assumed to be
  /// newer revisions.
  /// Note: If this field is not populated (has value of Zero), then the
  /// Generators shall populate this information using the revision of the
  /// Configuration Manager (CM_STD_OBJ_CONFIGURATION_MANAGER_INFO.Revision).
  UINT32    OemRevision;

  /// The minor revision of an ACPI table if required by the table.
  /// Note: If this field is not populated (has value of Zero), then the
  /// Generators shall populate this information based on the latest minor
  /// revision of the table that is supported by the generator.
  /// e.g. This field can be used to specify the minor revision to be set
  /// for the FADT table.
  UINT8     MinorRevision;
} CM_STD_OBJ_ACPI_TABLE_INFO;

/** A structure used to describe the SMBIOS table generators to be invoked.

  The SmbiosTableData member of this structure is used to provide
  the SMBIOS table data which is required by the following standard
  generator(s):
    - RAW

  Providing the SMBIOS table data is optional and depends on the
  generator that is being invoked. If unused, set the SmbiosTableData
  to NULL.
*/
typedef struct CmStdObjSmbiosTableInfo {
  /// The SMBIOS Table Generator ID
  SMBIOS_TABLE_GENERATOR_ID    TableGeneratorId;

  /// Optional pointer to the SMBIOS table data
  SMBIOS_STRUCTURE             *SmbiosTableData;
} CM_STD_OBJ_SMBIOS_TABLE_INFO;

/** A structure that describes the
    Boot Architecture flags.

    ID: EStdObjBootArchInfo
*/
typedef struct CmStdBootArchInfo {
  /** This is the BOOT_ARCH flags field of the FADT Table
      described in the ACPI Table Specification.
  */
  UINT16    BootArchFlags;
} CM_STD_BOOT_ARCH_INFO;

/** A structure that describes the
    Power Management Profile Information for the Platform.

    ID: EStdObjPowerManagementProfileInfo
*/
typedef struct CmStdPowerManagementProfileInfo {
  /** This is the Preferred_PM_Profile field of the FADT Table
      described in the ACPI Specification
  */
  UINT8    PowerManagementProfile;
} CM_STD_POWER_MANAGEMENT_PROFILE_INFO;

/** A structure that describes the
    Serial Port information for the Platform.

    ID: EStdObjSerialConsolePortInfo or
        EStdObjSerialDebugPortInfo or
        EStdObjSerialPortInfo
*/
typedef struct CmStdSerialPortInfo {
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
} CM_STD_SERIAL_PORT_INFO;

/** A structure that describes the
    PCI Configuration Space information for the Platform.

    ID: EStdObjPciConfigSpaceInfo
*/
typedef struct CmStdPciConfigSpaceInfo {
  /// The physical base address for the PCI segment
  UINT64             BaseAddress;

  /// The PCI segment group number
  UINT16             PciSegmentGroupNumber;

  /// The start bus number
  UINT8              StartBusNumber;

  /// The end bus number
  UINT8              EndBusNumber;

  /// Optional field: Reference Token for address mapping.
  /// Token identifying a CM_STD_OBJ_REF structure.
  CM_OBJECT_TOKEN    AddressMapToken;

  /// Optional field: Reference Token for interrupt mapping.
  /// Token identifying a CM_STD_OBJ_REF structure.
  CM_OBJECT_TOKEN    InterruptMapToken;
} CM_STD_PCI_CONFIG_SPACE_INFO;

/** A structure that describes the
    Hypervisor Vendor ID information for the Platform.

    ID: EStdObjHypervisorVendorIdentity
*/
typedef struct CmStdHypervisorVendorId {
  /// The hypervisor Vendor ID
  UINT64    HypervisorVendorId;
} CM_STD_HYPERVISOR_VENDOR_ID;

/** A structure that describes the
    Fixed feature flags for the Platform.

    ID: EStdObjFixedFeatureFlags
*/
typedef struct CmStdFixedFeatureFlags {
  /// The Fixed feature flags
  UINT32    Flags;
} CM_STD_FIXED_FEATURE_FLAGS;

/** A structure that describes the Processor Hierarchy Node (Type 0) in PPTT

    ID: EStdObjProcHierarchyInfo
*/
typedef struct CmStdProcHierarchyInfo {
  /// A unique token used to identify this object
  CM_OBJECT_TOKEN    Token;
  /// Processor structure flags (ACPI 6.3 - January 2019, PPTT, Table 5-155)
  UINT32             Flags;
  /// Token for the parent CM_STD_PROC_HIERARCHY_INFO object in the processor
  /// topology. A value of CM_NULL_TOKEN means this node has no parent.
  CM_OBJECT_TOKEN    ParentToken;
  /// Token of the associated CM_M_GICC_INFO object which has the
  /// corresponding ACPI Processor ID. A value of CM_NULL_TOKEN means this
  /// node represents a group of associated processors and it does not have an
  /// associated GIC CPU interface.
  CM_OBJECT_TOKEN    ApicToken;
  /// Number of resources private to this Node
  UINT32             NoOfPrivateResources;
  /// Token of the array which contains references to the resources private to
  /// this CM_STD_PROC_HIERARCHY_INFO instance. This field is ignored if
  /// the NoOfPrivateResources is 0, in which case it is recommended to set
  /// this field to CM_NULL_TOKEN.
  CM_OBJECT_TOKEN    PrivateResourcesArrayToken;
  /// Optional field: Reference Token for the Lpi state of this processor.
  /// Token identifying a CM_STD_OBJ_REF structure, itself referencing
  /// CM_STD_LPI_INFO objects.
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
} CM_STD_PROC_HIERARCHY_INFO;

/** A structure that describes the
    Generic Interrupts.
*/
typedef struct CmStdGenericInterrupt {
  /// Interrupt number
  UINT32    Interrupt;

  /// Flags
  /// BIT0: 0: Interrupt is Level triggered
  ///       1: Interrupt is Edge triggered
  /// BIT1: 0: Interrupt is Active high
  ///       1: Interrupt is Active low
  UINT32    Flags;
} CM_STD_GENERIC_INTERRUPT;

/** A structure that describes the Lpi information.

  The Low Power Idle states are described in DSDT/SSDT and associated
  to cpus/clusters in the cpu topology.

  ID: EStdObjLpiInfo
*/
typedef struct CmStdLpiInfo {
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
} CM_STD_LPI_INFO;

/** A structure that describes a PCI Address Map.

  The memory-ranges used by the PCI bus are described by this object.

  ID: EStdObjPciAddressMapInfo
*/
typedef struct CmStdPciAddressMapInfo {
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
} CM_STD_PCI_ADDRESS_MAP_INFO;

/** A structure that describes a PCI Interrupt Map.

  The legacy PCI interrupts used by PCI devices are described by this object.

  Cf Devicetree Specification - Release v0.3
  s2.4.3 "Interrupt Nexus Properties"

  ID: EStdObjPciInterruptMapInfo
*/
typedef struct CmStdPciInterruptMapInfo {
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
  CM_STD_GENERIC_INTERRUPT    IntcInterrupt;
} CM_STD_PCI_INTERRUPT_MAP_INFO;

/** A structure that describes a reference to another Configuration Manager
    object.

    This is useful for creating an array of reference tokens. The framework
    can then query the configuration manager for these arrays using the
    object ID EArmObjCmRef.

    This can be used is to represent one-to-many relationships between objects.

    ID: EArmObjCmRef
*/
typedef struct CmStdObjRef {
  /// Token of the CM object being referenced
  CM_OBJECT_TOKEN    ReferenceToken;
} CM_STD_OBJ_REF;

typedef AML_CPC_INFO CM_STD_CPC_INFO;

#pragma pack()

#endif // STANDARD_NAMESPACE_OBJECTS_H_
