cmake_minimum_required(VERSION 3.13)
project(bomberman)

set(CMAKE_CXX_STANDARD 17)

find_package(Curses REQUIRED)
include_directories(${CURSES_INCLUDE_DIR})

add_executable(client src/client/client.cpp src/shared/Board.cpp src/shared/Board.h src/shared/BoardTileFragment.cpp src/shared/BoardTileFragment.h src/shared/Crate.cpp src/shared/Crate.h src/shared/Wall.cpp src/shared/Wall.h src/shared/Floor.cpp src/shared/Floor.h src/shared/Slot.cpp src/shared/Slot.h src/shared/Explosion.cpp src/shared/Explosion.h src/shared/Bomb.cpp src/shared/Bomb.h src/shared/Player.cpp src/shared/Player.h src/shared/Powerup.cpp src/shared/Powerup.h src/shared/BoardTile.cpp src/shared/BoardTile.h src/shared/CursesHelper.cpp src/shared/CursesHelper.h src/shared/WindowHelper.cpp src/shared/WindowHelper.h src/shared/Room.cpp src/shared/Room.h src/shared/Game.cpp src/shared/Game.h)
add_executable(server src/server/server.cpp src/shared/Board.cpp src/shared/Board.h src/shared/BoardTileFragment.cpp src/shared/BoardTileFragment.h src/shared/Crate.cpp src/shared/Crate.h src/shared/Wall.cpp src/shared/Wall.h src/shared/Floor.cpp src/shared/Floor.h src/shared/Slot.cpp src/shared/Slot.h src/shared/Explosion.cpp src/shared/Explosion.h src/shared/Bomb.cpp src/shared/Bomb.h src/shared/Player.cpp src/shared/Player.h src/shared/Powerup.cpp src/shared/Powerup.h src/shared/BoardTile.cpp src/shared/BoardTile.h src/shared/CursesHelper.cpp src/shared/CursesHelper.h src/shared/WindowHelper.cpp src/shared/WindowHelper.h src/shared/Room.cpp src/shared/Room.h src/shared/Game.cpp src/shared/Game.h)

target_link_libraries(client ${CURSES_LIBRARIES})
target_link_libraries(server ${CURSES_LIBRARIES})
