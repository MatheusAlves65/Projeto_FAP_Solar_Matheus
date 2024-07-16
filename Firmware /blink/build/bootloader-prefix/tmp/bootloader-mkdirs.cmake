# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/Users/matheus/esp/esp-idf/components/bootloader/subproject"
  "/Users/matheus/Documents/FAP_solar/blink/build/bootloader"
  "/Users/matheus/Documents/FAP_solar/blink/build/bootloader-prefix"
  "/Users/matheus/Documents/FAP_solar/blink/build/bootloader-prefix/tmp"
  "/Users/matheus/Documents/FAP_solar/blink/build/bootloader-prefix/src/bootloader-stamp"
  "/Users/matheus/Documents/FAP_solar/blink/build/bootloader-prefix/src"
  "/Users/matheus/Documents/FAP_solar/blink/build/bootloader-prefix/src/bootloader-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/Users/matheus/Documents/FAP_solar/blink/build/bootloader-prefix/src/bootloader-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/Users/matheus/Documents/FAP_solar/blink/build/bootloader-prefix/src/bootloader-stamp${cfgdir}") # cfgdir has leading slash
endif()
