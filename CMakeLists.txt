# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.20.0)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(lorawan)

FILE(GLOB app_sources src/*.c)
target_sources(app PRIVATE ${app_sources})


# Add include directory for board specific CAF def files
zephyr_include_directories(
  configuration/${BOARD}
  configuration/default
  src/modules
  )

target_sources(app PRIVATE src/modules/module_lorawan.c)