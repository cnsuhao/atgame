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

#include "perfMonitor.h"

PerfMonitor::PerfMonitor()
    : current_FPS_(0), tv_last_sec_(0), last_tick_(0), tickindex_(0),
      ticksum_(0) 
{
  for (int i = 0; i < NUM_SAMPLES; ++i)
        ticklist_[i] = 0;
}

PerfMonitor::~PerfMonitor() {}

uint64 PerfMonitor::UpdateTick(uint64 currentTick) 
{
    ticksum_ -= ticklist_[tickindex_];
    ticksum_ += currentTick;
    ticklist_[tickindex_] = currentTick;
    tickindex_ = (tickindex_ + 1) % NUM_SAMPLES;

    return ((uint64) ticksum_ / NUM_SAMPLES);
}

bool PerfMonitor::Update(float &fFPS) 
{
    uint64 time = GetAbsoluteMsecTime();
    uint64 tick = time - last_tick_;
    uint64 d = UpdateTick(tick);
    last_tick_ = time;

    if (time - tv_last_sec_ >= 1000) {
        current_FPS_ = 1000.0f / d;
        tv_last_sec_ = time;
        fFPS = current_FPS_;
        return true;
    } else {
        fFPS = current_FPS_;
        return false;
    }
}
