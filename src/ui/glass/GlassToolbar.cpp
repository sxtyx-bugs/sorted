#include "GlassToolbar.h"
#include "../Theme.h"
#include <QPainter>
#include <QLinearGradient>

namespace Ui {
namespace Glass {

GlassToolbar::GlassToolbar(QWidget* parent) : GlassPanel(parent) {
    setObjectName("glassToolbar");
    setCornerRadius(0);
    setShowSpecular(true);
    setSpecularRadius(300);
}

void GlassToolbar::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    bool dark = Theme::isDark();
    QRectF r = rect();
    if (r.isEmpty()) return;

    // 1. Horizontal Translucent Liquid Fill
    QLinearGradient grad(r.topLeft(), r.bottomLeft());
    if (dark) {
        grad.setColorAt(0.0, QColor(36, 36, 40, 210));
        grad.setColorAt(1.0, QColor(28, 28, 32, 195));
    } else {
        grad.setColorAt(0.0, QColor(255, 255, 255, 220));
        grad.setColorAt(1.0, QColor(250, 250, 248, 205));
    }
    painter.fillRect(r, grad);

    // 2. Bottom Hairline Separator
    QLinearGradient bottomLine(r.bottomLeft(), r.bottomRight());
    if (dark) {
        bottomLine.setColorAt(0.0, QColor(255, 255, 255, 30));
        bottomLine.setColorAt(0.5, QColor(255, 255, 255, 45));
        bottomLine.setColorAt(1.0, QColor(255, 255, 255, 30));
    } else {
        bottomLine.setColorAt(0.0, QColor(229, 229, 224, 150));
        bottomLine.setColorAt(0.5, QColor(255, 255, 255, 220));
        bottomLine.setColorAt(1.0, QColor(229, 229, 224, 150));
    }
    painter.setPen(QPen(QBrush(bottomLine), 1.0));
    painter.drawLine(r.bottomLeft() - QPointF(0, 0.5), r.bottomRight() - QPointF(0, 0.5));
}

} // namespace Glass
} // namespace Ui
