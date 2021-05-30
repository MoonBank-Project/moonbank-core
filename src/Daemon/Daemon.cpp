// Copyright (c) 2011-2017 The Cryptonote developers
// Copyright (c) 2016-2018, The Karbo developers
// Copyright (c) 2017-2018 The Circle Foundation & Conceal Devs
// Copyright (c) 2018-2019 Conceal Network & Conceal Devs
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "version.h"

#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>

#include "DaemonCommandsHandler.h"

#include "../CheckpointData.h"
#include "Common/ColouredMsg.h"
#include "Common/SignalHandler.h"
#include "Common/PathTools.h"
#include "crypto/hash.h"
#include "CryptoNoteCore/Core.h"
#include "CryptoNoteCore/CoreConfig.h"
#include "CryptoNoteCore/CryptoNoteTools.h"
#include "CryptoNoteCore/Currency.h"
#include "CryptoNoteCore/MinerConfig.h"
#include "CryptoNoteProtocol/CryptoNoteProtocolHandler.h"
#include "CryptoNoteProtocol/ICryptoNoteProtocolQuery.h"
#include "P2p/NetNode.h"
#include "P2p/NetNodeConfig.h"
#include "Rpc/RpcServer.h"
#include "Rpc/RpcServerConfig.h"
#include "version.h"

#include "Logging/ConsoleLogger.h"
#include <Logging/LoggerManager.h>

#if defined(WIN32)
#include <crtdbg.h>
#endif

using Common::JsonValue;
using namespace CryptoNote;
using namespace Logging;

namespace po = boost::program_options;

namespace
{
  const command_line::arg_descriptor<std::string> arg_version   = {"version", "Shows OS and MoonBank Software version details"};
  const command_line::arg_descriptor<std::string> arg_log_file  = {"log-file", "", ""};
  const command_line::arg_descriptor<int>         arg_log_level = {"log-level", "", 2};
  const command_line::arg_descriptor<bool>        arg_console   = {"no-console", "Disable daemon console commands"};

  const command_line::arg_descriptor<std::string> arg_load_checkpoints = {"load-checkpoints", "Launched with --load-checkpoints=filename.csv for faster initial blockchain sync", "default"};

  const command_line::arg_descriptor<std::string> arg_set_node_id     = { "node-id", "If you're setting your daemon to become a public node, then setting an ID is recommended", "" };
  const command_line::arg_descriptor<std::string> arg_set_fee_address = { "fee-address", "Sets the convenience charge address for remote wallets that use this node.", "" };
  const command_line::arg_descriptor<int>         arg_set_fee_amount  = { "fee-amount", "Sets the convenience charge amount for remote wallets that use this node.", 0 };
  const command_line::arg_descriptor<std::string> arg_set_view_key    = { "view-key", "Set secret view-key for remote node fee confirmation", "" };

  /* to be deleted eventually */
  const command_line::arg_descriptor<bool> arg_testnet_on = {"testnet", "Used to deploy a private testnet. Use it with --data-dir flag. \"moonbank-wallet\" must be launched with --testnet flag.", false};
  const command_line::arg_descriptor<bool> arg_print_hash = { "print-hash", "Creates an example and exits" };
}

bool command_line_preprocessor(const boost::program_options::variables_map& vm, LoggerRef& logger);

void print_example_hash() {
  Logging::ConsoleLogger logger;
  CryptoNote::Transaction tx = CryptoNote::CurrencyBuilder(logger).generateGenesisTransaction();
  CryptoNote::BinaryArray txb = CryptoNote::toBinaryArray(tx);
  std::string tx_hex = Common::toHex(txb);
  std::cout << "Created Hash: " << tx_hex << "." << std::endl;
  return;
}

JsonValue buildLoggerConfiguration(Level level, const std::string& logfile) {
  JsonValue loggerConfiguration(JsonValue::OBJECT);
  loggerConfiguration.insert("globalLevel", static_cast<int64_t>(level));

  JsonValue& cfgLoggers = loggerConfiguration.insert("loggers", JsonValue::ARRAY);
  JsonValue& fileLogger = cfgLoggers.pushBack(JsonValue::OBJECT);
  fileLogger.insert("type", "file");
  fileLogger.insert("filename", logfile);
  fileLogger.insert("level", static_cast<int64_t>(TRACE));

  JsonValue& consoleLogger = cfgLoggers.pushBack(JsonValue::OBJECT);
  consoleLogger.insert("type", "console");
  consoleLogger.insert("level", static_cast<int64_t>(TRACE));
  consoleLogger.insert("pattern", "");

  return loggerConfiguration;
}

int main(int argc, char* argv[])
{

#ifdef _WIN32
  _CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif

  LoggerManager logManager;
  LoggerRef logger(logManager, "Daemon.cpp");

  try {
    po::options_description desc_cmd_only("Command line options");
    po::options_description desc_cmd_sett("Command line options and settings options");

   desc_cmd_sett.add_options() 
      ("enable-blockchain-indexes,i", po::bool_switch()->default_value(false), "Enable blockchain indexes");

    command_line::add_arg(desc_cmd_only, command_line::arg_help);
    command_line::add_arg(desc_cmd_only, command_line::arg_version);
    command_line::add_arg(desc_cmd_only, command_line::arg_data_dir, Tools::getDefaultDataDirectory());
    command_line::add_arg(desc_cmd_sett, arg_set_node_id);
    command_line::add_arg(desc_cmd_sett, arg_log_file);
    command_line::add_arg(desc_cmd_sett, arg_log_level);
    command_line::add_arg(desc_cmd_sett, arg_console);
  	command_line::add_arg(desc_cmd_sett, arg_set_view_key);
    command_line::add_arg(desc_cmd_sett, arg_testnet_on);
    command_line::add_arg(desc_cmd_sett, arg_print_hash);
    command_line::add_arg(desc_cmd_sett, arg_load_checkpoints);
    command_line::add_arg(desc_cmd_sett, arg_set_fee_address);
    command_line::add_arg(desc_cmd_sett, arg_set_fee_amount);

    RpcServerConfig::initOptions(desc_cmd_sett);
    CoreConfig::initOptions(desc_cmd_sett);
    NetNodeConfig::initOptions(desc_cmd_sett);
    MinerConfig::initOptions(desc_cmd_sett);

    po::options_description desc_options("Allowed options");
    desc_options.add(desc_cmd_only).add(desc_cmd_sett);

    po::variables_map vm;
    bool r = command_line::handle_error_helper(desc_options, [&]() {
      po::store(po::parse_command_line(argc, argv, desc_options), vm);

      if (command_line::get_arg(vm, command_line::arg_help)) {
        std::cout << "MoonBank v" << PROJECT_VERSION << ENDL << ENDL;
        std::cout << desc_options << std::endl;
        return false;
      }

      if (command_line::get_arg(vm, arg_print_hash)) {
		    print_example_hash();
        return false;
      }

      std::string data_dir = command_line::get_arg(vm, command_line::arg_data_dir);
      boost::filesystem::path data_dir_path(data_dir);

      po::notify(vm);
      return true;
    });

    if (!r) {
      return 1;
    }

    auto modulePath = Common::NativePathToGeneric(argv[0]);
    auto cfgLogFile = Common::NativePathToGeneric(command_line::get_arg(vm, arg_log_file));

    if (cfgLogFile.empty()) {
      cfgLogFile = Common::ReplaceExtenstion(modulePath, ".log");
    } else {
      if (!Common::HasParentPath(cfgLogFile)) {
        cfgLogFile = Common::CombinePath(Common::GetPathDirectory(modulePath), cfgLogFile);
      }
    }

    // configure logging
    Level cfgLogLevel = static_cast<Level>(static_cast<int>(Logging::ERROR) + command_line::get_arg(vm, arg_log_level));
    logManager.configure(buildLoggerConfiguration(cfgLogLevel, cfgLogFile));

    std::cout << std::endl;
    logger(INFO, BRIGHT_MAGENTA) << "\tMoonBank v" << PROJECT_VERSION;
    std::cout << std::endl;

    if (command_line_preprocessor(vm, logger)) {
      return 0;
    }

    bool testnet_mode = command_line::get_arg(vm, arg_testnet_on);
    if (testnet_mode) {
      logger(INFO, BRIGHT_YELLOW) << "Starting in testnet mode!";
    }

    //create objects and link them
    CryptoNote::CurrencyBuilder currencyBuilder(logManager);
    currencyBuilder.testnet(testnet_mode);

    try {
      currencyBuilder.currency();
    } catch (std::exception&) {
      std::cout << "Incorrect Genesis TX!";
      return 1;
    }

    CryptoNote::Currency currency = currencyBuilder.currency();
    CryptoNote::core ccore(currency, nullptr, logManager, vm["enable-blockchain-indexes"].as<bool>());

    bool use_checkpoints = !command_line::get_arg(vm, arg_load_checkpoints).empty();
    CryptoNote::Checkpoints checkpoints(logManager);
    if (use_checkpoints && !testnet_mode) {
      logger(INFO) << "Loading Checkpoints for faster initial sync...";
      std::string checkpoints_file = command_line::get_arg(vm, arg_load_checkpoints);
      if (checkpoints_file == "default") {
        for (const auto& cp : CryptoNote::CHECKPOINTS) {
          checkpoints.add_checkpoint(cp.height, cp.blockId);
        }
        if (CryptoNote::CHECKPOINTS.size() > 0) {
          logger(INFO, BRIGHT_GREEN) << "Loaded " << CryptoNote::CHECKPOINTS.size() << " default checkpoints";
        }
      } else {
        bool results = checkpoints.load_checkpoints_from_file(checkpoints_file);
        if (!results) {
          throw std::runtime_error("Failed to load checkpoints");
        }
      }
    }

    CoreConfig coreConfig;
    coreConfig.init(vm);
    NetNodeConfig netNodeConfig;
    netNodeConfig.init(vm);
    netNodeConfig.setTestnet(testnet_mode);
    MinerConfig minerConfig;
    minerConfig.init(vm);
    RpcServerConfig rpcConfig;
    rpcConfig.init(vm);

    if (!coreConfig.configFolderDefaulted) {
      if (!Tools::directoryExists(coreConfig.configFolder)) {
        throw std::runtime_error("Directory does not exist: " + coreConfig.configFolder);
      }
    } else {
      if (!Tools::create_directories_if_necessary(coreConfig.configFolder)) {
        throw std::runtime_error("Can't create directory: " + coreConfig.configFolder);
      }
    }

    System::Dispatcher dispatcher;
    CryptoNote::CryptoNoteProtocolHandler cprotocol(currency, dispatcher, ccore, nullptr, logManager);
    CryptoNote::NodeServer p2psrv(dispatcher, cprotocol, logManager);
    CryptoNote::RpcServer rpcServer(dispatcher, logManager, ccore, p2psrv, cprotocol);

    cprotocol.set_p2p_endpoint(&p2psrv);
    ccore.set_cryptonote_protocol(&cprotocol);
    DaemonCommandsHandler dch(ccore, p2psrv, logManager, cprotocol, &rpcServer);

    rpcServer.setFeeAddress(command_line::get_arg(vm, arg_set_fee_address));
    rpcServer.setFeeAmount(command_line::get_arg(vm, arg_set_fee_amount));

    // initialize objects
    logger(INFO) << "Initializing P2P server...";
    if (!p2psrv.init(netNodeConfig)) {
      logger(ERROR, BRIGHT_RED) << "Failed to initialize P2P server.";
      return 1;
    }
    logger(INFO, BRIGHT_GREEN) << "P2P server has been initialized!";

    // initialize core here
    logger(INFO) << "Initializing core...";
    if (!ccore.init(coreConfig, minerConfig, true)) {
      logger(ERROR, BRIGHT_RED) << "Failed to initialize core";
      return 1;
    }
    logger(INFO, BRIGHT_GREEN) << "Core has been initialized!";

    // start components
    if (!command_line::has_arg(vm, arg_console)) {
      dch.start_handling();
    }

    logger(INFO) << "Starting daemon RPC server on address " << rpcConfig.getBindAddress();
    
    std::string id_str = command_line::get_arg(vm, arg_set_node_id);
    if (!id_str.empty() && id_str.size() > 128) {
      logger(ERROR, BRIGHT_RED) << "Too long contact info";
      return 1;
    }
    if (command_line::has_arg(vm, arg_set_node_id)) {
      if (!id_str.empty()) {
        rpcServer.setNodeInfo(id_str);
      }
    }

    /* Set address for remote node fee */
  	if (command_line::has_arg(vm, arg_set_fee_address) && command_line::has_arg(vm, arg_set_fee_amount)) {
	    std::string addr_str = command_line::get_arg(vm, arg_set_fee_address);
	    if (!addr_str.empty()) {
        AccountPublicAddress acc = boost::value_initialized<AccountPublicAddress>();
        if (!currency.parseAccountAddressString(addr_str, acc)) {
          logger(ERROR, BRIGHT_RED) << "Bad fee address: " << addr_str;
          return 1;
        }
        rpcServer.setFeeAddress(addr_str);
        logger(INFO, BRIGHT_YELLOW) << "Remote node fee address set: " << addr_str;
      }

      uint32_t fee_amount = command_line::get_arg(vm, arg_set_fee_amount);
      uint32_t max_node_fee = 100000;
      if (fee_amount > max_node_fee) {
        fee_amount = max_node_fee;
        uint64_t formatted_fee = fee_amount;
        logger(WARNING, YELLOW) << "Your node fee is too high! To be reasonable, it has been change to " + currency.formatAmount(formatted_fee) + " $BANK.";
      } else if (fee_amount > 0 && fee_amount <= max_node_fee) {
        uint64_t formatted_fee = fee_amount;
        logger(INFO, BRIGHT_YELLOW) << "Remote node fee set: " + currency.formatAmount(formatted_fee);
      }
    }
  
    /* This sets the view-key so we can confirm that
       the fee is part of the transaction blob */       
    if (command_line::has_arg(vm, arg_set_view_key)) {
      std::string vk_str = command_line::get_arg(vm, arg_set_view_key);
	    if (!vk_str.empty()) {
        rpcServer.setViewKey(vk_str);
        logger(INFO, BRIGHT_YELLOW) << "Secret view key set: " << vk_str;
      }
    }
 
    rpcServer.start(rpcConfig.bindIp, rpcConfig.bindPort);
    logger(INFO, BRIGHT_GREEN) << "Core RPC server has been initialized on " << rpcConfig.getBindAddress();

    Tools::SignalHandler::install([&dch, &p2psrv] {
      dch.stop_handling();
      p2psrv.sendStopSignal();
    });

    logger(INFO) << "Starting P2P net loop...";
    p2psrv.run();
    logger(INFO) << "P2P net loop stopped";

    dch.stop_handling();

    //stop components
    logger(INFO) << "Stopping core RPC server...";
    rpcServer.stop();

    //deinitialize components
    logger(INFO) << "Deinitializing core...";
    ccore.deinit();
    logger(INFO) << "Deinitializing P2P...";
    p2psrv.deinit();

    ccore.set_cryptonote_protocol(NULL);
    cprotocol.set_p2p_endpoint(NULL);
  } catch (const std::exception& e) {
    logger(ERROR, BRIGHT_RED) << "Exception: " << e.what();
    return 1;
  }

  logger(INFO, BRIGHT_GREEN) << "The node has successfully shutdown.";
  return 0;
}

bool command_line_preprocessor(const boost::program_options::variables_map &vm, LoggerRef &logger) {
  if (command_line::get_arg(vm, command_line::arg_version)) {
    std::cout << "\t==[ Version Information ]==" << std::endl
      << BrightPurpleMsg("MoonBank Software: v") << BrightYellowMsg(PROJECT_VERSION) << std::endl
      << BrightPurpleMsg("OS Version: ") << BrightYellowMsg(Tools::get_os_version_string()) << std::endl;
    return true;
  }

  return false;
}
