#pragma once

#include <QPoint>
#include <QtCharts/QChart>
#include <QtCharts/QChartView>

class QMouseEvent;
class QWheelEvent;

class InteractiveChartView final : public QChartView {
public:
    explicit InteractiveChartView(QChart* chart, QWidget* parent = nullptr);

protected:
    void wheelEvent(QWheelEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

private:
    bool panning_ = false;
    QPoint lastPos_;
};
