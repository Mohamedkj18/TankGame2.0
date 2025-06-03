#include "Core/MyPlayer.h"
#include "Core/MyBattleInfo.h"
#include "Core/MySatelliteView.h"

// ------------------------ Abstract Player ------------------------

MyPlayer::MyPlayer(int player_index, size_t x, size_t y, size_t max_steps, size_t num_shells)
    : Player(player_index, x, y, max_steps, num_shells), player_index(player_index), x(x), y(y), max_steps(max_steps), num_shells(num_shells) {}

void MyPlayer::updateTankWithBattleInfo(TankAlgorithm &tank, SatelliteView &satellite_view)
{

    std::set<int> friendlyTanks;
    std::set<int> enemyTanks;
    std::set<int> mines;
    std::set<int> walls;
    std::set<int> shells;

    for (int i = 0; i < x; ++i)
    {
        for (int j = 0; j < y; ++j)
        {
            char object = satellite_view.getObjectAt(i, j);
            int bijectionIndex = bijection(i, j);

            if (object == '%')friendlyTanks.insert(bijectionIndex);
    
            else if (object >= '0' && object <= '9')
            {
                if (object - '0' == player_index)
                    friendlyTanks.insert(bijectionIndex);
                else
                    enemyTanks.insert(bijectionIndex);
            }
            else if (object == '@')mines.insert(bijectionIndex);
            else if (object == '#')walls.insert(bijectionIndex);
            else if (object == '*')shells.insert(bijectionIndex);
            
        }
    }
    MyBattleInfo battleInfo(x, y,
                            friendlyTanks, enemyTanks,
                            mines, walls, shells);

    tank.updateBattleInfo(battleInfo);
}

void MyPlayer::updatePlanedPositions(){
    for (const &pair: plannedPositions){
        pair.second.erase(pair.second.begin());
    }
}

bool MyPlayer::isSquareValid(int x, int y, int step){
    if (info.isMine(x,y) || info.isWall(x,y)){
        return false;
    }
    for (const &pair: plannedPositions){
        std::pair<int, int> pos = pair.second[step];
        if (pair.first == x && pair.second == y){
            return false
        }
    }
}





std::vector<std::pair<int, int>> Player1::getPath(std::pair<int, int> start, std::pair<int, int> target){
    //BFS
    std::queue<std::pair<int, int>> queue;
    std::unordered_map<std::pair<int, int>, bool, pair_hash> visited;
    std::unordered_map<std::pair<int, int>, std::pair<int, int>, pair_hash> parent;
    std::string n;
    queue.push(start);
    visited[start] = true;
    int step = 0;

    while (!queue.empty()) {
        auto current = queue.front();
        queue.pop();
        if (current == target) {
            std::vector<std::pair<int,int>> path;
            while (current != start) {
                auto p = parent[current];
                if(current!=start)path.push_back(current);
                current = p;
            }
            std::reverse(path.begin(), path.end());
            return path;
        }

        for (const auto& dir : directions) {
            auto next = move(current, dir);
            if (!visited[next] && isSquareValid(next.first, next.second, step)) {
                visited[next] = true;
                parent[next] = current;
                queue.push(next);
            }
        }
    }

    return {};
}

std::vector<std::pair<int, int>> Player2::getPath(std::pair<int, int> start, std::pair<int, int> target){
    //DFS
    std::stack<std::pair<int, int>> stack;
    std::unordered_set<std::pair<int, int>, pair_hash> visited;
    std::unordered_map<std::pair<int, int>, std::pair<int, int>, pair_hash> parent;

    
    stack.push(start);
    visited.insert(start);
    visited.insert(target);
    std::pair<int,int>fleeingTo = findFirstLegalLocationToFlee(start.first, start.second);
    
    
    while (!stack.empty()) {
        auto current = stack.top();
        stack.pop();

    
        if (current == fleeingTo) {
            std::vector<std::pair<int, int>> path;
            while (current != start) {
                path.push_back(current);
                current = parent[current];
            }
            std::reverse(path.begin(), path.end());
            
            return path;
        }

        for (const auto& dir : directions) {
            auto next = move(current, dir);


            if (visited.find(next) == visited.end() && !isSquareValid(next.first, next.second)) {
                stack.push(next);
                visited.insert(next);
                parent[next] = current;
            }
        }
    }

    return {};
}


