Gorc
====

Description
-----------

Early-development Dark Forces II game engine recreation.

Instructions
------------

* Create symbolic links to the `resource` and `episode` directories inside the `game` subdirectory.
* Optional: Rip the CD audio to the `game/restricted/music/1` and `game/restricted/music/2` subdirectories in OGG format.
* Run `make` from the project root.

Requirements
------------

* Standard C++11 compiler (Clang 3.3, GCC 4.8)
* GNU Make
* Flex 2.5
* Bison 2.5
* SFML 1.6
* Glew
* [Boost](http://boost.org)
* [NullUnit](http://github.com/jdmclark/nullunit)
* Dark Forces II (for running test cases)

