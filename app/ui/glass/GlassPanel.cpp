#include "GlassPanel.h"
#include "../Theme.h"
#include <QPainter>
#include <QPainterPath>
#include <QLinearGradient>
#include <QRadialGradient>
#include <QMouseEvent>

namespace Ui {
namespace Glass {

GlassPanel::GlassPanel(QWidget* parent) : QFrame(parent) {
    setMouseTracking(true);
    setAttribute(Qt::WA_Hover, true);
}

void GlassPanel::setCornerRadius(int radius) {
    if (m_cornerRadius != radius) {
        m_cornerRadius = radius;
        update();
    }
}

void GlassPanel::setShowSpecular(bool show) {
    if (m_showSpecular != show) {
        m_showSpecular = show;
        update();
    }
}

void GlassPanel::setSpecularRadius(int radius) {
    if (m_specularRadius != radius) {
        m_specularRadius = radius;
        update();
    }
}

void GlassPanel::setCustomTint(const QColor& tint) {
    m_hasCustomTint = true;
    m_customTint = tint;
    update();
}

void GlassPanel::clearCustomTint() {
    m_hasCustomTint = false;
    update();
}

void GlassPanel::enterEvent(QEnterEvent* event) {
    m_isHovered = true;
    m_mousePos = event->position().toPoint();
    if (m_showSpecular) update();
    QFrame::enterEvent(event);
}

void GlassPanel::leaveEvent(QEvent* event) {
    m_isHovered = false;
    if (m_showSpecular) update();
    QFrame::leaveEvent(event);
}

void GlassPanel::mouseMoveEvent(QMouseEvent* event) {
    m_mousePos = event->pos();
    if (m_showSpecular && m_isHovered) {
        update();
    }
    QFrame::mouseMoveEvent(event);
}

void GlassPanel::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    bool dark = Theme::isDark();
    QRectF r = rect();
    if (r.isEmpty()) return;

    // Outer boundary with corner radius
    QPainterPath path;
    path.addRoundedRect(r.adjusted(0.5, 0.5, -0.5, -0.5), m_cornerRadius, m_cornerRadius);

    // 1. Multi-stop Translucent Liquid Glass Fill
    QLinearGradient grad(r.topLeft(), r.bottomRight());
    if (m_hasCustomTint) {
        grad.setColorAt(0.0, m_customTint);
        grad.setColorAt(1.0, m_customTint.darker(110));
    } else if (dark) {
        grad.setColorAt(0.0, QColor(42, 42, 48, 195));
        grad.setColorAt(0.5, QColor(34, 34, 38, 180));
        grad.setColorAt(1.0, QColor(26, 26, 30, 190));
    } else {
        grad.setColorAt(0.0, QColor(255, 255, 255, 215));
        grad.setColorAt(0.5, QColor(252, 252, 250, 195));
        grad.setColorAt(1.0, QColor(246, 246, 244, 205));
    }

    painter.fillPath(path, grad);

    // 2. Mouse-dependent Specular Highlight (Liquid Sheen)
    if (m_showSpecular && m_isHovered && !m_mousePos.isNull()) {
        QPainterPath clipPath;
        clipPath.addRoundedRect(r.adjusted(1, 1, -1, -1), m_cornerRadius, m_cornerRadius);
        painter.save();
        painter.setClipPath(clipPath);

        QRadialGradient specularGrad(m_mousePos, m_specularRadius);
        if (dark) {
            specularGrad.setColorAt(0.0, QColor(255, 255, 255, 45));
            specularGrad.setColorAt(0.35, QColor(255, 255, 255, 15));
            specularGrad.setColorAt(1.0, QColor(255, 255, 255, 0));
        } else {
            specularGrad.setColorAt(0.0, QColor(255, 255, 255, 110));
            specularGrad.setColorAt(0.35, QColor(255, 255, 255, 35));
            specularGrad.setColorAt(1.0, QColor(255, 255, 255, 0));
        }

        painter.fillPath(clipPath, specularGrad);
        painter.restore();
    }

    // 3. Soft Inner Rim Highlight (Top bevel reflection)
    QPainterPath innerPath;
    innerPath.addRoundedRect(r.adjusted(1.5, 1.5, -1.5, -1.5), m_cornerRadius - 1, m_cornerRadius - 1);
    QLinearGradient innerRim(r.topLeft(), r.bottomLeft());
    if (dark) {
        innerRim.setColorAt(0.0, QColor(255, 255, 255, 40));
        innerRim.setColorAt(0.2, QColor(255, 255, 255, 15));
        innerRim.setColorAt(1.0, QColor(255, 255, 255, 0));
    } else {
        innerRim.setColorAt(0.0, QColor(255, 255, 255, 160));
        innerRim.setColorAt(0.2, QColor(255, 255, 255, 60));
        innerRim.setColorAt(1.0, QColor(255, 255, 255, 10));
    }
    painter.strokePath(innerPath, QPen(QBrush(innerRim), 1.0));

    // 4. Subtle Edge Reflection / Border Stroke
    QPen borderPen;
    if (dark) {
        borderPen = QPen(QColor(255, 255, 255, 36), 1.0);
    } else {
        borderPen = QPen(QColor(255, 255, 255, 180), 1.0);
    }
    painter.strokePath(path, borderPen);
}

} // namespace Glass
} // namespace Ui
