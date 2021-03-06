#ifndef __WORLD_H__
#define __WORLD_H__

#include <iostream>
#include <vector>
#include <deque>
#include <list>
#include <algorithm>
#include <cmath>
#include <SFML/Graphics.hpp>

#include "Vector.hpp"
#include "Entity.hpp"
#include "Camera.hpp"
#include "ResourceManager.hpp"
#include "Player.hpp"
#include "Guides.hpp"
#include "TrackingCamera.hpp"
#include "WorldConfig.hpp"
#include "WorldCell.hpp"
#include "WorldPathfinder.hpp"
#include "Ocean.hpp"
#include "Interactable.hpp"

class Player;

class World
{
public:
    World(sf::RenderWindow &window, ResourceManager &rm, int64_t width, int64_t height);
    ~World();

    void update(sf::Time &elapsed);
    void transform();
    void draw(sf::RenderTarget *screen);

    Entity *addEntity(Entity *entity);
    const std::vector<Entity *> &getEntitys() const;

    void onMouseButtonReleased(const sf::Event &event);
    void onMouseWheelScrolled(const sf::Event &event);
    void onKeyReleased(const sf::Event &event);

    bool findPath(const Entity &entity, const Vector3f &end,
                  const bool &diagonal,
                  std::deque<Vector3f> &resultPath);

    bool canMoveTo(const Entity &entity, const Vector3f &localPoint) const;

    bool findNearbyFreePosition(const Vector3f &position, Vector3f &out_position);

    bool saveState(std::string path);
    bool loadState(std::string path);
    void loadDefault();

private:
    Player *player_;
    Entity cursor_;
    Ocean ocean_;
    std::vector<Entity *> entities_;

    Camera *camera_;
    ResourceManager *rm_;
    sf::RenderWindow *window_;

    std::unordered_map<int, WorldCell *> cellCache_;
    std::vector<WorldCell *> activeCells_;
    int activeCellId_;

    std::vector<Entity *> visibleEntities_;
    std::vector<Entity *> floorEntities_;

    WorldConfig worldConfig_;

    WorldPathfinder pathfinder_;
    PathfinderVisualizer pathfinderGrid_;
    bool gridVisible_;

    void input_(sf::Time &elapsed);

    void updateCells_();
    void updateVisibileList_();
};

#endif // __WORLD_H__