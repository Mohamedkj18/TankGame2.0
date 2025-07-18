#pragma once
#include "Algorithms/Roles/Role.h"

class MyTankAlgorithm;

class DecoyRole : public Role
{
public:
    using Role::Role;
    ~DecoyRole() override = default;

    std::vector<std::pair<int, int>> prepareActions(MyTankAlgorithm &algo) override;
    std::string getRoleName() const override { return "Decoy"; }
    std::unique_ptr<Role> clone() const override
    {
        return std::make_unique<DecoyRole>(*this);
    }
    std::vector<ActionRequest> getNextMoves(std::vector<std::pair<int, int>> path, MyTankAlgorithm &algo);
};
