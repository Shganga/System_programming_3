#ifndef MERCHANT_HPP
#define MERCHANT_HPP

#include "player.hpp"

class Merchant : public Player {
public:
    std::string get_type() const override;
};

#endif
