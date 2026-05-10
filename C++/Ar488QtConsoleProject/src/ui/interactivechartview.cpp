#include "interactivechartview.h"

#include <QMouseEvent>
#include <QPainter>
#include <QWheelEvent>
#include <QtCharts/QChart>

InteractiveChartView::InteractiveChartView(QChart* chart, QWidget* parent)
    : QChartView(chart, parent) {
    setRubberBand(QChartView::RectangleRubberBand);
    setRenderHint(QPainter::Antialiasing, true);
}

void InteractiveChartView::wheelEvent(QWheelEvent* event) {
    if (event->angleDelta().y() > 0) {
        chart()->zoom(1.15);
    } else {
        chart()->zoom(0.87);
    }
    event->accept();
}

void InteractiveChartView::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::RightButton) {
        panning_ = true;
        lastPos_ = event->pos();
        setCursor(Qt::ClosedHandCursor);
        event->accept();
        return;
    }
    QChartView::mousePressEvent(event);
}

void InteractiveChartView::mouseMoveEvent(QMouseEvent* event) {
    if (panning_) {
        const QPoint delta = event->pos() - lastPos_;
        chart()->scroll(-delta.x(), delta.y());
        lastPos_ = event->pos();
        event->accept();
        return;
    }
    QChartView::mouseMoveEvent(event);
}

void InteractiveChartView::mouseReleaseEvent(QMouseEvent* event) {
    if (event->button() == Qt::RightButton && panning_) {
        panning_ = false;
        unsetCursor();
        event->accept();
        return;
    }
    QChartView::mouseReleaseEvent(event);
}
