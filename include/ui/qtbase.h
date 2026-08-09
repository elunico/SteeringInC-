#ifndef QTBASE_H
#define QTBASE_H

namespace tom::render {

struct QtBase {
    static int x;
    static int y;
    int        h = 30;

    QtBase(int h);
    virtual ~QtBase() = default;
};

}  // namespace tom::render

#endif  // QTBASE_H