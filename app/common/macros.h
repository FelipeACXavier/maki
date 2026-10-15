#pragma once

#include <QModelIndex>
#include <QString>

#define UNUSED_RETURN(VALUE) static_cast<void>(VALUE)

namespace maki
{
template <typename Container>
std::vector<int> sortedRows(const Container& table)
{
  if (!table)
    return {};

  const auto selectedRows = table->selectionModel()->selectedRows();
  if (selectedRows.empty())
    return {};

  std::vector<int> rows;
  rows.reserve(selectedRows.size());
  std::transform(selectedRows.cbegin(), selectedRows.cend(), std::back_inserter(rows), [](const QModelIndex& index) { return index.row(); });
  std::sort(rows.begin(), rows.end(), std::greater<int>());

  // Avoid applying the action multiple times to the same row.
  rows.erase(std::unique(rows.begin(), rows.end()), rows.end());

  return rows;
}

template <typename Container, typename Func>
bool applyOnSortedTable(const Container& table, Func&& action)
{
  std::vector<int> rows = sortedRows(table);
  if (rows.empty())
    return false;

  for (const int row : rows)
  {
    if (row < 0 || row >= table->rowCount())
      continue;

    std::invoke(action, row);
  }

  return true;
}

template <typename Container, typename Func>
auto findIf(Container& container, Func&& predicate) -> std::optional<typename Container::value_type>
{
  const auto it = std::find_if(std::begin(container), std::end(container), std::forward<Func>(predicate));
  if (it == std::cend(container))
    return std::nullopt;

  return *it;
}

template <typename Container, typename Func>
auto findIf(const Container& container, Func&& predicate) -> std::optional<typename Container::value_type>
{
  const auto it = std::find_if(std::cbegin(container), std::cend(container), std::forward<Func>(predicate));
  if (it == std::cend(container))
    return std::nullopt;

  return *it;
}

template <class T, typename Container, typename Func>
T countIf(const Container& container, Func&& predicate)
{
  return std::count_if(std::cbegin(container), std::cend(container), std::forward<Func>(predicate));
}

template <class T, typename Container, typename Func>
T accumulate(const Container& container, Func&& predicate)
{
  return std::accumulate(std::cbegin(container), std::cend(container), T{0}, std::forward<Func>(predicate));
}

template <typename Container, typename Func>
bool anyOf(const Container& container, Func&& predicate)
{
  return std::any_of(std::cbegin(container), std::cend(container), std::forward<Func>(predicate));
}

inline bool isInt(const QString& value)
{
  bool ok = false;
  UNUSED_RETURN(value.toInt(&ok));
  return ok;
}

inline bool isUInt(const QString& value)
{
  bool ok = false;
  UNUSED_RETURN(value.toUInt(&ok));
  return ok;
}

inline bool isDouble(const QString& value)
{
  bool ok = false;
  UNUSED_RETURN(value.toDouble(&ok));
  return ok;
}

}  // namespace maki