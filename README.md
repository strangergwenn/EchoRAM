# EchoRAM

EchoRam is a lightweight matchmaking service for connecting players together, without complex setup, database, or game-specific behaviour - just build and run. It's perfect for indie games with cooperative multiplayer.

**Not ready for any kind of use yet**.

Features

 * RAM-only database for string attributes (level name, player name, player skill, etc)
 * Attribute search (get player named foobar, etc)
 * Simple update & query system
 * Can use plaintext TCP or SSL
 
TODO : 

 * Matchmaking API & algorithm
 * UTF8 support
 * IPV6 support

## How to build

Dependencies

 * OpenSSH
 * CMake

Windows build

 * Open a terminal in the root folder
 * 'Run Generate.bat'
 * Open the 'EchoRam.sln' solution in build/Win64
 * Switch build to 'Release'
 * Start the build, the output will be named 'EchoRAM.exe' in the Release folder
 
Linux build

 * Open a terminal in the root folder
 * Run 'Generate.sh'
 * Run 'cd build/Linux''
 * Run 'make'
 * The output will be named 'EchoRAM'

## Command-line parameters

The EchoRAM executable features the following command-line options.

 * --port <n> : Listening on port n
 * --clients <n> : Accepting n clients
 * --update-period <n> : Updating database every n seconds
 * --client-idle-time <n> : Clients will be autoremoved every n seconds without update or heartbeat
 * --use-ssl <n> : Use SSL for encryption (0 or 1)
 * --public-cert <f> : Public SSL certificate file
 * --private-key <f> : Private SSL key file
