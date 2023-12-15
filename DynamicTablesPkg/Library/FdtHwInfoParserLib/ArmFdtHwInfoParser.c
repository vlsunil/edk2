/** @file
  ARM Flattened Device Tree parser helper.

  Copyright (c) 2021, ARM Limited. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#include "FdtHwInfoParser.h"
#include "BootArch/ArmBootArchParser.h"
#include "GenericTimer/ArmGenericTimerParser.h"
#include "Gic/ArmGicDispatcher.h"

/** Ordered table of parsers/dispatchers for ARM.

  A parser parses a Device Tree to populate a specific CmObj type. None,
  one or many CmObj can be created by the parser.
  The created CmObj are then handed to the parser's caller through the
  HW_INFO_ADD_OBJECT interface.
  This can also be a dispatcher. I.e. a function that not parsing a
  Device Tree but calling other parsers.
*/
CONST FDT_HW_INFO_PARSER_FUNC  ArchHwInfoParserTable[] = {
  ArmBootArchInfoParser,
  ArmGenericTimerInfoParser,
  ArmGicDispatcher
};
