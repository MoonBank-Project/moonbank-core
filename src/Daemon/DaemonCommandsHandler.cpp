// Copyright (c) 2011-2017 The Cryptonote developers
// Copyright (c) 2017-2018 The Circle Foundation & Conceal Devs
// Copyright (c) 2018-2019 Conceal Network & Conceal Devs
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <ctime>
#include "math.h"
#include <boost/format.hpp>

#include "DaemonCommandsHandler.h"

#include "Common/ColouredMsg.h"
#include "CryptoNoteCore/Miner.h"
#include "CryptoNoteCore/Core.h"
#include "CryptoNoteCore/Currency.h"
#include "CryptoNoteProtocol/CryptoNoteProtocolHandler.h"
#include "P2p/NetNode.h"
#include "Serialization/SerializationTools.h"
#include "version.h"

namespace {
  template <typename T>
  static bool print_as_json(const T& obj) {
    std::cout << CryptoNote::storeToJson(obj) << ENDL;
    return true;
  }
}

DaemonCommandsHandler::DaemonCommandsHandler(CryptoNote::core& core, CryptoNote::NodeServer& srv, Logging::LoggerManager& log, const CryptoNote::ICryptoNoteProtocolQuery& protocol, CryptoNote::RpcServer* prpc_server) :
  m_core(core), m_srv(srv), logger(log, "daemon"), m_logManager(log), protocolQuery(protocol), m_prpc_server(prpc_server) {
  m_consoleHandler.setHandler("exit", boost::bind(&DaemonCommandsHandler::exit, this, _1), "Shutdown the daemon");
  m_consoleHandler.setHandler("help", boost::bind(&DaemonCommandsHandler::help, this, _1), "Show this help");
  m_consoleHandler.setHandler("save", boost::bind(&DaemonCommandsHandler::save, this, _1), "Save the blockchain");
  m_consoleHandler.setHandler("print_pl", boost::bind(&DaemonCommandsHandler::print_pl, this, _1), "Print peer list");
  m_consoleHandler.setHandler("rollback_chain", boost::bind(&DaemonCommandsHandler::rollback_chain, this, _1), "Rollback chain to specific height, rollback_chain <height>");
  m_consoleHandler.setHandler("print_cn", boost::bind(&DaemonCommandsHandler::print_cn, this, _1), "Print connections");
  m_consoleHandler.setHandler("print_bc", boost::bind(&DaemonCommandsHandler::print_bc, this, _1), "Print blockchain info in a given blocks range, print_bc <begin_height> [<end_height>]");
  m_consoleHandler.setHandler("print_block", boost::bind(&DaemonCommandsHandler::print_block, this, _1), "Print block, print_block <block_hash> | <block_height>");
  m_consoleHandler.setHandler("status", boost::bind(&DaemonCommandsHandler::print_stat, this, _1), "Print statistics, print_stat <nothing=last> | <block_hash> | <block_height>");
  m_consoleHandler.setHandler("print_tx", boost::bind(&DaemonCommandsHandler::print_tx, this, _1), "Print transaction, print_tx <transaction_hash>");
  m_consoleHandler.setHandler("start_mining", boost::bind(&DaemonCommandsHandler::start_mining, this, _1), "Start mining for specified address, start_mining <addr> [threads=1]");
  m_consoleHandler.setHandler("stop_mining", boost::bind(&DaemonCommandsHandler::stop_mining, this, _1), "Stop mining");
  m_consoleHandler.setHandler("print_pool", boost::bind(&DaemonCommandsHandler::print_pool, this, _1), "Print transaction pool (long format)");
  m_consoleHandler.setHandler("print_pool_sh", boost::bind(&DaemonCommandsHandler::print_pool_sh, this, _1), "Print transaction pool (short format)");
  m_consoleHandler.setHandler("show_hr", boost::bind(&DaemonCommandsHandler::show_hr, this, _1), "Start showing hash rate");
  m_consoleHandler.setHandler("hide_hr", boost::bind(&DaemonCommandsHandler::hide_hr, this, _1), "Stop showing hash rate");
  m_consoleHandler.setHandler("set_log", boost::bind(&DaemonCommandsHandler::set_log, this, _1), "set_log <level> - Change current log level, <level> is a number 0-4");
  m_consoleHandler.setHandler("print_ban", boost::bind(&DaemonCommandsHandler::print_ban, this, _1), "Print banned nodes");
  m_consoleHandler.setHandler("ban", boost::bind(&DaemonCommandsHandler::ban, this, _1), "Ban a given <IP> for a given amount of <seconds>, ban <IP> [<seconds>]");
  m_consoleHandler.setHandler("unban", boost::bind(&DaemonCommandsHandler::unban, this, _1), "Unban a given <IP>, unban <IP>");
}

//--------------------------------------------------------------------------------
std::string DaemonCommandsHandler::get_commands_str()
{
  std::stringstream ss;
  ss << CryptoNote::PROJECT_NAME << " v" << PROJECT_VERSION << ENDL;
  ss << "Commands: " << ENDL;
  std::string usage = m_consoleHandler.getUsage();
  boost::replace_all(usage, "\n", "\n  ");
  usage.insert(0, "  ");
  ss << usage << ENDL;
  return ss.str();
}

//--------------------------------------------------------------------------------
bool DaemonCommandsHandler::exit(const std::vector<std::string>& args) {
  m_consoleHandler.requestStop();
  m_srv.sendStopSignal();
  return true;
}
//--------------------------------------------------------------------------------
bool DaemonCommandsHandler::help(const std::vector<std::string>& args) {
  std::cout << get_commands_str() << ENDL;
  return true;
}
//--------------------------------------------------------------------------------
bool DaemonCommandsHandler::save(const std::vector<std::string>& args) {
  m_core.saveBlockchain();
  return true;
}
//--------------------------------------------------------------------------------
bool DaemonCommandsHandler::support(const std::vector<std::string>& args) {
  std::cout << std::endl
    << BrightYellowMsg("If you need further assistance, you can get help directly from the core team") << std::endl
    << BrightYellowMsg("or community members through any of these methods:") << std::endl
    << BrightPurpleMsg("\tDiscord: ") << "  https://discord.gg/9G3emf7jfj" << std::endl
    << BrightRedMsg("\tTelegram Announcement:  ") << "  https://t.me/MoonBank_Announcement " << std::endl
    << BrightBlueMsg("\tTelegram Group:") << "  https://t.me/MoonBank_Project" << std::endl
    << BrightBlueMsg("\tTwitter: ") << "  https://twitter.com/MoonBankProject" << std::endl
    << std::endl;

  return true;
}
//--------------------------------------------------------------------------------
bool DaemonCommandsHandler::print_pl(const std::vector<std::string>& args) {
  m_srv.log_peerlist();
  return true;
}
std::string DaemonCommandsHandler::get_mining_speed(uint32_t hr) {
  if (hr>1e9) return (boost::format("%.2f GH/s") % (hr / 1e9)).str();
  if (hr>1e6) return (boost::format("%.2f MH/s") % (hr / 1e6)).str();
  if (hr>1e3) return (boost::format("%.2f kH/s") % (hr / 1e3)).str();
  return (boost::format("%.0f H/s") % hr).str();
}
//--------------------------------------------------------------------------------
float DaemonCommandsHandler::get_sync_percentage(uint64_t height, uint64_t target_height) {
  target_height = target_height ? target_height < height ? height : target_height : height;
  float pc = 100.0f * height / target_height;
  if (height < target_height && pc > 99.9f) {
    return 99.9f; // to avoid 100% when not fully synced
  }

  return pc;
}
//--------------------------------------------------------------------------------
bool DaemonCommandsHandler::show_hr(const std::vector<std::string>& args)
{
  if (!m_core.get_miner().is_mining())
  {
    std::cout << "Mining is not started. You need to start mining before you can see hash rate." << ENDL;
  } else
  {
    m_core.get_miner().do_print_hashrate(true);
  }
  return true;
}
//--------------------------------------------------------------------------------
bool DaemonCommandsHandler::hide_hr(const std::vector<std::string>& args)
{
  m_core.get_miner().do_print_hashrate(false);
  return true;
}
//--------------------------------------------------------------------------------
bool DaemonCommandsHandler::print_bc_outs(const std::vector<std::string>& args)
{
  if (args.size() != 1)
  {
    std::cout << "need file path as parameter" << ENDL;
    return true;
  }
  m_core.print_blockchain_outs(args[0]);
  return true;
}
//--------------------------------------------------------------------------------
bool DaemonCommandsHandler::print_cn(const std::vector<std::string>& args)
{
  m_srv.get_payload_object().log_connections();
  return true;
}
//--------------------------------------------------------------------------------
bool DaemonCommandsHandler::print_bc(const std::vector<std::string> &args) {
  if (!args.size()) {
    std::cout << "need block index parameter" << ENDL;
    return false;
  }

  uint32_t start_index = 0;
  uint32_t end_index = 0;
  uint32_t end_block_parametr = m_core.get_current_blockchain_height();
  if (!Common::fromString(args[0], start_index)) {
    std::cout << "wrong starter block index parameter" << ENDL;
    return false;
  }

  if (args.size() > 1 && !Common::fromString(args[1], end_index)) {
    std::cout << "wrong end block index parameter" << ENDL;
    return false;
  }

  if (end_index == 0) {
    end_index = end_block_parametr;
  }

  if (end_index > end_block_parametr) {
    std::cout << "end block index parameter shouldn't be greater than " << end_block_parametr << ENDL;
    return false;
  }

  if (end_index <= start_index) {
    std::cout << "end block index should be greater than starter block index" << ENDL;
    return false;
  }

  m_core.print_blockchain(start_index, end_index);
  return true;
}
//--------------------------------------------------------------------------------
bool DaemonCommandsHandler::print_bci(const std::vector<std::string>& args)
{
  m_core.print_blockchain_index();
  return true;
}

bool DaemonCommandsHandler::set_log(const std::vector<std::string>& args)
{
  if (args.size() != 1) {
    std::cout << "use: set_log <log_level_number_0-5>" << ENDL;
    return true;
  }

  uint16_t l = 0;
  if (!Common::fromString(args[0], l)) {
    std::cout << "wrong number format, use: set_log <log_level_number_0-4>" << ENDL;
    return true;
  }

  ++l;

  if (l > Logging::TRACE) {
    std::cout << "wrong number range, use: set_log <log_level_number_0-4>" << ENDL;
    return true;
  }

  m_logManager.setMaxLevel(static_cast<Logging::Level>(l));
  return true;
}

//--------------------------------------------------------------------------------
bool DaemonCommandsHandler::print_block_by_height(uint32_t height)
{
  std::list<CryptoNote::Block> blocks;
  m_core.get_blocks(height, 1, blocks);

  if (1 == blocks.size()) {
    std::cout << "block_id: " << get_block_hash(blocks.front()) << ENDL;
    print_as_json(blocks.front());
  } else {
    uint32_t current_height;
    Crypto::Hash top_id;
    m_core.get_blockchain_top(current_height, top_id);
    std::cout << "block wasn't found. Current block chain height: " << current_height << ", requested: " << height << std::endl;
    return false;
  }

  return true;
}

//--------------------------------------------------------------------------------
bool DaemonCommandsHandler::rollback_chain(const std::vector<std::string> &args) {
  if (args.empty()) {
    std::cout << "expected: rollback_chain <block_height>" << std::endl;
    return true;
  }

  const std::string &arg = args.front();
  uint32_t height = boost::lexical_cast<uint32_t>(arg);
  rollbackchainto(height);
  return true;
}

//--------------------------------------------------------------------------------
bool DaemonCommandsHandler::rollbackchainto(uint32_t height)
{
  m_core.rollback_chain_to(height);
  return true;
}
//--------------------------------------------------------------------------------
bool DaemonCommandsHandler::print_block_by_hash(const std::string& arg)
{
  Crypto::Hash block_hash;
  if (!parse_hash256(arg, block_hash)) {
    return false;
  }

  std::list<Crypto::Hash> block_ids;
  block_ids.push_back(block_hash);
  std::list<CryptoNote::Block> blocks;
  std::list<Crypto::Hash> missed_ids;
  m_core.get_blocks(block_ids, blocks, missed_ids);

  if (1 == blocks.size())
  {
    print_as_json(blocks.front());
  } else
  {
    std::cout << "block wasn't found: " << arg << std::endl;
    return false;
  }

  return true;
}
//--------------------------------------------------------------------------------
uint64_t DaemonCommandsHandler::calculatePercent(const CryptoNote::Currency& currency, uint64_t value, uint64_t total) {
  return static_cast<uint64_t>(100.0 * currency.coin() * static_cast<double>(value) / static_cast<double>(total));
}
//--------------------------------------------------------------------------------
bool DaemonCommandsHandler::print_stat(const std::vector<std::string>& args) {
  uint32_t height = m_core.get_current_blockchain_height();
  uint32_t maxHeight = m_core.get_current_blockchain_height() - 1;
  if (args.empty()) {
    height = maxHeight;
  } else {
    try {
      height = boost::lexical_cast<uint32_t>(args.front());
    } catch (boost::bad_lexical_cast&) {
      Crypto::Hash block_hash;
      if (!parse_hash256(args.front(), block_hash) || !m_core.getBlockHeight(block_hash, height)) {
        return false;
      }
    }
    if (height > maxHeight) {
      std::cout << "printing for last available block: " << maxHeight << std::endl;
      height = maxHeight;
    }
  }

  uint32_t last_known_block_index = std::max(static_cast<uint32_t>(1), protocolQuery.getObservedHeight()) - 1;
  bool synced = ((uint32_t)height == (uint32_t)last_known_block_index);
  uint64_t difficulty = m_core.getNextBlockDifficulty();
  uint64_t hashrate = (uint32_t)round(difficulty / CryptoNote::parameters::DIFFICULTY_TARGET);

  uint64_t totalCoinsInNetwork = m_core.coinsEmittedAtHeight(height);
  uint64_t totalCoinsOnDeposits = m_core.depositAmountAtHeight(height);
  uint64_t amountOfActiveCoins = totalCoinsInNetwork - totalCoinsOnDeposits;
  uint64_t amountOfCoins = CryptoNote::parameters::MONEY_SUPPLY;

  std::time_t uptime = std::time(nullptr) - m_core.getStartTime();
  uint32_t secs = (unsigned int)fmod(uptime, 60.0);
  uint32_t mins = (unsigned int)floor(fmod((uptime / 60.0), 60.0));
  uint32_t hour = (unsigned int)floor(fmod((uptime / 60.0 / 60.0), 24.0));
  uint32_t days = (unsigned int)floor(uptime / 60.0 / 60.0 / 24.0);

  const auto& currency = m_core.currency();
  std::cout
    << "Daemon Height: " << height << std::endl
    << "Blockchain Height: " << last_known_block_index << std::endl
    << "Sync Status: " << (synced ? "synced " : "syncing ") << "(" << get_sync_percentage(height, last_known_block_index) << "%)" << std::endl
    << "Hashrate: " << get_mining_speed(hashrate) << std::endl
    << "Block Difficulty: " << m_core.difficultyAtHeight(height) << std::endl
    << "Total coins in network: " << currency.formatAmount(totalCoinsInNetwork)
    << " (" << currency.formatAmount(calculatePercent(currency, totalCoinsInNetwork, amountOfCoins)) << "%)" << std::endl
    << "Total coins banked: " << currency.formatAmount(totalCoinsOnDeposits)
    << " (" << currency.formatAmount(calculatePercent(currency, totalCoinsOnDeposits, totalCoinsInNetwork)) << "%)" << std::endl
    << "Amount of active coins: " << currency.formatAmount(amountOfActiveCoins)
    << " (" << currency.formatAmount(calculatePercent(currency, amountOfActiveCoins, totalCoinsInNetwork)) << "%)" << std::endl
    << "Software Version: " << PROJECT_VERSION << std::endl
    << "Uptime: " << days << "D " << hour << "H " << mins << "M " << secs << "s "
    << std::endl;
  return true;
}
//--------------------------------------------------------------------------------
bool DaemonCommandsHandler::print_block(const std::vector<std::string> &args) {
  if (args.empty()) {
    std::cout << "expected: print_block (<block_hash> | <block_height>)" << std::endl;
    return true;
  }

  const std::string &arg = args.front();
  try {
    uint32_t height = boost::lexical_cast<uint32_t>(arg);
    print_block_by_height(height);
  } catch (boost::bad_lexical_cast &) {
    print_block_by_hash(arg);
  }

  return true;
}
//--------------------------------------------------------------------------------
bool DaemonCommandsHandler::print_tx(const std::vector<std::string>& args)
{
  if (args.empty()) {
    std::cout << "expected: print_tx <transaction hash>" << std::endl;
    return true;
  }

  const std::string &str_hash = args.front();
  Crypto::Hash tx_hash;
  if (!parse_hash256(str_hash, tx_hash)) {
    return true;
  }

  std::vector<Crypto::Hash> tx_ids;
  tx_ids.push_back(tx_hash);
  std::list<CryptoNote::Transaction> txs;
  std::list<Crypto::Hash> missed_ids;
  m_core.getTransactions(tx_ids, txs, missed_ids, true);

  if (1 == txs.size()) {
    print_as_json(txs.front());
  } else {
    std::cout << "transaction wasn't found: <" << str_hash << '>' << std::endl;
  }

  return true;
}
//--------------------------------------------------------------------------------
bool DaemonCommandsHandler::print_pool(const std::vector<std::string>& args)
{
  logger(Logging::INFO) << "Pool state: " << ENDL << m_core.print_pool(false);
  return true;
}
//--------------------------------------------------------------------------------
bool DaemonCommandsHandler::print_pool_sh(const std::vector<std::string>& args)
{
  logger(Logging::INFO) << "Pool state: " << ENDL << m_core.print_pool(true);
  return true;
}
//--------------------------------------------------------------------------------
bool DaemonCommandsHandler::start_mining(const std::vector<std::string> &args) {
  if (!args.size()) {
    std::cout << "Please, specify wallet address to mine for: start_mining <addr> [threads=1]" << std::endl;
    return true;
  }

  CryptoNote::AccountPublicAddress adr;
  if (!m_core.currency().parseAccountAddressString(args.front(), adr)) {
    std::cout << "target account address has wrong format" << std::endl;
    return true;
  }

  size_t threads_count = 1;
  if (args.size() > 1) {
    bool ok = Common::fromString(args[1], threads_count);
    threads_count = (ok && 0 < threads_count) ? threads_count : 1;
  }

  m_core.get_miner().start(adr, threads_count);
  return true;
}

//--------------------------------------------------------------------------------
bool DaemonCommandsHandler::stop_mining(const std::vector<std::string>& args) {
  m_core.get_miner().stop();
  return true;
}

//--------------------------------------------------------------------------------
bool DaemonCommandsHandler::print_ban(const std::vector<std::string>& args) {
  m_srv.log_banlist();
  return true;
}

bool DaemonCommandsHandler::ban(const std::vector<std::string>& args)
{
  if (args.size() != 1 && args.size() != 2) return false;
  std::string addr = args[0];
  uint32_t ip;
  time_t seconds;
  if (args.size() > 1) {
    try {
      seconds = std::stoi(args[1]);
    } catch (const std::exception &e) {
      return false;
    }
    if (seconds == 0) {
      return false;
    }
  } 
  try {
    ip = Common::stringToIpAddress(addr);
  } catch (const std::exception &e) {
     return false;
  }
  return m_srv.ban_host(ip, seconds);
}

bool DaemonCommandsHandler::unban(const std::vector<std::string>& args)
{
  if (args.size() != 1) return false;
  std::string addr = args[0];
  uint32_t ip;
  try {
    ip = Common::stringToIpAddress(addr);
  }	catch (const std::exception &e) {
    return false;
  }
  return m_srv.unban_host(ip);
}