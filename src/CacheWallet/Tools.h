// Copyright (c) 2018-2020 - The TurtleCoin Developers
// Copyright (c) 2020 - The MoonBank Developers
//
// Distributed under the GNU Lesser General Public License v3.0.
// Please read MoonBank/License.md

#include <cctype>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <sstream>

#include <Common/ColouredMsg.h>
#include <Common/ConsoleHandler.h>
#include <Common/StringTools.h>
#include <Common/PasswordContainer.h>

void confirmPassword(std::string walletPass);

bool confirm(std::string msg);

std::string formatAmount(uint64_t amount);
std::string formatDollars(uint64_t amount);
std::string formatCents(uint64_t amount);


template <class X> struct Maybe
{
    X x;
    bool isJust;

    Maybe(const X &x) : x (x), isJust(true) {}
    Maybe() : isJust(false) {}
};

template <class X> Maybe<X> Just(const X&x)
{
    return Maybe<X>(x);
}

template <class X> Maybe<X> Nothing()
{
    return Maybe<X>();
}