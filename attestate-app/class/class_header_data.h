#pragma once

#include <QtCore>

namespace cls {

class HeaderData {
public:
    typedef uint8_t Index;

    HeaderData() {}
    explicit HeaderData(Index sections)
        : texts_(sections)
        , alignments_(sections)
        , orientations_(sections)
    {}

    void setData(Index i, const QVariant& data, int role = Qt::DisplayRole);
    QVariant data(Index i, int role = Qt::DisplayRole) const;

    void insert(Index i);
    void append();
    void remove(Index i);

    inline int count() const { return texts_.size(); }

private:
    QVector<QVariant> texts_;
    QVector<QVariant> alignments_;
    QVector<QVariant> orientations_;
};

} // namespace cls
