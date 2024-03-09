/** @file
  Configuration Manager object token fixer

  Copyright (c) 2021, Arm Limited. All rights reserved.<BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent

  @par Glossary:
    - Cm or CM   - Configuration Manager
    - Obj or OBJ - Object
**/

#include <Library/DebugLib.h>
#include <Protocol/ConfigurationManagerProtocol.h>
#include "CmObjectTokenFixer.h"

/** Token fixer not implemented.

  Most of the objects are not generated by this parser. Add the missing
  functions when needed.

  CmObjectToken fixer function that updates the Tokens in the CmObjects.

  @param [in]  CmObject    Pointer to the Configuration Manager Object.
  @param [in]  Token       Token to be updated in the CmObject.

  @retval EFI_SUCCESS           Success.
  @retval EFI_INVALID_PARAMETER A parameter is invalid.
  @retval EFI_UNSUPPORTED       Not supported.
**/
STATIC
EFI_STATUS
EFIAPI
TokenFixerNotImplemented (
  IN  CM_OBJ_DESCRIPTOR  *CmObject,
  IN  CM_OBJECT_TOKEN    Token
  )
{
  ASSERT (0);
  return EFI_UNSUPPORTED;
}

/** EArmObjItsGroup token fixer.

  CmObjectToken fixer function that updates the Tokens in the CmObjects.

  @param [in]  CmObject    Pointer to the Configuration Manager Object.
  @param [in]  Token       Token to be updated in the CmObject.

  @retval EFI_SUCCESS           Success.
  @retval EFI_INVALID_PARAMETER A parameter is invalid.
  @retval EFI_UNSUPPORTED       Not supported.
**/
STATIC
EFI_STATUS
EFIAPI
TokenFixerItsGroup (
  IN  CM_OBJ_DESCRIPTOR  *CmObject,
  IN  CM_OBJECT_TOKEN    Token
  )
{
  ASSERT (CmObject != NULL);
  ((CM_ARM_ITS_GROUP_NODE *)CmObject->Data)->Token = Token;
  return EFI_SUCCESS;
}

/** EArmObjNamedComponent token fixer.

  CmObjectToken fixer function that updates the Tokens in the CmObjects.

  @param [in]  CmObject    Pointer to the Configuration Manager Object.
  @param [in]  Token       Token to be updated in the CmObject.

  @retval EFI_SUCCESS           Success.
  @retval EFI_INVALID_PARAMETER A parameter is invalid.
  @retval EFI_UNSUPPORTED       Not supported.
**/
STATIC
EFI_STATUS
EFIAPI
TokenFixerNamedComponentNode (
  IN  CM_OBJ_DESCRIPTOR  *CmObject,
  IN  CM_OBJECT_TOKEN    Token
  )
{
  ASSERT (CmObject != NULL);
  ((CM_ARM_NAMED_COMPONENT_NODE *)CmObject->Data)->Token = Token;
  return EFI_SUCCESS;
}

/** EArmObjRootComplex token fixer.

  CmObjectToken fixer function that updates the Tokens in the CmObjects.

  @param [in]  CmObject    Pointer to the Configuration Manager Object.
  @param [in]  Token       Token to be updated in the CmObject.

  @retval EFI_SUCCESS           Success.
  @retval EFI_INVALID_PARAMETER A parameter is invalid.
  @retval EFI_UNSUPPORTED       Not supported.
**/
STATIC
EFI_STATUS
EFIAPI
TokenFixerRootComplexNode (
  IN  CM_OBJ_DESCRIPTOR  *CmObject,
  IN  CM_OBJECT_TOKEN    Token
  )
{
  ASSERT (CmObject != NULL);
  ((CM_ARM_ROOT_COMPLEX_NODE *)CmObject->Data)->Token = Token;
  return EFI_SUCCESS;
}

/** EArmObjSmmuV3 token fixer.

  CmObjectToken fixer function that updates the Tokens in the CmObjects.

  @param [in]  CmObject    Pointer to the Configuration Manager Object.
  @param [in]  Token       Token to be updated in the CmObject.

  @retval EFI_SUCCESS           Success.
  @retval EFI_INVALID_PARAMETER A parameter is invalid.
  @retval EFI_UNSUPPORTED       Not supported.
**/
STATIC
EFI_STATUS
EFIAPI
TokenFixerSmmuV3Node (
  IN  CM_OBJ_DESCRIPTOR  *CmObject,
  IN  CM_OBJECT_TOKEN    Token
  )
{
  ASSERT (CmObject != NULL);
  ((CM_ARM_SMMUV3_NODE *)CmObject->Data)->Token = Token;
  return EFI_SUCCESS;
}

/** TokenFixer functions table.

  A CmObj having a CM_OBJECT_TOKEN field might need to have its
  Token fixed. Each CmObj can have its Token in a specific way.
*/
CONST
CM_OBJECT_TOKEN_FIXER  TokenFixer[EArmObjMax] = {
  NULL,                             ///<  0 - Reserved
  NULL,                             ///<  1 - Boot Architecture Info
  NULL,                             ///<  2 - CPU Info
  NULL,                             ///<  3 - Power Management Profile Info
  NULL,                             ///<  4 - GIC CPU Interface Info
  NULL,                             ///<  5 - GIC Distributor Info
  NULL,                             ///<  6 - GIC MSI Frame Info
  NULL,                             ///<  7 - GIC Redistributor Info
  NULL,                             ///<  8 - GIC ITS Info
  NULL,                             ///<  9 - Serial Console Port Info
  NULL,                             ///< 10 - Serial Debug Port Info
  NULL,                             ///< 11 - Generic Timer Info
  NULL,                             ///< 12 - Platform GT Block Info
  NULL,                             ///< 13 - Generic Timer Block Frame Info
  NULL,                             ///< 14 - Platform Generic Watchdog
  NULL,                             ///< 15 - PCI Configuration Space Info
  NULL,                             ///< 16 - Hypervisor Vendor Id
  NULL,                             ///< 17 - Fixed feature flags for FADT
  TokenFixerItsGroup,               ///< 18 - ITS Group
  TokenFixerNamedComponentNode,     ///< 19 - Named Component
  TokenFixerRootComplexNode,        ///< 20 - Root Complex
  TokenFixerNotImplemented,         ///< 21 - SMMUv1 or SMMUv2
  TokenFixerSmmuV3Node,             ///< 22 - SMMUv3
  TokenFixerNotImplemented,         ///< 23 - PMCG
  NULL,                             ///< 24 - GIC ITS Identifier Array
  NULL,                             ///< 25 - ID Mapping Array
  NULL,                             ///< 26 - SMMU Interrupt Array
  TokenFixerNotImplemented,         ///< 27 - Processor Hierarchy Info
  TokenFixerNotImplemented,         ///< 28 - Cache Info
  TokenFixerNotImplemented,         ///< 29 - Reserved
  NULL,                             ///< 30 - CM Object Reference
  NULL,                             ///< 31 - Memory Affinity Info
  NULL,                             ///< 32 - Device Handle Acpi
  NULL,                             ///< 33 - Device Handle Pci
  NULL,                             ///< 34 - Generic Initiator Affinity
  NULL,                             ///< 35 - Generic Serial Port Info
  NULL,                             ///< 36 - CMN-600 Info
  NULL,                             ///< 37 - Lpi Info
  NULL,                             ///< 38 - Pci Address Map Info
  NULL,                             ///< 39 - Pci Interrupt Map Info
};

/** CmObj token fixer.

  Some CmObj structures have a self-token, i.e. they are storing their own
  token value in the CmObj. Dynamically created CmObj need to have their
  self-token assigned at some point.

  @param [in]  CmObjDesc   Pointer to the Configuration Manager Object.
  @param [in]  Token       Token to update the CmObjDesc with.

  @retval EFI_SUCCESS           Success.
  @retval EFI_INVALID_PARAMETER A parameter is invalid.
  @retval EFI_UNSUPPORTED       Not supported.
**/
EFI_STATUS
EFIAPI
FixupCmObjectSelfToken (
  IN  CM_OBJ_DESCRIPTOR  *CmObjDesc,
  IN  CM_OBJECT_TOKEN    Token
  )
{
  EFI_STATUS             Status;
  CM_OBJECT_TOKEN_FIXER  TokenFixerFunc;
  CM_OBJECT_ID           ArmNamespaceObjId;

  if (CmObjDesc == NULL) {
    ASSERT (0);
    return EFI_INVALID_PARAMETER;
  }

  // Only support Arm objects for now.
  if (GET_CM_NAMESPACE_ID (CmObjDesc->ObjectId) != EObjNameSpaceArm) {
    ASSERT (0);
    return EFI_UNSUPPORTED;
  }

  ArmNamespaceObjId = GET_CM_OBJECT_ID (CmObjDesc->ObjectId);
  if (ArmNamespaceObjId >= EArmObjMax) {
    ASSERT (0);
    return EFI_INVALID_PARAMETER;
  }

  // Fixup self-token if necessary.
  TokenFixerFunc = TokenFixer[ArmNamespaceObjId];
  if (TokenFixerFunc != NULL) {
    Status = TokenFixerFunc (CmObjDesc, Token);
    if (EFI_ERROR (Status)) {
      ASSERT (0);
      return Status;
    }
  }

  return EFI_SUCCESS;
}
