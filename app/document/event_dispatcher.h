#pragma once

#include <QMetaObject>
#include <QObject>
#include <cstdint>
#include <functional>
#include <memory>
#include <unordered_map>
#include <utility>
#include <vector>

#include "test_and_set.h"

template <typename Event>
class EventDispatcher : public QObject
{
public:
  using Callback = std::function<void(const Event&)>;
  using Id = std::uint64_t;

private:
  struct State
  {
    Id nextId = 0;
    std::unordered_map<Id, Callback> listeners;
  };

public:
  class Subscription
  {
  public:
    Subscription() = default;

    Subscription(std::weak_ptr<State> state, Id id)
        : mState(std::move(state))
        , mId(id)
    {
    }

    ~Subscription()
    {
      reset();
    }

    Subscription(const Subscription&) = delete;
    Subscription& operator=(const Subscription&) = delete;

    Subscription(Subscription&& other) noexcept
        : mState(std::move(other.mState))
        , mId(std::exchange(other.mId, 0))
    {
    }

    Subscription& operator=(Subscription&& other) noexcept
    {
      if (this != &other)
      {
        reset();
        mState = std::move(other.mState);
        mId = std::exchange(other.mId, 0);
      }
      return *this;
    }

    void reset()
    {
      if (mId == 0)
        return;

      if (auto state = mState.lock())
        state->listeners.erase(mId);

      mState.reset();
      mId = 0;
    }

  private:
    std::weak_ptr<State> mState;
    Id mId = 0;
  };

  explicit EventDispatcher(QObject* parent = nullptr)
      : QObject(parent)
      , mState(std::make_shared<State>())
  {
  }

  Subscription subscribe(Callback callback)
  {
    const Id id = ++mState->nextId;
    mState->listeners.emplace(id, std::move(callback));

    return Subscription(mState, id);
  }

  void publish(const Event& event)
  {
    // Do not publish anything if the dispatcher is blocked
    if (mBlocked.Value())
      return;

    mPending.push_back(std::move(event));
    if (!mScheduled.SetAndFailOnDifferent(false, true))
      return;

    QMetaObject::invokeMethod(this, [this]() { flush(); }, Qt::QueuedConnection);
  }

  void blockEvents()
  {
    mBlocked.SetUnconditionally(true);
    mPending.clear();
  }

  void unblockEvents()
  {
    mBlocked.SetUnconditionally(false);
  }

  void flush()
  {
    mScheduled.SetUnconditionally(false);

    auto events = std::move(mPending);
    mPending.clear();

    for (const auto& event : events)
    {
      // Snapshot IDs so callbacks may unsubscribe safely.
      std::vector<Id> ids;
      ids.reserve(mState->listeners.size());

      for (const auto& [id, callback] : mState->listeners)
        ids.push_back(id);

      for (const auto id : ids)
      {
        auto it = mState->listeners.find(id);
        if (it == mState->listeners.end())
          continue;

        // Copy callback: invoking it may modify the listener map.
        auto callback = it->second;
        callback(event);
      }
    }
  }

private:
  std::shared_ptr<State> mState;

  std::vector<Event> mPending;
  TestAndSet<bool> mScheduled = false;
  TestAndSet<bool> mBlocked = false;
};