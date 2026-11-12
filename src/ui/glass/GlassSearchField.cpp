#include "GlassSearchField.h"
#include "../Theme.h"
#include <QPainter>
#include <QPainterPath>
#include <QLinearGradient>
#include <QEasingCurve>

namespace Ui {
namespace Glass {

GlassSearchField::GlassSearchField(QWidget* parent) : QLineEdit(parent) {
    setObjectName("glassSearchField");
    setTextMargins(32, 0, 10, 0);
    setFixedHeight(34);

    m_focusAnim = new QPropertyAnimation(this, "focusProgress", this);
    m_focusAnim->setDuration(150);
    m_focusAnim->setEasingCurve(QEasingCurve::OutQuad);
}

void GlassSearchField::setFocusProgress(qreal progress) {
    m_focusProgress = progress;
    update();
}

void GlassSearchField::focusInEvent(QFocusEvent* event) {
    m_focusAnim->stop();
    m_focusAnim->setStartValue(m_focusProgress);
    m_focusAnim->setEndValue(1.0);
    m_focusAnim->start();
    QLineEdit::focusInEvent(event);
}

void GlassSearchField::focusOutEvent(QFocusEvent* event) {
    m_focusAnim->stop();
    m_focusAnim->setStartValue(m_focusProgress);
    m_focusAnim->setEndValue(0.0);
    m_focusAnim->start();
    QLineEdit::focusOutEvent(event);
}

void GlassSearchField::paintEvent(QPaintEvent* event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    bool dark = Theme::isDark();
    QRectF r = rect().adjusted(0.5, 0.5, -0.5, -0.5);

    QPainterPath path;
    path.addRoundedRect(r, 10, 10);

    // 1. Translucent Liquid Fill
    QLinearGradient grad(r.topLeft(), r.bottomLeft());
    if (dark) {
        grad.setColorAt(0.0, QColor(42, 42, 48, 190));
        grad.setColorAt(1.0, QColor(32, 32, 38, 175));
    } else {
        grad.setColorAt(0.0, QColor(255, 255, 255, 220));
        grad.setColorAt(1.0, QColor(250, 250, 248, 195));
    }
    painter.fillPath(path, grad);

    // 2. Border & Focus Ring
    QColor borderColor;
    if (m_focusProgress > 0.0) {
        borderColor = dark ? QColor(255, 255, 255, static_cast<int>(180 * m_focusProgress))
                           : QColor(29, 29, 31, static_cast<int>(200 * m_focusProgress));
    } else {
        borderColor = dark ? QColor(255, 255, 255, 35) : QColor(229, 229, 224, 180);
    }
    painter.strokePath(path, QPen(borderColor, 1.0));

    // 3. Search Vector Icon inside left margin
    painter.save();
    painter.setRenderHint(QPainter::Antialiasing, true);
    QPen iconPen(dark ? QColor(161, 161, 166) : QColor(134, 134, 139), 1.5);
    iconPen.setCapStyle(Qt::RoundCap);
    painter.setPen(iconPen);

    int cx = 16, cy = height() / 2 - 1, radius = 5;
    painter.drawEllipse(QPoint(cx, cy), radius, radius);
    painter.drawLine(cx + 4, cy + 4, cx + 8, cy + 8);
    painter.restore();

    // 4. Default Text & Selection Rendering
    QLineEdit::paintEvent(event);
}

} // namespace Glass
} // namespace Ui
