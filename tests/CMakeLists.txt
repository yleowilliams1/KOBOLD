# Unity as its own target so -Werror/sanitizers don't apply to it
add_library(unity STATIC ${PROJECT_SOURCE_DIR}/vendor/unity.c)

add_executable(test_runner
  test_main.c
  test_game.c
)
target_link_libraries(test_runner PRIVATE game_core unity kobold_options)

# One ctest entry per suite
foreach(suite IN ITEMS game)
  add_test(NAME ${suite} COMMAND test_runner ${suite})
endforeach()
