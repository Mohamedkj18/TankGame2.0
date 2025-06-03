#include "Core/MyPlayer.h"
#include "Core/MyBattleInfo.h"
#include "Core/MySatelliteView.h"
#include "utils/DirectionUtils.h"

MyPlayer::MyPlayer(int player_index, size_t x, size_t y, size_t max_steps, size_t num_shells)
    : Player(player_index, x, y, max_steps, num_shells), player_index(player_index), x(x), y(y), max_steps(max_steps), num_shells(num_shells) {}

void MyPlayer::updateTankWithBattleInfo(TankAlgorithm &tank, SatelliteView &satellite_view) {
    std::set<int> friendlyTanks, enemyTanks, mines, walls, shells;

    for (int i = 0; i < x; ++i) {
        for (int j = 0; j < y; ++j) {
            char object = satellite_view.getObjectAt(i, j);
            int id = bijection(i, j);

            switch (object) {
                case '%': friendlyTanks.insert(id); break;
                case '@': mines.insert(id); break;
                case '#': walls.insert(id); break;
                case '*': shells.insert(id); break;
                default:
                    if (object >= '0' && object <= '9') {
                        if (object - '0' == player_index)
                            friendlyTanks.insert(id);
                        else
                            enemyTanks.insert(id);
                    }
                    break;
            }
        }
    }

    MyBattleInfo info(x, y, friendlyTanks, enemyTanks, mines, walls, shells);
    lastSatellite = MySatelliteView(x, y, satellite_view);  // optional storage

    int tankId = tank.getTankId(); // tank must expose getTankId()
    std::string role = assignRole(tankId);
    info.setRole(role);

    std::pair<int, int> myPos = {tank.getX() / 2, tank.getY() / 2};
    std::pair<int, int> target = getTargetForTank(tankId);

    std::vector<std::pair<int, int>> path = getPath(myPos, target);
    info.setBFSPath(path);

    tank.updateBattleInfo(info);
}

void MyPlayer::updatePlannedPositions() {
    for (auto &[_, path] : plannedPositions) {
        if (!path.empty())
            path.erase(path.begin());
    }
}

bool MyPlayer::isSquareValid(int x, int y, int step) {
    int idx = bijection(x, y);
    for (const auto &[_, positions] : plannedPositions) {
        if (step < (int)positions.size() && bijection(positions[step].first, positions[step].second) == idx)
            return false;
    }
    return true;
}

std::pair<int, int> MyPlayer::findFirstLegalLocationToFlee(int x, int y) {
    for (Direction dir : directions) {
        int nx = (x + stringToIntDirection[dir][0] + this->x) % this->x;
        int ny = (y + stringToIntDirection[dir][1] + this->y) % this->y;
        if (isSquareValid(nx, ny, 0))
            return {nx, ny};
    }
    return {x, y};
}

std::string MyPlayer::assignRole(int tankId) {
    if (tankRoles.count(tankId))
        return tankRoles[tankId];

    std::string role = (tankId % 2 == 0) ? "chaser" : "sniper";
    tankRoles[tankId] = role;
    return role;
}

std::pair<int, int> MyPlayer::getTargetForTank(int tankId) {
    for (int i = 0; i < (int)x; ++i) {
        for (int j = 0; j < (int)y; ++j) {
            char obj = lastSatellite.getObjectAt(i, j);
            if (obj >= '0' && obj <= '9' && obj - '0' != player_index)
                return {i, j};
        }
    }
    return {0, 0}; // fallback
}

// ------------------------ Player 1 ------------------------
std::vector<std::pair<int, int>> Player1::getPath(std::pair<int, int> start, std::pair<int, int> target) {
    std::queue<std::pair<int, int>> queue;
    std::unordered_map<std::pair<int, int>, bool, pair_hash> visited;
    std::unordered_map<std::pair<int, int>, std::pair<int, int>, pair_hash> parent;

    queue.push(start);
    visited[start] = true;

    int step = 0;

    while (!queue.empty()) {
        auto current = queue.front();
        queue.pop();

        if (current == target) {
            std::vector<std::pair<int, int>> path;
            while (current != start) {
                path.push_back(current);
                current = parent[current];
            }
            std::reverse(path.begin(), path.end());
            return path;
        }

        for (const auto &dir : directions) {
            auto next = move(current, dir);
            if (!visited[next] && isSquareValid(next.first, next.second, step)) {
                visited[next] = true;
                parent[next] = current;
                queue.push(next);
            }
        }
        step++;
    }

    return {}; // No path found
}


// ------------------------ Player 2 ------------------------
std::vector<std::pair<int, int>> Player2::getPath(std::pair<int, int> start, std::pair<int, int> target) {
    std::stack<std::pair<int, int>> stack;
    std::unordered_set<std::pair<int, int>, pair_hash> visited;
    std::unordered_map<std::pair<int, int>, std::pair<int, int>, pair_hash> parent;

    stack.push(start);
    visited.insert(start);

    std::pair<int, int> fallback = findFirstLegalLocationToFlee(start.first, start.second);

    while (!stack.empty()) {
        auto current = stack.top();
        stack.pop();

        if (current == fallback) {
            std::vector<std::pair<int, int>> path;
            while (current != start) {
                path.push_back(current);
                current = parent[current];
            }
            std::reverse(path.begin(), path.end());
            return path;
        }

        for (const auto &dir : directions) {
            auto next = move(current, dir);
            if (!visited.count(next) && isSquareValid(next.first, next.second, 0)) {
                visited.insert(next);
                parent[next] = current;
                stack.push(next);
            }
        }
    }

    return {}; // No fallback path found
}

