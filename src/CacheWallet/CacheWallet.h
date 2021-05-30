#include <cctype>
#include <future>

#include "INode.h"
#include "version.h"

#include <Common/ConsoleHandler.h>
#include <Common/SignalHandler.h>

#include <CryptoNoteCore/Account.h>
#include <CryptoNoteCore/Currency.h>

#include <Logging/FileLogger.h>
#include <Logging/LoggerManager.h>

#include <Mnemonics/electrum-words.h>

#include <NodeRpcProxy/NodeRpcProxy.h>

#include <Common/ColouredMsg.h>
#include <Common/PasswordContainer.h>
#include <MoonBankWallet/Transfer.h>
#include <MoonBankWallet/ParseArguments.h>

#include <System/Dispatcher.h>

#include <Wallet/WalletGreen.h>
// Copyright (c) 2018-2020 - The TurtleCoin Developers
// Copyright (c) 2020 - The MoonBank Developers
//
// Distributed under the GNU Lesser General Public License v3.0.
// Please read MoonBank/License.md

#include <boost/thread/thread.hpp>

#ifdef _WIN32
/* Prevents windows.h redefining min/max which breaks compilation */
#define NOMINMAX
#include <windows.h>
#endif

/* zedwallet port as of:
   https://github.com/turtlecoin/turtlecoin/commit/1c00362de4fc6cf5a0c3ab3804f1671576b3d1fa#
   
   Currently lacks support for view wallets and seed support.
*/

enum Action {Open, Generate, Import, SeedImport};

Action getAction(Config &config);

void logIncorrectMnemonicWords(std::vector<std::string> words);

void promptSaveKeys(CryptoNote::WalletGreen &wallet);

void printPrivateKeys(CryptoNote::WalletGreen &wallet);

void balance(CryptoNote::INode &node, CryptoNote::WalletGreen &wallet);

void welcomeMsg();

void help();

std::string getInputAndDoWorkWhileIdle(CryptoNote::WalletGreen &wallet);

void inputLoop(std::shared_ptr<WalletInfo> &walletInfo, CryptoNote::INode &node);

void exportKeys(std::shared_ptr<WalletInfo> &walletInfo);

void run(CryptoNote::WalletGreen &wallet, CryptoNote::INode &node,
         Config &config);

void blockchainHeight(CryptoNote::INode &node, CryptoNote::WalletGreen &wallet);

void listTransfers(bool incoming, bool outgoing, 
                   CryptoNote::WalletGreen &wallet);

void findNewTransactions(CryptoNote::INode &node,
                         std::shared_ptr<WalletInfo> &walletInfo);

void reset(CryptoNote::INode &node, std::shared_ptr<WalletInfo> &walletInfo);

void printOutgoingTransfer(CryptoNote::WalletTransaction t);

void printIncomingTransfer(CryptoNote::WalletTransaction t);

void checkForNewTransactions(std::shared_ptr<WalletInfo> &walletInfo);

void confirmPassword(std::string);

bool isValidMnemonic(std::string &mnemonic_phrase, 
                     Crypto::SecretKey &private_spend_key);

bool shutdown(CryptoNote::WalletGreen &wallet, CryptoNote::INode &node,
              bool &alreadyShuttingDown);

std::string getNewWalletFileName();

std::string getExistingWalletFileName(Config &config);

std::string getWalletPassword(bool verifyPwd);

std::shared_ptr<WalletInfo> importFromKeys(CryptoNote::WalletGreen &wallet, 
                                           Crypto::SecretKey privateSpendKey,
                                           Crypto::SecretKey privateViewKey);

Maybe<std::shared_ptr<WalletInfo>> openWallet(CryptoNote::WalletGreen &wallet,
                                              Config &config);

std::shared_ptr<WalletInfo> importWallet(CryptoNote::WalletGreen &wallet);

std::shared_ptr<WalletInfo> mnemonicImportWallet(CryptoNote::WalletGreen &wallet);

std::shared_ptr<WalletInfo> generateWallet(CryptoNote::WalletGreen &wallet);

Maybe<std::shared_ptr<WalletInfo>> handleAction(CryptoNote::WalletGreen &wallet,
                                                Action action, Config &config);

Crypto::SecretKey getPrivateKey(std::string outputMsg);

ColouredMsg getPrompt(std::shared_ptr<WalletInfo> &walletInfo);