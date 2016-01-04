/*
 * Copyright 2013 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include "atgMisc.h"

/******************************************************************
 * Helper class for a performance monitoring and get current tick time
 */
class PerfMonitor 
{
private:
    enum {
        NUM_SAMPLES = 100,
    };

    float current_FPS_;
    uint64 tv_last_sec_;

    uint64 last_tick_;
    int tickindex_;
    uint64 ticksum_;
    uint64 ticklist_[NUM_SAMPLES];

    uint64 UpdateTick(uint64 current_tick);

public:

  PerfMonitor();
  virtual ~PerfMonitor();

  bool Update(float &fFPS);
};

typedef PerfMonitor atgPerfMonitor;
