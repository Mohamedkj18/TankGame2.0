#pragma once

#include "Algorithms/Roles/Role.h"
#include "Algorithms/MyTankAlgorithm.h"
#include <optional>

class EvasiorRole : public Role
{

public:
    using Role::Role;
    ~EvasiorRole() override = default;

    std::vector<std::pair<int, int>> prepareActions(MyTankAlgorithm &algo) override;
    std::string getRoleName() const override { return "Evasior"; }
    std::unique_ptr<Role> clone() const override
    {
        return std::make_unique<EvasiorRole>(*this);
    }
    std::vector<ActionRequest> getNextMoves(std::vector<std::pair<int, int>> path, std::pair<int, int> target, MyTankAlgorithm &algo);

private:
    void concatenateSets(std::set<std::pair<int, int>> targetSet, std::set<std::pair<int, int>> setToBeAdded);
    std::set<std::pair<int, int>> createRedZone(std::set<std::pair<int, int>> shells, int distFromTarget);
    std::set<std::pair<int, int>> transformToPairs(std::set<int> toBeTransformed);
};
