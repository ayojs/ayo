// Copyright 2012 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "src/counters.h"

#include <iomanip>

#include "src/base/platform/platform.h"
#include "src/builtins/builtins-definitions.h"
#include "src/isolate.h"
#include "src/log-inl.h"
#include "src/log.h"

namespace v8 {
namespace internal {

StatsTable::StatsTable(Counters* counters)
    : lookup_function_(NULL),
      create_histogram_function_(NULL),
      add_histogram_sample_function_(NULL) {}

void StatsTable::SetCounterFunction(CounterLookupCallback f) {
  lookup_function_ = f;
}

int* StatsCounterBase::FindLocationInStatsTable() const {
  return counters_->FindLocation(name_);
}

StatsCounterThreadSafe::StatsCounterThreadSafe(Counters* counters,
                                               const char* name)
    : StatsCounterBase(counters, name) {}

void StatsCounterThreadSafe::Set(int Value) {
  if (ptr_) {
    base::LockGuard<base::Mutex> Guard(&mutex_);
    SetLoc(ptr_, Value);
  }
}

void StatsCounterThreadSafe::Increment() {
  if (ptr_) {
    base::LockGuard<base::Mutex> Guard(&mutex_);
    IncrementLoc(ptr_);
  }
}

void StatsCounterThreadSafe::Increment(int value) {
  if (ptr_) {
    base::LockGuard<base::Mutex> Guard(&mutex_);
    IncrementLoc(ptr_, value);
  }
}

void StatsCounterThreadSafe::Decrement() {
  if (ptr_) {
    base::LockGuard<base::Mutex> Guard(&mutex_);
    DecrementLoc(ptr_);
  }
}

void StatsCounterThreadSafe::Decrement(int value) {
  if (ptr_) {
    base::LockGuard<base::Mutex> Guard(&mutex_);
    DecrementLoc(ptr_, value);
  }
}

void Histogram::AddSample(int sample) {
  if (Enabled()) {
    counters_->AddHistogramSample(histogram_, sample);
  }
}

void* Histogram::CreateHistogram() const {
  return counters_->CreateHistogram(name_, min_, max_, num_buckets_);
}

void TimedHistogram::Start(base::ElapsedTimer* timer, Isolate* isolate) {
  if (Enabled()) timer->Start();
  if (isolate) Logger::CallEventLogger(isolate, name(), Logger::START, true);
}

void TimedHistogram::Stop(base::ElapsedTimer* timer, Isolate* isolate) {
  if (Enabled()) {
    // Compute the delta between start and stop, in microseconds.
    int64_t sample = resolution_ == HistogramTimerResolution::MICROSECOND
                         ? timer->Elapsed().InMicroseconds()
                         : timer->Elapsed().InMilliseconds();
    timer->Stop();
    AddSample(static_cast<int>(sample));
  }
  if (isolate != nullptr) {
    Logger::CallEventLogger(isolate, name(), Logger::END, true);
  }
}

Counters::Counters(Isolate* isolate)
    : isolate_(isolate),
      stats_table_(this),
// clang format off
#define SC(name, caption) name##_(this, "c:" #caption),
      STATS_COUNTER_TS_LIST(SC)
#undef SC
      // clang format on
      runtime_call_stats_() {
  static const struct {
    Histogram Counters::*member;
    const char* caption;
    int min;
    int max;
    int num_buckets;
  } kHistograms[] = {
#define HR(name, caption, min, max, num_buckets) \
  {&Counters::name##_, #caption, min, max, num_buckets},
      HISTOGRAM_RANGE_LIST(HR)
#undef HR
  };
  for (const auto& histogram : kHistograms) {
    this->*histogram.member =
        Histogram(histogram.caption, histogram.min, histogram.max,
                  histogram.num_buckets, this);
  }

  const int DefaultTimedHistogramNumBuckets = 50;

  static const struct {
    HistogramTimer Counters::*member;
    const char* caption;
    int max;
    HistogramTimerResolution res;
  } kHistogramTimers[] = {
#define HT(name, caption, max, res) \
  {&Counters::name##_, #caption, max, HistogramTimerResolution::res},
      HISTOGRAM_TIMER_LIST(HT)
#undef HT
  };
  for (const auto& timer : kHistogramTimers) {
    this->*timer.member = HistogramTimer(timer.caption, 0, timer.max, timer.res,
                                         DefaultTimedHistogramNumBuckets, this);
  }

  static const struct {
    TimedHistogram Counters::*member;
    const char* caption;
    int max;
    HistogramTimerResolution res;
  } kTimedHistograms[] = {
#define HT(name, caption, max, res) \
  {&Counters::name##_, #caption, max, HistogramTimerResolution::res},
      TIMED_HISTOGRAM_LIST(HT)
#undef HT
  };
  for (const auto& timer : kTimedHistograms) {
    this->*timer.member = TimedHistogram(timer.caption, 0, timer.max, timer.res,
                                         DefaultTimedHistogramNumBuckets, this);
  }

  static const struct {
    AggregatableHistogramTimer Counters::*member;
    const char* caption;
  } kAggregatableHistogramTimers[] = {
#define AHT(name, caption) {&Counters::name##_, #caption},
      AGGREGATABLE_HISTOGRAM_TIMER_LIST(AHT)
#undef AHT
  };
  for (const auto& aht : kAggregatableHistogramTimers) {
    this->*aht.member = AggregatableHistogramTimer(
        aht.caption, 0, 10000000, DefaultTimedHistogramNumBuckets, this);
  }

  static const struct {
    Histogram Counters::*member;
    const char* caption;
  } kHistogramPercentages[] = {
#define HP(name, caption) {&Counters::name##_, #caption},
      HISTOGRAM_PERCENTAGE_LIST(HP)
#undef HP
  };
  for (const auto& percentage : kHistogramPercentages) {
    this->*percentage.member = Histogram(percentage.caption, 0, 101, 100, this);
  }

  // Exponential histogram assigns bucket limits to points
  // p[1], p[2], ... p[n] such that p[i+1] / p[i] = constant.
  // The constant factor is equal to the n-th root of (high / low),
  // where the n is the number of buckets, the low is the lower limit,
  // the high is the upper limit.
  // For n = 50, low = 1000, high = 500000: the factor = 1.13.
  static const struct {
    Histogram Counters::*member;
    const char* caption;
  } kLegacyMemoryHistograms[] = {
#define HM(name, caption) {&Counters::name##_, #caption},
      HISTOGRAM_LEGACY_MEMORY_LIST(HM)
#undef HM
  };
  for (const auto& histogram : kLegacyMemoryHistograms) {
    this->*histogram.member =
        Histogram(histogram.caption, 1000, 500000, 50, this);
  }

  // For n = 100, low = 4000, high = 2000000: the factor = 1.06.
  static const struct {
    Histogram Counters::*member;
    AggregatedMemoryHistogram<Histogram> Counters::*aggregated;
    const char* caption;
  } kMemoryHistograms[] = {
#define HM(name, caption) \
  {&Counters::name##_, &Counters::aggregated_##name##_, #caption},
      HISTOGRAM_MEMORY_LIST(HM)
#undef HM
  };
  for (const auto& histogram : kMemoryHistograms) {
    this->*histogram.member =
        Histogram(histogram.caption, 4000, 2000000, 100, this);
    this->*histogram.aggregated =
        AggregatedMemoryHistogram<Histogram>(&(this->*histogram.member));
  }

  // clang-format off
  static const struct {
    StatsCounter Counters::*member;
    const char* caption;
  } kStatsCounters[] = {
#define SC(name, caption) {&Counters::name##_, "c:" #caption},
      STATS_COUNTER_LIST_1(SC) STATS_COUNTER_LIST_2(SC)
#undef SC
#define SC(name)                                             \
  {&Counters::count_of_##name##_, "c:" "V8.CountOf_" #name}, \
  {&Counters::size_of_##name##_, "c:" "V8.SizeOf_" #name},
      INSTANCE_TYPE_LIST(SC)
#undef SC
#define SC(name)                            \
  {&Counters::count_of_CODE_TYPE_##name##_, \
    "c:" "V8.CountOf_CODE_TYPE-" #name},     \
  {&Counters::size_of_CODE_TYPE_##name##_,  \
    "c:" "V8.SizeOf_CODE_TYPE-" #name},
      CODE_KIND_LIST(SC)
#undef SC
#define SC(name)                              \
  {&Counters::count_of_FIXED_ARRAY_##name##_, \
    "c:" "V8.CountOf_FIXED_ARRAY-" #name},     \
  {&Counters::size_of_FIXED_ARRAY_##name##_,  \
    "c:" "V8.SizeOf_FIXED_ARRAY-" #name},
      FIXED_ARRAY_SUB_INSTANCE_TYPE_LIST(SC)
#undef SC
  };
  // clang-format on
  for (const auto& counter : kStatsCounters) {
    this->*counter.member = StatsCounter(this, counter.caption);
  }
}

void Counters::ResetCounterFunction(CounterLookupCallback f) {
  stats_table_.SetCounterFunction(f);

#define SC(name, caption) name##_.Reset();
  STATS_COUNTER_LIST_1(SC)
  STATS_COUNTER_LIST_2(SC)
#undef SC

#define SC(name, caption) name##_.Reset();
  STATS_COUNTER_TS_LIST(SC)
#undef SC

#define SC(name)              \
  count_of_##name##_.Reset(); \
  size_of_##name##_.Reset();
  INSTANCE_TYPE_LIST(SC)
#undef SC

#define SC(name)                        \
  count_of_CODE_TYPE_##name##_.Reset(); \
  size_of_CODE_TYPE_##name##_.Reset();
  CODE_KIND_LIST(SC)
#undef SC

#define SC(name)                          \
  count_of_FIXED_ARRAY_##name##_.Reset(); \
  size_of_FIXED_ARRAY_##name##_.Reset();
  FIXED_ARRAY_SUB_INSTANCE_TYPE_LIST(SC)
#undef SC
}

void Counters::ResetCreateHistogramFunction(CreateHistogramCallback f) {
  stats_table_.SetCreateHistogramFunction(f);

#define HR(name, caption, min, max, num_buckets) name##_.Reset();
  HISTOGRAM_RANGE_LIST(HR)
#undef HR

#define HT(name, caption, max, res) name##_.Reset();
    HISTOGRAM_TIMER_LIST(HT)
#undef HT

#define HT(name, caption, max, res) name##_.Reset();
    TIMED_HISTOGRAM_LIST(HT)
#undef HT

#define AHT(name, caption) name##_.Reset();
    AGGREGATABLE_HISTOGRAM_TIMER_LIST(AHT)
#undef AHT

#define HP(name, caption) name##_.Reset();
    HISTOGRAM_PERCENTAGE_LIST(HP)
#undef HP

#define HM(name, caption) name##_.Reset();
    HISTOGRAM_LEGACY_MEMORY_LIST(HM)
    HISTOGRAM_MEMORY_LIST(HM)
#undef HM
}

class RuntimeCallStatEntries {
 public:
  void Print(std::ostream& os) {
    if (total_call_count == 0) return;
    std::sort(entries.rbegin(), entries.rend());
    os << std::setw(50) << "Runtime Function/C++ Builtin" << std::setw(12)
       << "Time" << std::setw(18) << "Count" << std::endl
       << std::string(88, '=') << std::endl;
    for (Entry& entry : entries) {
      entry.SetTotal(total_time, total_call_count);
      entry.Print(os);
    }
    os << std::string(88, '-') << std::endl;
    Entry("Total", total_time, total_call_count).Print(os);
  }

  // By default, the compiler will usually inline this, which results in a large
  // binary size increase: std::vector::push_back expands to a large amount of
  // instructions, and this function is invoked repeatedly by macros.
  V8_NOINLINE void Add(RuntimeCallCounter* counter) {
    if (counter->count() == 0) return;
    entries.push_back(
        Entry(counter->name(), counter->time(), counter->count()));
    total_time += counter->time();
    total_call_count += counter->count();
  }

 private:
  class Entry {
   public:
    Entry(const char* name, base::TimeDelta time, uint64_t count)
        : name_(name),
          time_(time.InMicroseconds()),
          count_(count),
          time_percent_(100),
          count_percent_(100) {}

    bool operator<(const Entry& other) const {
      if (time_ < other.time_) return true;
      if (time_ > other.time_) return false;
      return count_ < other.count_;
    }

    V8_NOINLINE void Print(std::ostream& os) {
      os.precision(2);
      os << std::fixed << std::setprecision(2);
      os << std::setw(50) << name_;
      os << std::setw(10) << static_cast<double>(time_) / 1000 << "ms ";
      os << std::setw(6) << time_percent_ << "%";
      os << std::setw(10) << count_ << " ";
      os << std::setw(6) << count_percent_ << "%";
      os << std::endl;
    }

    V8_NOINLINE void SetTotal(base::TimeDelta total_time,
                              uint64_t total_count) {
      if (total_time.InMicroseconds() == 0) {
        time_percent_ = 0;
      } else {
        time_percent_ = 100.0 * time_ / total_time.InMicroseconds();
      }
      count_percent_ = 100.0 * count_ / total_count;
    }

   private:
    const char* name_;
    int64_t time_;
    uint64_t count_;
    double time_percent_;
    double count_percent_;
  };

  uint64_t total_call_count = 0;
  base::TimeDelta total_time;
  std::vector<Entry> entries;
};

void RuntimeCallCounter::Reset() {
  count_ = 0;
  time_ = 0;
}

void RuntimeCallCounter::Dump(v8::tracing::TracedValue* value) {
  value->BeginArray(name_);
  value->AppendDouble(count_);
  value->AppendDouble(time_);
  value->EndArray();
}

void RuntimeCallCounter::Add(RuntimeCallCounter* other) {
  count_ += other->count();
  time_ += other->time().InMicroseconds();
}

void RuntimeCallTimer::Snapshot() {
  base::TimeTicks now = Now();
  // Pause only / topmost timer in the timer stack.
  Pause(now);
  // Commit all the timer's elapsed time to the counters.
  RuntimeCallTimer* timer = this;
  while (timer != nullptr) {
    timer->CommitTimeToCounter();
    timer = timer->parent();
  }
  Resume(now);
}

RuntimeCallStats::RuntimeCallStats()
    : in_use_(false), thread_id_(ThreadId::Current()) {
  static const char* const kNames[] = {
#define CALL_BUILTIN_COUNTER(name) "GC_" #name,
      FOR_EACH_GC_COUNTER(CALL_BUILTIN_COUNTER)  //
#undef CALL_BUILTIN_COUNTER
#define CALL_RUNTIME_COUNTER(name) #name,
      FOR_EACH_MANUAL_COUNTER(CALL_RUNTIME_COUNTER)  //
#undef CALL_RUNTIME_COUNTER
#define CALL_RUNTIME_COUNTER(name, nargs, ressize) #name,
      FOR_EACH_INTRINSIC(CALL_RUNTIME_COUNTER)  //
#undef CALL_RUNTIME_COUNTER
#define CALL_BUILTIN_COUNTER(name) #name,
      BUILTIN_LIST_C(CALL_BUILTIN_COUNTER)  //
#undef CALL_BUILTIN_COUNTER
#define CALL_BUILTIN_COUNTER(name) "API_" #name,
      FOR_EACH_API_COUNTER(CALL_BUILTIN_COUNTER)  //
#undef CALL_BUILTIN_COUNTER
#define CALL_BUILTIN_COUNTER(name) #name,
      FOR_EACH_HANDLER_COUNTER(CALL_BUILTIN_COUNTER)  //
#undef CALL_BUILTIN_COUNTER
  };
  for (int i = 0; i < counters_count; i++) {
    this->*(counters[i]) = RuntimeCallCounter(kNames[i]);
  }
}

// static
const RuntimeCallStats::CounterId RuntimeCallStats::counters[] = {
#define CALL_BUILTIN_COUNTER(name) &RuntimeCallStats::GC_##name,
    FOR_EACH_GC_COUNTER(CALL_BUILTIN_COUNTER)  //
#undef CALL_BUILTIN_COUNTER
#define CALL_RUNTIME_COUNTER(name) &RuntimeCallStats::name,
    FOR_EACH_MANUAL_COUNTER(CALL_RUNTIME_COUNTER)  //
#undef CALL_RUNTIME_COUNTER
#define CALL_RUNTIME_COUNTER(name, nargs, ressize) \
  &RuntimeCallStats::Runtime_##name,          //
    FOR_EACH_INTRINSIC(CALL_RUNTIME_COUNTER)  //
#undef CALL_RUNTIME_COUNTER
#define CALL_BUILTIN_COUNTER(name) &RuntimeCallStats::Builtin_##name,
    BUILTIN_LIST_C(CALL_BUILTIN_COUNTER)  //
#undef CALL_BUILTIN_COUNTER
#define CALL_BUILTIN_COUNTER(name) &RuntimeCallStats::API_##name,
    FOR_EACH_API_COUNTER(CALL_BUILTIN_COUNTER)  //
#undef CALL_BUILTIN_COUNTER
#define CALL_BUILTIN_COUNTER(name) &RuntimeCallStats::Handler_##name,
    FOR_EACH_HANDLER_COUNTER(CALL_BUILTIN_COUNTER)  //
#undef CALL_BUILTIN_COUNTER
};

// static
const int RuntimeCallStats::counters_count =
    arraysize(RuntimeCallStats::counters);

// static
void RuntimeCallStats::Enter(RuntimeCallStats* stats, RuntimeCallTimer* timer,
                             CounterId counter_id) {
  DCHECK(ThreadId::Current().Equals(stats->thread_id()));
  RuntimeCallCounter* counter = &(stats->*counter_id);
  DCHECK(counter->name() != nullptr);
  timer->Start(counter, stats->current_timer_.Value());
  stats->current_timer_.SetValue(timer);
  stats->current_counter_.SetValue(counter);
}

// static
void RuntimeCallStats::Leave(RuntimeCallStats* stats, RuntimeCallTimer* timer) {
  DCHECK(ThreadId::Current().Equals(stats->thread_id()));
  CHECK(stats->current_timer_.Value() == timer);
  stats->current_timer_.SetValue(timer->Stop());
  RuntimeCallTimer* cur_timer = stats->current_timer_.Value();
  stats->current_counter_.SetValue(cur_timer ? cur_timer->counter() : nullptr);
}

void RuntimeCallStats::Add(RuntimeCallStats* other) {
  for (const RuntimeCallStats::CounterId counter_id :
       RuntimeCallStats::counters) {
    RuntimeCallCounter* counter = &(this->*counter_id);
    RuntimeCallCounter* other_counter = &(other->*counter_id);
    counter->Add(other_counter);
  }
}

// static
void RuntimeCallStats::CorrectCurrentCounterId(RuntimeCallStats* stats,
                                               CounterId counter_id) {
  RuntimeCallTimer* timer = stats->current_timer_.Value();
  // When RCS are enabled dynamically there might be no current timer set up.
  if (timer == nullptr) return;
  RuntimeCallCounter* counter = &(stats->*counter_id);
  timer->set_counter(counter);
  stats->current_counter_.SetValue(counter);
}

void RuntimeCallStats::Print(std::ostream& os) {
  RuntimeCallStatEntries entries;
  if (current_timer_.Value() != nullptr) {
    current_timer_.Value()->Snapshot();
  }
  for (const RuntimeCallStats::CounterId counter_id :
       RuntimeCallStats::counters) {
    RuntimeCallCounter* counter = &(this->*counter_id);
    entries.Add(counter);
  }
  entries.Print(os);
}

void RuntimeCallStats::Reset() {
  if (V8_LIKELY(FLAG_runtime_stats == 0)) return;

  // In tracing, we only what to trace the time spent on top level trace events,
  // if runtime counter stack is not empty, we should clear the whole runtime
  // counter stack, and then reset counters so that we can dump counters into
  // top level trace events accurately.
  while (current_timer_.Value()) {
    current_timer_.SetValue(current_timer_.Value()->Stop());
  }

  for (const RuntimeCallStats::CounterId counter_id :
       RuntimeCallStats::counters) {
    RuntimeCallCounter* counter = &(this->*counter_id);
    counter->Reset();
  }

  in_use_ = true;
}

void RuntimeCallStats::Dump(v8::tracing::TracedValue* value) {
  for (const RuntimeCallStats::CounterId counter_id :
       RuntimeCallStats::counters) {
    RuntimeCallCounter* counter = &(this->*counter_id);
    if (counter->count() > 0) counter->Dump(value);
  }

  in_use_ = false;
}

}  // namespace internal
}  // namespace v8
