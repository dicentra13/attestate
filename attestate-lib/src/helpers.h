#pragma once

#include <QString>
#include <QChar>

#include <memory>
#include <iostream>

namespace attestate {

template <class P>
bool byPointerCmp (const std::shared_ptr<P>& l, const std::shared_ptr<P>& r)
{
    return (!l && !r) || (l && r && *l == *r);
}

template <class P>
bool byPointerCmp (const std::shared_ptr<P>& l, std::nullptr_t)
{
    return !l;
}

template <class P>
bool byPointerCmp (std::nullptr_t, const std::shared_ptr<P>& r)
{
    return !r;
}

template <class P>
bool byPointerCmp (const std::unique_ptr<P>& l, const std::unique_ptr<P>& r)
{
    return (!l && !r) || (l && r && *l == *r);
}

template <class P>
bool byPointerCmp (const std::unique_ptr<P>& l, std::nullptr_t)
{
    return !l;
}

template <class P>
bool byPointerCmp (std::nullptr_t, const std::unique_ptr<P>& r)
{
    return !r;
}


inline std::ostream& operator << (std::ostream& o, const QString & s)
{
    o << s.toStdString();
    return o;
}

inline std::ostream& operator << (std::ostream& o, const QChar & c)
{
    o << QString(c).toStdString();
    return o;
}

} // namespace attestate
