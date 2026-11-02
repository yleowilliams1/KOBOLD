#include "unity.h"
#include <stdio.h>
#include <string.h>

void setUp(void) {}
void tearDown(void) {}

/* Each suite lives in its own file and exposes one function */
void suite_game(void);
/* void suite_physics(void); */

typedef struct { const char *name; void (*run)(void); } Suite;

static const Suite suites[] = {
    {"game", suite_game},
    /* {"physics", suite_physics}, */
};
#define NUM_SUITES (sizeof suites / sizeof suites[0])

static const Suite *find_suite(const char *name) {
    for (size_t i = 0; i < NUM_SUITES; i++)
        if (strcmp(suites[i].name, name) == 0) return &suites[i];
    return NULL;
}

int main(int argc, char **argv) {
    if (argc > 1 && strcmp(argv[1], "--list") == 0) {
        for (size_t i = 0; i < NUM_SUITES; i++) puts(suites[i].name);
        return 0;
    }

    /* Validate names before running anything */
    for (int i = 1; i < argc; i++) {
        if (!find_suite(argv[i])) {
            fprintf(stderr, "unknown suite '%s' (try --list)\n", argv[i]);
            return 2;
        }
    }

    UNITY_BEGIN();
    if (argc == 1) {
        for (size_t i = 0; i < NUM_SUITES; i++) suites[i].run();
    } else {
        for (int i = 1; i < argc; i++) find_suite(argv[i])->run();
    }
    return UNITY_END();
}
