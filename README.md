# Vision

We are going to write an engine for creating 3D games, which will help people create compelling experiences. 

When creating games, developers face a lot of problems. Including graphic visualization, animations, sound, moving objects and characters, their actions in accordance with scripts, as well as built-in graphic scenes, compliance with physical effects and laws, and much more.

The solutions to such problems are always about the same and quite difficult for high-quality implementation, so they create game engines that save developers with a layer of abstraction.

Our engine will include:

- A library for writing convenient and understandable gameplay scripts
- Facilities to quickly bootstrap your game enabling fast prototyping
- Built-in profiling and logging to help you find bottlenecks and fix logic errors offline

To demonstrate its abilities we are going to create a game. First person view, explore the immersive world around you.

Player is a magician who travels between multiple locations(something like buildings, temples, towers, ruins, castles, dungeons).

In each location there are monsters which can be killed and puzzles which can be solved using spells. After completing everything inside one location, the player gets a new spell or some upgrade (more damage or modification for spells he already knew) and gets teleported to another location.
