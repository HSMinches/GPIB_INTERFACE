// qmake: add `QT += charts`
// CMake: link against Qt6::Charts (or Qt5::Charts)

// file: src/mainwindow.h
#pragma once

#include <QDateTime>
#include <QMainWindow>
#include <QThread>
#include <QString>
#include <QVector>

class Ar488Controller;
class QChart;
class QChartView;
class QComboBox;
class QDateTimeAxis;
class QDateTimeEdit;
class QLabel;
class QLineEdit;
class QLineSeries;
class QPlainTextEdit;
class QPushButton;
class QRadioButton;
class QSpinBox;
class QStackedWidget;
class QTableWidget;
class QTimer;
class QToolBox;
class QTabWidget;
class QValueAxis;
class QWidget;

class MainWindow final : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override;

signals:
    void requestOpenPort(const QString& portName, int baudRate, int gpibAddress);
    void requestClosePort();
    void requestDetectAdapter();
    void requestWriteScpi(int gpibAddress, const QString& command);
    void requestQueryScpi(int gpibAddress, const QString& command);
    void requestRunBatch(int gpibAddress, const QString& script);

private slots:
    void refreshPorts();
    void onConnectClicked();
    void onConnectionChanged(bool connected, const QString& message);
    void onAdapterDetected(bool ok, const QString& text);
    void onWriteClicked();
    void onQueryClicked();
    void onBatchClicked();
    void onQueryCompleted(const QString& command, const QString& reply);
    void onBatchLineResult(const QString& command, const QString& reply);
    void onAddEquipmentClicked();
    void onRemoveEquipmentClicked();
    void onSaveEquipmentCsvClicked();
    void onLoadEquipmentCsvClicked();
    void onEquipmentActivated(int row, int column);
    void onAddDatalogJobClicked();
    void onRemoveDatalogJobClicked();
    void onBrowseDatalogCsvClicked();
    void onStartDatalogClicked();
    void onStopDatalogClicked();
    void onTriggerDatalogClicked();
    void onDatalogTimer();
    void onStartGraphLiveClicked();
    void onStopGraphLiveClicked();
    void onLoadGraphCsvClicked();
    void onClearGraphClicked();
    void onResetGraphViewClicked();
    void onGraphTimer();
    void onBusyChanged(bool busy);
    void toggleSidebar();

private:
    struct DataLogJob {
        int address = 10;
        QString equipmentCode;
        QString equipmentType;
        QString query;
        QString mode;
        int intervalMs = 1000;
        int maxSamples = 0;
        QDateTime startTime;
        QDateTime endTime;
        QString csvPath;

        int samplesWritten = 0;
        QDateTime nextDueTime;
        bool finished = false;
        bool stopped = false;
        bool triggerPending = false;
    };

    Ar488Controller* controller_ = nullptr;
    QThread controllerThread_;

    bool connected_ = false;
    bool busy_ = false;
    bool sidebarExpanded_ = true;

    QWidget* sidebarContainer_ = nullptr;
    QWidget* sidebarContent_ = nullptr;
    QPushButton* sidebarToggleButton_ = nullptr;
    QPushButton* equipmentSidebarButton_ = nullptr;

    QStackedWidget* sidebarPages_ = nullptr;
    QWidget* commandSidebarPage_ = nullptr;
    QWidget* equipmentSidebarPage_ = nullptr;

    QLabel* sidebarGroupTitleLabel_ = nullptr;
    QToolBox* commandToolBox_ = nullptr;
    QRadioButton* presetToCommandRadio_ = nullptr;
    QRadioButton* presetToBatchRadio_ = nullptr;

    QWidget* activeGpibBar_ = nullptr;
    QLabel* activeGpibLabel_ = nullptr;
    QSpinBox* gpibAddressSpin_ = nullptr;

    QTabWidget* mainTabs_ = nullptr;
    QWidget* connectionTab_ = nullptr;
    QWidget* scpiTab_ = nullptr;
    QWidget* batchTab_ = nullptr;
    QWidget* logTab_ = nullptr;
    QWidget* datalogTab_ = nullptr;
    QWidget* graphTab_ = nullptr;

    QComboBox* portCombo_ = nullptr;
    QComboBox* baudCombo_ = nullptr;
    QPushButton* refreshButton_ = nullptr;
    QPushButton* connectButton_ = nullptr;
    QPushButton* detectButton_ = nullptr;
    QLabel* adapterStatusLabel_ = nullptr;

    QSpinBox* equipmentAddressSpin_ = nullptr;
    QLineEdit* equipmentCodeEdit_ = nullptr;
    QComboBox* equipmentTypeCombo_ = nullptr;
    QPushButton* addEquipmentButton_ = nullptr;
    QPushButton* removeEquipmentButton_ = nullptr;
    QPushButton* saveEquipmentCsvButton_ = nullptr;
    QPushButton* loadEquipmentCsvButton_ = nullptr;
    QTableWidget* equipmentTable_ = nullptr;

    QComboBox* datalogEquipmentCombo_ = nullptr;
    QLineEdit* datalogQueryEdit_ = nullptr;
    QComboBox* datalogModeCombo_ = nullptr;
    QSpinBox* datalogIntervalSpin_ = nullptr;
    QSpinBox* datalogSamplesSpin_ = nullptr;
    QDateTimeEdit* datalogStartTimeEdit_ = nullptr;
    QDateTimeEdit* datalogEndTimeEdit_ = nullptr;
    QLineEdit* datalogCsvPathEdit_ = nullptr;
    QPushButton* datalogBrowseCsvButton_ = nullptr;
    QPushButton* datalogAddJobButton_ = nullptr;
    QPushButton* datalogRemoveJobButton_ = nullptr;
    QPushButton* datalogStartButton_ = nullptr;
    QPushButton* datalogStopButton_ = nullptr;
    QPushButton* datalogTriggerButton_ = nullptr;
    QLabel* datalogStatusLabel_ = nullptr;
    QTableWidget* datalogJobsTable_ = nullptr;
    QTimer* datalogTimer_ = nullptr;

    QVector<DataLogJob> datalogJobs_;
    bool datalogRunning_ = false;
    bool datalogStopRequested_ = false;
    bool datalogWaitingForReply_ = false;
    int datalogPendingJobIndex_ = -1;
    int datalogRoundRobinCursor_ = 0;

    QComboBox* graphEquipmentCombo_ = nullptr;
    QLineEdit* graphQueryEdit_ = nullptr;
    QSpinBox* graphIntervalSpin_ = nullptr;
    QPushButton* graphStartLiveButton_ = nullptr;
    QPushButton* graphStopLiveButton_ = nullptr;
    QPushButton* graphLoadCsvButton_ = nullptr;
    QPushButton* graphClearButton_ = nullptr;
    QPushButton* graphResetViewButton_ = nullptr;
    QLabel* graphStatusLabel_ = nullptr;
    QChart* graphChart_ = nullptr;
    QChartView* graphChartView_ = nullptr;
    QLineSeries* graphSeries_ = nullptr;
    QDateTimeAxis* graphAxisX_ = nullptr;
    QValueAxis* graphAxisY_ = nullptr;
    QTimer* graphTimer_ = nullptr;
    bool graphLiveRunning_ = false;
    bool graphWaitingForReply_ = false;
    int graphPendingAddress_ = -1;
    QString graphPendingQuery_;

    QLineEdit* commandEdit_ = nullptr;
    QPushButton* writeButton_ = nullptr;
    QPushButton* queryButton_ = nullptr;
    QPlainTextEdit* replyView_ = nullptr;

    QPlainTextEdit* batchEdit_ = nullptr;
    QPushButton* runBatchButton_ = nullptr;
    QPushButton* clearBatchButton_ = nullptr;
    QPlainTextEdit* batchResultView_ = nullptr;

    QPlainTextEdit* logView_ = nullptr;

    void buildUi();
    QWidget* buildSidebar(QWidget* parent);
    QWidget* buildPresetPage(const QString& description, const QStringList& commands, QWidget* parent);
    QWidget* buildEquipmentSidebarPage(QWidget* parent);
    void wireUi();
    void updateUiState();
    void updateSidebarState();
    void updateCommandGroupTitle();
    void appendLog(const QString& message);
    QString currentPortName() const;
    int currentBaudRate() const;
    void insertPresetCommand(const QString& command);
    void appendBatchLine(const QString& line);

    void refreshEquipmentSelectors();
    void refreshDatalogJobsTable();
    QString datalogJobStatusText(const DataLogJob& job) const;
    QString defaultDatalogCsvPath(const QString& equipmentCode, const QString& query) const;
    bool appendDatalogCsvRow(const DataLogJob& job, const QString& value, const QDateTime& timestamp);
    void finalizeDatalogRunIfNeeded();

    void updateGraphAxes();
    void setGraphStatus(const QString& text);
};


