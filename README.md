# EchoRam

EchoRam is a lightweight matchmaking service for connecting players together, without complex setup, database, or game-specific behaviour - just build and run. It's perfect for indie games with cooperative multiplayer.

**Not ready for any kind of use yet**.

Features

 * RAM-only database for string attributes (level name, player name, player skill, etc)
 * Attribute search (get player named foobar, etc)
 * Simple update & query system
 * Plain TCP sockets
 
TODO : 

 * Better search system
 * Matchmaking
 * Garbage collection of clients
 * SSL sockets, IPV6 support ?

## How to build

Dependencies

 * OpenSSH
 * CMake

Windows build

 * Open a terminal in the root folder
 * 'Run Generate.bat'
 * Open the 'EchoRam.sln' solution in build/Win64
 * Switch build to 'Release'
 * Start the build, the output will be named 'Server.exe' in the Release folder
 
Linux build

 * Open a terminal in the root folder
 * Run 'Generate.sh'
 * Run 'cd build/Linux''
 * Run 'make'
 * The output will be named 'Server'
