if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CARGO_CMD cargo build)
    set(TARGET_DIR "debug")
else ()
    set(CARGO_CMD cargo build --release)
    set(TARGET_DIR "release")
endif ()

set(SCRAPER_BINARY "${CMAKE_CURRENT_BINARY_DIR}/${TARGET_DIR}/hockeyscraper")

message("Current CMAKE_CURRENT_BINARY_DIR in hockeyscraper CMakeLists: ${CMAKE_CURRENT_BINARY_DIR}.\n Current source dir ${CMAKE_CURRENT_SOURCE_DIR}.\n Project source dir. ${PROJECT_SOURCE_DIR}")

add_custom_target(hockeyscraper ALL
        COMMENT "Compiling scraping application 'hockeyscraper'"
        COMMAND CARGO_TARGET_DIR=${CMAKE_CURRENT_BINARY_DIR} ${CARGO_CMD}
        COMMAND cp ${SCRAPER_BINARY} ${PROJECT_SOURCE_DIR}/bin
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

set_target_properties(hockeyscraper PROPERTIES LOCATION ${CMAKE_CURRENT_BINARY_DIR})

add_test(NAME scrape_test COMMAND cargo test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})