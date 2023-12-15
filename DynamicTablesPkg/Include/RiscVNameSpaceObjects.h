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
  ERiscVObjReserved,       ///<  0 - Reserved
  ERiscVObjRintcInfo,      ///<  1 - RINTC Info
  ERiscVObjImsicInfo,      ///<  2 - IMSIC Info
  ERiscVObjAplicInfo,      ///<  3 - APLIC Frame Info
  ERiscVObjPlicInfo,       ///<  4 - PLIC Info
  ERiscVObjIsaStringInfo,  ///<  5 - ISA string Info
  ERiscVObjCmoInfo,        ///<  6 - CMO Info
  ERiscVObjTimerInfo,      ///<  7 - Timer Info
  ERiscVObjCmRef,          ///<  8 - CM Object Reference
  ERiscVObjMax
} ERISCV_OBJECT_ID;

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

  UINT64            TimeBaseFrequency;

} CM_RISCV_TIMER_INFO;

/** A structure that describes a reference to another Configuration Manager
    object.

    This is useful for creating an array of reference tokens. The framework
    can then query the configuration manager for these arrays using the
    object ID EArmObjCmRef.

    This can be used is to represent one-to-many relationships between objects.

    ID: ERiscVObjCmRef
*/
typedef struct CmRiscVObjRef {
  /// Token of the CM object being referenced
  CM_OBJECT_TOKEN    ReferenceToken;
} CM_RISCV_OBJ_REF;

#pragma pack()

#endif // RISCV_NAMESPACE_OBJECTS_H_
