#pragma once

#include <exception>
#include <functional>
#include <optional>
#include <stdexcept>
#include <string>

template <class T>
class Result
{
public:
  Result(T data)
      : mData(data)
  {
  }

  Result(const std::exception& e)
      : mData(std::nullopt)
      , mExceptionMsg(e.what())
  {
  }

  Result(std::function<T()> f)
  {
    try
    {
      mData = f();
    }
    catch (const std::exception& e)
    {
      mData = std::nullopt;
      mExceptionMsg = e.what();
    }
  }

  template <class U>
  Result(const Result<U>& r)
      : mData(std::nullopt)
  {
    if (r.IsSuccess())
      throw std::bad_cast();

    mExceptionMsg = r.mExceptionMsg;
  }

  bool IsSuccess() const noexcept
  {
    return bool(mData);
  }

  std::string ErrorMessage() const noexcept
  {
    return mExceptionMsg;
  }

  T Value() const
  {
    return mData.value();
  }

  T ValueOrDefault(const T& defaultValue) const
  {
    return IsSuccess() ? Value() : defaultValue;
  }

  template <class U>
  Result<U> As() const
  {
    if (IsSuccess())
      throw std::bad_cast();

    return Result<U>::Failed(mExceptionMsg);
  }

  Result<T> Or(const Result<T>& r)
  {
    if (IsSuccess())
      return *this;
    else
      return r;
  }

  Result<T> Or(std::function<Result<T>()> f)
  {
    if (IsSuccess())
      return *this;
    else
      return f();
  }

  template <class U>
  Result<U> Chain(std::function<Result<U>(const T& v)> f)
  {
    if (!IsSuccess())
      return this->As<U>();
    else
      return f(Value());
  }

  static Result<T> Failed(const std::string& msg)
  {
    return Result<T>(std::runtime_error(msg));
  }

  template <class U>
  static Result Failed(const Result<U>& result)
  {
    return Result<T>(std::runtime_error(result.ErrorMessage()));
  }

  explicit operator bool() const noexcept
  {
    return IsSuccess();
  }

  bool operator==(const Result<T>& v2) const
  {
    return IsSuccess() == v2.IsSuccess() && (!IsSuccess() || Value() == v2.Value());
  }

protected:
  std::optional<T> mData;
  std::string mExceptionMsg;
};

class VoidResult
{
public:
  VoidResult()
      : mSuccess(true)
  {
  }

  VoidResult(const std::exception& e)
      : mSuccess(false)
      , mExceptionMsg(e.what())
  {
  }

  VoidResult(std::function<void()> f)
      : mSuccess(false)
  {
    try
    {
      f();
      mSuccess = true;
    }
    catch (const std::exception& e)
    {
      mExceptionMsg = e.what();
      mSuccess = false;
    }
  }

  template <class U>
  VoidResult(const Result<U>& r)
      : mSuccess(r.IsSuccess())
  {
    if (!r.IsSuccess())
    {
      mExceptionMsg = r.ErrorMessage();
    }
  }

  bool IsSuccess() const noexcept
  {
    return mSuccess;
  }

  std::string ErrorMessage() const noexcept
  {
    return mExceptionMsg;
  }

  template <class U>
  Result<U> As() const
  {
    if (IsSuccess())
      throw std::bad_cast();

    return Result<U>::Failed(mExceptionMsg);
  }

  template <class U>
  VoidResult Or(const Result<U>& r)
  {
    if (IsSuccess())
      return *this;
    else
      return VoidResult(r);
  }

  VoidResult And(const VoidResult& r)
  {
    if (IsSuccess() && r.IsSuccess())
      return VoidResult();
    else if (!IsSuccess() && !r.IsSuccess())
      return VoidResult::Failed(ErrorMessage() + " and " + r.ErrorMessage());
    else if (!IsSuccess())
      return *this;
    else
      return r;
  }

  static VoidResult Failed(const std::string& msg)
  {
    return VoidResult(std::runtime_error(msg));
  }

  template <class U>
  static VoidResult Failed(const Result<U>& result)
  {
    return VoidResult(std::runtime_error(result.ErrorMessage()));
  }

  static VoidResult FailIf(std::function<bool()> f, const std::string& msg)
  {
    return f() ? VoidResult::Failed(msg) : VoidResult();
  }

  explicit operator bool() const noexcept
  {
    return IsSuccess();
  }

private:
  bool mSuccess;
  std::string mExceptionMsg;
};

template <class T>
class DataResult : public VoidResult
{
public:
  DataResult(T data)
      : VoidResult()
      , mData(data)
  {
  }

  DataResult(T data, const std::exception& e)
      : VoidResult(e)
      , mData(data)
  {
  }

  static DataResult<T> Failed(T data, const std::string& msg)
  {
    return DataResult<T>(data, std::runtime_error(msg));
  }

  T Value() const
  {
    return mData.value();
  }

protected:
  std::optional<T> mData;
};

#define RETURN_ON_FAILURE(func) \
  do                            \
  {                             \
    auto ret = func;            \
    if (!ret.IsSuccess())       \
    {                           \
      return ret;               \
    }                           \
  } while (0)

#define RETURN_ON_FAILURE_AS(func, t) \
  do                                  \
  {                                   \
    auto ret = func;                  \
    if (!ret.IsSuccess())             \
    {                                 \
      return ret.As<t>();             \
    }                                 \
  } while (0)

#define ASSIGN_OR_RETURN_ON_FAILURE(v, func) \
  do                                         \
  {                                          \
    auto ret = func;                         \
    if (!ret.IsSuccess())                    \
    {                                        \
      return ret;                            \
    }                                        \
    v = ret.Value();                         \
  } while (0)

#define ASSIGN_OR_RETURN_ON_FAILURE_AS(v, func, t) \
  do                                               \
  {                                                \
    auto retResult = func;                         \
    if (!retResult.IsSuccess())                    \
    {                                              \
      return retResult.As<t>();                    \
    }                                              \
                                                   \
    v = retResult.Value();                         \
  } while (0)

#define LOG_AND_RETURN(t, m)     \
  do                             \
  {                              \
    LOG_ERROR(m);                \
    return Result<t>::Failed(m); \
  } while (0)

#define LOG_AND_RETURN_VOID(m)    \
  do                              \
  {                               \
    LOG_ERROR(m);                 \
    return VoidResult::Failed(m); \
  } while (0)

#define LOG_ERROR_ON_FAILURE(m)      \
  do                                 \
  {                                  \
    auto ret = m;                    \
    if (!ret.IsSuccess())            \
      LOG_ERROR(ret.ErrorMessage()); \
  } while (0)

#define LOG_WARN_ON_FAILURE(m)         \
  do                                   \
  {                                    \
    auto ret = m;                      \
    if (!ret.IsSuccess())              \
      LOG_WARNING(ret.ErrorMessage()); \
  } while (0)

#define LOG_AND_RETURN_VOID_ON_FAILURE(m)            \
  do                                                 \
  {                                                  \
    auto ret = m;                                    \
    if (!ret.IsSuccess())                            \
    {                                                \
      LOG_ERROR("%s", ret.ErrorMessage());           \
      return VoidResult::Failed(ret.ErrorMessage()); \
    }                                                \
  } while (0)
