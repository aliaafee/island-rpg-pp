# Island Adventure

Explore an archipelago of procedurally generated Islands. Point and click to move.

![Demo Animation](screenshots/05.png?raw=true)

## Required Dependencies

smfl https://www.sfml-dev.org/
Install in the standard manner for your system.

## Build

    mkdir build
    cd build
    cmake ..
    make

To enable debug symbols use:

    cmake -DCMAKE_BUILD_TYPE=Debug ..
    
Only tested on Archlinux.

## Run

    island-rpg ../resources/

## Run Tests

    cd build
    cmake ..
    make all_tests test

## TODO

* More interesting terrain, and make the islands more island like. Variation in color of ocean based on depth.
* Add fauna. Marine, land and air.
* Add structures, buildings etc.
* Traverese water, with a boat, maybe?
* Add tasks, interactions, quests, maybe a story.
    
    
## Artwork

Sources of Artwork

Character

https://pixel-boy.itch.io/ninja-adventure-asset-pack

Vegetation

Bleed
https://opengameart.org/content/tree-collection-v26-bleeds-game-art
http://remusprites.carbonmade.com/

Tiles

Daniel Stephens (Scribe)
https://opengameart.org/content/unknown-horizons-tileset
https://unknown-horizons.org/
