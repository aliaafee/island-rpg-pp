#include "World.hpp"

World::World(sf::RenderWindow &window, ResourceManager &rm,
             int64_t width, int64_t height) : window_(&window),
                                              rm_(&rm),
                                              player_(new Player(rm)),
                                              cursor_(new Actor(rm)),
                                              pathfinder_(
                                                  Vector3f(0, 0, 0),
                                                  600, 600,
                                                  60, 60
                                              )
{
    std::cout << "Creating World"
              << "\n";

    pathfinder_.clearGrid();

    camera_ = new Camera(
        Vector3f(0, 0, 0),
        Vector3f(800 / 2, 600 / 2, 0),
        Vector2f(64, 32),
        10);

    addActor(player_);
    addActor(cursor_);

    player_->move(20, 20, 0);

    // addActor(new Player(rm_));

    // addActor(new Player(rm_));

    // actors_[0]->setPosition(Vector3f(0, 20, 0));
    // // actors_[1]->setPosition(Vector3f(0, 30, 10));

    // Actor *a;
    // for (float i; i < 10; i++)
    // {
    //     a = new Player(rm_);
    //     a->setPosition(Vector3f(20 + i * 6, 0, 0));
    //     addActor(a);
    // }
}

World::~World()
{
    std::cout << "Destroying World"
              << "\n";

    delete camera_;

    for (auto &actor : actors_)
    {
        delete actor;
    }
    actors_.clear();
}

void World::input_(sf::Time &elapsed)
{
    // Cursor
    Vector2i mousePosition = sf::Mouse::getPosition(*window_);
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
    if (abs(panDir.x) > 0 || abs(panDir.y) > 0)
    {
        camera_->pan(panDir);
    }
}

void World::update(sf::Time &elapsed)
{
    input_(elapsed);

    for (auto &actor : actors_)
    {
        actor->update(elapsed, *this);
    }
}

void World::transform()
{
    for (auto &actor : actors_)
    {
        actor->transform(*camera_);
    }
}

void World::draw(sf::RenderTarget *screen)
{
    std::sort(actors_.begin(), actors_.end(), actorDepthComp);

    for (auto &actor : actors_)
    {
        actor->draw(screen);
    }
}

void World::addActor(Actor *actor)
{
    actors_.push_back(actor);
}

void World::onMouseButtonReleased(const sf::Event &event)
{
    std::cout << "Mouse Release ";
    if (event.mouseButton.button == sf::Mouse::Left)
    {
        std::cout << "Left"
                  << "\n";
        // std::deque<Vector3f> path = {
        //     Vector3f(0, 0, 0),
        //     Vector3f(0, 50, 0),
        //     Vector3f(50, 50, 0),
        //     Vector3f(50, 0, 0)
        // };
        // player_->walkPath(path);
        player_->walkTo(cursor_->getPosition());
    }
    else if (event.mouseButton.button == sf::Mouse::Right)
    {
        std::cout << "Right"
                  << "\n";
        player_->stop();
    }
}

const std::vector<Actor *> &World::getActors() const
{
    return actors_;
}