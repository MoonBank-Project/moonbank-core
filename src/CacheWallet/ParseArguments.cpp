// Copyright (c) 2018-2020 - The TurtleCoin Developers
// Copyright (c) 2020 - The MoonBank Developers
//
// Distributed under the GNU Lesser General Public License v3.0.
// Please read MoonBank/License.md

#include "ParseArguments.h"

/* Thanks to https://stackoverflow.com/users/85381/iain for this small command
   line parsing snippet! https://stackoverflow.com/a/868894/8737306 */
char* getCmdOption(char ** begin, char ** end, const std::string & option)
{
    char ** itr = std::find(begin, end, option);
    if (itr != end && ++itr != end)
    {
        return *itr;
    }
    return 0;
}

bool cmdOptionExists(char** begin, char** end, const std::string& option)
{
    return std::find(begin, end, option) != end;
}


Config parseArguments(int argc, char **argv)
{
    Config config;

    config.exit = false;

    config.host = "127.0.0.1";
    config.port = CryptoNote::RPC_DEFAULT_PORT;

    config.walletGiven = false;
    config.passGiven = false;
    config.walletFile = "";
    config.walletPass = "";

    if (cmdOptionExists(argv, argv+argc, "-h")
     || cmdOptionExists(argv, argv+argc, "--help"))
    {
        helpMessage();
        config.exit = true;
        return config;
    }

    if (cmdOptionExists(argv, argv+argc, "-v")
     || cmdOptionExists(argv, argv+argc, "--version"))
    {
        versionMessage();
        config.exit = true;
        return config;
    }

    if (cmdOptionExists(argv, argv+argc, "--wallet-file"))
    {
        char *wallet = getCmdOption(argv, argv+argc, "--wallet-file");

        if (!wallet)
        {
            std::cout << "--wallet-file was specified, but no wallet file "
                      << "was given!" << std::endl;

            helpMessage();
            config.exit = true;
            return config;
        }

        config.walletFile = std::string(wallet);
        config.walletGiven = true;
    }

    if (cmdOptionExists(argv, argv+argc, "--password"))
    {
        char *password = getCmdOption(argv, argv+argc, "--password");

        if (!password)
        {
            std::cout << "--password was specified, but no password was "
                      << "given!" << std::endl;

            helpMessage();
            config.exit = true;
            return config;
        }

        config.walletPass = std::string(password);
        config.passGiven = true;
    }

    if (cmdOptionExists(argv, argv+argc, "--remote-daemon"))
    {
        char *url = getCmdOption(argv, argv + argc, "--remote-daemon");

        /* No url following --remote-daemon */
        if (!url)
        {
            std::cout << "--remote-daemon was specified, but no daemon was "
                      << "given!" << std::endl;

            helpMessage();

            config.exit = true;
        }
        else
        {
            std::string urlString(url);

            /* Get the index of the ":" */
            size_t splitter = urlString.find_first_of(":");

            /* No ":" present */
            if (splitter == std::string::npos)
            {
                config.host = urlString;
            }
            else
            {
                /* Host is everything before ":" */
                config.host = urlString.substr(0, splitter);

                /* Port is everything after ":" */
                std::string port = urlString.substr(splitter + 1,   
                                                    std::string::npos);

                try
                {
                    config.port = std::stoi(port);
                }
                catch (const std::invalid_argument)
                {
                    std::cout << "Failed to parse daemon port!" << std::endl;
                    config.exit = true;
                }
            }
        }
    }

    return config;
}
void versionMessage()
{
    std::cout << "MoonBank v" << PROJECT_VERSION << " \"moonbank-wallet-beta\""
              << std::endl;
}

void helpMessage()
{
    versionMessage();

    std::cout << std::endl << "moonbank-wallet-beta [--version] [--help] "
              << "[--remote-daemon <url>] [--wallet-file <file>] "
              << "[--password <pass>]"
              << std::endl << std::endl
              << "Commands:" << std::endl << "  -h, " << std::left
              << std::setw(25) << "--help"
              << "Display this help message and exit"
              << std::endl << "  -v, " << std::left << std::setw(25)
              << "--version" << "Display the version information and exit"
              << std::endl << "      " << std::left << std::setw(25)
              << "--remote-daemon <url>" << "Connect to the remote daemon at "
              << "<url>"
              << std::endl << "      " << std::left << std::setw(25)
              << "--wallet-file <file>" << "Open the wallet <file>"
              << std::endl << "      " << std::left << std::setw(25)
              << "--password <pass>" << "Use the password <pass> to open the "
              << "wallet" << std::endl;
}