#pragma once

#include "Algorithms/Roles/Role.h"
#include "Algorithms/MyTankAlgorithm.h"

class ChaserRole : public Role
{
public:
    using Role::Role;
    ~ChaserRole() override = default;

    std::vector<std::pair<int, int>> prepareActions(MyTankAlgorithm &algo) override;

    std::string getRoleName() const override { return "Chaser"; }
    std::unique_ptr<Role> clone() const override
    {
        return std::make_unique<ChaserRole>(*this);
    }
};
