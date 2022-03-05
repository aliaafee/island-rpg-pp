#ifndef __ACTOR_H__
#define __ACTOR_H__

#include <iostream>
#include <vector>
#include <SFML/Graphics.hpp>

#include "camera.h"

class World;

class Actor
{
public:
    Actor();
    ~Actor();

    virtual void update();
    virtual void update(std::vector<Actor *> &other_actors);
    virtual void update(World &world);

    virtual void transform(Camera &camera);
    virtual void draw(sf::RenderTarget *screen);

    const sf::Vector3f &getPosition() const;
    void setPosition(const sf::Vector3f &position);
    void incPosition(const sf::Vector3f &velocity);
private:
    sf::Vector3f _position;
    sf::Vector3f _size;

    sf::CircleShape *_circle;
};

bool actorDepthComp(Actor* a, Actor* b);

#endif // __ACTOR_H__