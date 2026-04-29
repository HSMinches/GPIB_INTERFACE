
#include "mainwindow.h"

#include "ar488controller.h"

#include <QAbstractItemView>
#include <QComboBox>
#include <QDateTime>
#include <QDateTimeEdit>
#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QGridLayout>
#include <QGroupBox>
#include <QHeaderView>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QList>
#include <QMetaObject>
#include <QMouseEvent>
#include <QPainter>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QRadioButton>
#include <QRegularExpression>
#include <QSerialPortInfo>
#include <QSizePolicy>
#include <QSpinBox>
#include <QStackedWidget>
#include <QStatusBar>
#include <QTabWidget>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QTextCursor>
#include <QTextStream>
#include <QTimer>
#include <QToolBox>
#include <QVBoxLayout>
#include <QWheelEvent>
#include <QWidget>
#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QtCharts/QDateTimeAxis>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>

namespace {
constexpr int kMinGpibAddress = 1;
constexpr int kMaxGpibAddress = 29;
constexpr int kDefaultGpibAddress = 10;
constexpr int kSidebarExpandedWidth = 390;
constexpr int kSidebarCollapsedWidth = 96;

class InteractiveChartView final : public QChartView {
public:
    explicit InteractiveChartView(QChart* chart, QWidget* parent = nullptr)
        : QChartView(chart, parent) {
        setRubberBand(QChartView::RectangleRubberBand);
        setRenderHint(QPainter::Antialiasing, true);
    }

protected:
    void wheelEvent(QWheelEvent* event) override {
        if (event->angleDelta().y() > 0) {
            chart()->zoom(1.15);
        } else {
            chart()->zoom(0.87);
        }
        event->accept();
    }

    void mousePressEvent(QMouseEvent* event) override {
        if (event->button() == Qt::RightButton) {
            panning_ = true;
            lastPos_ = event->pos();
            setCursor(Qt::ClosedHandCursor);
            event->accept();
            return;
        }
        QChartView::mousePressEvent(event);
    }

    void mouseMoveEvent(QMouseEvent* event) override {
        if (panning_) {
            const QPoint delta = event->pos() - lastPos_;
            chart()->scroll(-delta.x(), delta.y());
            lastPos_ = event->pos();
            event->accept();
            return;
        }
        QChartView::mouseMoveEvent(event);
    }

    void mouseReleaseEvent(QMouseEvent* event) override {
        if (event->button() == Qt::RightButton && panning_) {
            panning_ = false;
            unsetCursor();
            event->accept();
            return;
        }
        QChartView::mouseReleaseEvent(event);
    }

private:
    bool panning_ = false;
    QPoint lastPos_;
};

QString stamp(const QString& message) {
    return QString("[%1] %2")
    .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss"))
        .arg(message);
}

void markAccent(QPushButton* button) {
    button->setProperty("accent", true);
}

void stylePresetButton(QPushButton* button) {
    button->setObjectName("presetButton");
    button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    button->setMinimumHeight(36);
}

void styleTableHeader(QTableWidget* table) {
    table->horizontalHeader()->setFixedHeight(38);
    table->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    table->verticalHeader()->setDefaultSectionSize(30);
}

QString escapeCsvField(const QString& value) {
    QString escaped = value;
    escaped.replace("\"", "\"\"");
    return "\"" + escaped + "\"";
}

QStringList parseCsvLine(const QString& line) {
    QStringList result;
    QString field;
    bool inQuotes = false;

    for (int i = 0; i < line.size(); ++i) {
        const QChar ch = line.at(i);

        if (ch == '"') {
            if (inQuotes && i + 1 < line.size() && line.at(i + 1) == '"') {
                field += '"';
                ++i;
            } else {
                inQuotes = !inQuotes;
            }
            continue;
        }

        if (ch == ',' && !inQuotes) {
            result << field;
            field.clear();
            continue;
        }

        field += ch;
    }

    result << field;
    return result;
}

QString sanitizeFileComponent(QString text) {
    static const QString invalid = "\\/:*?\"<>| ,";
    for (const QChar ch : invalid) {
        text.replace(ch, '_');
    }
    while (text.contains("__")) {
        text.replace("__", "_");
    }
    return text.trimmed().isEmpty() ? QString("log") : text.trimmed();
}

bool extractNumericReply(const QString& text, double& value) {
    const QStringList lines = text.split(QRegularExpression("[\r\n]+"), Qt::SkipEmptyParts);
    const QRegularExpression rx(R"(([+-]?(?:\d+(?:\.\d*)?|\.\d+)(?:[eE][+-]?\d+)?))");

    for (const QString& rawLine : lines) {
        const QString line = rawLine.trimmed();
        if (line.isEmpty()) {
            continue;
        }

        const QRegularExpressionMatch match = rx.match(line);
        if (!match.hasMatch()) {
            continue;
        }

        bool ok = false;
        const double parsed = match.captured(1).toDouble(&ok);
        if (ok) {
            value = parsed;
            return true;
        }
    }

    return false;
}

bool parseGraphCsvRow(const QStringList& fields, QDateTime& timestamp, double& value) {
    if (fields.size() >= 6) {
        timestamp = QDateTime::fromString(fields.at(0).trimmed(), Qt::ISODateWithMs);
        if (!timestamp.isValid()) {
            timestamp = QDateTime::fromString(fields.at(0).trimmed(), Qt::ISODate);
        }

        bool ok = false;
        value = fields.at(5).trimmed().toDouble(&ok);
        return timestamp.isValid() && ok;
    }

    if (fields.size() >= 2) {
        timestamp = QDateTime::fromString(fields.at(0).trimmed(), Qt::ISODateWithMs);
        if (!timestamp.isValid()) {
            timestamp = QDateTime::fromString(fields.at(0).trimmed(), Qt::ISODate);
        }

        bool ok = false;
        value = fields.at(1).trimmed().toDouble(&ok);
        return timestamp.isValid() && ok;
    }

    return false;
}
}  // namespace

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent) {
    controller_ = new Ar488Controller();
    controller_->moveToThread(&controllerThread_);
    connect(&controllerThread_, &QThread::finished, controller_, &QObject::deleteLater);
    controllerThread_.start();

    datalogTimer_ = new QTimer(this);
    datalogTimer_->setInterval(150);

    graphTimer_ = new QTimer(this);
    graphTimer_->setInterval(150);

    buildUi();
    wireUi();
    refreshPorts();
    refreshEquipmentSelectors();
    refreshDatalogJobsTable();
    updateCommandGroupTitle();
    updateSidebarState();
    updateUiState();

    setWindowTitle("AR488 SCPI Console");
    setMinimumSize(1320, 860);
    statusBar()->showMessage("Ready");
}

MainWindow::~MainWindow() {
    if (controller_) {
        QMetaObject::invokeMethod(controller_, "closePort", Qt::BlockingQueuedConnection);
    }
    controllerThread_.quit();
    controllerThread_.wait();
}

QWidget* MainWindow::buildPresetPage(
    const QString& description,
    const QStringList& commands,
    QWidget* parent
    ) {
    auto* page = new QWidget(parent);
    auto* layout = new QVBoxLayout(page);
    layout->setContentsMargins(8, 8, 8, 8);
    layout->setSpacing(8);

    auto* descriptionLabel = new QLabel(description, page);
    descriptionLabel->setObjectName("pageSubtitle");
    descriptionLabel->setWordWrap(true);
    layout->addWidget(descriptionLabel);

    for (const QString& command : commands) {
        auto* button = new QPushButton(command, page);
        stylePresetButton(button);
        connect(button, &QPushButton::clicked, this, [this, command] {
            insertPresetCommand(command);
        });
        layout->addWidget(button);
    }

    layout->addStretch(1);
    return page;
}

QWidget* MainWindow::buildEquipmentSidebarPage(QWidget* parent) {
    auto* page = new QWidget(parent);
    auto* layout = new QVBoxLayout(page);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(8);

    auto* registryGroup = new QGroupBox("Manual Equipment Registry", page);
    auto* registryLayout = new QVBoxLayout(registryGroup);
    registryLayout->setContentsMargins(10, 16, 10, 10);
    registryLayout->setSpacing(8);

    auto* addressLabel = new QLabel("GPIB Address:", registryGroup);
    equipmentAddressSpin_ = new QSpinBox(registryGroup);
    equipmentAddressSpin_->setRange(kMinGpibAddress, kMaxGpibAddress);
    equipmentAddressSpin_->setValue(kDefaultGpibAddress);

    auto* codeLabel = new QLabel("Equipment Code:", registryGroup);
    equipmentCodeEdit_ = new QLineEdit(registryGroup);
    equipmentCodeEdit_->setPlaceholderText("Example: PS1, DMM_A, SCOPE_MAIN");

    auto* typeLabel = new QLabel("Equipment Type:", registryGroup);
    equipmentTypeCombo_ = new QComboBox(registryGroup);
    equipmentTypeCombo_->addItems({
        "Power Supply",
        "Multimeter",
        "Oscilloscope",
        "Function Generator",
        "Spectrum Analyzer"
    });

    addEquipmentButton_ = new QPushButton("Add / Update Equipment", registryGroup);
    removeEquipmentButton_ = new QPushButton("Remove Selected", registryGroup);
    saveEquipmentCsvButton_ = new QPushButton("Save CSV", registryGroup);
    loadEquipmentCsvButton_ = new QPushButton("Load CSV", registryGroup);

    markAccent(addEquipmentButton_);
    markAccent(saveEquipmentCsvButton_);

    auto* rowButtons = new QHBoxLayout();
    rowButtons->setSpacing(8);
    rowButtons->addWidget(addEquipmentButton_);
    rowButtons->addWidget(removeEquipmentButton_);

    auto* csvButtons = new QHBoxLayout();
    csvButtons->setSpacing(8);
    csvButtons->addWidget(loadEquipmentCsvButton_);
    csvButtons->addWidget(saveEquipmentCsvButton_);

    registryLayout->addWidget(addressLabel);
    registryLayout->addWidget(equipmentAddressSpin_);
    registryLayout->addWidget(codeLabel);
    registryLayout->addWidget(equipmentCodeEdit_);
    registryLayout->addWidget(typeLabel);
    registryLayout->addWidget(equipmentTypeCombo_);
    registryLayout->addLayout(rowButtons);
    registryLayout->addLayout(csvButtons);

    auto* tableGroup = new QGroupBox("Registered Equipment", page);
    auto* tableLayout = new QVBoxLayout(tableGroup);
    tableLayout->setContentsMargins(10, 16, 10, 10);
    tableLayout->setSpacing(8);

    auto* tableHint = new QLabel(
        "Double-click a row to load its GPIB address into the active address field.",
        tableGroup
        );
    tableHint->setObjectName("sectionHint");
    tableHint->setWordWrap(true);

    equipmentTable_ = new QTableWidget(tableGroup);
    equipmentTable_->setColumnCount(3);
    equipmentTable_->setHorizontalHeaderLabels(QStringList() << "Address" << "Equipment Code" << "Type");
    equipmentTable_->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    equipmentTable_->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    equipmentTable_->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
    equipmentTable_->setEditTriggers(QAbstractItemView::NoEditTriggers);
    equipmentTable_->setSelectionBehavior(QAbstractItemView::SelectRows);
    equipmentTable_->setAlternatingRowColors(true);
    styleTableHeader(equipmentTable_);

    tableLayout->addWidget(tableHint);
    tableLayout->addWidget(equipmentTable_, 1);

    layout->addWidget(registryGroup);
    layout->addWidget(tableGroup, 1);

    return page;
}

QWidget* MainWindow::buildSidebar(QWidget* parent) {
    auto* sidebar = new QWidget(parent);
    sidebar->setObjectName("sidebarContainer");

    auto* outerLayout = new QVBoxLayout(sidebar);
    outerLayout->setContentsMargins(8, 8, 8, 8);
    outerLayout->setSpacing(8);

    sidebarToggleButton_ = new QPushButton(sidebar);
    sidebarToggleButton_->setObjectName("sidebarToggle");
    sidebarToggleButton_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    sidebarToggleButton_->setMinimumHeight(38);
    outerLayout->addWidget(sidebarToggleButton_, 0, Qt::AlignTop);

    equipmentSidebarButton_ = new QPushButton(sidebar);
    equipmentSidebarButton_->setObjectName("sidebarNavButton");
    equipmentSidebarButton_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    equipmentSidebarButton_->setMinimumHeight(38);
    outerLayout->addWidget(equipmentSidebarButton_, 0, Qt::AlignTop);

    sidebarContent_ = new QWidget(sidebar);
    sidebarContent_->setObjectName("sidebarContent");

    auto* contentLayout = new QVBoxLayout(sidebarContent_);
    contentLayout->setContentsMargins(0, 0, 0, 0);
    contentLayout->setSpacing(8);

    sidebarPages_ = new QStackedWidget(sidebarContent_);

    commandSidebarPage_ = new QWidget(sidebarPages_);
    {
        auto* layout = new QVBoxLayout(commandSidebarPage_);
        layout->setContentsMargins(0, 0, 0, 0);
        layout->setSpacing(8);

        auto* hint = new QLabel(
            "Command presets stay here. Choose where presets go, then select a command group.",
            commandSidebarPage_
            );
        hint->setObjectName("sectionHint");
        hint->setWordWrap(true);

        presetToCommandRadio_ = new QRadioButton("Insert into SCPI", commandSidebarPage_);
        presetToBatchRadio_ = new QRadioButton("Append to Batch", commandSidebarPage_);
        presetToCommandRadio_->setChecked(true);

        sidebarGroupTitleLabel_ = new QLabel(commandSidebarPage_);
        sidebarGroupTitleLabel_->setObjectName("sidebarGroupHeader");
        sidebarGroupTitleLabel_->setWordWrap(true);

        commandToolBox_ = new QToolBox(commandSidebarPage_);

        commandToolBox_->addItem(
            buildPresetPage(
                "Low-level adapter control and GPIB bus setup.",
                {
                    "++ver",
                    "++ifc",
                    "++clr",
                    "++read eoi",
                    "++read",
                    "++addr ",
                    "++auto 0",
                    "++eoi 1",
                    "++eos 2",
                    "++read_tmo_ms 5000"
                },
                commandToolBox_
                ),
            "AR488 Adapter Commands"
            );

        commandToolBox_->addItem(
            buildPresetPage(
                "Generic IEEE-488.2 and SCPI commands.",
                {
                    "*IDN?",
                    "*CLS",
                    "*RST",
                    "*OPC?",
                    "*TST?"
                },
                commandToolBox_
                ),
            "Common SCPI Commands"
            );

        commandToolBox_->addItem(
            buildPresetPage(
                "Common power supply programming commands.",
                {
                    "VOLT 5",
                    "CURR 1",
                    "OUTP ON",
                    "OUTP OFF",
                    "OUTP?",
                    "VOLT?",
                    "CURR?"
                },
                commandToolBox_
                ),
            "Source / Output Commands"
            );

        commandToolBox_->addItem(
            buildPresetPage(
                "Read measured voltage and current values.",
                {
                    "MEAS:VOLT?",
                    "MEAS:CURR?",
                    "MEAS:VOLT:DC?",
                    "MEAS:CURR:DC?"
                },
                commandToolBox_
                ),
            "Measurement Commands"
            );

        commandToolBox_->addItem(
            buildPresetPage(
                "Error, status, and relay-related queries.",
                {
                    "SYST:ERR?",
                    "STAT:QUES:COND?",
                    "OUTP:RI:MODE?",
                    "OUTP:REL?"
                },
                commandToolBox_
                ),
            "System / Status Commands"
            );

        layout->addWidget(hint);
        layout->addWidget(presetToCommandRadio_);
        layout->addWidget(presetToBatchRadio_);
        layout->addWidget(sidebarGroupTitleLabel_);
        layout->addWidget(commandToolBox_, 1);
    }

    equipmentSidebarPage_ = buildEquipmentSidebarPage(sidebarPages_);

    sidebarPages_->addWidget(commandSidebarPage_);
    sidebarPages_->addWidget(equipmentSidebarPage_);
    sidebarPages_->setCurrentWidget(commandSidebarPage_);

    contentLayout->addWidget(sidebarPages_, 1);

    outerLayout->addWidget(sidebarContent_, 1);
    outerLayout->addStretch(1);

    return sidebar;
}

void MainWindow::buildUi() {
    auto* central = new QWidget(this);
    auto* root = new QHBoxLayout(central);
    root->setContentsMargins(8, 8, 8, 8);
    root->setSpacing(10);

    sidebarContainer_ = buildSidebar(central);

    auto* mainPane = new QWidget(central);
    auto* mainLayout = new QVBoxLayout(mainPane);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(10);

    activeGpibBar_ = new QWidget(mainPane);
    activeGpibBar_->setObjectName("activeGpibBar");
    auto* activeGpibLayout = new QHBoxLayout(activeGpibBar_);
    activeGpibLayout->setContentsMargins(12, 8, 12, 8);
    activeGpibLayout->setSpacing(10);

    activeGpibLabel_ = new QLabel("Active GPIB Address:", activeGpibBar_);
    activeGpibLabel_->setObjectName("activeGpibLabel");

    gpibAddressSpin_ = new QSpinBox(activeGpibBar_);
    gpibAddressSpin_->setRange(kMinGpibAddress, kMaxGpibAddress);
    gpibAddressSpin_->setValue(kDefaultGpibAddress);
    gpibAddressSpin_->setMinimumWidth(100);

    activeGpibLayout->addStretch(1);
    activeGpibLayout->addWidget(activeGpibLabel_);
    activeGpibLayout->addWidget(gpibAddressSpin_);

    mainTabs_ = new QTabWidget(mainPane);
    mainTabs_->setTabPosition(QTabWidget::North);
    mainTabs_->setDocumentMode(true);

    connectionTab_ = new QWidget(mainTabs_);
    {
        auto* layout = new QGridLayout(connectionTab_);
        layout->setContentsMargins(12, 12, 12, 12);
        layout->setHorizontalSpacing(10);
        layout->setVerticalSpacing(10);

        portCombo_ = new QComboBox(connectionTab_);
        baudCombo_ = new QComboBox(connectionTab_);
        refreshButton_ = new QPushButton("Refresh Ports", connectionTab_);
        connectButton_ = new QPushButton("Connect", connectionTab_);
        detectButton_ = new QPushButton("Detect AR488", connectionTab_);
        adapterStatusLabel_ = new QLabel("AR488: not checked", connectionTab_);
        adapterStatusLabel_->setObjectName("sectionHint");

        markAccent(connectButton_);
        markAccent(detectButton_);

        baudCombo_->addItem("9600", 9600);
        baudCombo_->addItem("19200", 19200);
        baudCombo_->addItem("38400", 38400);
        baudCombo_->addItem("57600", 57600);
        baudCombo_->addItem("115200", 115200);
        baudCombo_->setCurrentIndex(4);

        layout->addWidget(new QLabel("Serial Port:"), 0, 0);
        layout->addWidget(portCombo_, 0, 1, 1, 2);
        layout->addWidget(refreshButton_, 1, 0, 1, 3);
        layout->addWidget(new QLabel("Baud Rate:"), 2, 0);
        layout->addWidget(baudCombo_, 2, 1, 1, 2);
        layout->addWidget(connectButton_, 3, 0, 1, 3);
        layout->addWidget(detectButton_, 4, 0, 1, 3);
        layout->addWidget(adapterStatusLabel_, 5, 0, 1, 3);
        layout->setRowStretch(6, 1);
    }
    mainTabs_->addTab(connectionTab_, "Connection");

    scpiTab_ = new QWidget(mainTabs_);
    {
        auto* layout = new QGridLayout(scpiTab_);
        layout->setContentsMargins(12, 12, 12, 12);
        layout->setHorizontalSpacing(10);
        layout->setVerticalSpacing(10);

        commandEdit_ = new QLineEdit(scpiTab_);
        writeButton_ = new QPushButton("Write", scpiTab_);
        queryButton_ = new QPushButton("Query", scpiTab_);
        replyView_ = new QPlainTextEdit(scpiTab_);

        markAccent(writeButton_);
        markAccent(queryButton_);

        commandEdit_->setPlaceholderText("*IDN?");
        replyView_->setObjectName("replyView");
        replyView_->setReadOnly(true);

        layout->addWidget(new QLabel("Command:"), 0, 0);
        layout->addWidget(commandEdit_, 1, 0, 1, 4);
        layout->addWidget(writeButton_, 1, 4);
        layout->addWidget(queryButton_, 1, 5);
        layout->addWidget(new QLabel("Reply:"), 2, 0);
        layout->addWidget(replyView_, 3, 0, 1, 6);
    }
    mainTabs_->addTab(scpiTab_, "SCPI");

    batchTab_ = new QWidget(mainTabs_);
    {
        auto* layout = new QGridLayout(batchTab_);
        layout->setContentsMargins(12, 12, 12, 12);
        layout->setHorizontalSpacing(10);
        layout->setVerticalSpacing(10);

        batchEdit_ = new QPlainTextEdit(batchTab_);
        runBatchButton_ = new QPushButton("Run Batch", batchTab_);
        clearBatchButton_ = new QPushButton("Clear Batch", batchTab_);
        batchResultView_ = new QPlainTextEdit(batchTab_);

        markAccent(runBatchButton_);

        batchEdit_->setObjectName("batchEdit");
        batchResultView_->setObjectName("replyView");
        batchResultView_->setReadOnly(true);

        batchEdit_->setPlaceholderText(
            "# One instruction per line\n"
            "*CLS\n"
            "VOLT 5\n"
            "CURR 1\n"
            "OUTP ON\n"
            "WAIT 500\n"
            "MEAS:VOLT?\n"
            "MEAS:CURR?\n"
            "OUTP OFF\n"
            );

        layout->addWidget(new QLabel("Batch Script:"), 0, 0, 1, 4);
        layout->addWidget(batchEdit_, 1, 0, 1, 6);
        layout->addWidget(runBatchButton_, 2, 4);
        layout->addWidget(clearBatchButton_, 2, 5);
        layout->addWidget(new QLabel("Batch Query Output:"), 3, 0, 1, 4);
        layout->addWidget(batchResultView_, 4, 0, 1, 6);
    }
    mainTabs_->addTab(batchTab_, "Batch SCPI");

    logTab_ = new QWidget(mainTabs_);
    {
        auto* layout = new QVBoxLayout(logTab_);
        layout->setContentsMargins(12, 12, 12, 12);
        layout->setSpacing(10);

        auto* hint = new QLabel("Live communication log.", logTab_);
        hint->setObjectName("sectionHint");

        logView_ = new QPlainTextEdit(logTab_);
        logView_->setObjectName("logView");
        logView_->setReadOnly(true);

        layout->addWidget(hint);
        layout->addWidget(logView_, 1);
    }
    mainTabs_->addTab(logTab_, "Log");

    datalogTab_ = new QWidget(mainTabs_);
    {
        auto* layout = new QVBoxLayout(datalogTab_);
        layout->setContentsMargins(12, 12, 12, 12);
        layout->setSpacing(10);

        auto* configGroup = new QGroupBox("Datalog Job Configuration", datalogTab_);
        auto* configLayout = new QGridLayout(configGroup);
        configLayout->setHorizontalSpacing(10);
        configLayout->setVerticalSpacing(10);

        datalogEquipmentCombo_ = new QComboBox(configGroup);
        datalogQueryEdit_ = new QLineEdit(configGroup);
        datalogModeCombo_ = new QComboBox(configGroup);
        datalogIntervalSpin_ = new QSpinBox(configGroup);
        datalogSamplesSpin_ = new QSpinBox(configGroup);
        datalogStartTimeEdit_ = new QDateTimeEdit(QDateTime::currentDateTime(), configGroup);
        datalogEndTimeEdit_ = new QDateTimeEdit(QDateTime::currentDateTime().addSecs(60), configGroup);
        datalogCsvPathEdit_ = new QLineEdit(configGroup);
        datalogBrowseCsvButton_ = new QPushButton("Browse CSV", configGroup);
        datalogAddJobButton_ = new QPushButton("Add Job", configGroup);
        datalogRemoveJobButton_ = new QPushButton("Remove Selected Job", configGroup);

        markAccent(datalogAddJobButton_);

        datalogQueryEdit_->setPlaceholderText("MEAS:VOLT?");
        datalogModeCombo_->addItems({
            "Continuous",
            "Trigger Based",
            "Computer Time Based"
        });

        datalogIntervalSpin_->setRange(100, 3600000);
        datalogIntervalSpin_->setValue(1000);
        datalogIntervalSpin_->setSuffix(" ms");

        datalogSamplesSpin_->setRange(0, 1000000);
        datalogSamplesSpin_->setValue(0);
        datalogSamplesSpin_->setSpecialValueText("Unlimited");

        datalogStartTimeEdit_->setDisplayFormat("yyyy-MM-dd HH:mm:ss");
        datalogEndTimeEdit_->setDisplayFormat("yyyy-MM-dd HH:mm:ss");
        datalogStartTimeEdit_->setCalendarPopup(true);
        datalogEndTimeEdit_->setCalendarPopup(true);

        configLayout->addWidget(new QLabel("Equipment:"), 0, 0);
        configLayout->addWidget(datalogEquipmentCombo_, 0, 1, 1, 3);
        configLayout->addWidget(new QLabel("Read Query:"), 1, 0);
        configLayout->addWidget(datalogQueryEdit_, 1, 1, 1, 3);
        configLayout->addWidget(new QLabel("Mode:"), 2, 0);
        configLayout->addWidget(datalogModeCombo_, 2, 1);
        configLayout->addWidget(new QLabel("Interval:"), 2, 2);
        configLayout->addWidget(datalogIntervalSpin_, 2, 3);
        configLayout->addWidget(new QLabel("Max Samples:"), 3, 0);
        configLayout->addWidget(datalogSamplesSpin_, 3, 1);
        configLayout->addWidget(new QLabel("Start Time:"), 3, 2);
        configLayout->addWidget(datalogStartTimeEdit_, 3, 3);
        configLayout->addWidget(new QLabel("End Time:"), 4, 0);
        configLayout->addWidget(datalogEndTimeEdit_, 4, 1, 1, 3);
        configLayout->addWidget(new QLabel("CSV File:"), 5, 0);
        configLayout->addWidget(datalogCsvPathEdit_, 5, 1, 1, 2);
        configLayout->addWidget(datalogBrowseCsvButton_, 5, 3);
        configLayout->addWidget(datalogAddJobButton_, 6, 2);
        configLayout->addWidget(datalogRemoveJobButton_, 6, 3);

        auto* jobsGroup = new QGroupBox("Configured Datalog Jobs", datalogTab_);
        auto* jobsLayout = new QVBoxLayout(jobsGroup);
        jobsLayout->setContentsMargins(10, 16, 10, 10);
        jobsLayout->setSpacing(8);

        datalogStatusLabel_ = new QLabel("Datalog idle.", jobsGroup);
        datalogStatusLabel_->setObjectName("sectionHint");
        datalogStatusLabel_->setWordWrap(true);

        auto* runButtons = new QHBoxLayout();
        runButtons->setSpacing(8);

        datalogStartButton_ = new QPushButton("Start Logging", jobsGroup);
        datalogStopButton_ = new QPushButton("Stop", jobsGroup);
        datalogTriggerButton_ = new QPushButton("Trigger All", jobsGroup);

        markAccent(datalogStartButton_);

        runButtons->addWidget(datalogStartButton_);
        runButtons->addWidget(datalogStopButton_);
        runButtons->addWidget(datalogTriggerButton_);
        runButtons->addStretch(1);

        datalogJobsTable_ = new QTableWidget(jobsGroup);
        datalogJobsTable_->setColumnCount(10);
        datalogJobsTable_->setHorizontalHeaderLabels(QStringList()
                                                     << "Code"
                                                     << "Address"
                                                     << "Type"
                                                     << "Query"
                                                     << "Mode"
                                                     << "Interval"
                                                     << "Max Samples"
                                                     << "CSV"
                                                     << "Samples"
                                                     << "Status");
        datalogJobsTable_->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
        datalogJobsTable_->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
        datalogJobsTable_->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
        datalogJobsTable_->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
        datalogJobsTable_->horizontalHeader()->setSectionResizeMode(4, QHeaderView::ResizeToContents);
        datalogJobsTable_->horizontalHeader()->setSectionResizeMode(5, QHeaderView::ResizeToContents);
        datalogJobsTable_->horizontalHeader()->setSectionResizeMode(6, QHeaderView::ResizeToContents);
        datalogJobsTable_->horizontalHeader()->setSectionResizeMode(7, QHeaderView::Stretch);
        datalogJobsTable_->horizontalHeader()->setSectionResizeMode(8, QHeaderView::ResizeToContents);
        datalogJobsTable_->horizontalHeader()->setSectionResizeMode(9, QHeaderView::ResizeToContents);
        datalogJobsTable_->setEditTriggers(QAbstractItemView::NoEditTriggers);
        datalogJobsTable_->setSelectionBehavior(QAbstractItemView::SelectRows);
        datalogJobsTable_->setAlternatingRowColors(true);
        styleTableHeader(datalogJobsTable_);

        jobsLayout->addWidget(datalogStatusLabel_);
        jobsLayout->addLayout(runButtons);
        jobsLayout->addWidget(datalogJobsTable_, 1);

        layout->addWidget(configGroup);
        layout->addWidget(jobsGroup, 1);
    }
    mainTabs_->addTab(datalogTab_, "Datalog");

    graphTab_ = new QWidget(mainTabs_);
    {
        auto* layout = new QVBoxLayout(graphTab_);
        layout->setContentsMargins(12, 12, 12, 12);
        layout->setSpacing(10);

        auto* controlsGroup = new QGroupBox("Graph Controls", graphTab_);
        auto* controlsLayout = new QGridLayout(controlsGroup);
        controlsLayout->setHorizontalSpacing(10);
        controlsLayout->setVerticalSpacing(10);

        graphEquipmentCombo_ = new QComboBox(controlsGroup);
        graphQueryEdit_ = new QLineEdit(controlsGroup);
        graphIntervalSpin_ = new QSpinBox(controlsGroup);
        graphStartLiveButton_ = new QPushButton("Start Live", controlsGroup);
        graphStopLiveButton_ = new QPushButton("Stop Live", controlsGroup);
        graphLoadCsvButton_ = new QPushButton("Load CSV", controlsGroup);
        graphClearButton_ = new QPushButton("Clear Graph", controlsGroup);
        graphResetViewButton_ = new QPushButton("Reset View", controlsGroup);
        graphStatusLabel_ = new QLabel("Graph idle.", controlsGroup);

        markAccent(graphStartLiveButton_);
        markAccent(graphLoadCsvButton_);

        graphQueryEdit_->setPlaceholderText("MEAS:VOLT?");
        graphIntervalSpin_->setRange(100, 3600000);
        graphIntervalSpin_->setValue(1000);
        graphIntervalSpin_->setSuffix(" ms");

        controlsLayout->addWidget(new QLabel("Equipment:"), 0, 0);
        controlsLayout->addWidget(graphEquipmentCombo_, 0, 1, 1, 3);
        controlsLayout->addWidget(new QLabel("Read Query:"), 1, 0);
        controlsLayout->addWidget(graphQueryEdit_, 1, 1, 1, 3);
        controlsLayout->addWidget(new QLabel("Interval:"), 2, 0);
        controlsLayout->addWidget(graphIntervalSpin_, 2, 1);
        controlsLayout->addWidget(graphStartLiveButton_, 2, 2);
        controlsLayout->addWidget(graphStopLiveButton_, 2, 3);
        controlsLayout->addWidget(graphLoadCsvButton_, 3, 1);
        controlsLayout->addWidget(graphClearButton_, 3, 2);
        controlsLayout->addWidget(graphResetViewButton_, 3, 3);
        controlsLayout->addWidget(graphStatusLabel_, 4, 0, 1, 4);

        auto* chartGroup = new QGroupBox("Measurement Graph", graphTab_);
        auto* chartLayout = new QVBoxLayout(chartGroup);
        chartLayout->setContentsMargins(10, 16, 10, 10);
        chartLayout->setSpacing(8);

        graphSeries_ = new QLineSeries(chartGroup);
        graphChart_ = new QChart();
        graphChart_->legend()->hide();
        graphChart_->addSeries(graphSeries_);
        graphChart_->setTitle("Measurement Graph");

        graphAxisX_ = new QDateTimeAxis(graphChart_);
        graphAxisX_->setFormat("HH:mm:ss");
        graphAxisX_->setTitleText("Time");

        graphAxisY_ = new QValueAxis(graphChart_);
        graphAxisY_->setTitleText("Value");

        graphChart_->addAxis(graphAxisX_, Qt::AlignBottom);
        graphChart_->addAxis(graphAxisY_, Qt::AlignLeft);
        graphSeries_->attachAxis(graphAxisX_);
        graphSeries_->attachAxis(graphAxisY_);

        graphChartView_ = new InteractiveChartView(graphChart_, chartGroup);
        graphChartView_->setMinimumHeight(420);

        chartLayout->addWidget(graphChartView_, 1);

        layout->addWidget(controlsGroup);
        layout->addWidget(chartGroup, 1);
    }
    mainTabs_->addTab(graphTab_, "Graph");

    mainLayout->addWidget(activeGpibBar_, 0);
    mainLayout->addWidget(mainTabs_, 1);

    root->addWidget(sidebarContainer_, 0);
    root->addWidget(mainPane, 1);
    root->setStretch(0, 0);
    root->setStretch(1, 1);

    setCentralWidget(central);
    updateGraphAxes();
}

void MainWindow::wireUi() {
    connect(sidebarToggleButton_, &QPushButton::clicked, this, &MainWindow::toggleSidebar);

    connect(equipmentSidebarButton_, &QPushButton::clicked, this, [this] {
        if (!sidebarExpanded_) {
            sidebarExpanded_ = true;
        }
        sidebarPages_->setCurrentWidget(equipmentSidebarPage_);
        updateSidebarState();
    });

    connect(commandToolBox_, &QToolBox::currentChanged, this, [this](int) {
        updateCommandGroupTitle();
    });

    connect(refreshButton_, &QPushButton::clicked, this, &MainWindow::refreshPorts);
    connect(connectButton_, &QPushButton::clicked, this, &MainWindow::onConnectClicked);
    connect(detectButton_, &QPushButton::clicked, this, [this] { emit requestDetectAdapter(); });

    connect(writeButton_, &QPushButton::clicked, this, &MainWindow::onWriteClicked);
    connect(queryButton_, &QPushButton::clicked, this, &MainWindow::onQueryClicked);
    connect(commandEdit_, &QLineEdit::returnPressed, this, &MainWindow::onQueryClicked);

    connect(runBatchButton_, &QPushButton::clicked, this, &MainWindow::onBatchClicked);
    connect(clearBatchButton_, &QPushButton::clicked, batchEdit_, &QPlainTextEdit::clear);

    connect(addEquipmentButton_, &QPushButton::clicked, this, &MainWindow::onAddEquipmentClicked);
    connect(removeEquipmentButton_, &QPushButton::clicked, this, &MainWindow::onRemoveEquipmentClicked);
    connect(saveEquipmentCsvButton_, &QPushButton::clicked, this, &MainWindow::onSaveEquipmentCsvClicked);
    connect(loadEquipmentCsvButton_, &QPushButton::clicked, this, &MainWindow::onLoadEquipmentCsvClicked);
    connect(equipmentTable_, &QTableWidget::cellDoubleClicked, this, &MainWindow::onEquipmentActivated);

    connect(datalogBrowseCsvButton_, &QPushButton::clicked, this, &MainWindow::onBrowseDatalogCsvClicked);
    connect(datalogAddJobButton_, &QPushButton::clicked, this, &MainWindow::onAddDatalogJobClicked);
    connect(datalogRemoveJobButton_, &QPushButton::clicked, this, &MainWindow::onRemoveDatalogJobClicked);
    connect(datalogStartButton_, &QPushButton::clicked, this, &MainWindow::onStartDatalogClicked);
    connect(datalogStopButton_, &QPushButton::clicked, this, &MainWindow::onStopDatalogClicked);
    connect(datalogTriggerButton_, &QPushButton::clicked, this, &MainWindow::onTriggerDatalogClicked);
    connect(datalogTimer_, &QTimer::timeout, this, &MainWindow::onDatalogTimer);

    connect(graphStartLiveButton_, &QPushButton::clicked, this, &MainWindow::onStartGraphLiveClicked);
    connect(graphStopLiveButton_, &QPushButton::clicked, this, &MainWindow::onStopGraphLiveClicked);
    connect(graphLoadCsvButton_, &QPushButton::clicked, this, &MainWindow::onLoadGraphCsvClicked);
    connect(graphClearButton_, &QPushButton::clicked, this, &MainWindow::onClearGraphClicked);
    connect(graphResetViewButton_, &QPushButton::clicked, this, &MainWindow::onResetGraphViewClicked);
    connect(graphTimer_, &QTimer::timeout, this, &MainWindow::onGraphTimer);

    connect(this, &MainWindow::requestOpenPort, controller_, &Ar488Controller::openPort, Qt::QueuedConnection);
    connect(this, &MainWindow::requestClosePort, controller_, &Ar488Controller::closePort, Qt::QueuedConnection);
    connect(this, &MainWindow::requestDetectAdapter, controller_, &Ar488Controller::detectAdapter, Qt::QueuedConnection);
    connect(this, &MainWindow::requestWriteScpi, controller_, &Ar488Controller::writeScpi, Qt::QueuedConnection);
    connect(this, &MainWindow::requestQueryScpi, controller_, &Ar488Controller::queryScpi, Qt::QueuedConnection);
    connect(this, &MainWindow::requestRunBatch, controller_, &Ar488Controller::runBatch, Qt::QueuedConnection);

    connect(controller_, &Ar488Controller::logMessage, this, &MainWindow::appendLog);
    connect(controller_, &Ar488Controller::connectionChanged, this, &MainWindow::onConnectionChanged);
    connect(controller_, &Ar488Controller::adapterDetected, this, &MainWindow::onAdapterDetected);
    connect(controller_, &Ar488Controller::queryCompleted, this, &MainWindow::onQueryCompleted);
    connect(controller_, &Ar488Controller::batchLineResult, this, &MainWindow::onBatchLineResult);
    connect(controller_, &Ar488Controller::busyChanged, this, &MainWindow::onBusyChanged);

    connect(controller_, &Ar488Controller::errorOccurred, this, [this](const QString& message) {
        appendLog("ERROR: " + message);
        statusBar()->showMessage(message, 6000);

        if (graphWaitingForReply_) {
            graphWaitingForReply_ = false;
            if (graphLiveRunning_) {
                setGraphStatus("Graph read error; waiting for next point.");
            }
        }

        if (datalogWaitingForReply_) {
            datalogWaitingForReply_ = false;
            datalogPendingJobIndex_ = -1;
            finalizeDatalogRunIfNeeded();
        }

        mainTabs_->setCurrentWidget(logTab_);
    });

    connect(controller_, &Ar488Controller::batchStarted, this, [this] {
        batchResultView_->clear();
        appendLog("Batch started.");
        statusBar()->showMessage("Batch started", 3000);
        mainTabs_->setCurrentWidget(batchTab_);
    });

    connect(controller_, &Ar488Controller::batchFinished, this, [this] {
        appendLog("Batch finished.");
        statusBar()->showMessage("Batch finished", 4000);
    });
}

void MainWindow::refreshPorts() {
    const QString previousPort = currentPortName();

    portCombo_->clear();
    const auto ports = QSerialPortInfo::availablePorts();
    for (const QSerialPortInfo& info : ports) {
        QString label = info.portName();
        if (!info.description().isEmpty()) {
            label += " — " + info.description();
        }
        portCombo_->addItem(label, info.portName());
    }

    int index = portCombo_->findData(previousPort);
    if (index >= 0) {
        portCombo_->setCurrentIndex(index);
    } else if (portCombo_->count() > 0) {
        portCombo_->setCurrentIndex(0);
    }

    appendLog(QString("Ports refreshed: %1 found").arg(portCombo_->count()));
    updateUiState();
}

void MainWindow::onConnectClicked() {
    if (connected_) {
        emit requestClosePort();
        return;
    }

    if (currentPortName().isEmpty()) {
        appendLog("No serial port selected.");
        return;
    }

    emit requestOpenPort(currentPortName(), currentBaudRate(), gpibAddressSpin_->value());
}

void MainWindow::onConnectionChanged(bool connected, const QString& message) {
    connected_ = connected;
    appendLog(message);
    statusBar()->showMessage(message, 4000);

    if (!connected_) {
        adapterStatusLabel_->setText("AR488: not connected");
        graphLiveRunning_ = false;
        graphWaitingForReply_ = false;
        graphTimer_->stop();
        setGraphStatus("Graph stopped: disconnected.");
    }

    updateUiState();
}

void MainWindow::onAdapterDetected(bool ok, const QString& text) {
    Q_UNUSED(ok);
    adapterStatusLabel_->setText("AR488: " + text);
    appendLog(ok ? ("AR488 detected: " + text) : ("AR488 detection failed: " + text));
}

void MainWindow::onWriteClicked() {
    const QString command = commandEdit_->text().trimmed();
    if (command.isEmpty()) {
        return;
    }
    emit requestWriteScpi(gpibAddressSpin_->value(), command);
}

void MainWindow::onQueryClicked() {
    const QString command = commandEdit_->text().trimmed();
    if (command.isEmpty()) {
        return;
    }
    emit requestQueryScpi(gpibAddressSpin_->value(), command);
}

void MainWindow::onBatchClicked() {
    const QString script = batchEdit_->toPlainText();
    if (script.trimmed().isEmpty()) {
        appendLog("Batch is empty.");
        return;
    }

    emit requestRunBatch(gpibAddressSpin_->value(), script);
}

void MainWindow::onQueryCompleted(const QString&, const QString& reply) {
    if (datalogWaitingForReply_ && datalogPendingJobIndex_ >= 0 && datalogPendingJobIndex_ < datalogJobs_.size()) {
        DataLogJob& job = datalogJobs_[datalogPendingJobIndex_];
        const QDateTime now = QDateTime::currentDateTime();

        appendDatalogCsvRow(job, reply, now);
        ++job.samplesWritten;

        if (job.mode == "Trigger Based") {
            job.triggerPending = false;
        } else {
            job.nextDueTime = now.addMSecs(job.intervalMs);
        }

        if (job.maxSamples > 0 && job.samplesWritten >= job.maxSamples) {
            job.finished = true;
        }

        if (job.mode == "Computer Time Based" && job.endTime.isValid() && now >= job.endTime) {
            job.finished = true;
        }

        datalogWaitingForReply_ = false;
        datalogPendingJobIndex_ = -1;

        refreshDatalogJobsTable();
        finalizeDatalogRunIfNeeded();
        return;
    }

    if (graphWaitingForReply_) {
        graphWaitingForReply_ = false;

        double value = 0.0;
        if (extractNumericReply(reply, value)) {
            const qint64 x = QDateTime::currentDateTime().toMSecsSinceEpoch();
            graphSeries_->append(static_cast<qreal>(x), value);
            graphChart_->setTitle(QString("Live Graph — %1 [%2] — %3")
                                      .arg(graphEquipmentCombo_->currentText())
                                      .arg(graphPendingAddress_)
                                      .arg(graphPendingQuery_));
            updateGraphAxes();
            setGraphStatus(QString("Live point added: %1").arg(value));
        } else {
            appendLog("Graph live query returned non-numeric data: " + reply);
            setGraphStatus("Live reply was non-numeric.");
        }

        return;
    }

    replyView_->setPlainText(reply);
    mainTabs_->setCurrentWidget(scpiTab_);
}

void MainWindow::onBatchLineResult(const QString& command, const QString& reply) {
    batchResultView_->appendPlainText(QString("%1 => %2").arg(command, reply.isEmpty() ? "<empty>" : reply));
}

void MainWindow::onAddEquipmentClicked() {
    const int address = equipmentAddressSpin_->value();
    const QString code = equipmentCodeEdit_->text().trimmed();
    const QString type = equipmentTypeCombo_->currentText();

    if (code.isEmpty()) {
        appendLog("Equipment code is empty.");
        return;
    }

    int targetRow = -1;
    for (int row = 0; row < equipmentTable_->rowCount(); ++row) {
        auto* addressItem = equipmentTable_->item(row, 0);
        if (addressItem && addressItem->text().toInt() == address) {
            targetRow = row;
            break;
        }
    }

    if (targetRow < 0) {
        targetRow = equipmentTable_->rowCount();
        equipmentTable_->insertRow(targetRow);
    }

    equipmentTable_->setItem(targetRow, 0, new QTableWidgetItem(QString::number(address)));
    equipmentTable_->setItem(targetRow, 1, new QTableWidgetItem(code));
    equipmentTable_->setItem(targetRow, 2, new QTableWidgetItem(type));

    gpibAddressSpin_->setValue(address);
    refreshEquipmentSelectors();
    appendLog(QString("Equipment saved: addr=%1, code=%2, type=%3").arg(address).arg(code).arg(type));
    updateUiState();
}

void MainWindow::onRemoveEquipmentClicked() {
    const int row = equipmentTable_->currentRow();
    if (row < 0) {
        appendLog("No equipment row selected.");
        return;
    }

    const QString address = equipmentTable_->item(row, 0) ? equipmentTable_->item(row, 0)->text() : QString("?");
    const QString code = equipmentTable_->item(row, 1) ? equipmentTable_->item(row, 1)->text() : QString("?");

    equipmentTable_->removeRow(row);
    refreshEquipmentSelectors();
    appendLog(QString("Equipment removed: addr=%1, code=%2").arg(address).arg(code));
    updateUiState();
}

void MainWindow::onSaveEquipmentCsvClicked() {
    const QString path = QFileDialog::getSaveFileName(
        this,
        "Save Equipment CSV",
        "equipment_registry.csv",
        "CSV Files (*.csv)"
        );

    if (path.isEmpty()) {
        return;
    }

    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        appendLog("Failed to open CSV for writing: " + path);
        return;
    }

    QTextStream out(&file);
    out.setEncoding(QStringConverter::Utf8);

    out << "\"Address\",\"Equipment Code\",\"Type\"\n";

    for (int row = 0; row < equipmentTable_->rowCount(); ++row) {
        const QString address = equipmentTable_->item(row, 0) ? equipmentTable_->item(row, 0)->text() : QString();
        const QString code = equipmentTable_->item(row, 1) ? equipmentTable_->item(row, 1)->text() : QString();
        const QString type = equipmentTable_->item(row, 2) ? equipmentTable_->item(row, 2)->text() : QString();

        out << escapeCsvField(address) << ','
            << escapeCsvField(code) << ','
            << escapeCsvField(type) << '\n';
    }

    file.close();
    appendLog("Equipment registry saved to CSV: " + path);
}

void MainWindow::onLoadEquipmentCsvClicked() {
    const QString path = QFileDialog::getOpenFileName(
        this,
        "Load Equipment CSV",
        QString(),
        "CSV Files (*.csv)"
        );

    if (path.isEmpty()) {
        return;
    }

    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        appendLog("Failed to open CSV for reading: " + path);
        return;
    }

    QTextStream in(&file);
    in.setEncoding(QStringConverter::Utf8);

    QList<QStringList> rows;
    bool firstLine = true;

    while (!in.atEnd()) {
        const QString line = in.readLine();
        if (line.trimmed().isEmpty()) {
            continue;
        }

        const QStringList fields = parseCsvLine(line);

        if (firstLine) {
            firstLine = false;
            if (fields.size() >= 3 &&
                fields.at(0).trimmed().compare("Address", Qt::CaseInsensitive) == 0) {
                continue;
            }
        }

        if (fields.size() < 3) {
            appendLog("Skipping invalid CSV row: " + line);
            continue;
        }

        bool ok = false;
        const int address = fields.at(0).trimmed().toInt(&ok);
        if (!ok || address < kMinGpibAddress || address > kMaxGpibAddress) {
            appendLog("Skipping invalid equipment address in CSV: " + fields.at(0));
            continue;
        }

        rows.append({
            QString::number(address),
            fields.at(1).trimmed(),
            fields.at(2).trimmed()
        });
    }

    file.close();

    equipmentTable_->setRowCount(0);

    for (const QStringList& rowData : rows) {
        const int row = equipmentTable_->rowCount();
        equipmentTable_->insertRow(row);
        equipmentTable_->setItem(row, 0, new QTableWidgetItem(rowData.at(0)));
        equipmentTable_->setItem(row, 1, new QTableWidgetItem(rowData.at(1)));
        equipmentTable_->setItem(row, 2, new QTableWidgetItem(rowData.at(2)));
    }

    if (equipmentTable_->rowCount() > 0) {
        bool ok = false;
        const int firstAddress = equipmentTable_->item(0, 0)->text().toInt(&ok);
        if (ok) {
            gpibAddressSpin_->setValue(firstAddress);
            equipmentAddressSpin_->setValue(firstAddress);
        }
    }

    refreshEquipmentSelectors();
    updateUiState();
    appendLog(QString("Equipment registry loaded from CSV: %1 (%2 rows)").arg(path).arg(equipmentTable_->rowCount()));
}

void MainWindow::onEquipmentActivated(int row, int) {
    auto* addressItem = equipmentTable_->item(row, 0);
    auto* codeItem = equipmentTable_->item(row, 1);
    auto* typeItem = equipmentTable_->item(row, 2);

    if (!addressItem) {
        return;
    }

    bool ok = false;
    const int address = addressItem->text().toInt(&ok);
    if (!ok) {
        return;
    }

    gpibAddressSpin_->setValue(address);
    equipmentAddressSpin_->setValue(address);

    if (codeItem) {
        equipmentCodeEdit_->setText(codeItem->text());
    }

    if (typeItem) {
        const int typeIndex = equipmentTypeCombo_->findText(typeItem->text());
        if (typeIndex >= 0) {
            equipmentTypeCombo_->setCurrentIndex(typeIndex);
        }
    }

    appendLog(QString("Selected equipment at GPIB address %1").arg(address));
    mainTabs_->setCurrentWidget(scpiTab_);
}

void MainWindow::onAddDatalogJobClicked() {
    if (datalogEquipmentCombo_->count() == 0) {
        appendLog("No registered equipment available for datalog.");
        return;
    }

    const int comboIndex = datalogEquipmentCombo_->currentIndex();
    const int address = datalogEquipmentCombo_->currentData(Qt::UserRole).toInt();
    const QString code = datalogEquipmentCombo_->itemData(comboIndex, Qt::UserRole + 1).toString();
    const QString type = datalogEquipmentCombo_->itemData(comboIndex, Qt::UserRole + 2).toString();
    const QString query = datalogQueryEdit_->text().trimmed();
    const QString mode = datalogModeCombo_->currentText();
    const int intervalMs = datalogIntervalSpin_->value();
    const int maxSamples = datalogSamplesSpin_->value();
    const QDateTime startTime = datalogStartTimeEdit_->dateTime();
    const QDateTime endTime = datalogEndTimeEdit_->dateTime();

    if (query.isEmpty()) {
        appendLog("Datalog query is empty.");
        return;
    }

    if (mode == "Computer Time Based" && endTime <= startTime) {
        appendLog("Datalog end time must be greater than start time.");
        return;
    }

    DataLogJob job;
    job.address = address;
    job.equipmentCode = code;
    job.equipmentType = type;
    job.query = query;
    job.mode = mode;
    job.intervalMs = intervalMs;
    job.maxSamples = maxSamples;
    job.startTime = startTime;
    job.endTime = endTime;

    const QString csvPath = datalogCsvPathEdit_->text().trimmed();
    job.csvPath = csvPath.isEmpty() ? defaultDatalogCsvPath(code, query) : csvPath;

    datalogJobs_.append(job);
    refreshDatalogJobsTable();
    updateUiState();

    appendLog(QString("Datalog job added: %1 [%2] %3").arg(code).arg(address).arg(query));
}

void MainWindow::onRemoveDatalogJobClicked() {
    const int row = datalogJobsTable_->currentRow();
    if (row < 0 || row >= datalogJobs_.size()) {
        appendLog("No datalog job selected.");
        return;
    }

    appendLog(QString("Datalog job removed: %1 [%2]")
                  .arg(datalogJobs_.at(row).equipmentCode)
                  .arg(datalogJobs_.at(row).address));

    datalogJobs_.removeAt(row);
    refreshDatalogJobsTable();
    updateUiState();
}

void MainWindow::onBrowseDatalogCsvClicked() {
    const QString suggested = datalogCsvPathEdit_->text().trimmed().isEmpty()
    ? defaultDatalogCsvPath("log", "reading")
    : datalogCsvPathEdit_->text().trimmed();

    const QString path = QFileDialog::getSaveFileName(
        this,
        "Choose Datalog CSV",
        suggested,
        "CSV Files (*.csv)"
        );

    if (!path.isEmpty()) {
        datalogCsvPathEdit_->setText(path);
    }
}

void MainWindow::onStartDatalogClicked() {
    if (datalogJobs_.isEmpty()) {
        appendLog("No datalog jobs configured.");
        return;
    }

    if (!connected_) {
        appendLog("Connect to AR488 before starting datalog.");
        return;
    }

    const QDateTime now = QDateTime::currentDateTime();

    for (DataLogJob& job : datalogJobs_) {
        job.samplesWritten = 0;
        job.finished = false;
        job.stopped = false;
        job.triggerPending = false;

        if (job.mode == "Continuous") {
            job.nextDueTime = now;
        } else if (job.mode == "Computer Time Based") {
            job.nextDueTime = job.startTime;
        } else {
            job.nextDueTime = QDateTime();
        }

        QFile file(job.csvPath);
        if (!file.exists()) {
            if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
                QTextStream out(&file);
                out.setEncoding(QStringConverter::Utf8);
                out << "\"Timestamp\",\"Address\",\"Equipment Code\",\"Type\",\"Query\",\"Reading\"\n";
                file.close();
            }
        } else if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            const bool empty = file.size() == 0;
            file.close();
            if (empty && file.open(QIODevice::WriteOnly | QIODevice::Text)) {
                QTextStream out(&file);
                out.setEncoding(QStringConverter::Utf8);
                out << "\"Timestamp\",\"Address\",\"Equipment Code\",\"Type\",\"Query\",\"Reading\"\n";
                file.close();
            }
        }
    }

    datalogRunning_ = true;
    datalogStopRequested_ = false;
    datalogWaitingForReply_ = false;
    datalogPendingJobIndex_ = -1;
    datalogRoundRobinCursor_ = 0;

    datalogTimer_->start();
    refreshDatalogJobsTable();
    updateUiState();

    datalogStatusLabel_->setText("Datalog running.");
    appendLog("Datalog started.");
    mainTabs_->setCurrentWidget(datalogTab_);
}

void MainWindow::onStopDatalogClicked() {
    if (!datalogRunning_ && !datalogStopRequested_) {
        return;
    }

    datalogStopRequested_ = true;
    datalogStatusLabel_->setText("Stopping datalog...");
    appendLog("Datalog stop requested.");
    finalizeDatalogRunIfNeeded();
}

void MainWindow::onTriggerDatalogClicked() {
    if (!datalogRunning_) {
        appendLog("Start datalog before sending triggers.");
        return;
    }

    bool any = false;
    for (DataLogJob& job : datalogJobs_) {
        if (!job.finished && !job.stopped && job.mode == "Trigger Based") {
            job.triggerPending = true;
            any = true;
        }
    }

    if (!any) {
        appendLog("No trigger-based datalog jobs are active.");
        return;
    }

    refreshDatalogJobsTable();
    appendLog("Trigger requested for trigger-based datalog jobs.");
}

void MainWindow::onDatalogTimer() {
    if (!datalogRunning_ || datalogWaitingForReply_ || busy_) {
        return;
    }

    const QDateTime now = QDateTime::currentDateTime();

    for (DataLogJob& job : datalogJobs_) {
        if (job.finished || job.stopped) {
            continue;
        }

        if (job.maxSamples > 0 && job.samplesWritten >= job.maxSamples) {
            job.finished = true;
        }

        if (job.mode == "Computer Time Based" && job.endTime.isValid() && now > job.endTime) {
            job.finished = true;
        }
    }

    int selectedIndex = -1;
    const int jobCount = static_cast<int>(datalogJobs_.size());

    for (int offset = 0; offset < jobCount; ++offset) {
        const int index = (datalogRoundRobinCursor_ + offset) % (jobCount > 0 ? jobCount : 1);
        const DataLogJob& job = datalogJobs_.at(index);

        if (job.finished || job.stopped) {
            continue;
        }

        if (job.mode == "Continuous") {
            if (!job.nextDueTime.isValid() || now >= job.nextDueTime) {
                selectedIndex = index;
                break;
            }
        } else if (job.mode == "Computer Time Based") {
            if (now < job.startTime) {
                continue;
            }
            if (job.endTime.isValid() && now > job.endTime) {
                continue;
            }
            if (!job.nextDueTime.isValid() || now >= job.nextDueTime) {
                selectedIndex = index;
                break;
            }
        } else if (job.mode == "Trigger Based") {
            if (job.triggerPending) {
                selectedIndex = index;
                break;
            }
        }
    }

    if (selectedIndex < 0) {
        refreshDatalogJobsTable();
        finalizeDatalogRunIfNeeded();
        return;
    }

    datalogPendingJobIndex_ = selectedIndex;
    datalogWaitingForReply_ = true;
    datalogRoundRobinCursor_ = (selectedIndex + 1) % (jobCount > 0 ? jobCount : 1);

    const DataLogJob& job = datalogJobs_.at(selectedIndex);
    datalogStatusLabel_->setText(QString("Reading %1 [%2] ...").arg(job.equipmentCode).arg(job.address));
    emit requestQueryScpi(job.address, job.query);
}

void MainWindow::onStartGraphLiveClicked() {
    if (!connected_) {
        appendLog("Connect to AR488 before starting live graph.");
        return;
    }

    if (graphEquipmentCombo_->count() == 0) {
        appendLog("No registered equipment available for graph.");
        return;
    }

    const QString query = graphQueryEdit_->text().trimmed();
    if (query.isEmpty()) {
        appendLog("Graph query is empty.");
        return;
    }

    graphSeries_->clear();
    updateGraphAxes();

    graphLiveRunning_ = true;
    graphWaitingForReply_ = false;
    graphPendingAddress_ = graphEquipmentCombo_->currentData(Qt::UserRole).toInt();
    graphPendingQuery_ = query;
    graphTimer_->start();

    graphChart_->setTitle(QString("Live Graph — %1 [%2] — %3")
                              .arg(graphEquipmentCombo_->currentText())
                              .arg(graphPendingAddress_)
                              .arg(query));

    setGraphStatus("Live graph started.");
    mainTabs_->setCurrentWidget(graphTab_);
    updateUiState();
}

void MainWindow::onStopGraphLiveClicked() {
    graphLiveRunning_ = false;
    graphTimer_->stop();
    setGraphStatus("Live graph stopped.");
    updateUiState();
}

void MainWindow::onLoadGraphCsvClicked() {
    const QString path = QFileDialog::getOpenFileName(
        this,
        "Load Graph CSV",
        QString(),
        "CSV Files (*.csv)"
        );

    if (path.isEmpty()) {
        return;
    }

    graphLiveRunning_ = false;
    graphWaitingForReply_ = false;
    graphTimer_->stop();

    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        appendLog("Failed to open graph CSV: " + path);
        setGraphStatus("Failed to open CSV.");
        updateUiState();
        return;
    }

    QTextStream in(&file);
    in.setEncoding(QStringConverter::Utf8);

    graphSeries_->clear();

    int pointsLoaded = 0;
    bool firstLine = true;

    while (!in.atEnd()) {
        const QString line = in.readLine();
        if (line.trimmed().isEmpty()) {
            continue;
        }

        const QStringList fields = parseCsvLine(line);

        if (firstLine) {
            firstLine = false;
            if (!fields.isEmpty() &&
                fields.at(0).trimmed().compare("Timestamp", Qt::CaseInsensitive) == 0) {
                continue;
            }
        }

        QDateTime timestamp;
        double value = 0.0;
        if (!parseGraphCsvRow(fields, timestamp, value)) {
            continue;
        }

        graphSeries_->append(static_cast<qreal>(timestamp.toMSecsSinceEpoch()), value);
        ++pointsLoaded;
    }

    file.close();
    graphChart_->setTitle("CSV Graph — " + path);
    updateGraphAxes();
    setGraphStatus(QString("Loaded %1 points from CSV.").arg(pointsLoaded));
    mainTabs_->setCurrentWidget(graphTab_);
    updateUiState();
}

void MainWindow::onClearGraphClicked() {
    graphLiveRunning_ = false;
    graphWaitingForReply_ = false;
    graphTimer_->stop();
    graphSeries_->clear();
    graphChart_->setTitle("Measurement Graph");
    updateGraphAxes();
    setGraphStatus("Graph cleared.");
    updateUiState();
}

void MainWindow::onResetGraphViewClicked() {
    updateGraphAxes();
    setGraphStatus("Graph view reset.");
}

void MainWindow::onGraphTimer() {
    if (!graphLiveRunning_ || graphWaitingForReply_ || busy_ || datalogRunning_) {
        return;
    }

    const QString query = graphQueryEdit_->text().trimmed();
    if (query.isEmpty()) {
        setGraphStatus("Graph query is empty.");
        return;
    }

    graphPendingAddress_ = graphEquipmentCombo_->currentData(Qt::UserRole).toInt();
    graphPendingQuery_ = query;
    graphWaitingForReply_ = true;

    emit requestQueryScpi(graphPendingAddress_, graphPendingQuery_);
}

void MainWindow::onBusyChanged(bool busy) {
    busy_ = busy;
    updateUiState();
}

void MainWindow::toggleSidebar() {
    if (!sidebarExpanded_) {
        sidebarExpanded_ = true;
        sidebarPages_->setCurrentWidget(commandSidebarPage_);
    } else if (sidebarPages_->currentWidget() != commandSidebarPage_) {
        sidebarPages_->setCurrentWidget(commandSidebarPage_);
    } else {
        sidebarExpanded_ = false;
    }

    updateSidebarState();
}

void MainWindow::updateCommandGroupTitle() {
    if (!commandToolBox_ || !sidebarGroupTitleLabel_) {
        return;
    }

    const int index = commandToolBox_->currentIndex();
    if (index < 0) {
        sidebarGroupTitleLabel_->setText("Command Group");
        return;
    }

    sidebarGroupTitleLabel_->setText(commandToolBox_->itemText(index));
}

void MainWindow::updateSidebarState() {
    if (!sidebarContainer_ || !sidebarContent_ || !sidebarToggleButton_ || !equipmentSidebarButton_) {
        return;
    }

    sidebarContent_->setVisible(sidebarExpanded_);
    sidebarContainer_->setMinimumWidth(sidebarExpanded_ ? kSidebarExpandedWidth : kSidebarCollapsedWidth);
    sidebarContainer_->setMaximumWidth(sidebarExpanded_ ? kSidebarExpandedWidth : kSidebarCollapsedWidth);

    sidebarToggleButton_->setText("CMD");
    sidebarToggleButton_->setToolTip(sidebarExpanded_ ? "Show command presets / collapse sidebar" : "Expand command sidebar");
    sidebarToggleButton_->setStyleSheet("text-align: center; padding-left: 0px; padding-right: 0px;");

    equipmentSidebarButton_->setText(sidebarExpanded_ ? "Equipment" : "EQP");
    equipmentSidebarButton_->setToolTip("Open equipment registry");
    equipmentSidebarButton_->setStyleSheet("text-align: center; padding-left: 0px; padding-right: 0px;");
}

void MainWindow::updateUiState() {
    const bool hasPort = portCombo_->count() > 0;
    const bool canConnect = hasPort && !connected_ && !busy_ && !datalogRunning_ && !graphLiveRunning_;
    const bool canUseBusManually = connected_ && !busy_ && !datalogRunning_ && !graphLiveRunning_;
    const bool canConfigureDatalog = !datalogRunning_ && !graphLiveRunning_;
    const bool canConfigureGraph = !datalogRunning_;
    const bool hasTriggerJobs = std::any_of(datalogJobs_.cbegin(), datalogJobs_.cend(), [](const DataLogJob& job) {
        return !job.finished && !job.stopped && job.mode == "Trigger Based";
    });

    portCombo_->setEnabled(!connected_ && !busy_ && !datalogRunning_ && !graphLiveRunning_);
    baudCombo_->setEnabled(!connected_ && !busy_ && !datalogRunning_ && !graphLiveRunning_);
    refreshButton_->setEnabled(!connected_ && !busy_ && !datalogRunning_ && !graphLiveRunning_);
    connectButton_->setEnabled(connected_ || canConnect);
    connectButton_->setText(connected_ ? "Disconnect" : "Connect");

    detectButton_->setEnabled(canUseBusManually);

    gpibAddressSpin_->setEnabled(!busy_ && !datalogRunning_ && !graphLiveRunning_);

    equipmentAddressSpin_->setEnabled(canConfigureDatalog);
    equipmentCodeEdit_->setEnabled(canConfigureDatalog);
    equipmentTypeCombo_->setEnabled(canConfigureDatalog);
    addEquipmentButton_->setEnabled(canConfigureDatalog);
    removeEquipmentButton_->setEnabled(canConfigureDatalog && equipmentTable_->rowCount() > 0);
    saveEquipmentCsvButton_->setEnabled(canConfigureDatalog && equipmentTable_->rowCount() > 0);
    loadEquipmentCsvButton_->setEnabled(canConfigureDatalog);
    equipmentTable_->setEnabled(canConfigureDatalog);

    datalogEquipmentCombo_->setEnabled(canConfigureDatalog);
    datalogQueryEdit_->setEnabled(canConfigureDatalog);
    datalogModeCombo_->setEnabled(canConfigureDatalog);
    datalogIntervalSpin_->setEnabled(canConfigureDatalog);
    datalogSamplesSpin_->setEnabled(canConfigureDatalog);
    datalogStartTimeEdit_->setEnabled(canConfigureDatalog);
    datalogEndTimeEdit_->setEnabled(canConfigureDatalog);
    datalogCsvPathEdit_->setEnabled(canConfigureDatalog);
    datalogBrowseCsvButton_->setEnabled(canConfigureDatalog);
    datalogAddJobButton_->setEnabled(canConfigureDatalog && datalogEquipmentCombo_->count() > 0);
    datalogRemoveJobButton_->setEnabled(canConfigureDatalog && datalogJobsTable_->currentRow() >= 0);
    datalogStartButton_->setEnabled(connected_ && !busy_ && !datalogRunning_ && !graphLiveRunning_ && !datalogJobs_.isEmpty());
    datalogStopButton_->setEnabled(datalogRunning_ || datalogStopRequested_);
    datalogTriggerButton_->setEnabled(datalogRunning_ && hasTriggerJobs);

    graphEquipmentCombo_->setEnabled(canConfigureGraph && !graphLiveRunning_);
    graphQueryEdit_->setEnabled(canConfigureGraph && !graphLiveRunning_);
    graphIntervalSpin_->setEnabled(canConfigureGraph && !graphLiveRunning_);
    graphStartLiveButton_->setEnabled(connected_ && !busy_ && !datalogRunning_ && !graphLiveRunning_ &&
                                      graphEquipmentCombo_->count() > 0 && !graphQueryEdit_->text().trimmed().isEmpty());
    graphStopLiveButton_->setEnabled(graphLiveRunning_ || graphWaitingForReply_);
    graphLoadCsvButton_->setEnabled(!busy_ && !datalogRunning_ && !graphLiveRunning_);
    graphClearButton_->setEnabled(true);
    graphResetViewButton_->setEnabled(true);

    commandEdit_->setEnabled(canUseBusManually);
    writeButton_->setEnabled(canUseBusManually);
    queryButton_->setEnabled(canUseBusManually);

    batchEdit_->setEnabled(canUseBusManually);
    runBatchButton_->setEnabled(canUseBusManually);
    clearBatchButton_->setEnabled(true);
    batchResultView_->setEnabled(true);

    sidebarToggleButton_->setEnabled(true);
    equipmentSidebarButton_->setEnabled(true);

    if (sidebarContent_) {
        sidebarContent_->setEnabled(true);
    }
}

void MainWindow::appendLog(const QString& message) {
    logView_->appendPlainText(stamp(message));
}

QString MainWindow::currentPortName() const {
    return portCombo_->currentData().toString();
}

int MainWindow::currentBaudRate() const {
    return baudCombo_->currentData().toInt();
}

void MainWindow::insertPresetCommand(const QString& command) {
    if (presetToBatchRadio_ && presetToBatchRadio_->isChecked()) {
        appendBatchLine(command);
        mainTabs_->setCurrentWidget(batchTab_);
        return;
    }

    commandEdit_->setText(command);
    commandEdit_->setFocus();
    commandEdit_->setCursorPosition(command.size());
    mainTabs_->setCurrentWidget(scpiTab_);
}

void MainWindow::appendBatchLine(const QString& line) {
    batchEdit_->moveCursor(QTextCursor::End);

    const QString currentText = batchEdit_->toPlainText();
    if (!currentText.isEmpty() && !currentText.endsWith('\n')) {
        batchEdit_->insertPlainText("\n");
    }

    batchEdit_->insertPlainText(line);
    batchEdit_->setFocus();
}

void MainWindow::refreshEquipmentSelectors() {
    const int previousDatalogAddress =
        datalogEquipmentCombo_ && datalogEquipmentCombo_->count() > 0
            ? datalogEquipmentCombo_->currentData(Qt::UserRole).toInt()
            : -1;

    const int previousGraphAddress =
        graphEquipmentCombo_ && graphEquipmentCombo_->count() > 0
            ? graphEquipmentCombo_->currentData(Qt::UserRole).toInt()
            : -1;

    if (!datalogEquipmentCombo_ || !graphEquipmentCombo_) {
        return;
    }

    datalogEquipmentCombo_->clear();
    graphEquipmentCombo_->clear();

    for (int row = 0; row < equipmentTable_->rowCount(); ++row) {
        const QString address = equipmentTable_->item(row, 0) ? equipmentTable_->item(row, 0)->text() : QString();
        const QString code = equipmentTable_->item(row, 1) ? equipmentTable_->item(row, 1)->text() : QString();
        const QString type = equipmentTable_->item(row, 2) ? equipmentTable_->item(row, 2)->text() : QString();

        bool ok = false;
        const int addressValue = address.toInt(&ok);
        if (!ok) {
            continue;
        }

        const QString label = QString("%1 [%2] - %3").arg(code).arg(address).arg(type);

        datalogEquipmentCombo_->addItem(label, addressValue);
        {
            const int index = datalogEquipmentCombo_->count() - 1;
            datalogEquipmentCombo_->setItemData(index, addressValue, Qt::UserRole);
            datalogEquipmentCombo_->setItemData(index, code, Qt::UserRole + 1);
            datalogEquipmentCombo_->setItemData(index, type, Qt::UserRole + 2);
        }

        graphEquipmentCombo_->addItem(label, addressValue);
        {
            const int index = graphEquipmentCombo_->count() - 1;
            graphEquipmentCombo_->setItemData(index, addressValue, Qt::UserRole);
            graphEquipmentCombo_->setItemData(index, code, Qt::UserRole + 1);
            graphEquipmentCombo_->setItemData(index, type, Qt::UserRole + 2);
        }
    }

    int datalogIndex = -1;
    for (int i = 0; i < datalogEquipmentCombo_->count(); ++i) {
        if (datalogEquipmentCombo_->itemData(i, Qt::UserRole).toInt() == previousDatalogAddress) {
            datalogIndex = i;
            break;
        }
    }

    int graphIndex = -1;
    for (int i = 0; i < graphEquipmentCombo_->count(); ++i) {
        if (graphEquipmentCombo_->itemData(i, Qt::UserRole).toInt() == previousGraphAddress) {
            graphIndex = i;
            break;
        }
    }

    if (datalogIndex >= 0) {
        datalogEquipmentCombo_->setCurrentIndex(datalogIndex);
    } else if (datalogEquipmentCombo_->count() > 0) {
        datalogEquipmentCombo_->setCurrentIndex(0);
    }

    if (graphIndex >= 0) {
        graphEquipmentCombo_->setCurrentIndex(graphIndex);
    } else if (graphEquipmentCombo_->count() > 0) {
        graphEquipmentCombo_->setCurrentIndex(0);
    }
}

QString MainWindow::datalogJobStatusText(const DataLogJob& job) const {
    if (job.stopped) {
        return "Stopped";
    }
    if (job.finished) {
        return "Finished";
    }
    if (!datalogRunning_) {
        return "Configured";
    }
    if (job.mode == "Trigger Based") {
        return job.triggerPending ? "Trigger Pending" : "Armed";
    }
    if (job.mode == "Computer Time Based" && QDateTime::currentDateTime() < job.startTime) {
        return "Waiting Start";
    }
    return "Running";
}

void MainWindow::refreshDatalogJobsTable() {
    datalogJobsTable_->setRowCount(0);

    for (int i = 0; i < datalogJobs_.size(); ++i) {
        const DataLogJob& job = datalogJobs_.at(i);
        const int row = datalogJobsTable_->rowCount();
        datalogJobsTable_->insertRow(row);

        datalogJobsTable_->setItem(row, 0, new QTableWidgetItem(job.equipmentCode));
        datalogJobsTable_->setItem(row, 1, new QTableWidgetItem(QString::number(job.address)));
        datalogJobsTable_->setItem(row, 2, new QTableWidgetItem(job.equipmentType));
        datalogJobsTable_->setItem(row, 3, new QTableWidgetItem(job.query));
        datalogJobsTable_->setItem(row, 4, new QTableWidgetItem(job.mode));
        datalogJobsTable_->setItem(row, 5, new QTableWidgetItem(job.mode == "Trigger Based" ? "-" : QString::number(job.intervalMs)));
        datalogJobsTable_->setItem(row, 6, new QTableWidgetItem(job.maxSamples == 0 ? "Unlimited" : QString::number(job.maxSamples)));
        datalogJobsTable_->setItem(row, 7, new QTableWidgetItem(job.csvPath));
        datalogJobsTable_->setItem(row, 8, new QTableWidgetItem(QString::number(job.samplesWritten)));
        datalogJobsTable_->setItem(row, 9, new QTableWidgetItem(datalogJobStatusText(job)));
    }

    if (datalogRunning_) {
        datalogStatusLabel_->setText(datalogStopRequested_ ? "Stopping datalog..." : "Datalog running.");
    } else {
        datalogStatusLabel_->setText("Datalog idle.");
    }
}

QString MainWindow::defaultDatalogCsvPath(const QString& equipmentCode, const QString& query) const {
    const QString base = sanitizeFileComponent(equipmentCode);
    const QString queryPart = sanitizeFileComponent(query);
    const QString stampText = QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss");
    return QDir::currentPath() + QDir::separator() + base + "_" + queryPart + "_" + stampText + ".csv";
}

bool MainWindow::appendDatalogCsvRow(const DataLogJob& job, const QString& value, const QDateTime& timestamp) {
    QFile file(job.csvPath);
    if (!file.open(QIODevice::Append | QIODevice::Text)) {
        appendLog("Failed to append datalog CSV: " + job.csvPath);
        return false;
    }

    QTextStream out(&file);
    out.setEncoding(QStringConverter::Utf8);
    out << escapeCsvField(timestamp.toString(Qt::ISODateWithMs)) << ','
        << escapeCsvField(QString::number(job.address)) << ','
        << escapeCsvField(job.equipmentCode) << ','
        << escapeCsvField(job.equipmentType) << ','
        << escapeCsvField(job.query) << ','
        << escapeCsvField(value) << '\n';

    file.close();
    return true;
}

void MainWindow::finalizeDatalogRunIfNeeded() {
    if (datalogStopRequested_) {
        if (datalogWaitingForReply_) {
            return;
        }

        for (DataLogJob& job : datalogJobs_) {
            if (!job.finished) {
                job.stopped = true;
            }
        }

        datalogRunning_ = false;
        datalogStopRequested_ = false;
        datalogTimer_->stop();
        refreshDatalogJobsTable();
        updateUiState();
        appendLog("Datalog stopped. CSV files saved.");
        datalogStatusLabel_->setText("Datalog stopped.");
        return;
    }

    bool allFinished = true;
    for (const DataLogJob& job : datalogJobs_) {
        if (!job.finished && !job.stopped) {
            allFinished = false;
            break;
        }
    }

    if (datalogRunning_ && allFinished && !datalogWaitingForReply_) {
        datalogRunning_ = false;
        datalogTimer_->stop();
        refreshDatalogJobsTable();
        updateUiState();
        appendLog("Datalog completed. CSV files saved.");
        datalogStatusLabel_->setText("Datalog completed.");
    }
}

void MainWindow::updateGraphAxes() {
    if (!graphSeries_ || !graphAxisX_ || !graphAxisY_) {
        return;
    }

    const QList<QPointF> points = graphSeries_->points();
    if (points.isEmpty()) {
        const QDateTime now = QDateTime::currentDateTime();
        graphAxisX_->setRange(now.addSecs(-10), now.addSecs(10));
        graphAxisY_->setRange(-1.0, 1.0);
        return;
    }

    qreal minX = points.first().x();
    qreal maxX = points.first().x();
    qreal minY = points.first().y();
    qreal maxY = points.first().y();

    for (const QPointF& point : points) {
        minX = std::min(minX, point.x());
        maxX = std::max(maxX, point.x());
        minY = std::min(minY, point.y());
        maxY = std::max(maxY, point.y());
    }

    if (qFuzzyCompare(minX, maxX)) {
        minX -= 1000.0;
        maxX += 1000.0;
    }

    if (qFuzzyCompare(minY, maxY)) {
        minY -= 1.0;
        maxY += 1.0;
    } else {
        const qreal pad = (maxY - minY) * 0.08;
        minY -= pad;
        maxY += pad;
    }

    graphAxisX_->setRange(
        QDateTime::fromMSecsSinceEpoch(static_cast<qint64>(minX)),
        QDateTime::fromMSecsSinceEpoch(static_cast<qint64>(maxX))
        );
    graphAxisY_->setRange(minY, maxY);
}

void MainWindow::setGraphStatus(const QString& text) {
    if (graphStatusLabel_) {
        graphStatusLabel_->setText(text);
    }
}