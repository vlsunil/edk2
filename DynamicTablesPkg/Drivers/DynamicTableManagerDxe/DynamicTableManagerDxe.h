
#include <ConfigurationManagerObject.h>
EFI_STATUS
EFIAPI
VerifyMandatoryTablesArePresent (
  IN CONST CM_STD_OBJ_ACPI_TABLE_INFO  *CONST  AcpiTableInfo,
  IN       UINT32                              AcpiTableCount
  );

BOOLEAN
EFIAPI
IsAcpiTableInstalled (UINTN TableId);

///
/// Bit definitions for acceptable ACPI table presence formats.
/// Currently only ACPI tables present in the ACPI info list and
/// already installed will count towards "Table Present" during
/// verification routine.
///
#define ACPI_TABLE_PRESENT_INFO_LIST  BIT0
#define ACPI_TABLE_PRESENT_INSTALLED  BIT1

///
/// Order of ACPI table being verified during presence inspection.
///
#define ACPI_TABLE_VERIFY_FADT   0
#define ACPI_TABLE_VERIFY_MADT   1
#define ACPI_TABLE_VERIFY_GTDT   2
#define ACPI_TABLE_VERIFY_DSDT   3
#define ACPI_TABLE_VERIFY_DBG2   4
#define ACPI_TABLE_VERIFY_SPCR   5
#define ACPI_TABLE_VERIFY_RHCT   6
#define ACPI_TABLE_VERIFY_COUNT  7

///
/// Private data structure to verify the presence of mandatory
/// or optional ACPI tables.
///
typedef struct {
  /// ESTD ID for the ACPI table of interest.
  ESTD_ACPI_TABLE_ID    EstdTableId;
  /// Standard UINT32 ACPI signature.
  UINT32                AcpiTableSignature;
  /// 4 character ACPI table name (the 5th char8 is for null terminator).
  CHAR8                 AcpiTableName[sizeof (UINT32) + 1];
  /// Indicator on whether the ACPI table is required.
  BOOLEAN               IsMandatory;
  /// Formats of verified presences, as defined by ACPI_TABLE_PRESENT_*
  /// This field should be initialized to 0 and will be populated during
  /// verification routine.
  UINT16                Presence;
} ACPI_TABLE_PRESENCE_INFO;

