#pragma once

#include <mutex>
#include <optional>
#include <vector>

template <class T>
class SafeVector
{
public:
  SafeVector()
      : mData({})
  {
  }

  SafeVector(const std::vector<T>& data)
      : mData(data)
  {
  }

  uint32_t PushBack(const T& value)
  {
    std::unique_lock<std::mutex> lck(mDataMutex);
    mData.push_back(value);

    return mData.size();
  }

  std::optional<T> Front() const
  {
    std::unique_lock<std::mutex> lck(mDataMutex);
    if (mData.empty())
      return std::nullopt;

    return mData.front();
  }

  std::optional<T> Back() const
  {
    std::unique_lock<std::mutex> lck(mDataMutex);
    if (mData.empty())
      return std::nullopt;

    return mData.back();
  }

  std::optional<T> At(uint32_t index) const
  {
    std::unique_lock<std::mutex> lck(mDataMutex);
    if (index >= mData.size())
      return std::nullopt;

    return mData.at(index);
  }

  bool IsEmpty() const
  {
    std::unique_lock<std::mutex> lck(mDataMutex);
    return mData.empty();
  }

  uint32_t Size() const
  {
    std::unique_lock<std::mutex> lck(mDataMutex);
    return mData.size();
  }

  std::vector<T> Copy() const
  {
    std::unique_lock<std::mutex> lck(mDataMutex);
    return mData;
  }

  std::optional<T> ModifyElement(uint32_t index, const std::function<void(T& value)>& func)
  {
    std::unique_lock<std::mutex> lck(mDataMutex);
    if (index >= mData.size())
      return std::nullopt;

    func(mData.at(index));

    return mData.at(index);
  }

  bool ModifyElements(const std::function<bool(T& value)>& func)
  {
    std::unique_lock<std::mutex> lck(mDataMutex);
    for (auto it = mData.begin(); it != mData.end(); ++it)
    {
      if (!func(*it))
        return false;
    }

    return true;
  }

  std::optional<T> FindIf(const std::function<bool(const T& value)>& func) const
  {
    std::unique_lock<std::mutex> lck(mDataMutex);
    for (const auto& item : mData)
    {
      if (func(item))
        return item;
    }

    return std::nullopt;
  }

  uint32_t CountIf(const std::function<bool(const T& value)>& func) const
  {
    std::unique_lock<std::mutex> lck(mDataMutex);
    if (mData.empty())
      return 0;

    uint32_t itemCount = 0;
    for (const auto& item : mData)
    {
      if (func(item))
        itemCount++;
    }

    return itemCount;
  }

  std::optional<T> EraseFirst(const std::function<bool(const T& value)>& func)
  {
    T toReturn;

    std::unique_lock<std::mutex> lck(mDataMutex);
    for (auto it = mData.begin(); it != mData.end(); ++it)
    {
      if (!func(*it))
        continue;

      toReturn = *it;
      mData.erase(it);

      return toReturn;
    }

    return std::nullopt;
  }

  uint32_t EraseIf(const std::function<bool(const T& value)>& func)
  {
    uint32_t erasedItems = 0;

    std::unique_lock<std::mutex> lck(mDataMutex);
    for (auto it = mData.begin(); it != mData.end();)
    {
      if (func(*it))
      {
        it = mData.erase(it);
        erasedItems++;
      }
      else
      {
        ++it;
      }
    }

    return erasedItems;
  }

private:
  std::vector<T> mData;
  mutable std::mutex mDataMutex;
};
