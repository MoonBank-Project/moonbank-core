// Copyright (c) 2012-2016, The CryptoNote developers, The Bytecoin developers
// Copyright (c) 2020 - The MoonBank Developers
//
// Distributed under the GNU Lesser General Public License v3.0.
// Please read MoonBank/License.md

#pragma once

#include "Common/IInputStream.h"
#include "Common/IOutputStream.h"
#include "Serialization/ISerializer.h"
#include "Transfers/TransfersSynchronizer.h"
#include "Wallet/WalletIndices.h"
#include "IWallet.h"

namespace CryptoNote {

class WalletSerializerV2 {
public:
  WalletSerializerV2(
    ITransfersObserver& transfersObserver,
    Crypto::PublicKey& viewPublicKey,
    Crypto::SecretKey& viewSecretKey,
    uint64_t& actualBalance,
    uint64_t& pendingBalance,
    uint64_t& lockedDepositBalance,
    uint64_t& unlockedDepositBalance,
    WalletsContainer& walletsContainer,
    TransfersSyncronizer& synchronizer,
    UnlockTransactionJobs& unlockTransactions,
    WalletTransactions& transactions,
    WalletTransfers& transfers,
    WalletDeposits& deposits,
    UncommitedTransactions& uncommitedTransactions,
    std::string& extra,
    uint32_t transactionSoftLockTime
  );

  void load(Common::IInputStream& source, uint8_t version);
  void save(Common::IOutputStream& destination, WalletSaveLevel saveLevel);

  std::unordered_set<Crypto::PublicKey>& addedKeys();
  std::unordered_set<Crypto::PublicKey>& deletedKeys();

  static const uint8_t MIN_VERSION = 6;
  static const uint8_t SERIALIZATION_VERSION = 6;

private:
  void loadKeyListAndBanalces(CryptoNote::ISerializer& serializer, bool saveMoonBank);
  void saveKeyListAndBanalces(CryptoNote::ISerializer& serializer, bool saveMoonBank);
    
  void loadTransactions(CryptoNote::ISerializer& serializer);
  void saveTransactions(CryptoNote::ISerializer& serializer);

  void loadDeposits(CryptoNote::ISerializer& serializer);
  void saveDeposits(CryptoNote::ISerializer& serializer);

  void loadTransfers(CryptoNote::ISerializer& serializer);
  void saveTransfers(CryptoNote::ISerializer& serializer);

  void loadTransfersSynchronizer(CryptoNote::ISerializer& serializer);
  void saveTransfersSynchronizer(CryptoNote::ISerializer& serializer);

  void loadUnlockTransactionsJobs(CryptoNote::ISerializer& serializer);
  void saveUnlockTransactionsJobs(CryptoNote::ISerializer& serializer);

  ITransfersObserver& m_transfersObserver;
  uint64_t& m_actualBalance;
  uint64_t& m_pendingBalance;
  uint64_t& m_lockedDepositBalance;
  uint64_t& m_unlockedDepositBalance;
  WalletsContainer& m_walletsContainer;
  TransfersSyncronizer& m_synchronizer;
  UnlockTransactionJobs& m_unlockTransactions;
  WalletTransactions& m_transactions;
  WalletTransfers& m_transfers;
  WalletDeposits& m_deposits;
  UncommitedTransactions& m_uncommitedTransactions;
  std::string& m_extra;
  uint32_t m_transactionSoftLockTime;

  std::unordered_set<Crypto::PublicKey> m_addedKeys;
  std::unordered_set<Crypto::PublicKey> m_deletedKeys;
};

} //namespace CryptoNote
