#include <memory>
#include "Core/GameObject.h"
#include "Core/GameManager.h"
#include "utils/DirectionUtils.h"

// ------------------------ MovingGameObject ------------------------

GameObject::GameObject(int x, int y, Direction dir, GameManager *game)
    : x(x), y(y), direction(dir), game(game) {}

int GameObject::getX()
{
    return x;
}

int GameObject::getY()
{
    return y;
}

bool GameObject::moveForward()
{
    std::string n;
    updatePosition(direction);
    if (checkForAWall())
    {

        updatePosition(reverseDirection[direction]);
        return false;
    }

    return true;
}

void GameObject::updatePosition(Direction dir)
{
    std::array<int, 2> d = stringToIntDirection[dir];
    x = (x + d[0] + game->getWidth() * 2) % (game->getWidth() * 2);
    y = (y + d[1] + game->getHeight() * 2) % (game->getHeight() * 2);
}
