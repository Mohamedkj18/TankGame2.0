#pragma once

#include "Algorithms/Roles/Role.h"
#include "Algorithms/MyTankAlgorithm.h"
#include <optional>

class DefenderRole : public Role
{

public:
    using Role::Role;
    ~DefenderRole() override = default;

    std::vector<std::pair<int, int>> prepareActions(MyTankAlgorithm &algo) override;
    std::string getRoleName() const override { return "Defender"; }
    std::unique_ptr<Role> clone() const override
    {
        return std::make_unique<DefenderRole>(*this);
    }
    std::vector<ActionRequest> getNextMoves(std::vector<std::pair<int, int>> path, std::pair<int, int> target, MyTankAlgorithm &algo);
};
