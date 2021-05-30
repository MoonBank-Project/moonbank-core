// Copyright (c) 2020 - The MoonBank Developers
//
// Distributed under the GNU Lesser General Public License v3.0.
// Please read MoonBank/License.md

#pragma once

#include "CryptoNoteCore/Currency.h"
#include "WalletLegacy/WalletLegacy.h"
#include "Common/StringTools.h"

#include <Logging/LoggerRef.h>

using namespace Logging;

namespace CryptoNote
{
  class TransferCommand
  {
    public:
      const CryptoNote::Currency& m_currency;
      size_t fake_outs_count;
      std::vector<CryptoNote::WalletLegacyTransfer> dsts;
      std::vector<uint8_t> extra;
      uint64_t fee;
      std::map<std::string, std::vector<WalletLegacyTransfer>> aliases;
      std::vector<std::string> messages;
      uint64_t ttl;
      
      /* deposit */
      uint64_t mixin;
      uint64_t amount;
      uint64_t term;

      /* withdraw */
      std::vector<DepositId> dId;

      TransferCommand(const CryptoNote::Currency& currency);

      bool parseTransfer(LoggerRef& logger, const std::vector<std::string> &args);
      
      bool parseCreateDeposit(LoggerRef& logger, const std::vector<std::string> &args);
      bool parseWithdrawDeposit(LoggerRef& logger, const std::vector<std::string> &args);
  };

  template <typename IterT, typename ValueT = typename IterT::value_type>
  class ArgumentReader {
  public:

    ArgumentReader(IterT begin, IterT end) :
      m_begin(begin), m_end(end), m_cur(begin) {
    }

    bool eof() const {
      return m_cur == m_end;
    }

    ValueT next() {
      if (eof()) {
        throw std::runtime_error("unexpected end of arguments");
      }

      return *m_cur++;
    }

  private:

    IterT m_cur;
    IterT m_begin;
    IterT m_end;
  };
}