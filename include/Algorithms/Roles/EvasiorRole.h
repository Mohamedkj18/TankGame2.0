#pragma once

#include <optional>
#include "Role.h"
#include "Algorithms/MyTankAlgorithm.h"

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
    std::set<std::pair<int, int>> createRedZone(std::set<std::pair<int, int>> shells);

private:
    void concatenateSets(std::set<std::pair<int, int>> targetSet, std::set<std::pair<int, int>> setToBeAdded);
};
