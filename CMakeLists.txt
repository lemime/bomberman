cmake_minimum_required(VERSION 3.13)
project(bomberman)

set(CMAKE_CXX_STANDARD 17)

SET(CMAKE_CXX_FLAGS -pthread)

find_package(Curses REQUIRED)
include_directories(${CURSES_INCLUDE_DIR})

add_executable(client
        src/client/client.cpp
        src/client/client.h
        src/game_logic/Board.cpp src/game_logic/Board.h
        src/game_logic/BoardTileFragment.cpp src/game_logic/BoardTileFragment.h
        src/game_logic/Crate.cpp src/game_logic/Crate.h
        src/game_logic/Wall.cpp src/game_logic/Wall.h
        src/game_logic/Floor.cpp src/game_logic/Floor.h
        src/game_logic/Slot.cpp src/game_logic/Slot.h
        src/game_logic/Explosion.cpp src/game_logic/Explosion.h
        src/game_logic/Bomb.cpp src/game_logic/Bomb.h
        src/game_logic/GamePlayer.cpp src/game_logic/GamePlayer.h
        src/game_logic/Powerup.cpp src/game_logic/Powerup.h
        src/game_logic/BoardTile.cpp src/game_logic/BoardTile.h
        src/game_logic/CursesHelper.cpp src/game_logic/CursesHelper.h
        src/game_logic/WindowHelper.cpp src/game_logic/WindowHelper.h
        src/game_logic/GameRoom.cpp src/game_logic/GameRoom.h
        src/game_logic/Triggerable.cpp src/game_logic/Triggerable.h
        src/network_logic/NetworkHelper.cpp src/network_logic/NetworkHelper.h
        src/network_logic/PollHelper.cpp src/network_logic/PollHelper.h
        src/network_logic/ClientHandler.cpp src/network_logic/ClientHandler.h
        src/network_logic/NetworkPlayer.cpp src/network_logic/NetworkPlayer.h
        src/utility/Logger.h src/utility/Logger.cpp
        src/utility/FileLogger.cpp src/utility/FileLogger.h
        src/utility/SplitMessage.cpp src/utility/SplitMessage.h)
add_executable(server
        src/server/server.cpp
        src/server/server.h
        src/game_logic/Board.cpp src/game_logic/Board.h
        src/game_logic/BoardTileFragment.cpp src/game_logic/BoardTileFragment.h
        src/game_logic/Crate.cpp src/game_logic/Crate.h
        src/game_logic/Wall.cpp src/game_logic/Wall.h
        src/game_logic/Floor.cpp src/game_logic/Floor.h
        src/game_logic/Slot.cpp src/game_logic/Slot.h
        src/game_logic/Explosion.cpp src/game_logic/Explosion.h
        src/game_logic/Bomb.cpp src/game_logic/Bomb.h
        src/game_logic/GamePlayer.cpp src/game_logic/GamePlayer.h
        src/game_logic/Powerup.cpp src/game_logic/Powerup.h
        src/game_logic/BoardTile.cpp src/game_logic/BoardTile.h
        src/game_logic/CursesHelper.cpp src/game_logic/CursesHelper.h
        src/game_logic/WindowHelper.cpp src/game_logic/WindowHelper.h
        src/game_logic/GameRoom.cpp src/game_logic/GameRoom.h
        src/game_logic/Triggerable.cpp src/game_logic/Triggerable.h
        src/network_logic/NetworkHelper.cpp src/network_logic/NetworkHelper.h
        src/network_logic/PollHelper.cpp src/network_logic/PollHelper.h
        src/network_logic/RoomAssigner.cpp src/network_logic/RoomAssigner.h
        src/network_logic/RoomHandler.cpp src/network_logic/RoomHandler.h
        src/network_logic/ClientHandler.cpp src/network_logic/ClientHandler.h
        src/network_logic/NetworkRoom.cpp src/network_logic/NetworkRoom.h
        src/network_logic/NetworkPlayer.cpp src/network_logic/NetworkPlayer.h
        src/utility/Logger.h src/utility/Logger.cpp
        src/utility/FileLogger.cpp src/utility/FileLogger.h
        src/utility/SplitMessage.cpp src/utility/SplitMessage.h)

target_link_libraries(client ${CURSES_LIBRARIES})
target_link_libraries(server ${CURSES_LIBRARIES})
