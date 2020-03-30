#pragma once

#include <stdint.h>
#include <sys/time.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>

static uint64_t GetNumRounds() {
    static const auto value = []() {
        auto env_key = "NUM_ROUNDS";
        auto env_var = getenv(env_key);
        size_t env_value;
        auto value = !!env_var && 1 == sscanf(env_var, "%lu", &env_value)
            ? env_value
            : 100;
        printf("Using %s=%lu\n", env_key, value);
        return value;
    }();
    return value;
}

static uint64_t GetFrequencyHZ() {
    static const auto value = []() {
        auto env_key = "FREQUENCY_HZ";
        auto env_var = getenv(env_key);
        size_t env_value;
        auto value = !!env_var && 1 == sscanf(env_var, "%lu", &env_value)
            ? env_value
            : 10;
        printf("Using %s=%lu\n", env_key, value);
        return value;
    }();
    return value;
}

static double get_wall_time() {
  struct timeval time;
  if (gettimeofday(&time, NULL)) {
    perror("gettimeofday error!");
    return 0;
  }
  return (double)time.tv_sec + (double)time.tv_usec * .000001;
}
