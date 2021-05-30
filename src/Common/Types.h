// Copyright (c) 2020 - The MoonBank Developers
//
// Please read the included License.md file

#include <cstdint>

/* Height or Timestamp, 
 * 32-bit is enough, but historically we already have several very large values in blockchain
*/
typedef uint32_t Height;
typedef uint32_t Timestamp;
typedef uint64_t BlockOrTimestamp;

typedef uint64_t Difficulty;
typedef uint64_t Amount;
typedef uint16_t Mixin;

typedef uint64_t Prefix;

typedef uint8_t  Version;
typedef int      Port;
typedef int64_t  SignedAmount;