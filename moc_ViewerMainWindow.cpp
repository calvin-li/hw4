/****************************************************************************
** Meta object code from reading C++ file 'ViewerMainWindow.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "ViewerMainWindow.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ViewerMainWindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_ViewerMainWindow[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      18,   17,   17,   17, 0x0a,
      36,   17,   17,   17, 0x0a,
      50,   17,   17,   17, 0x0a,
      66,   17,   17,   17, 0x0a,
      82,   17,   17,   17, 0x0a,
      97,   17,   17,   17, 0x0a,
     112,   17,   17,   17, 0x0a,
     136,   17,   17,   17, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_ViewerMainWindow[] = {
    "ViewerMainWindow\0\0lightPosChanged()\0"
    "viewChanged()\0cameraChanged()\0"
    "rotateChanged()\0lightChanged()\0"
    "colorChanged()\0shadingChanged(QString)\0"
    "refresh()\0"
};

void ViewerMainWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        ViewerMainWindow *_t = static_cast<ViewerMainWindow *>(_o);
        switch (_id) {
        case 0: _t->lightPosChanged(); break;
        case 1: _t->viewChanged(); break;
        case 2: _t->cameraChanged(); break;
        case 3: _t->rotateChanged(); break;
        case 4: _t->lightChanged(); break;
        case 5: _t->colorChanged(); break;
        case 6: _t->shadingChanged((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 7: _t->refresh(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData ViewerMainWindow::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject ViewerMainWindow::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_ViewerMainWindow,
      qt_meta_data_ViewerMainWindow, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &ViewerMainWindow::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *ViewerMainWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *ViewerMainWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_ViewerMainWindow))
        return static_cast<void*>(const_cast< ViewerMainWindow*>(this));
    return QMainWindow::qt_metacast(_clname);
}

int ViewerMainWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 8)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 8;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
