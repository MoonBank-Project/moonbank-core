// Copyright (c) 2011-2017 The Cryptonote developers
// Copyright (c) 2017-2018 The Circle Foundation & Conceal Devs
// Copyright (c) 2018-2019 Conceal Network & Conceal Devs
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once

#include <vector>
#include <ostream>
#include <istream>

#include "crypto/hash.h"
#include "crypto/chacha8.h"

namespace CryptoNote {
class AccountBase;
class ISerializer;
}

namespace CryptoNote {

class WalletUserTransactionsMoonBank;

class WalletLegacySerializer {
public:
  WalletLegacySerializer(CryptoNote::AccountBase& account, WalletUserTransactionsMoonBank& transactionsMoonBank);

  void serialize(std::ostream& stream, const std::string& password, bool saveDetailed, const std::string& moonbank);
  void deserialize(std::istream& stream, const std::string& password, std::string& moonbank);
  bool deserialize(std::istream& stream, const std::string& password);  

private:
  void saveKeys(CryptoNote::ISerializer& serializer);
  void loadKeys(CryptoNote::ISerializer& serializer);

  Crypto::chacha8_iv encrypt(const std::string& plain, const std::string& password, std::string& cipher);
  void decrypt(const std::string& cipher, std::string& plain, Crypto::chacha8_iv iv, const std::string& password);

  CryptoNote::AccountBase& account;
  WalletUserTransactionsMoonBank& transactionsMoonBank;
  const uint32_t walletSerializationVersion;
};

} //namespace CryptoNote
