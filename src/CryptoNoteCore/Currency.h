// Copyright (c) 2011-2017 The Cryptonote developers
// Copyright (c) 2017-2018 The Circle Foundation & Conceal Devs
// Copyright (c) 2018-2019 Conceal Network & Conceal Devs
// Copyright (c) 2020 - The MoonBank Developers
//
// Distributed under the GNU Lesser General Public License v3.0.
// Please read MoonBank/License.md

#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <boost/utility.hpp>
#include "../CryptoNoteConfig.h"
#include "Common/Types.h"
#include "../crypto/hash.h"
#include "../Logging/LoggerRef.h"
#include "CryptoNoteBasic.h"
#include "Difficulty.h"

namespace CryptoNote
{

  class AccountBase;

  class Currency
  {
  public:
    static const std::vector<Amount> PRETTY_AMOUNTS;
    static const std::vector<Amount> REWARD_INCREASING_FACTOR;

    Height maxBlockHeight() const { return m_maxBlockHeight; }
    size_t maxBlockBlobSize() const { return m_maxBlockBlobSize; }
    size_t maxTxSize() const { return m_maxTxSize; }
    Prefix publicAddressBase58Prefix() const { return m_publicAddressBase58Prefix; }
    Height minedMoneyUnlockWindow() const { return m_minedMoneyUnlockWindow; }

    size_t timestampCheckWindow() const { return m_timestampCheckWindow; }
    size_t timestampCheckWindow(uint8_t blockMajorVersion) const { return timestampCheckWindow(); }

    uint64_t blockFutureTimeLimit() const { return m_blockFutureTimeLimit; }
    uint64_t blockFutureTimeLimit(uint8_t blockMajorVersion) const { return blockFutureTimeLimit(); }

    Amount moneySupply() const { return m_moneySupply; }
    //uint64_t genesisBlockReward() const { return m_genesisBlockReward; }

    size_t rewardBlocksWindow() const { return m_rewardBlocksWindow; }

    uint32_t zawyDifficultyBlockIndex() const { return m_zawyDifficultyBlockIndex; }
    size_t zawyDifficultyV2() const { return m_zawyDifficultyV2; }
    uint8_t zawyDifficultyBlockVersion() const { return m_zawyDifficultyBlockVersion; }

    size_t blockGrantedFullRewardZone() const { return m_blockGrantedFullRewardZone; }
    size_t minerTxBlobReservedSize() const { return m_minerTxBlobReservedSize; }

    size_t numberOfDecimalPlaces() const { return m_numberOfDecimalPlaces; }
    uint64_t coin() const { return m_coin; }

    Amount minimumFee() const { return m_minimumFee; }
    Amount minimumFeeBanking() const { return m_minimumFeeBanking; }

    Amount defaultDustThreshold() const { return m_defaultDustThreshold; }

    BlockOrTimestamp difficultyTarget() const { return m_difficultyTarget; }
    size_t difficultyWindow() const { return m_difficultyWindow; }
    size_t difficultyBlocksCount() const { return CryptoNote::parameters::DIFFICULTY_BLOCKS_COUNT; }

    Amount depositMinAmount() const { return m_depositMinAmount; }
    Height depositMinTerm() const { return m_depositMinTerm; }
    Height depositMaxTerm() const { return m_depositMaxTerm; }

    uint64_t depositMinTotalRateFactor() const { return m_depositMinTotalRateFactor; }
    uint64_t depositMaxTotalRate() const { return m_depositMaxTotalRate; }

    size_t maxBlockSizeInitial() const { return m_maxBlockSizeInitial; }
    uint64_t maxBlockSizeGrowthSpeedNumerator() const { return m_maxBlockSizeGrowthSpeedNumerator; }
    uint64_t maxBlockSizeGrowthSpeedDenominator() const { return m_maxBlockSizeGrowthSpeedDenominator; }

    uint64_t lockedTxAllowedDeltaSeconds() const { return m_lockedTxAllowedDeltaSeconds; }
    size_t lockedTxAllowedDeltaBlocks() const { return m_lockedTxAllowedDeltaBlocks; }

    uint64_t mempoolTxLiveTime() const { return m_mempoolTxLiveTime; }
    uint64_t mempoolTxFromAltBlockLiveTime() const { return m_mempoolTxFromAltBlockLiveTime; }
    uint64_t numberOfPeriodsToForgetTxDeletedFromPool() const { return m_numberOfPeriodsToForgetTxDeletedFromPool; }

    uint32_t upgradeHeight(uint8_t majorVersion) const;
    unsigned int upgradeVotingThreshold() const { return m_upgradeVotingThreshold; }
    uint32_t upgradeVotingWindow() const { return m_upgradeVotingWindow; }
    uint32_t upgradeWindow() const { return m_upgradeWindow; }
    uint32_t minNumberVotingBlocks() const { return (m_upgradeVotingWindow * m_upgradeVotingThreshold + 99) / 100; }
    uint32_t maxUpgradeDistance() const { return 7 * m_upgradeWindow; }
    uint32_t calculateUpgradeHeight(uint32_t voteCompleteHeight) const { return voteCompleteHeight + m_upgradeWindow; }

    size_t transactionMaxSize() const { return m_transactionMaxSize; }
    size_t fusionTxMaxSize() const { return m_fusionTxMaxSize; }
    size_t fusionTxMinInputCount() const { return m_fusionTxMinInputCount; }
    size_t fusionTxMinInOutCountRatio() const { return m_fusionTxMinInOutCountRatio; }

    const std::string &blocksFileName() const { return m_blocksFileName; }
    const std::string &blocksMoonBankFileName() const { return m_blocksMoonBankFileName; }
    const std::string &blockIndexesFileName() const { return m_blockIndexesFileName; }
    const std::string &txPoolFileName() const { return m_txPoolFileName; }
    const std::string &blockchinIndicesFileName() const { return m_blockchinIndicesFileName; }

    bool isTestnet() const { return m_testnet; }

    const Block &genesisBlock() const { return m_genesisBlock; }
    const Crypto::Hash &genesisBlockHash() const { return m_genesisBlockHash; }

    bool getBlockReward(size_t medianSize, size_t currentBlockSize, uint64_t alreadyGeneratedCoins, uint64_t fee, uint32_t height,
                        uint64_t &reward, int64_t &emissionChange) const;
    uint64_t calculateInterest(uint64_t amount, uint32_t term) const;
    uint64_t calculateTotalTransactionInterest(const Transaction &tx) const;
    uint64_t getTransactionInputAmount(const TransactionInput &in, uint32_t height) const;
    uint64_t getTransactionAllInputsAmount(const Transaction &tx, uint32_t height) const;
    bool getTransactionFee(const Transaction &tx, uint64_t &fee, uint32_t height) const;
    uint64_t getTransactionFee(const Transaction &tx, uint32_t height) const;
    size_t maxBlockCumulativeSize(uint64_t height) const;

    bool constructMinerTx(uint32_t height, size_t medianSize, uint64_t alreadyGeneratedCoins, size_t currentBlockSize,
                          uint64_t fee, const AccountPublicAddress &minerAddress, Transaction &tx,
                          const BinaryArray &extraNonce = BinaryArray(), size_t maxOuts = 1) const;

    bool isFusionTransaction(const Transaction &transaction) const;
    bool isFusionTransaction(const Transaction &transaction, size_t size) const;
    bool isFusionTransaction(const std::vector<uint64_t> &inputsAmounts, const std::vector<uint64_t> &outputsAmounts, size_t size) const;
    bool isAmountApplicableInFusionTransactionInput(uint64_t amount, uint64_t threshold, uint32_t height) const;
    bool isAmountApplicableInFusionTransactionInput(uint64_t amount, uint64_t threshold, uint8_t &amountPowerOfTen, uint32_t height) const;

    std::string accountAddressAsString(const AccountBase &account) const;
    std::string accountAddressAsString(const AccountPublicAddress &accountPublicAddress) const;
    bool parseAccountAddressString(const std::string &str, AccountPublicAddress &addr) const;

    std::string formatAmount(uint64_t amount) const;
    std::string formatAmount(int64_t amount) const;
    bool parseAmount(const std::string &str, uint64_t &amount) const;

    difficulty_type LWMA3Difficulty(std::vector<uint64_t> timestamps, std::vector<difficulty_type> cumulativeDifficulties) const;

    bool checkProofOfWork(Crypto::cn_context &context, const Block &block, difficulty_type currentDifficulty, Crypto::Hash &proofOfWork) const;

    size_t getApproximateMaximumInputCount(size_t transactionSize, size_t outputCount, size_t mixinCount) const;

  private:
    Currency(Logging::ILogger &log) : logger(log, "currency")
    {
    }

    bool init();

    bool generateGenesisBlock();
    uint64_t baseRewardFunction(uint64_t alreadyGeneratedCoins, uint32_t height) const;

  private:
    Height m_maxBlockHeight;
    size_t m_maxBlockBlobSize;
    size_t m_maxTxSize;
    Prefix m_publicAddressBase58Prefix;
    size_t m_minedMoneyUnlockWindow;

    size_t m_timestampCheckWindow;

    uint64_t m_blockFutureTimeLimit;

    Amount m_moneySupply;
    //uint64_t m_genesisBlockReward;

    size_t m_rewardBlocksWindow;

    uint32_t m_zawyDifficultyBlockIndex;
    size_t m_zawyDifficultyV2;
    uint8_t m_zawyDifficultyBlockVersion;

    size_t m_blockGrantedFullRewardZone;
    size_t m_minerTxBlobReservedSize;

    size_t m_numberOfDecimalPlaces;
    uint64_t m_coin;

    Amount m_minimumFee;
    Amount m_minimumFeeBanking;
    Amount m_defaultDustThreshold;

    BlockOrTimestamp m_difficultyTarget;
    size_t m_difficultyWindow;

    Amount m_depositMinAmount;
    Height m_depositMinTerm;
    Height m_depositMaxTerm;

    uint64_t m_depositMinTotalRateFactor;
    uint64_t m_depositMaxTotalRate;

    size_t m_maxBlockSizeInitial;
    uint64_t m_maxBlockSizeGrowthSpeedNumerator;
    uint64_t m_maxBlockSizeGrowthSpeedDenominator;

    uint64_t m_lockedTxAllowedDeltaSeconds;
    size_t m_lockedTxAllowedDeltaBlocks;

    uint64_t m_mempoolTxLiveTime;
    uint64_t m_mempoolTxFromAltBlockLiveTime;
    uint64_t m_numberOfPeriodsToForgetTxDeletedFromPool;

    Height m_upgradeHeightV2;
    Height m_upgradeHeightV3;

    unsigned int m_upgradeVotingThreshold;
    uint32_t m_upgradeVotingWindow;
    uint32_t m_upgradeWindow;

    size_t m_transactionMaxSize;
    size_t m_fusionTxMaxSize;
    size_t m_fusionTxMinInputCount;
    size_t m_fusionTxMinInOutCountRatio;

    std::string m_blocksFileName;
    std::string m_blocksMoonBankFileName;
    std::string m_blockIndexesFileName;
    std::string m_txPoolFileName;
    std::string m_blockchinIndicesFileName;

    bool m_testnet;
    std::string m_genesisCoinbaseTxHex;

    Block m_genesisBlock;
    Crypto::Hash m_genesisBlockHash;

    Logging::LoggerRef logger;

    friend class CurrencyBuilder;
  };

  class CurrencyBuilder : boost::noncopyable
  {
  public:
    CurrencyBuilder(Logging::ILogger &log);

    Currency currency()
    {
      if (!m_currency.init())
      {
        throw std::runtime_error("Failed to initialize currency object");
      }
      return m_currency;
    }

    Transaction generateGenesisTransaction();
    //Transaction generateGenesisTransaction(const std::vector<AccountPublicAddress>& targets);

    CurrencyBuilder &maxBlockNumber(Height val)
    {
      m_currency.m_maxBlockHeight = val;
      return *this;
    }
    CurrencyBuilder &maxBlockBlobSize(size_t val)
    {
      m_currency.m_maxBlockBlobSize = val;
      return *this;
    }
    CurrencyBuilder &maxTxSize(size_t val)
    {
      m_currency.m_maxTxSize = val;
      return *this;
    }
    CurrencyBuilder &publicAddressBase58Prefix(Prefix val)
    {
      m_currency.m_publicAddressBase58Prefix = val;
      return *this;
    }
    CurrencyBuilder &minedMoneyUnlockWindow(size_t val)
    {
      m_currency.m_minedMoneyUnlockWindow = val;
      return *this;
    }

    CurrencyBuilder &timestampCheckWindow(size_t val)
    {
      m_currency.m_timestampCheckWindow = val;
      return *this;
    }
    CurrencyBuilder &blockFutureTimeLimit(uint64_t val)
    {
      m_currency.m_blockFutureTimeLimit = val;
      return *this;
    }
    CurrencyBuilder &moneySupply(Amount val)
    {
      m_currency.m_moneySupply = val;
      return *this;
    }

    CurrencyBuilder &rewardBlocksWindow(size_t val)
    {
      m_currency.m_rewardBlocksWindow = val;
      return *this;
    }

    CurrencyBuilder &zawyDifficultyBlockIndex(uint32_t val)
    {
      m_currency.m_zawyDifficultyBlockIndex = val;
      return *this;
    }
    CurrencyBuilder &zawyDifficultyV2(size_t val)
    {
      m_currency.m_zawyDifficultyV2 = val;
      return *this;
    }
    CurrencyBuilder &zawyDifficultyBlockVersion(uint8_t val)
    {
      m_currency.m_zawyDifficultyBlockVersion = val;
      return *this;
    }

    CurrencyBuilder &blockGrantedFullRewardZone(size_t val)
    {
      m_currency.m_blockGrantedFullRewardZone = val;
      return *this;
    }
    CurrencyBuilder &minerTxBlobReservedSize(size_t val)
    {
      m_currency.m_minerTxBlobReservedSize = val;
      return *this;
    }

    CurrencyBuilder &numberOfDecimalPlaces(size_t val);

    CurrencyBuilder &minimumFee(Amount val)
    {
      m_currency.m_minimumFee = val;
      return *this;
    }
    CurrencyBuilder &minimumFeeBanking(Amount val)
    {
      m_currency.m_minimumFeeBanking = val;
      return *this;
    }

    CurrencyBuilder &defaultDustThreshold(Amount val)
    {
      m_currency.m_defaultDustThreshold = val;
      return *this;
    }

    CurrencyBuilder &difficultyTarget(BlockOrTimestamp val)
    {
      m_currency.m_difficultyTarget = val;
      return *this;
    }

    CurrencyBuilder &difficultyWindow(size_t val);

    CurrencyBuilder &depositMinAmount(Amount val)
    {
      m_currency.m_depositMinAmount = val;
      return *this;
    }
    CurrencyBuilder &depositMinTerm(Height val)
    {
      m_currency.m_depositMinTerm = val;
      return *this;
    }
    CurrencyBuilder &depositMaxTerm(Height val)
    {
      m_currency.m_depositMaxTerm = val;
      return *this;
    }
    CurrencyBuilder &depositMinTotalRateFactor(uint64_t val)
    {
      m_currency.m_depositMinTotalRateFactor = val;
      return *this;
    }
    CurrencyBuilder &depositMaxTotalRate(uint64_t val)
    {
      m_currency.m_depositMaxTotalRate = val;
      return *this;
    }

    CurrencyBuilder &maxBlockSizeInitial(size_t val)
    {
      m_currency.m_maxBlockSizeInitial = val;
      return *this;
    }
    CurrencyBuilder &maxBlockSizeGrowthSpeedNumerator(uint64_t val)
    {
      m_currency.m_maxBlockSizeGrowthSpeedNumerator = val;
      return *this;
    }
    CurrencyBuilder &maxBlockSizeGrowthSpeedDenominator(uint64_t val)
    {
      m_currency.m_maxBlockSizeGrowthSpeedDenominator = val;
      return *this;
    }

    CurrencyBuilder &lockedTxAllowedDeltaSeconds(uint64_t val)
    {
      m_currency.m_lockedTxAllowedDeltaSeconds = val;
      return *this;
    }
    CurrencyBuilder &lockedTxAllowedDeltaBlocks(size_t val)
    {
      m_currency.m_lockedTxAllowedDeltaBlocks = val;
      return *this;
    }

    CurrencyBuilder &mempoolTxLiveTime(uint64_t val)
    {
      m_currency.m_mempoolTxLiveTime = val;
      return *this;
    }
    CurrencyBuilder &mempoolTxFromAltBlockLiveTime(uint64_t val)
    {
      m_currency.m_mempoolTxFromAltBlockLiveTime = val;
      return *this;
    }
    CurrencyBuilder &numberOfPeriodsToForgetTxDeletedFromPool(uint64_t val)
    {
      m_currency.m_numberOfPeriodsToForgetTxDeletedFromPool = val;
      return *this;
    }

    CurrencyBuilder &upgradeHeightV2(Height val)
    {
      m_currency.m_upgradeHeightV2 = val;
      return *this;
    }
    CurrencyBuilder &upgradeHeightV3(Height val)
    {
      m_currency.m_upgradeHeightV3 = val;
      return *this;
    }

    CurrencyBuilder &upgradeVotingThreshold(unsigned int val);
    CurrencyBuilder &upgradeVotingWindow(uint32_t val)
    {
      m_currency.m_upgradeVotingWindow = val;
      return *this;
    }
    CurrencyBuilder &upgradeWindow(uint32_t val);

    CurrencyBuilder &transactionMaxSize(size_t val)
    {
      m_currency.m_transactionMaxSize = val;
      return *this;
    }
    CurrencyBuilder &fusionTxMaxSize(size_t val)
    {
      m_currency.m_fusionTxMaxSize = val;
      return *this;
    }
    CurrencyBuilder &fusionTxMinInputCount(size_t val)
    {
      m_currency.m_fusionTxMinInputCount = val;
      return *this;
    }
    CurrencyBuilder &fusionTxMinInOutCountRatio(size_t val)
    {
      m_currency.m_fusionTxMinInOutCountRatio = val;
      return *this;
    }

    CurrencyBuilder &blocksFileName(const std::string &val)
    {
      m_currency.m_blocksFileName = val;
      return *this;
    }
    CurrencyBuilder &blocksMoonBankFileName(const std::string &val)
    {
      m_currency.m_blocksMoonBankFileName = val;
      return *this;
    }
    CurrencyBuilder &blockIndexesFileName(const std::string &val)
    {
      m_currency.m_blockIndexesFileName = val;
      return *this;
    }
    CurrencyBuilder &txPoolFileName(const std::string &val)
    {
      m_currency.m_txPoolFileName = val;
      return *this;
    }
    CurrencyBuilder &blockchinIndicesFileName(const std::string &val)
    {
      m_currency.m_blockchinIndicesFileName = val;
      return *this;
    }

    CurrencyBuilder &genesisCoinbaseTxHex(const std::string &val)
    {
      m_currency.m_genesisCoinbaseTxHex = val;
      return *this;
    }
    CurrencyBuilder &testnet(bool val)
    {
      m_currency.m_testnet = val;
      return *this;
    }

  private:
    Currency m_currency;
  };

} // namespace CryptoNote
