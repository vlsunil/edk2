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
  EStdObjSerialPortInfo,              ///< 10 - Generic Serial Port Info
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

    ID: EArmObjProcHierarchyInfo
*/
typedef struct CmArmProcHierarchyInfo {
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
  /// this CM_ARM_PROC_HIERARCHY_INFO instance. This field is ignored if
  /// the NoOfPrivateResources is 0, in which case it is recommended to set
  /// this field to CM_NULL_TOKEN.
  CM_OBJECT_TOKEN    PrivateResourcesArrayToken;
  /// Optional field: Reference Token for the Lpi state of this processor.
  /// Token identifying a CM_ARM_OBJ_REF structure, itself referencing
  /// CM_ARM_LPI_INFO objects.
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

#pragma pack()

#endif // STANDARD_NAMESPACE_OBJECTS_H_
