# CppHelpers
Just a bunch of header only libraries to make working with files, strings, logs... more enjoyable.

Some examples are given below for each helper.

## logging.h

```cpp
#include "logging.h"
...
// Increase log level every iteration
for (int i = 0; i <= (int)logging::LogLevel::Trace; ++i)
{
  logging::gMinLogLevel = (logging::LogLevel)i;

  LOG_ERROR("Error");
  LOG_WARNING("Warning");
  LOG_INFO("Information");
  LOG_DEBUG("Debug");
  LOG_TRACE("Trace");  
}
```
Output:
```
27/04/2023 14:57:11.976681893 [E] main:22: Error
27/04/2023 14:57:11.976809922 [E] main:22: Error
27/04/2023 14:57:11.976834516 [W] main:23: Warning
27/04/2023 14:57:11.976859960 [E] main:22: Error
27/04/2023 14:57:11.976878048 [W] main:23: Warning
27/04/2023 14:57:11.976896704 [I] main:24: Information
27/04/2023 14:57:11.976918112 [E] main:22: Error
27/04/2023 14:57:11.976935470 [W] main:23: Warning
27/04/2023 14:57:11.976953377 [I] main:24: Information
27/04/2023 14:57:11.976973335 [D] main:25: Debug
27/04/2023 14:57:11.976990954 [E] main:22: Error
27/04/2023 14:57:11.977008535 [W] main:23: Warning
27/04/2023 14:57:11.977025679 [I] main:24: Information
27/04/2023 14:57:11.977043618 [D] main:25: Debug
27/04/2023 14:57:11.977061607 [T] main:26: Trace
```
## string_helpers.h

Generic string functions.
```cpp
#include "string_helpers.h"
...
std::string str = "My New String";
LOG_INFO("Original string: " + str);
ToUpperCase(str);
LOG_INFO("Upper case: " + str);

ToLowerCase(str);
LOG_INFO("Lower case: " + str);

for (auto& s : Split(str, ' '))
  LOG_INFO("Split: " + s);

std::string ogStr;
for (auto& s : Split(str, ' '))
  ogStr += ToUpperCase(s, 0, 1) + " ";

LOG_INFO("Original string: " + ogStr);
```

Output:
```
27/04/2023 14:55:59.287343429 [I] main:19: Original string: My New String
27/04/2023 14:55:59.287390813 [I] main:21: Upper case: MY NEW STRING
27/04/2023 14:55:59.287401568 [I] main:24: Lower case: my new string
27/04/2023 14:55:59.287410552 [I] main:27: Split: my
27/04/2023 14:55:59.287417720 [I] main:27: Split: new
27/04/2023 14:55:59.287424475 [I] main:27: Split: string
27/04/2023 14:55:59.287431449 [I] main:33: Original string: My New String 
```

## file_system_helpers.h
Naturally,it contains file system helpers. For now, only linux is supported. For windows, the library can be compiled with `-DCPPHELPERS_FILE_SYSTEM=OFF`

```cpp
#include "file_system_helpers.h"
...
// Check if a directory or file exists
std::string dir = "../file_system";
LOG_INFO("Does directory exist: %u", DoesFileExist(dir));

// Get all files in the directory 
auto files = GetFilesInDirectory(dir);
for (const auto& file : files)
{
  // Check for header files for example
  if (IsOfType(file, "h"))
    LOG_INFO("Header file %s in folder: %s: Executable? %u", GetFilename(file).c_str(), RemoveFilename(file).c_str(), IsFileExecutable(file));
}
```

Output:
```
27/04/2023 14:55:10.72907195 [I] main:19: Does directory exist: 1
27/04/2023 14:55:10.78206504 [I] main:27: Header file file_system_helpers in folder: ./: Executable? 0
27/04/2023 14:55:10.78288509 [I] main:27: Header file sync_process in folder: ./: Executable? 0
```

## sync_process.h
Easily launch synchronous bash commands from cpp

```cpp
#include "sync_process.h"
...
// String to hold the command output
std::string output;

int result = SyncProcess::Execute("neofetch", output);

// 0 on success
if (result)
  LOG_ERROR("Failed: " + SyncProcess::Error(result));
else
  LOG_INFO("\n" + output);
```

## safe_vector.h
Thread safe std::vector wrapper

```cpp
#include "safe_vector.h"
...
SafeVector<int> vec({1, 2, 3, 4, 5});

LOG_INFO("Element at index == size? %u", (bool)vec.At(vec.Size())); // false

vec.ModifyElements([](int& value) {
  value += 5;
  LOG_DEBUG("New element: %d", value);
  return true;
});
```

## test_and_set.h
Thread safe "one-shot" variable wrapper

```cpp
#include "test_and_set.h"
...
TestAndSet<int> num(1);
LOG_INFO("Current value: %d", num.Value());

num.SetOnDifferent(1, 2);
LOG_INFO("Value after attempt: %d", num.Value());

bool changed = num.SetAndFailOnDifferent(1, 2);
LOG_INFO("Value after second attempt: %d, new: %u", num.Value(), changed);

changed = num.SetAndFailOnDifferent({1, 2, 3}, 3);
LOG_INFO("Value after third attempt: %d, new: %u", num.Value(), changed);
```

Output:
```
27/04/2023 14:47:31.155647092 [I] main:40: Current value: 1
27/04/2023 14:47:31.155651614 [I] main:43: Value after attempt: 1
27/04/2023 14:47:31.155655412 [I] main:46: Value after second attempt: 2, new: 1
27/04/2023 14:47:31.155659980 [I] main:49: Value after third attempt: 3, new: 1
```

## result.h
Wrapper around the handy std::optional with support for error messages

```cpp
#include "result.h"
...
Result<int> Divide(int num, int den)
{
  if (den == 0)
    return Result<int>::Failed("Cannot divide by 0");

  if (num % den == 0)
    return num / den;

  return Result<int>::Failed(Format("%d is not divisible by %d", num, den));
}
...

// Test the result with a few numbers
for (int i = 0; i <= 10; i += 5)
{
  auto ret = Divide(i + 5, i);
  if (!ret.IsSuccess())
    LOG_WARNING(ret.ErrorMessage());
  else
    LOG_INFO("%d / %d = %d", i + 5, i, ret.Value());
}
```
Output:
```
27/04/2023 14:47:31.155668784 [W] main:55: Cannot divide by 0
27/04/2023 14:47:31.155673694 [I] main:57: 10 / 5 = 2
27/04/2023 14:47:31.155678679 [W] main:55: 15 is not divisible by 10
```
