## @file
#  Dsc include file for Dynamic Tables Framework.
#
#  Copyright (c) 2017 - 2022, Arm Limited. All rights reserved.<BR>
#
#  SPDX-License-Identifier: BSD-2-Clause-Patent
#
##

[Defines]

[BuildOptions]
  RELEASE_*_*_CC_FLAGS     = -DMDEPKG_NDEBUG

[LibraryClasses.common]
  AcpiHelperLib|DynamicTablesPkg/Library/Common/AcpiHelperLib/AcpiHelperLib.inf
  AmlLib|DynamicTablesPkg/Library/Common/AmlLib/AmlLib.inf
  SsdtPcieSupportLib|DynamicTablesPkg/Library/Common/SsdtPcieSupportLib/SsdtPcieSupportLib.inf
  SsdtSerialPortFixupLib|DynamicTablesPkg/Library/Common/SsdtSerialPortFixupLib/SsdtSerialPortFixupLib.inf
  TableHelperLib|DynamicTablesPkg/Library/Common/TableHelperLib/TableHelperLib.inf
  SmbiosStringTableLib|DynamicTablesPkg/Library/Common/SmbiosStringTableLib/SmbiosStringTableLib.inf

[Components.common]
  #
  # Generators
  #
  DynamicTablesPkg/Library/Acpi/Common/AcpiDbg2Lib/AcpiDbg2Lib.inf
  DynamicTablesPkg/Library/Acpi/Common/AcpiFadtLib/AcpiFadtLib.inf
  DynamicTablesPkg/Library/Acpi/Common/AcpiMcfgLib/AcpiMcfgLib.inf
  DynamicTablesPkg/Library/Acpi/Common/AcpiRawLib/AcpiRawLib.inf
  DynamicTablesPkg/Library/Acpi/Common/AcpiSpcrLib/AcpiSpcrLib.inf
  # AML Fixup
  DynamicTablesPkg/Library/Acpi/Common/AcpiSsdtSerialPortLib/SsdtSerialPortLib.inf
  # AML Codegen
  DynamicTablesPkg/Library/Acpi/Common/AcpiSsdtCpuTopologyLib/SsdtCpuTopologyLib.inf
  DynamicTablesPkg/Library/Acpi/Common/AcpiSsdtPcieLib/SsdtPcieLib.inf

[Components.ARM]
  DynamicTablesPkg/Library/Acpi/Arm/AcpiGtdtLibArm/AcpiGtdtLibArm.inf
  DynamicTablesPkg/Library/Acpi/Arm/AcpiIortLibArm/AcpiIortLibArm.inf
  DynamicTablesPkg/Library/Acpi/Arm/AcpiMadtLibArm/AcpiMadtLibArm.inf
  DynamicTablesPkg/Library/Acpi/Arm/AcpiPpttLibArm/AcpiPpttLibArm.inf
  DynamicTablesPkg/Library/Acpi/Arm/AcpiSratLibArm/AcpiSratLibArm.inf
  DynamicTablesPkg/Library/Acpi/Arm/AcpiPcctLibArm/AcpiPcctLibArm.inf
  # AML Fixup
  DynamicTablesPkg/Library/Acpi/Arm/AcpiSsdtCmn600LibArm/SsdtCmn600LibArm.inf

  #
  # Dynamic Table Factory Dxe
  #
  DynamicTablesPkg/Drivers/DynamicTableFactoryDxe/DynamicTableFactoryDxe.inf {
    <LibraryClasses>
      NULL|DynamicTablesPkg/Library/Acpi/Common/AcpiDbg2Lib/AcpiDbg2Lib.inf
      NULL|DynamicTablesPkg/Library/Acpi/Common/AcpiFadtLib/AcpiFadtLib.inf
      NULL|DynamicTablesPkg/Library/Acpi/Arm/AcpiGtdtLibArm/AcpiGtdtLibArm.inf
      NULL|DynamicTablesPkg/Library/Acpi/Arm/AcpiIortLibArm/AcpiIortLibArm.inf
      NULL|DynamicTablesPkg/Library/Acpi/Arm/AcpiMadtLibArm/AcpiMadtLibArm.inf
      NULL|DynamicTablesPkg/Library/Acpi/Common/AcpiMcfgLib/AcpiMcfgLib.inf
      NULL|DynamicTablesPkg/Library/Acpi/Arm/AcpiPpttLibArm/AcpiPpttLibArm.inf
      NULL|DynamicTablesPkg/Library/Acpi/Common/AcpiRawLib/AcpiRawLib.inf
      NULL|DynamicTablesPkg/Library/Acpi/Common/AcpiSpcrLib/AcpiSpcrLib.inf
      NULL|DynamicTablesPkg/Library/Acpi/Arm/AcpiSratLibArm/AcpiSratLibArm.inf
      NULL|DynamicTablesPkg/Library/Acpi/Arm/AcpiPcctLibArm/AcpiPcctLibArm.inf

      # AML Fixup
      NULL|DynamicTablesPkg/Library/Acpi/Common/AcpiSsdtSerialPortLib/SsdtSerialPortLib.inf
      NULL|DynamicTablesPkg/Library/Acpi/Arm/AcpiSsdtCmn600LibArm/SsdtCmn600LibArm.inf

      # AML Codegen
      NULL|DynamicTablesPkg/Library/Acpi/Common/AcpiSsdtCpuTopologyLib/SsdtCpuTopologyLib.inf
      NULL|DynamicTablesPkg/Library/Acpi/Common/AcpiSsdtPcieLib/SsdtPcieLib.inf
  }

[Components.RISCV64]
  DynamicTablesPkg/Library/Acpi/RiscV/AcpiMadtLibRiscV/AcpiMadtLibRiscV.inf
  DynamicTablesPkg/Library/Acpi/RiscV/AcpiRhctLibRiscV/AcpiRhctLibRiscV.inf

  #
  # Dynamic Table Factory Dxe
  #
  DynamicTablesPkg/Drivers/DynamicTableFactoryDxe/DynamicTableFactoryDxe.inf {
    <LibraryClasses>
      NULL|DynamicTablesPkg/Library/Acpi/Common/AcpiDbg2Lib/AcpiDbg2Lib.inf
      NULL|DynamicTablesPkg/Library/Acpi/Common/AcpiFadtLib/AcpiFadtLib.inf
      NULL|DynamicTablesPkg/Library/Acpi/RiscV/AcpiRhctLibRiscV/AcpiRhctLibRiscV.inf
      NULL|DynamicTablesPkg/Library/Acpi/RiscV/AcpiMadtLibRiscV/AcpiMadtLibRiscV.inf
      NULL|DynamicTablesPkg/Library/Acpi/Common/AcpiMcfgLib/AcpiMcfgLib.inf
      NULL|DynamicTablesPkg/Library/Acpi/Common/AcpiRawLib/AcpiRawLib.inf
      NULL|DynamicTablesPkg/Library/Acpi/Common/AcpiSpcrLib/AcpiSpcrLib.inf

      # AML Fixup
      NULL|DynamicTablesPkg/Library/Acpi/Common/AcpiSsdtSerialPortLib/SsdtSerialPortLib.inf
      NULL|DynamicTablesPkg/Library/Acpi/Arm/AcpiSsdtCmn600LibArm/SsdtCmn600LibArm.inf

      # AML Codegen
      NULL|DynamicTablesPkg/Library/Acpi/Common/AcpiSsdtCpuTopologyLib/SsdtCpuTopologyLib.inf
      NULL|DynamicTablesPkg/Library/Acpi/Common/AcpiSsdtPcieLib/SsdtPcieLib.inf
  }
  #
  # Dynamic Tables Manager Dxe
  #
  DynamicTablesPkg/Drivers/DynamicTableManagerDxe/DynamicTableManagerDxe.inf

