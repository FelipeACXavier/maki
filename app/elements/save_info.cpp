#include "save_info.h"

QDataStream& operator<<(QDataStream& out, const SaveInfo& info)
{
  out << info.size;
  out << info.nodeId;
  out << info.pixmap;
  out << info.fields;
  out << info.position;
  out << info.properties;

  return out;
}

QDataStream& operator>>(QDataStream& in, SaveInfo& info)
{
  in >> info.size;
  in >> info.nodeId;
  in >> info.pixmap;
  in >> info.fields;
  in >> info.position;
  in >> info.properties;

  return in;
}
