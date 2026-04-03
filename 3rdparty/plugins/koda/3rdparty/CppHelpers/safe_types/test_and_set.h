#pragma once

#include <chrono>
#include <condition_variable>
#include <mutex>
#include <vector>

template <class T>
class TestAndSet
{
public:
  TestAndSet(const T& v)
      : mData(v)
  {
  }

  void SetOnDifferent(const T& oldValue, const T& value)
  {
    std::unique_lock<std::mutex> lck(mDataMutex);
    if (mData != oldValue)
      mData = value;
  }

  void SetUnconditionally(const T& value)
  {
    std::unique_lock<std::mutex> lck(mDataMutex);
    mData = value;
  }

  bool SetAndFailOnDifferent(const T& oldValue, const T& newValue)
  {
    std::unique_lock<std::mutex> lck(mDataMutex);
    if (oldValue != mData)
      return false;

    mData = newValue;

    return true;
  }

  bool SetAndFailOnDifferent(const std::vector<T>& oldValues, const T& newValue)
  {
    std::unique_lock<std::mutex> lck(mDataMutex);
    bool update = false;
    for (const T& oldValue : oldValues)
    {
      if (oldValue == mData)
      {
        update = true;
        break;
      }
    }

    if (update)
      mData = newValue;

    return update;
  }

  bool SetAndFailOnEqual(const T& oldValue, const T& newValue)
  {
    std::unique_lock<std::mutex> lck(mDataMutex);
    if (oldValue == mData)
      return false;

    mData = newValue;

    return true;
  }

  T Value() const
  {
    std::unique_lock<std::mutex> lck(mDataMutex);
    return mData;
  }

  T WaitDifferent(std::condition_variable& cv, const T& value)
  {
    std::unique_lock<std::mutex> lck(mDataMutex);
    cv.wait(lck, [this, value] { return mData != value; });

    return mData;
  }

  T WaitForDifferent(std::condition_variable& cv, std::chrono::seconds time, const T& value)
  {
    std::unique_lock<std::mutex> lck(mDataMutex);
    cv.wait_for(lck, time, [this, value] { return mData != value; });

    return mData;
  }

  bool operator==(const T& value) const
  {
    std::unique_lock<std::mutex> lck(mDataMutex);
    return mData == value;
  }

  bool operator!=(const T& value) const
  {
    std::unique_lock<std::mutex> lck(mDataMutex);
    return mData != value;
  }

private:
  T mData;
  mutable std::mutex mDataMutex;
};
