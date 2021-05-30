// Copyright (c) 2020 - The MoonBank Developers
//
// Distributed under the GNU Lesser General Public License v3.0.
// Please read MoonBank/License.md

#pragma once

#include <cstddef>
#include <initializer_list>

namespace CryptoNote {
	struct CheckpointData {
    uint32_t height;
    const char *blockId;
	};

#ifdef __GNUC__
  __attribute__((unused))
#endif

  /**
   * @param CheckpointData Uses data to help sync with the network quicker and to avoid split-chains.
   */

  const std::initializer_list<CheckpointData> CHECKPOINTS = {  
    {0,      "b2606e524141479e3c47fd5c57e998b579d04a08d74dcbfe298b883b42865c75"},
    {2500,   "85ed4b0c79a6241f807890d9e317cf5059bb72b75d77f96ca7bc0cb3353766b3"},
    {5000,   "5dba2f5aa014540551388b22d9d127e2596362b35e624f99872db5200344780c"},
    {7500,   "5c3f75a71b35916a57ec69999a45bd65e49735f669646a870ff5633a2301e2dc"},
    {10000,  "05ca89f788dad5f291692af348fb2566eac5e8a2e284d3ff12cc13c775e2270d"},
    {12500,  "5a4a8a35945e4f7fda3ab0f69aa7ca1f505ba299eb602dc40b450c0c81a5e888"},
    {15000,  "477109839fdb419d0d8b0d826cff85d0b3a60083b557545ea53b9c48755d2b34"},
    {17500,  "50a74a92b34ab6d153d6287f9fff13bc1684ba276a23f3a38ce09856027867ca"},
    {20000,  "26c5bae8a1c69da6fc0768fbb5621edf23cc969606298112f55cf423cec50e23"},
    {22500,  "ef934521346e0d801714b991e29eca223d99c6c9ae57f2ec80346d65cc63e4b6"},
    {25000,  "0f2bb1a35cd6e8f4753a0d955e434491c39345b25ba91d75769f3ebb8fcc3d11"},
    {27500,  "b473484d96f7d3d6755c68570468cea99666a951d408e7b00db2be17c86dde4c"},
    {30000,  "b12aadc6a418fbae5c603327ba5fdff62f0db35411cae3a068506542d44043ac"},
    {32500,  "eaec1c62a16e7a18f0cc5cc3ce558012449374b21f002a3006bca805b6ed8d21"},
    {35000,  "3fd8289b9f31bb461e8a40dacda69b5424aa486c556ddc561ec26d3d4c8ca02d"},
    {37500,  "670d975251a5721854cf9a7edcc3cd6b7177a786fedadc08202dd1023fff3d79"},
    {40000,  "f7f65cc42a0d885b7a045c3026f894a3b86ecc783147d75196cf293d9c995560"},
    {42500,  "8472d77cfbffc5c4a1559d43e7fd7ab4cea1487357080fcde4e013edb9809b65"},
    {45000,  "75ce15ae067f03ddd999ece24cfce87f2603c06b6efd3d780c8ea281ecc2f7e7"},
    {47500,  "640dc48803c0b51aeff5dd9c03d3dd05076d31448a051ba8ee2b4b341835dd2c"},
    {50000,  "c4424df1757396302def1b0272f0bcef7b5ee6ace65d34d676bf1f42c9231e61"},
    {52500,  "2a181a297877a7334fe84176361f2e061e44a3c6c2dceb900ed0c933b340720b"}
	};
} // namespace CryptoNote
