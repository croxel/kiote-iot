set(KIOTE_IOT_DIR ${CMAKE_CURRENT_LIST_DIR} CACHE PATH "Kiote IoT root directory")

zephyr_include_directories(include)

add_subdirectory(drivers)
