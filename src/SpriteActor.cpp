#include "SpriteActor.hpp"

SpriteActor::SpriteActor(ResourceManager *rm) : Actor(rm)
{
    // sf::Texture* tex = rm->loadTexture("graphics/test/player.png");

    // sprite_.setTexture(*tex);
    sprite_.setOrigin(32, 64);
}

SpriteActor::~SpriteActor()
{
}

void SpriteActor::transform(Camera &camera)
{
    Actor::transform(camera);
    sprite_.setPosition(getScreenPosition().x, getScreenPosition().y);
}

void SpriteActor::draw(sf::RenderTarget *screen)
{
    Actor::draw(screen);
    screen->draw(sprite_);
}

void SpriteActor::setTexture(sf::Texture *texture)
{
    sprite_.setTexture(*texture);
}
