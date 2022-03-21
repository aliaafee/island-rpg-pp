#include "World.hpp"

World::World(sf::RenderWindow &window,
             ResourceManager &rm,
             int64_t width, int64_t height) : window_(&window),
                                              rm_(&rm),
                                              camera_(
                                                  new TrackingCamera(Vector3f(0, 0, 0),
                                                                     Vector3f(window.getSize().x / 2, window.getSize().y / 2, 0),
                                                                     Vector2f(64, 32),
                                                                     10,
                                                                     window.getSize().x, window.getSize().y)),
                                              worldConfig_(
                                                  4000000.f, 4000000.f,
                                                  10000, 10000,
                                                  40, 40,
                                                  *camera_),
                                              player_(new Player(rm)),
                                              cursor_(new Entity(rm)),
                                              ocean_(new Ocean(rm)),
                                              pathfinder_(
                                                  Vector3f(0, 0, 0),
                                                  worldConfig_),
                                              pathfinderGrid_(pathfinder_)
{
    // std::cout << "Creating World"
    //           << "\n";

    addEntity(player_);
    player_->move(20, 20, 0);

    addEntity(cursor_);
    cursor_->setSize(Vector3f(5, 5, 5));

    addEntity(ocean_);
    ocean_->setSize(
        worldConfig_.getCellWidth() * 3.f,
        worldConfig_.getCellHeight() * 3.f,
        0);

    Entity *grid = new PathfinderVisualizer(pathfinder_);
    addEntity(grid);

    TrackingCamera *camera = reinterpret_cast<TrackingCamera *>(camera_);
    camera->setTrackTarget(*player_, 1, 5, 60);

    // player_->setPosition(400000 / 2, 400000 / 2, 0);
    player_->setPosition(700, 200, 0);
    camera->setPosition(player_->getPosition());
}

World::~World()
{
    // std::cout << "Destroying World"
    //           << "\n";

    for (auto &entity : entities_)
    {
        delete entity;
    }

    for (auto &item : cellCache_)
    {
        delete item.second;
    }

    entities_.clear();
}

void World::input_(sf::Time &elapsed)
{
    // Cursor
    Vector2f mousePosition = window_->mapPixelToCoords(sf::Mouse::getPosition(*window_));
    cursor_->setPosition(
        camera_->projectGround(mousePosition));

    // Camera Pan
    float speed = elapsed.asSeconds() * 4.0 * 60;
    Vector2f panDir(0, 0);
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
    {
        panDir.x = speed;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
    {
        panDir.x = -speed;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
    {
        panDir.y = -speed;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
    {
        panDir.y = speed;
    }
    if (std::abs(panDir.x) > 0 || std::abs(panDir.y) > 0)
    {
        camera_->pan(panDir);
    }
}

void World::updateCells_()
{
    activeCells_.clear();
    visibleEntities_.clear();
    floorEntities_.clear();

    int min_i = worldConfig_.rows();
    int min_j = worldConfig_.cols();
    int max_i = 0;
    int max_j = 0;
    for (auto [i, j] : worldConfig_.getAdjacentIds(player_->getPosition(), 9))
    {
        WorldCell *currentCell;

        auto search = cellCache_.find(worldConfig_.getId(i, j));
        if (search == cellCache_.end())
        {
            currentCell = new WorldCell(
                *rm_,
                worldConfig_,
                i, j);
            cellCache_[worldConfig_.getId(i, j)] = currentCell;
        }
        else
        {
            currentCell = search->second;
        }

        activeCells_.push_back(currentCell);
        for (auto entity : currentCell->getEntities())
        {
            visibleEntities_.push_back(entity);
        }
        if (i < min_i)
            min_i = i;
        if (j < min_j)
            min_j = j;
        if (i > max_i)
            max_i = i;
        if (j > max_j)
            max_j = j;
    }

    pathfinder_.setPosition(
        worldConfig_.getCellPosition(min_i, min_j));

    pathfinder_.setActiveCells(min_i, min_j, activeCells_);

    ocean_->setPosition(pathfinder_.getPosition());

    for (auto &cell : activeCells_)
    {
        cell->translateOrigin(pathfinder_.getPosition());
        floorEntities_.push_back(cell->getFloor());
    }

    visibleEntities_.push_back(player_);
    player_->translateOrigin(pathfinder_.getPosition());

    // visibleEntities_.push_back(&pathfinderGrid_);
    visibleEntities_.push_back(cursor_);
}

void World::update(sf::Time &elapsed)
{
    ocean_->update(elapsed, *this);

    updateCells_();

    camera_->updateWindow(*window_);

    input_(elapsed);

    for (auto &entity : floorEntities_)
    {
        entity->update(elapsed, *this);
    }

    for (auto &entity : visibleEntities_)
    {
        entity->update(elapsed, *this);
    }

    camera_->update(elapsed);
}

void World::transform()
{
    ocean_->transform(*camera_);

    for (auto &entity : floorEntities_)
    {
        entity->transform(*camera_);
    }

    for (auto &entity : visibleEntities_)
    {
        entity->transform(*camera_);
    }
}

void World::draw(sf::RenderTarget *screen)
{
    ocean_->draw(screen);

    std::sort(floorEntities_.begin(), floorEntities_.end(), entityDepthComp);

    for (auto &entity : floorEntities_)
    {
        entity->draw(screen);
    }

    std::sort(visibleEntities_.begin(), visibleEntities_.end(), entityDepthComp);

    for (auto &entity : visibleEntities_)
    {
        entity->draw(screen);
    }
}

Entity *World::addEntity(Entity *entity)
{
    entities_.push_back(entity);
    return entity;
}

void World::onMouseButtonReleased(const sf::Event &event)
{
    // std::cout << "Mouse Release ";
    if (event.mouseButton.button == sf::Mouse::Left)
    {
        // std::cout << "Left"
        //           << "\n";
        player_->walkTo(cursor_->getPosition());
        std::cout << "e" << worldConfig_.getElevation(cursor_->getPosition()) << "\n";
    }
    else if (event.mouseButton.button == sf::Mouse::Right)
    {
        // std::cout << "Right"
        //           << "\n";
    }
}

void World::onMouseWheelScrolled(const sf::Event &event)
{
    if (event.mouseWheelScroll.delta < 0)
    {
        camera_->zoom(1.1);
    }
    else
    {
        camera_->zoom(0.9);
    }
}

const std::vector<Entity *> &World::getEntitys() const
{
    return entities_;
}

bool World::findPath(const Entity &entity, const Vector3f &end,
                     const bool &diagonal,
                     std::deque<Vector3f> &resultPath)
{
    // if (!canMoveTo(entity, entity.getLocalPosition()))
    // {
    //     std::cout << "Entity is at an invalid position...";
    //     Vector3f validPos;
    //     if (pathfinder_.findFreePosition(entity.getPosition(), validPos))
    //     {
    //         std::cout << "Consider move to " << validPos;
    //     }
    //     std::cout << "\n";
    //     return false;
    // }
    return pathfinder_.findPath(entity.getPosition(), end, diagonal, resultPath);
}

bool World::canMoveTo(const Entity &entity, const Vector3f &localPoint) const
{
    return pathfinder_.isAreaFree(localPoint, entity.getSize() * 0.8f);
}