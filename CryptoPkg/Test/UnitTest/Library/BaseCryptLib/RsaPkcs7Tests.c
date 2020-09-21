/** @file
  Application for RSA Key Retrieving (from PEM and X509) & Signature Validation.

  Copyright (c) 2010 - 2011, Intel Corporation. All rights reserved.<BR>
  Copyright (c) Microsoft Corporation.
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "TestBaseCryptLib.h"


//
// Password-protected PEM Key data for RSA Private Key Retrieving (encryption key is "client").
// (Generated by OpenSSL utility).
// $ openssl genrsa -aes256 -out TestKeyPem -passout pass:client 1024
// password should match PemPass in this file
// $ xxd --include TestKeyPem
//
GLOBAL_REMOVE_IF_UNREFERENCED CONST UINT8 TestKeyPem[] = {
   0x2d, 0x2d, 0x2d, 0x2d, 0x2d, 0x42, 0x45, 0x47, 0x49, 0x4e, 0x20, 0x52,
  0x53, 0x41, 0x20, 0x50, 0x52, 0x49, 0x56, 0x41, 0x54, 0x45, 0x20, 0x4b,
  0x45, 0x59, 0x2d, 0x2d, 0x2d, 0x2d, 0x2d, 0x0a, 0x50, 0x72, 0x6f, 0x63,
  0x2d, 0x54, 0x79, 0x70, 0x65, 0x3a, 0x20, 0x34, 0x2c, 0x45, 0x4e, 0x43,
  0x52, 0x59, 0x50, 0x54, 0x45, 0x44, 0x0a, 0x44, 0x45, 0x4b, 0x2d, 0x49,
  0x6e, 0x66, 0x6f, 0x3a, 0x20, 0x41, 0x45, 0x53, 0x2d, 0x32, 0x35, 0x36,
  0x2d, 0x43, 0x42, 0x43, 0x2c, 0x34, 0x42, 0x44, 0x31, 0x30, 0x45, 0x39,
  0x35, 0x42, 0x38, 0x33, 0x39, 0x42, 0x30, 0x44, 0x33, 0x35, 0x36, 0x31,
  0x38, 0x41, 0x44, 0x36, 0x45, 0x46, 0x44, 0x36, 0x34, 0x32, 0x34, 0x44,
  0x36, 0x0a, 0x0a, 0x42, 0x50, 0x39, 0x49, 0x32, 0x42, 0x30, 0x6c, 0x50,
  0x7a, 0x4c, 0x50, 0x50, 0x38, 0x2f, 0x4b, 0x71, 0x35, 0x34, 0x2f, 0x56,
  0x68, 0x75, 0x4b, 0x35, 0x5a, 0x63, 0x72, 0x32, 0x55, 0x6c, 0x32, 0x75,
  0x43, 0x57, 0x4a, 0x62, 0x59, 0x33, 0x50, 0x77, 0x39, 0x6e, 0x4c, 0x6d,
  0x45, 0x41, 0x6a, 0x47, 0x74, 0x4a, 0x68, 0x57, 0x46, 0x66, 0x50, 0x47,
  0x38, 0x62, 0x6d, 0x41, 0x6f, 0x73, 0x56, 0x0a, 0x47, 0x76, 0x42, 0x30,
  0x6d, 0x2b, 0x7a, 0x5a, 0x43, 0x2b, 0x30, 0x6c, 0x57, 0x6c, 0x72, 0x59,
  0x7a, 0x51, 0x4b, 0x6a, 0x57, 0x79, 0x44, 0x6e, 0x4d, 0x58, 0x72, 0x33,
  0x51, 0x39, 0x69, 0x57, 0x32, 0x4b, 0x33, 0x68, 0x4d, 0x6b, 0x71, 0x51,
  0x4c, 0x31, 0x68, 0x65, 0x71, 0x52, 0x66, 0x66, 0x74, 0x47, 0x57, 0x51,
  0x5a, 0x36, 0x78, 0x4e, 0x6a, 0x72, 0x30, 0x7a, 0x6f, 0x51, 0x59, 0x73,
  0x0a, 0x34, 0x76, 0x69, 0x55, 0x46, 0x72, 0x7a, 0x2b, 0x52, 0x76, 0x4b,
  0x43, 0x2f, 0x33, 0x69, 0x71, 0x57, 0x59, 0x78, 0x55, 0x35, 0x4c, 0x6a,
  0x45, 0x74, 0x63, 0x5a, 0x4f, 0x2b, 0x53, 0x6d, 0x39, 0x42, 0x4c, 0x62,
  0x66, 0x58, 0x49, 0x71, 0x56, 0x72, 0x53, 0x6a, 0x54, 0x79, 0x58, 0x49,
  0x39, 0x70, 0x76, 0x78, 0x6f, 0x67, 0x50, 0x39, 0x38, 0x6b, 0x2b, 0x6c,
  0x41, 0x66, 0x37, 0x47, 0x36, 0x0a, 0x75, 0x39, 0x2b, 0x30, 0x31, 0x4d,
  0x47, 0x5a, 0x69, 0x36, 0x6b, 0x53, 0x73, 0x67, 0x48, 0x57, 0x7a, 0x43,
  0x41, 0x49, 0x51, 0x75, 0x38, 0x72, 0x6a, 0x4d, 0x34, 0x65, 0x74, 0x64,
  0x50, 0x62, 0x4a, 0x49, 0x77, 0x34, 0x65, 0x47, 0x6f, 0x32, 0x45, 0x49,
  0x44, 0x45, 0x54, 0x61, 0x52, 0x70, 0x73, 0x76, 0x47, 0x6a, 0x54, 0x6f,
  0x30, 0x51, 0x56, 0x69, 0x79, 0x79, 0x4a, 0x4f, 0x48, 0x32, 0x0a, 0x61,
  0x32, 0x71, 0x69, 0x2f, 0x47, 0x7a, 0x2f, 0x64, 0x48, 0x61, 0x62, 0x68,
  0x4d, 0x4e, 0x35, 0x4e, 0x53, 0x58, 0x56, 0x4d, 0x31, 0x54, 0x2f, 0x6d,
  0x69, 0x6f, 0x74, 0x68, 0x78, 0x59, 0x72, 0x2f, 0x4a, 0x69, 0x37, 0x6d,
  0x4e, 0x45, 0x75, 0x4a, 0x57, 0x38, 0x74, 0x6d, 0x75, 0x55, 0x4b, 0x58,
  0x33, 0x66, 0x63, 0x39, 0x42, 0x39, 0x32, 0x51, 0x6e, 0x54, 0x68, 0x43,
  0x69, 0x49, 0x2f, 0x0a, 0x79, 0x4f, 0x31, 0x32, 0x4c, 0x46, 0x58, 0x38,
  0x74, 0x4b, 0x4a, 0x37, 0x4b, 0x7a, 0x6f, 0x6b, 0x36, 0x44, 0x74, 0x6d,
  0x35, 0x73, 0x41, 0x74, 0x2b, 0x65, 0x4b, 0x76, 0x6f, 0x61, 0x47, 0x62,
  0x75, 0x4a, 0x78, 0x62, 0x52, 0x63, 0x36, 0x63, 0x4d, 0x58, 0x57, 0x46,
  0x36, 0x4d, 0x72, 0x4d, 0x30, 0x53, 0x78, 0x65, 0x4e, 0x6b, 0x5a, 0x77,
  0x5a, 0x36, 0x6c, 0x62, 0x4d, 0x39, 0x63, 0x55, 0x0a, 0x6c, 0x75, 0x34,
  0x4c, 0x56, 0x64, 0x34, 0x73, 0x56, 0x4c, 0x61, 0x76, 0x68, 0x75, 0x32,
  0x58, 0x48, 0x48, 0x53, 0x56, 0x30, 0x32, 0x32, 0x6d, 0x51, 0x72, 0x73,
  0x32, 0x69, 0x68, 0x74, 0x58, 0x44, 0x2b, 0x6c, 0x4d, 0x63, 0x2f, 0x35,
  0x62, 0x54, 0x41, 0x55, 0x6b, 0x4b, 0x4f, 0x42, 0x73, 0x43, 0x69, 0x4f,
  0x4b, 0x42, 0x56, 0x2b, 0x66, 0x70, 0x49, 0x62, 0x2b, 0x6d, 0x44, 0x33,
  0x58, 0x0a, 0x39, 0x37, 0x66, 0x36, 0x54, 0x66, 0x68, 0x37, 0x4f, 0x4f,
  0x6a, 0x74, 0x44, 0x79, 0x31, 0x6f, 0x52, 0x36, 0x70, 0x68, 0x48, 0x47,
  0x6e, 0x73, 0x43, 0x78, 0x72, 0x53, 0x72, 0x64, 0x48, 0x73, 0x2f, 0x34,
  0x33, 0x72, 0x61, 0x65, 0x42, 0x78, 0x59, 0x45, 0x41, 0x42, 0x4e, 0x59,
  0x68, 0x54, 0x47, 0x57, 0x49, 0x4d, 0x4a, 0x6b, 0x50, 0x63, 0x54, 0x53,
  0x73, 0x76, 0x77, 0x46, 0x37, 0x6d, 0x0a, 0x33, 0x6c, 0x38, 0x6b, 0x44,
  0x50, 0x48, 0x43, 0x4e, 0x68, 0x6e, 0x6e, 0x42, 0x69, 0x7a, 0x36, 0x2f,
  0x43, 0x38, 0x56, 0x31, 0x37, 0x78, 0x57, 0x34, 0x50, 0x2b, 0x79, 0x71,
  0x4a, 0x78, 0x58, 0x63, 0x49, 0x53, 0x72, 0x7a, 0x57, 0x53, 0x55, 0x72,
  0x34, 0x74, 0x71, 0x6b, 0x55, 0x58, 0x43, 0x57, 0x4c, 0x43, 0x66, 0x76,
  0x57, 0x7a, 0x4e, 0x65, 0x5a, 0x34, 0x4f, 0x34, 0x34, 0x54, 0x65, 0x0a,
  0x74, 0x31, 0x59, 0x65, 0x36, 0x77, 0x2b, 0x71, 0x4f, 0x55, 0x38, 0x50,
  0x42, 0x68, 0x72, 0x65, 0x4d, 0x38, 0x75, 0x32, 0x32, 0x42, 0x4f, 0x31,
  0x65, 0x2b, 0x44, 0x7a, 0x63, 0x74, 0x6c, 0x67, 0x43, 0x43, 0x6c, 0x38,
  0x79, 0x69, 0x37, 0x6f, 0x43, 0x56, 0x74, 0x66, 0x75, 0x59, 0x2f, 0x4c,
  0x72, 0x42, 0x61, 0x31, 0x74, 0x69, 0x43, 0x41, 0x37, 0x6c, 0x34, 0x75,
  0x58, 0x6b, 0x73, 0x4c, 0x0a, 0x2b, 0x31, 0x51, 0x79, 0x69, 0x4b, 0x31,
  0x6e, 0x43, 0x4f, 0x76, 0x74, 0x30, 0x46, 0x7a, 0x71, 0x62, 0x71, 0x78,
  0x54, 0x37, 0x53, 0x35, 0x4c, 0x56, 0x33, 0x5a, 0x33, 0x74, 0x34, 0x4a,
  0x46, 0x4f, 0x50, 0x62, 0x67, 0x63, 0x7a, 0x4e, 0x6b, 0x58, 0x55, 0x2b,
  0x4f, 0x74, 0x50, 0x6b, 0x6e, 0x45, 0x45, 0x76, 0x67, 0x57, 0x64, 0x76,
  0x31, 0x4f, 0x30, 0x6d, 0x52, 0x4a, 0x50, 0x31, 0x4e, 0x0a, 0x71, 0x43,
  0x32, 0x33, 0x4a, 0x6a, 0x36, 0x38, 0x4c, 0x30, 0x46, 0x63, 0x46, 0x4c,
  0x56, 0x56, 0x59, 0x76, 0x61, 0x44, 0x53, 0x76, 0x54, 0x45, 0x64, 0x50,
  0x54, 0x34, 0x62, 0x2f, 0x7a, 0x66, 0x64, 0x36, 0x51, 0x52, 0x6b, 0x38,
  0x70, 0x4d, 0x36, 0x77, 0x66, 0x61, 0x32, 0x50, 0x63, 0x75, 0x57, 0x65,
  0x79, 0x38, 0x48, 0x38, 0x76, 0x4e, 0x4b, 0x67, 0x2f, 0x65, 0x76, 0x34,
  0x77, 0x37, 0x0a, 0x6b, 0x6f, 0x6f, 0x4e, 0x59, 0x64, 0x77, 0x59, 0x69,
  0x6c, 0x37, 0x41, 0x50, 0x76, 0x42, 0x50, 0x4d, 0x63, 0x6c, 0x51, 0x76,
  0x63, 0x64, 0x71, 0x7a, 0x52, 0x7a, 0x4a, 0x6e, 0x4a, 0x74, 0x37, 0x70,
  0x35, 0x7a, 0x69, 0x2b, 0x2b, 0x4c, 0x43, 0x59, 0x55, 0x4d, 0x3d, 0x0a,
  0x2d, 0x2d, 0x2d, 0x2d, 0x2d, 0x45, 0x4e, 0x44, 0x20, 0x52, 0x53, 0x41,
  0x20, 0x50, 0x52, 0x49, 0x56, 0x41, 0x54, 0x45, 0x20, 0x4b, 0x45, 0x59,
  0x2d, 0x2d, 0x2d, 0x2d, 0x2d, 0x0a
 };

//
// Password for private key retrieving from encrypted PEM ("TestKeyPem").
//
GLOBAL_REMOVE_IF_UNREFERENCED CONST CHAR8 *PemPass = "client";

//
// Test CA X509 Certificate for X509 Verification Routine (Generated by OpenSSL utility).
// $ openssl req -x509 -days 10000 -key TestKeyPem -out TestCACert -outform DER -subj "/C=US/ST=WA/L=Seattle/O=Tianocore/OU=EDK2/CN=UEFI"
// use password from PemPass variable in this file
// $ xxd --include TestCACert
//
GLOBAL_REMOVE_IF_UNREFERENCED CONST UINT8 TestCACert[] = {
  0x30, 0x82, 0x02, 0x98, 0x30, 0x82, 0x02, 0x01, 0xa0, 0x03, 0x02, 0x01,
  0x02, 0x02, 0x14, 0x39, 0xde, 0x9e, 0xce, 0x3a, 0x36, 0x11, 0x38, 0x6f,
  0x64, 0xb4, 0x69, 0xa7, 0x93, 0xdd, 0xff, 0xbd, 0x3e, 0x75, 0x6a, 0x30,
  0x0d, 0x06, 0x09, 0x2a, 0x86, 0x48, 0x86, 0xf7, 0x0d, 0x01, 0x01, 0x0b,
  0x05, 0x00, 0x30, 0x5e, 0x31, 0x0b, 0x30, 0x09, 0x06, 0x03, 0x55, 0x04,
  0x06, 0x13, 0x02, 0x55, 0x53, 0x31, 0x0b, 0x30, 0x09, 0x06, 0x03, 0x55,
  0x04, 0x08, 0x0c, 0x02, 0x57, 0x41, 0x31, 0x10, 0x30, 0x0e, 0x06, 0x03,
  0x55, 0x04, 0x07, 0x0c, 0x07, 0x53, 0x65, 0x61, 0x74, 0x74, 0x6c, 0x65,
  0x31, 0x12, 0x30, 0x10, 0x06, 0x03, 0x55, 0x04, 0x0a, 0x0c, 0x09, 0x54,
  0x69, 0x61, 0x6e, 0x6f, 0x63, 0x6f, 0x72, 0x65, 0x31, 0x0d, 0x30, 0x0b,
  0x06, 0x03, 0x55, 0x04, 0x0b, 0x0c, 0x04, 0x45, 0x44, 0x4b, 0x32, 0x31,
  0x0d, 0x30, 0x0b, 0x06, 0x03, 0x55, 0x04, 0x03, 0x0c, 0x04, 0x55, 0x45,
  0x46, 0x49, 0x30, 0x1e, 0x17, 0x0d, 0x32, 0x30, 0x30, 0x36, 0x32, 0x39,
  0x32, 0x32, 0x34, 0x37, 0x33, 0x36, 0x5a, 0x17, 0x0d, 0x34, 0x37, 0x31,
  0x31, 0x31, 0x35, 0x32, 0x32, 0x34, 0x37, 0x33, 0x36, 0x5a, 0x30, 0x5e,
  0x31, 0x0b, 0x30, 0x09, 0x06, 0x03, 0x55, 0x04, 0x06, 0x13, 0x02, 0x55,
  0x53, 0x31, 0x0b, 0x30, 0x09, 0x06, 0x03, 0x55, 0x04, 0x08, 0x0c, 0x02,
  0x57, 0x41, 0x31, 0x10, 0x30, 0x0e, 0x06, 0x03, 0x55, 0x04, 0x07, 0x0c,
  0x07, 0x53, 0x65, 0x61, 0x74, 0x74, 0x6c, 0x65, 0x31, 0x12, 0x30, 0x10,
  0x06, 0x03, 0x55, 0x04, 0x0a, 0x0c, 0x09, 0x54, 0x69, 0x61, 0x6e, 0x6f,
  0x63, 0x6f, 0x72, 0x65, 0x31, 0x0d, 0x30, 0x0b, 0x06, 0x03, 0x55, 0x04,
  0x0b, 0x0c, 0x04, 0x45, 0x44, 0x4b, 0x32, 0x31, 0x0d, 0x30, 0x0b, 0x06,
  0x03, 0x55, 0x04, 0x03, 0x0c, 0x04, 0x55, 0x45, 0x46, 0x49, 0x30, 0x81,
  0x9f, 0x30, 0x0d, 0x06, 0x09, 0x2a, 0x86, 0x48, 0x86, 0xf7, 0x0d, 0x01,
  0x01, 0x01, 0x05, 0x00, 0x03, 0x81, 0x8d, 0x00, 0x30, 0x81, 0x89, 0x02,
  0x81, 0x81, 0x00, 0x9f, 0xef, 0x1b, 0x46, 0x45, 0x55, 0x33, 0x4b, 0xee,
  0x95, 0x14, 0xd3, 0x5a, 0x3e, 0xd9, 0x29, 0xfb, 0xd9, 0x29, 0x4e, 0x8b,
  0xf1, 0xf5, 0x68, 0x7c, 0x58, 0x86, 0x0c, 0xda, 0xd7, 0xe0, 0xd2, 0x9a,
  0xe8, 0x37, 0x16, 0x4d, 0x54, 0x92, 0x18, 0x20, 0x4c, 0x09, 0xa1, 0xcf,
  0xe1, 0xaa, 0x7a, 0x5a, 0x64, 0x7e, 0x5c, 0xeb, 0x4e, 0x15, 0x8e, 0x40,
  0xd1, 0xcb, 0x7d, 0x01, 0x71, 0x15, 0x11, 0xd2, 0xc7, 0xdb, 0x6b, 0x00,
  0xdc, 0x02, 0xcb, 0x5a, 0x6d, 0x2b, 0x2a, 0x75, 0xb6, 0x3f, 0xec, 0xc1,
  0x9d, 0xbf, 0xda, 0xe5, 0x3a, 0x77, 0x4b, 0x21, 0x1c, 0x99, 0x42, 0x84,
  0x5e, 0x27, 0x53, 0x9b, 0xe6, 0xc1, 0xa1, 0x95, 0x58, 0xba, 0xbe, 0x62,
  0x58, 0xd5, 0x09, 0xa8, 0xe6, 0xb6, 0x1b, 0xb1, 0x18, 0x28, 0x13, 0xc7,
  0x89, 0x1c, 0x68, 0xce, 0x15, 0xaf, 0x2e, 0x68, 0xac, 0x1c, 0xf7, 0x02,
  0x03, 0x01, 0x00, 0x01, 0xa3, 0x53, 0x30, 0x51, 0x30, 0x1d, 0x06, 0x03,
  0x55, 0x1d, 0x0e, 0x04, 0x16, 0x04, 0x14, 0x50, 0xe5, 0x05, 0xa3, 0x6e,
  0x8f, 0x00, 0xf7, 0x93, 0x30, 0xe5, 0x25, 0x20, 0xdc, 0x8a, 0xc3, 0xad,
  0x14, 0x6d, 0x90, 0x30, 0x1f, 0x06, 0x03, 0x55, 0x1d, 0x23, 0x04, 0x18,
  0x30, 0x16, 0x80, 0x14, 0x50, 0xe5, 0x05, 0xa3, 0x6e, 0x8f, 0x00, 0xf7,
  0x93, 0x30, 0xe5, 0x25, 0x20, 0xdc, 0x8a, 0xc3, 0xad, 0x14, 0x6d, 0x90,
  0x30, 0x0f, 0x06, 0x03, 0x55, 0x1d, 0x13, 0x01, 0x01, 0xff, 0x04, 0x05,
  0x30, 0x03, 0x01, 0x01, 0xff, 0x30, 0x0d, 0x06, 0x09, 0x2a, 0x86, 0x48,
  0x86, 0xf7, 0x0d, 0x01, 0x01, 0x0b, 0x05, 0x00, 0x03, 0x81, 0x81, 0x00,
  0x8e, 0xe4, 0x27, 0x42, 0x16, 0x6e, 0xbd, 0x28, 0x47, 0x09, 0x99, 0xc1,
  0x55, 0x02, 0x82, 0x1a, 0xe1, 0xd0, 0xf3, 0xef, 0x4d, 0xaf, 0x30, 0x9a,
  0x29, 0x4b, 0x74, 0x03, 0x6a, 0x95, 0x28, 0xf1, 0xbe, 0x62, 0x68, 0x9f,
  0x82, 0x59, 0x7a, 0x49, 0x91, 0xb6, 0xaf, 0x6b, 0x23, 0x30, 0xb4, 0xf4,
  0xdd, 0xfa, 0x30, 0x3f, 0xb6, 0xed, 0x74, 0x3f, 0x91, 0xe8, 0xd7, 0x84,
  0x1a, 0xf3, 0xc6, 0x3d, 0xd8, 0x59, 0x8d, 0x68, 0x6e, 0xb3, 0x66, 0x9e,
  0xe8, 0xeb, 0x1a, 0x8b, 0x1e, 0x92, 0x71, 0x73, 0x8c, 0x4f, 0x63, 0xce,
  0x71, 0x7b, 0x97, 0x3b, 0x59, 0xd2, 0x9b, 0xe4, 0xd0, 0xef, 0x31, 0x9f,
  0x0d, 0x61, 0x27, 0x97, 0x9d, 0xe8, 0xe0, 0xcd, 0x8d, 0xc1, 0x4d, 0xad,
  0xf7, 0x3a, 0x8d, 0xb8, 0x86, 0x8c, 0x23, 0x1d, 0x4c, 0x02, 0x5c, 0x53,
  0x46, 0x84, 0xb2, 0x97, 0x0c, 0xd3, 0x35, 0x6b
};

//
// X509 Cert Data for RSA Public Key Retrieving and X509 Verification (Generated by OpenSSL utility).
// $ openssl req -new -key TestKeyPem -out TestCertCsr -subj "/C=US/ST=WA/L=Seattle/O=Tianocore/OU=EDK2CHILD/CN=UEFI"
// $ openssl x509 -days 10000 -CA TestCACert.pem -CAkey TestKeyPem -req -out TestCert -set_serial 3432 --outform DER -in TestCertCsr
// password should be in the PemPass variable
// $ xxd --include TestCert
GLOBAL_REMOVE_IF_UNREFERENCED CONST UINT8 TestCert[] = {
  0x30, 0x82, 0x02, 0x31, 0x30, 0x82, 0x01, 0x9a, 0x02, 0x02, 0x0d, 0x68,
  0x30, 0x0d, 0x06, 0x09, 0x2a, 0x86, 0x48, 0x86, 0xf7, 0x0d, 0x01, 0x01,
  0x0b, 0x05, 0x00, 0x30, 0x5e, 0x31, 0x0b, 0x30, 0x09, 0x06, 0x03, 0x55,
  0x04, 0x06, 0x13, 0x02, 0x55, 0x53, 0x31, 0x0b, 0x30, 0x09, 0x06, 0x03,
  0x55, 0x04, 0x08, 0x0c, 0x02, 0x57, 0x41, 0x31, 0x10, 0x30, 0x0e, 0x06,
  0x03, 0x55, 0x04, 0x07, 0x0c, 0x07, 0x53, 0x65, 0x61, 0x74, 0x74, 0x6c,
  0x65, 0x31, 0x12, 0x30, 0x10, 0x06, 0x03, 0x55, 0x04, 0x0a, 0x0c, 0x09,
  0x54, 0x69, 0x61, 0x6e, 0x6f, 0x63, 0x6f, 0x72, 0x65, 0x31, 0x0d, 0x30,
  0x0b, 0x06, 0x03, 0x55, 0x04, 0x0b, 0x0c, 0x04, 0x45, 0x44, 0x4b, 0x32,
  0x31, 0x0d, 0x30, 0x0b, 0x06, 0x03, 0x55, 0x04, 0x03, 0x0c, 0x04, 0x55,
  0x45, 0x46, 0x49, 0x30, 0x1e, 0x17, 0x0d, 0x32, 0x30, 0x30, 0x36, 0x32,
  0x39, 0x32, 0x33, 0x31, 0x35, 0x33, 0x36, 0x5a, 0x17, 0x0d, 0x34, 0x37,
  0x31, 0x31, 0x31, 0x35, 0x32, 0x33, 0x31, 0x35, 0x33, 0x36, 0x5a, 0x30,
  0x63, 0x31, 0x0b, 0x30, 0x09, 0x06, 0x03, 0x55, 0x04, 0x06, 0x13, 0x02,
  0x55, 0x53, 0x31, 0x0b, 0x30, 0x09, 0x06, 0x03, 0x55, 0x04, 0x08, 0x0c,
  0x02, 0x57, 0x41, 0x31, 0x10, 0x30, 0x0e, 0x06, 0x03, 0x55, 0x04, 0x07,
  0x0c, 0x07, 0x53, 0x65, 0x61, 0x74, 0x74, 0x6c, 0x65, 0x31, 0x12, 0x30,
  0x10, 0x06, 0x03, 0x55, 0x04, 0x0a, 0x0c, 0x09, 0x54, 0x69, 0x61, 0x6e,
  0x6f, 0x63, 0x6f, 0x72, 0x65, 0x31, 0x12, 0x30, 0x10, 0x06, 0x03, 0x55,
  0x04, 0x0b, 0x0c, 0x09, 0x45, 0x44, 0x4b, 0x32, 0x43, 0x48, 0x49, 0x4c,
  0x44, 0x31, 0x0d, 0x30, 0x0b, 0x06, 0x03, 0x55, 0x04, 0x03, 0x0c, 0x04,
  0x55, 0x45, 0x46, 0x49, 0x30, 0x81, 0x9f, 0x30, 0x0d, 0x06, 0x09, 0x2a,
  0x86, 0x48, 0x86, 0xf7, 0x0d, 0x01, 0x01, 0x01, 0x05, 0x00, 0x03, 0x81,
  0x8d, 0x00, 0x30, 0x81, 0x89, 0x02, 0x81, 0x81, 0x00, 0x9f, 0xef, 0x1b,
  0x46, 0x45, 0x55, 0x33, 0x4b, 0xee, 0x95, 0x14, 0xd3, 0x5a, 0x3e, 0xd9,
  0x29, 0xfb, 0xd9, 0x29, 0x4e, 0x8b, 0xf1, 0xf5, 0x68, 0x7c, 0x58, 0x86,
  0x0c, 0xda, 0xd7, 0xe0, 0xd2, 0x9a, 0xe8, 0x37, 0x16, 0x4d, 0x54, 0x92,
  0x18, 0x20, 0x4c, 0x09, 0xa1, 0xcf, 0xe1, 0xaa, 0x7a, 0x5a, 0x64, 0x7e,
  0x5c, 0xeb, 0x4e, 0x15, 0x8e, 0x40, 0xd1, 0xcb, 0x7d, 0x01, 0x71, 0x15,
  0x11, 0xd2, 0xc7, 0xdb, 0x6b, 0x00, 0xdc, 0x02, 0xcb, 0x5a, 0x6d, 0x2b,
  0x2a, 0x75, 0xb6, 0x3f, 0xec, 0xc1, 0x9d, 0xbf, 0xda, 0xe5, 0x3a, 0x77,
  0x4b, 0x21, 0x1c, 0x99, 0x42, 0x84, 0x5e, 0x27, 0x53, 0x9b, 0xe6, 0xc1,
  0xa1, 0x95, 0x58, 0xba, 0xbe, 0x62, 0x58, 0xd5, 0x09, 0xa8, 0xe6, 0xb6,
  0x1b, 0xb1, 0x18, 0x28, 0x13, 0xc7, 0x89, 0x1c, 0x68, 0xce, 0x15, 0xaf,
  0x2e, 0x68, 0xac, 0x1c, 0xf7, 0x02, 0x03, 0x01, 0x00, 0x01, 0x30, 0x0d,
  0x06, 0x09, 0x2a, 0x86, 0x48, 0x86, 0xf7, 0x0d, 0x01, 0x01, 0x0b, 0x05,
  0x00, 0x03, 0x81, 0x81, 0x00, 0x0d, 0xa4, 0x18, 0xa2, 0xb6, 0x09, 0xe1,
  0x77, 0x22, 0x97, 0x46, 0x29, 0x1b, 0xd8, 0x67, 0x0a, 0xc0, 0x91, 0x36,
  0x53, 0xe1, 0x4c, 0x73, 0x1b, 0xc1, 0x90, 0x6d, 0x98, 0x46, 0x7e, 0x65,
  0x71, 0x1c, 0xf0, 0x62, 0x9f, 0x9e, 0x62, 0x0b, 0x8b, 0x73, 0x35, 0x4c,
  0x2d, 0xc3, 0x5d, 0x38, 0x22, 0xfe, 0x43, 0x0f, 0xf2, 0x57, 0x17, 0x75,
  0xa8, 0x76, 0x79, 0xab, 0x4e, 0x33, 0xa5, 0x91, 0xbd, 0x55, 0x5b, 0xc0,
  0x7e, 0xfb, 0x1d, 0xc9, 0xf3, 0x5f, 0x12, 0x6f, 0x7c, 0xdc, 0x24, 0x5a,
  0x84, 0x16, 0x28, 0x5b, 0xf9, 0xcc, 0x8b, 0xfe, 0x11, 0xe6, 0x29, 0xcf,
  0xac, 0x90, 0x66, 0xc0, 0x70, 0x25, 0xf8, 0x71, 0xdb, 0x29, 0xcb, 0x6b,
  0x10, 0xa7, 0xbe, 0x3e, 0x9d, 0x61, 0xd8, 0x04, 0xe0, 0x71, 0x63, 0x83,
  0xa3, 0xca, 0x26, 0x6d, 0x7f, 0xf3, 0xaa, 0x8e, 0xb2, 0x66, 0x98, 0x41,
  0xd6
};

//
// Message Hash for Signing & Verification Validation.
//
GLOBAL_REMOVE_IF_UNREFERENCED CONST UINT8 MsgHash[] = {
  0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09,
  0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09
  };

//
// Payload for PKCS#7 Signing & Verification Validation.
//
GLOBAL_REMOVE_IF_UNREFERENCED CONST CHAR8 *Payload = "Payload Data for PKCS#7 Signing";


UNIT_TEST_STATUS
EFIAPI
TestVerifyRsaCertPkcs1SignVerify (
  IN UNIT_TEST_CONTEXT           Context
  )
{
  BOOLEAN        Status;
  VOID           *RsaPrivKey;
  VOID           *RsaPubKey;
  UINT8          *Signature;
  UINTN          SigSize;
  UINT8          *Subject;
  UINTN          SubjectSize;
  RETURN_STATUS  ReturnStatus;
  CHAR8          CommonName[64];
  UINTN          CommonNameSize;
  CHAR8          OrgName[64];
  UINTN          OrgNameSize;

  //
  // Retrieve RSA private key from encrypted PEM data.
  //
  Status = RsaGetPrivateKeyFromPem (TestKeyPem, sizeof (TestKeyPem), PemPass, &RsaPrivKey);
  UT_ASSERT_TRUE (Status);

  //
  // Retrieve RSA public key from X509 Certificate.
  //
  RsaPubKey = NULL;
  Status    = RsaGetPublicKeyFromX509 (TestCert, sizeof (TestCert), &RsaPubKey);
  UT_ASSERT_TRUE (Status);

  //
  // Generate RSA PKCS#1 Signature.
  //
  SigSize = 0;
  Status  = RsaPkcs1Sign (RsaPrivKey, MsgHash, SHA1_DIGEST_SIZE, NULL, &SigSize);
  UT_ASSERT_FALSE (Status);
  UT_ASSERT_NOT_EQUAL (SigSize, 0);

  Signature = AllocatePool (SigSize);
  Status    = RsaPkcs1Sign (RsaPrivKey, MsgHash, SHA1_DIGEST_SIZE, Signature, &SigSize);
  UT_ASSERT_TRUE (Status);

  //
  // Verify RSA PKCS#1-encoded Signature.
  //
  Status = RsaPkcs1Verify (RsaPubKey, MsgHash, SHA1_DIGEST_SIZE, Signature, SigSize);
  UT_ASSERT_TRUE (Status);

  //
  // X509 Certificate Subject Retrieving.
  //
  SubjectSize = 0;
  Status  = X509GetSubjectName (TestCert, sizeof (TestCert), NULL, &SubjectSize);
  Subject = (UINT8 *)AllocatePool (SubjectSize);
  Status  = X509GetSubjectName (TestCert, sizeof (TestCert), Subject, &SubjectSize);
  UT_ASSERT_TRUE (Status);

  //
  // Get CommonName from X509 Certificate Subject
  //
  CommonNameSize = 64;
  ZeroMem (CommonName, CommonNameSize);
  ReturnStatus = X509GetCommonName (TestCert, sizeof (TestCert), CommonName, &CommonNameSize);
  UT_ASSERT_NOT_EFI_ERROR (ReturnStatus);

  UT_ASSERT_EQUAL (CommonNameSize, 5);
  UT_ASSERT_MEM_EQUAL (CommonName, "UEFI", 5);

  OrgNameSize = 64;
  ZeroMem (OrgName, OrgNameSize);
  ReturnStatus = X509GetOrganizationName (TestCert, sizeof (TestCert), OrgName, &OrgNameSize);
  UT_ASSERT_NOT_EFI_ERROR (ReturnStatus);

  UT_ASSERT_EQUAL (OrgNameSize, 10);
  UT_ASSERT_MEM_EQUAL (OrgName, "Tianocore", 10);

  //
  // X509 Certificate Verification.
  //
  Status = X509VerifyCert (TestCert, sizeof (TestCert), TestCACert, sizeof (TestCACert));
  UT_ASSERT_TRUE (Status);

  //
  // Release Resources.
  //
  RsaFree  (RsaPubKey);
  RsaFree  (RsaPrivKey);
  FreePool (Signature);
  FreePool (Subject);

  return UNIT_TEST_PASSED;
}

UNIT_TEST_STATUS
EFIAPI
TestVerifyPkcs7SignVerify (
  IN UNIT_TEST_CONTEXT           Context
  )
{
  BOOLEAN  Status;
  UINT8    *P7SignedData;
  UINTN    P7SignedDataSize;
  UINT8    *SignCert;

  P7SignedData = NULL;
  SignCert     = NULL;

  //
  // Construct Signer Certificate from RAW data.
  //
  Status = X509ConstructCertificate (TestCert, sizeof (TestCert), (UINT8 **) &SignCert);
  UT_ASSERT_TRUE (Status);
  UT_ASSERT_NOT_NULL (SignCert);

  //
  // Create PKCS#7 signedData on Payload.
  // Note: Caller should release P7SignedData manually.
  //
  Status = Pkcs7Sign (
             TestKeyPem,
             sizeof (TestKeyPem),
             (CONST UINT8 *) PemPass,
             (UINT8 *) Payload,
             AsciiStrLen (Payload),
             SignCert,
             NULL,
             &P7SignedData,
             &P7SignedDataSize
             );
  UT_ASSERT_TRUE (Status);
  UT_ASSERT_NOT_EQUAL (P7SignedDataSize, 0);

  Status = Pkcs7Verify (
             P7SignedData,
             P7SignedDataSize,
             TestCACert,
             sizeof (TestCACert),
             (UINT8 *) Payload,
             AsciiStrLen (Payload)
             );
  UT_ASSERT_TRUE (Status);

  if (P7SignedData != NULL) {
    FreePool (P7SignedData);
  }
  if (SignCert != NULL) {
    X509Free (SignCert);
  }

  return UNIT_TEST_PASSED;
}

TEST_DESC mRsaCertTest[] = {
    //
    // -----Description--------------------------------------Class----------------------Function-----------------Pre---Post--Context
    //
    {"TestVerifyRsaCertPkcs1SignVerify()", "CryptoPkg.BaseCryptLib.RsaCert", TestVerifyRsaCertPkcs1SignVerify, NULL, NULL, NULL},
};

UINTN mRsaCertTestNum = ARRAY_SIZE(mRsaCertTest);

TEST_DESC mPkcs7Test[] = {
    //
    // -----Description--------------------------------------Class----------------------Function-----------------Pre---Post--Context
    //
    {"TestVerifyPkcs7SignVerify()",        "CryptoPkg.BaseCryptLib.Pkcs7",   TestVerifyPkcs7SignVerify,        NULL, NULL, NULL},
};

UINTN mPkcs7TestNum = ARRAY_SIZE(mPkcs7Test);