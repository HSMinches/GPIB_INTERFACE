/****************************************************************************
** Meta object code from reading C++ file 'mainwindow.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.11.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../src/mainwindow.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mainwindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 69
#error "This file was generated using the moc from 6.11.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
QT_WARNING_DISABLE_GCC("-Wuseless-cast")
namespace {
struct qt_meta_tag_ZN10MainWindowE_t {};
} // unnamed namespace

template <> constexpr inline auto MainWindow::qt_create_metaobjectdata<qt_meta_tag_ZN10MainWindowE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "MainWindow",
        "requestOpenPort",
        "",
        "portName",
        "baudRate",
        "gpibAddress",
        "requestClosePort",
        "requestDetectAdapter",
        "requestWriteScpi",
        "command",
        "requestQueryScpi",
        "requestRunBatch",
        "script",
        "refreshPorts",
        "onConnectClicked",
        "onConnectionChanged",
        "connected",
        "message",
        "onAdapterDetected",
        "ok",
        "text",
        "onWriteClicked",
        "onQueryClicked",
        "onBatchClicked",
        "onQueryCompleted",
        "reply",
        "onBatchLineResult",
        "onAddEquipmentClicked",
        "onRemoveEquipmentClicked",
        "onSaveEquipmentCsvClicked",
        "onLoadEquipmentCsvClicked",
        "onEquipmentActivated",
        "row",
        "column",
        "onAddDatalogJobClicked",
        "onRemoveDatalogJobClicked",
        "onBrowseDatalogCsvClicked",
        "onStartDatalogClicked",
        "onStopDatalogClicked",
        "onTriggerDatalogClicked",
        "onDatalogTimer",
        "onStartGraphLiveClicked",
        "onStopGraphLiveClicked",
        "onLoadGraphCsvClicked",
        "onClearGraphClicked",
        "onResetGraphViewClicked",
        "onGraphTimer",
        "onBusyChanged",
        "busy",
        "toggleSidebar"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'requestOpenPort'
        QtMocHelpers::SignalData<void(const QString &, int, int)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 3 }, { QMetaType::Int, 4 }, { QMetaType::Int, 5 },
        }}),
        // Signal 'requestClosePort'
        QtMocHelpers::SignalData<void()>(6, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'requestDetectAdapter'
        QtMocHelpers::SignalData<void()>(7, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'requestWriteScpi'
        QtMocHelpers::SignalData<void(int, const QString &)>(8, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 5 }, { QMetaType::QString, 9 },
        }}),
        // Signal 'requestQueryScpi'
        QtMocHelpers::SignalData<void(int, const QString &)>(10, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 5 }, { QMetaType::QString, 9 },
        }}),
        // Signal 'requestRunBatch'
        QtMocHelpers::SignalData<void(int, const QString &)>(11, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 5 }, { QMetaType::QString, 12 },
        }}),
        // Slot 'refreshPorts'
        QtMocHelpers::SlotData<void()>(13, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onConnectClicked'
        QtMocHelpers::SlotData<void()>(14, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onConnectionChanged'
        QtMocHelpers::SlotData<void(bool, const QString &)>(15, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Bool, 16 }, { QMetaType::QString, 17 },
        }}),
        // Slot 'onAdapterDetected'
        QtMocHelpers::SlotData<void(bool, const QString &)>(18, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Bool, 19 }, { QMetaType::QString, 20 },
        }}),
        // Slot 'onWriteClicked'
        QtMocHelpers::SlotData<void()>(21, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onQueryClicked'
        QtMocHelpers::SlotData<void()>(22, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onBatchClicked'
        QtMocHelpers::SlotData<void()>(23, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onQueryCompleted'
        QtMocHelpers::SlotData<void(const QString &, const QString &)>(24, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QString, 9 }, { QMetaType::QString, 25 },
        }}),
        // Slot 'onBatchLineResult'
        QtMocHelpers::SlotData<void(const QString &, const QString &)>(26, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QString, 9 }, { QMetaType::QString, 25 },
        }}),
        // Slot 'onAddEquipmentClicked'
        QtMocHelpers::SlotData<void()>(27, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onRemoveEquipmentClicked'
        QtMocHelpers::SlotData<void()>(28, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onSaveEquipmentCsvClicked'
        QtMocHelpers::SlotData<void()>(29, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onLoadEquipmentCsvClicked'
        QtMocHelpers::SlotData<void()>(30, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onEquipmentActivated'
        QtMocHelpers::SlotData<void(int, int)>(31, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Int, 32 }, { QMetaType::Int, 33 },
        }}),
        // Slot 'onAddDatalogJobClicked'
        QtMocHelpers::SlotData<void()>(34, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onRemoveDatalogJobClicked'
        QtMocHelpers::SlotData<void()>(35, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onBrowseDatalogCsvClicked'
        QtMocHelpers::SlotData<void()>(36, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onStartDatalogClicked'
        QtMocHelpers::SlotData<void()>(37, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onStopDatalogClicked'
        QtMocHelpers::SlotData<void()>(38, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onTriggerDatalogClicked'
        QtMocHelpers::SlotData<void()>(39, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onDatalogTimer'
        QtMocHelpers::SlotData<void()>(40, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onStartGraphLiveClicked'
        QtMocHelpers::SlotData<void()>(41, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onStopGraphLiveClicked'
        QtMocHelpers::SlotData<void()>(42, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onLoadGraphCsvClicked'
        QtMocHelpers::SlotData<void()>(43, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onClearGraphClicked'
        QtMocHelpers::SlotData<void()>(44, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onResetGraphViewClicked'
        QtMocHelpers::SlotData<void()>(45, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onGraphTimer'
        QtMocHelpers::SlotData<void()>(46, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onBusyChanged'
        QtMocHelpers::SlotData<void(bool)>(47, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Bool, 48 },
        }}),
        // Slot 'toggleSidebar'
        QtMocHelpers::SlotData<void()>(49, 2, QMC::AccessPrivate, QMetaType::Void),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<MainWindow, qt_meta_tag_ZN10MainWindowE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject MainWindow::staticMetaObject = { {
    QMetaObject::SuperData::link<QMainWindow::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10MainWindowE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10MainWindowE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN10MainWindowE_t>.metaTypes,
    nullptr
} };

void MainWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<MainWindow *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->requestOpenPort((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<int>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<int>>(_a[3]))); break;
        case 1: _t->requestClosePort(); break;
        case 2: _t->requestDetectAdapter(); break;
        case 3: _t->requestWriteScpi((*reinterpret_cast<std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2]))); break;
        case 4: _t->requestQueryScpi((*reinterpret_cast<std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2]))); break;
        case 5: _t->requestRunBatch((*reinterpret_cast<std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2]))); break;
        case 6: _t->refreshPorts(); break;
        case 7: _t->onConnectClicked(); break;
        case 8: _t->onConnectionChanged((*reinterpret_cast<std::add_pointer_t<bool>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2]))); break;
        case 9: _t->onAdapterDetected((*reinterpret_cast<std::add_pointer_t<bool>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2]))); break;
        case 10: _t->onWriteClicked(); break;
        case 11: _t->onQueryClicked(); break;
        case 12: _t->onBatchClicked(); break;
        case 13: _t->onQueryCompleted((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2]))); break;
        case 14: _t->onBatchLineResult((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2]))); break;
        case 15: _t->onAddEquipmentClicked(); break;
        case 16: _t->onRemoveEquipmentClicked(); break;
        case 17: _t->onSaveEquipmentCsvClicked(); break;
        case 18: _t->onLoadEquipmentCsvClicked(); break;
        case 19: _t->onEquipmentActivated((*reinterpret_cast<std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<int>>(_a[2]))); break;
        case 20: _t->onAddDatalogJobClicked(); break;
        case 21: _t->onRemoveDatalogJobClicked(); break;
        case 22: _t->onBrowseDatalogCsvClicked(); break;
        case 23: _t->onStartDatalogClicked(); break;
        case 24: _t->onStopDatalogClicked(); break;
        case 25: _t->onTriggerDatalogClicked(); break;
        case 26: _t->onDatalogTimer(); break;
        case 27: _t->onStartGraphLiveClicked(); break;
        case 28: _t->onStopGraphLiveClicked(); break;
        case 29: _t->onLoadGraphCsvClicked(); break;
        case 30: _t->onClearGraphClicked(); break;
        case 31: _t->onResetGraphViewClicked(); break;
        case 32: _t->onGraphTimer(); break;
        case 33: _t->onBusyChanged((*reinterpret_cast<std::add_pointer_t<bool>>(_a[1]))); break;
        case 34: _t->toggleSidebar(); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (MainWindow::*)(const QString & , int , int )>(_a, &MainWindow::requestOpenPort, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (MainWindow::*)()>(_a, &MainWindow::requestClosePort, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (MainWindow::*)()>(_a, &MainWindow::requestDetectAdapter, 2))
            return;
        if (QtMocHelpers::indexOfMethod<void (MainWindow::*)(int , const QString & )>(_a, &MainWindow::requestWriteScpi, 3))
            return;
        if (QtMocHelpers::indexOfMethod<void (MainWindow::*)(int , const QString & )>(_a, &MainWindow::requestQueryScpi, 4))
            return;
        if (QtMocHelpers::indexOfMethod<void (MainWindow::*)(int , const QString & )>(_a, &MainWindow::requestRunBatch, 5))
            return;
    }
}

const QMetaObject *MainWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MainWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10MainWindowE_t>.strings))
        return static_cast<void*>(this);
    return QMainWindow::qt_metacast(_clname);
}

int MainWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 35)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 35;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 35)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 35;
    }
    return _id;
}

// SIGNAL 0
void MainWindow::requestOpenPort(const QString & _t1, int _t2, int _t3)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1, _t2, _t3);
}

// SIGNAL 1
void MainWindow::requestClosePort()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void MainWindow::requestDetectAdapter()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}

// SIGNAL 3
void MainWindow::requestWriteScpi(int _t1, const QString & _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 3, nullptr, _t1, _t2);
}

// SIGNAL 4
void MainWindow::requestQueryScpi(int _t1, const QString & _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 4, nullptr, _t1, _t2);
}

// SIGNAL 5
void MainWindow::requestRunBatch(int _t1, const QString & _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 5, nullptr, _t1, _t2);
}
QT_WARNING_POP
