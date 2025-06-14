#pragma once
#include "Algorithms/Roles/Role.h"
#include "Algorithms/MyTankAlgorithm.h"

class MyTankAlgorithm;

class SniperRole : public Role
{


public:
    using Role::Role;
    ~SniperRole() override = default;

    std::vector<std::pair<int, int>> prepareActions(MyTankAlgorithm &algo) override;

    std::string getRoleName() const override { return "Sniper"; }
    std::unique_ptr<Role> clone() const override
    {
        return std::make_unique<SniperRole>(*this);
    }
};