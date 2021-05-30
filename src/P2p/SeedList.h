// Copyright (c) 2020 - The MoonBank Developers
//
// Distributed under the GNU Lesser General Public License v3.0.
// Please read MoonBank/License.md

#pragma once

#include <cstdint>
#include <initializer_list>

namespace CryptoNote {
  const std::initializer_list<const char *> SEED_NODES = {
    "82.118.23.241:38999", /* Blockchain Explorer Node */
    "5.34.178.117:38999",
    "82.118.23.77:38999"
  };
}
