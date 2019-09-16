# Project-Iron-Throne-Server
Well, I finally decided to make this repo public :)

## How to Compile
```bash
cd libs
git clone https://github.com/google/flatbuffers.git
cd ../build
cmake ..
make && make install
```

## Project Architecture
There are 3 main types of modules that make up the modular architecture of this project
- Brain - LightBringer
  - This is not named explicitly but is important to maintain ease in the creation of the other modules.  Acts as a highway or queue system between modules in addition to implementing logic into the server
  - Future development includes a Cli to interact with the different modules and view debug information
- Master Modules
  - Used to connect to an upstream server that schedules and receives bot actions
  - There is currently a module to connect to the Arsenal Teamserver
- Puppet Modules
  - Used to handle connections from bots

## Code Names -> Description :)
There are a few people who wanted me document my code names for this project so here we go.

| Code Name | Description |
| --------- | ----------- |
| Project Iron Throne | Overarching code name for the modular architecture implemented in the server and the bot.  This is why the repo is named 'Project Iron Throne *Server*' since there is also a bot associated with it.  |
| Arsenal | [Arsenal](https://github.com/kcarretto/arsenal) is a project that was created to manage beacons during red team engagements.  I created a module to connect this C2 with the Arsenal Teamserver to handle the management of beacons. |
| LightBringer | Brain of the C2.  Currently, this portion of the C2 just acts like glue between the Puppet and Master modules.  LightBringer wraps a queue containing pointers to APIT objects which are used for the internal communication mechanism. |
| BlackIce | The first Puppet module written for Project Iron Throne's Server.  This module handles a specific protocol which bots use to beacon out to the C2. |
