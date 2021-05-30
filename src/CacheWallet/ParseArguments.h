// Copyright (c) 2018-2020 - The TurtleCoin Developers
// Copyright (c) 2020 - The MoonBank Developers
//
// Distributed under the GNU Lesser General Public License v3.0.
// Please read MoonBank/License.md

#include "version.h"
#include "CryptoNoteConfig.h"

#include <algorithm>
#include <iostream>
#include <iomanip>
#include <string>

struct Config {
    bool exit;

    std::string host;
    int port;
    
    bool walletGiven;
    bool passGiven;
    std::string walletFile;
    std::string walletPass;
};

char* getCmdOption(char ** begin, char ** end, const std::string & option);

bool cmdOptionExists(char** begin, char** end, const std::string& option);

Config parseArguments(int argc, char **argv);

void helpMessage();

void versionMessage();