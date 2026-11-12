#include "GlassButton.h"
#include "../Theme.h"
#include <QPainter>
#include <QPainterPath>
#include <QLinearGradient>
#include <QRadialGradient>
#include <QEasingCurve>
#include <QMouseEvent>
#include <QFontMetrics>

namespace Ui {
namespace Glass {

GlassButton::GlassButton(QWidget* parent) : QPushButton(parent) {
    init();
}

GlassButton::GlassButton(const QString& text, QWidget* parent) : QPushButton(text, parent) {
    init();
}

GlassButton::GlassButton(const QString& text, ButtonVariant variant, QWidget* parent)
    : QPushButton(text, parent), m_variant(variant) {
    init();
}

void GlassButton::init() {
    setMouseTracking(true);
    setAttribute(Qt::WA_Hover, true);
    setCursor(Qt::PointingHandCursor);

    m_hoverAnim = new QPropertyAnimation(this, "hoverProgress", this);
    m_hoverAnim->setDuration(120);
    m_hoverAnim->setEasingCurve(QEasingCurve::OutQuad);

    m_pressAnim = new QPropertyAnimation(this, "pressProgress", this);
    m_pressAnim->setDuration(80);
    m_pressAnim->setEasingCurve(QEasingCurve::OutQuad);
}

void GlassButton::setVariant(ButtonVariant variant) {
    m_variant = variant;
    update();
}

void GlassButton::setCornerRadius(int radius) {
    m_cornerRadius = radius;
    update();
}

void GlassButton::setHoverProgress(qreal progress) {
    m_hoverProgress = progress;
    update();
}

void GlassButton::setPressProgress(qreal progress) {
    m_pressProgress = progress;
    update();
}

QSize GlassButton::sizeHint() const {
    if (m_variant == ButtonVariant::IconOnly) {
        return QSize(34, 34);
    }
    QSize base = QPushButton::sizeHint();
    return QSize(qMax(base.width() + 24, 76), qMax(base.height() + 8, 32));
}

void GlassButton::enterEvent(QEnterEvent* event) {
    m_isHovered = true;
    m_mousePos = event->position().toPoint();
    m_hoverAnim->stop();
    m_hoverAnim->setStartValue(m_hoverProgress);
    m_hoverAnim->setEndValue(1.0);
    m_hoverAnim->start();
    QPushButton::enterEvent(event);
}

void GlassButton::leaveEvent(QEvent* event) {
    m_isHovered = false;
    m_hoverAnim->stop();
    m_hoverAnim->setStartValue(m_hoverProgress);
    m_hoverAnim->setEndValue(0.0);
    m_hoverAnim->start();
    QPushButton::leaveEvent(event);
}

void GlassButton::mouseMoveEvent(QMouseEvent* event) {
    m_mousePos = event->pos();
    if (m_isHovered) {
        update();
    }
    QPushButton::mouseMoveEvent(event);
}

void GlassButton::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        m_isPressed = true;
        m_pressAnim->stop();
        m_pressAnim->setStartValue(m_pressProgress);
        m_pressAnim->setEndValue(1.0);
        m_pressAnim->start();
    }
    QPushButton::mousePressEvent(event);
}

void GlassButton::mouseReleaseEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        m_isPressed = false;
        m_pressAnim->stop();
        m_pressAnim->setStartValue(m_pressProgress);
        m_pressAnim->setEndValue(0.0);
        m_pressAnim->start();
    }
    QPushButton::mouseReleaseEvent(event);
}

void GlassButton::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    bool dark = Theme::isDark();
    QRectF r = rect().adjusted(0.5, 0.5, -0.5, -0.5);
    if (r.isEmpty()) return;

    QPainterPath path;
    path.addRoundedRect(r, m_cornerRadius, m_cornerRadius);

    // 1. Fill based on variant
    if (m_variant == ButtonVariant::Primary) {
        // High-contrast charcoal (Light) / pure white (Dark) with glass sheen
        QColor baseColor = dark ? QColor(255, 255, 255) : QColor(29, 29, 31);
        if (m_hoverProgress > 0.0) {
            baseColor = dark ? QColor(230, 230, 235) : QColor(50, 50, 54);
        }
        if (m_pressProgress > 0.0) {
            baseColor = dark ? QColor(210, 210, 215) : QColor(0, 0, 0);
        }
        painter.fillPath(path, baseColor);

        // Specular highlight following cursor
        if (m_isHovered && !m_mousePos.isNull()) {
            painter.save();
            painter.setClipPath(path);
            QRadialGradient spec(m_mousePos, 70);
            spec.setColorAt(0.0, QColor(255, 255, 255, dark ? 80 : 45));
            spec.setColorAt(1.0, QColor(255, 255, 255, 0));
            painter.fillPath(path, spec);
            painter.restore();
        }
    } else {
        // Subtle / IconOnly: Translucent frosted glass button
        QLinearGradient grad(r.topLeft(), r.bottomLeft());
        int alphaBase = dark ? 50 : 120;
        int alphaHover = dark ? 85 : 180;
        int currentAlpha = static_cast<int>(alphaBase + (alphaHover - alphaBase) * m_hoverProgress);

        if (dark) {
            grad.setColorAt(0.0, QColor(60, 60, 68, currentAlpha));
            grad.setColorAt(1.0, QColor(40, 40, 46, currentAlpha));
        } else {
            grad.setColorAt(0.0, QColor(255, 255, 255, currentAlpha));
            grad.setColorAt(1.0, QColor(245, 245, 242, currentAlpha));
        }
        painter.fillPath(path, grad);

        // Specular sheen tracking mouse
        if (m_isHovered && !m_mousePos.isNull()) {
            painter.save();
            painter.setClipPath(path);
            QRadialGradient spec(m_mousePos, 90);
            spec.setColorAt(0.0, QColor(255, 255, 255, dark ? 55 : 140));
            spec.setColorAt(1.0, QColor(255, 255, 255, 0));
            painter.fillPath(path, spec);
            painter.restore();
        }

        // Inner rim highlight at top
        QPainterPath innerPath;
        innerPath.addRoundedRect(r.adjusted(1, 1, -1, -1), m_cornerRadius - 1, m_cornerRadius - 1);
        QLinearGradient innerRim(r.topLeft(), r.bottomLeft());
        innerRim.setColorAt(0.0, QColor(255, 255, 255, dark ? 40 : 180));
        innerRim.setColorAt(0.5, QColor(255, 255, 255, dark ? 10 : 40));
        innerRim.setColorAt(1.0, QColor(255, 255, 255, 0));
        painter.strokePath(innerPath, QPen(QBrush(innerRim), 1.0));

        // Edge stroke
        QPen borderPen(dark ? QColor(255, 255, 255, 38) : QColor(229, 229, 224, 200), 1.0);
        painter.strokePath(path, borderPen);
    }

    // 2. Keyboard Focus Ring
    if (hasFocus()) {
        QPainterPath focusPath;
        focusPath.addRoundedRect(r.adjusted(-1.5, -1.5, 1.5, 1.5), m_cornerRadius + 2, m_cornerRadius + 2);
        painter.strokePath(focusPath, QPen(QColor(dark ? 255 : 29, dark ? 255 : 29, dark ? 255 : 31, 120), 1.5));
    }

    // 3. Draw Icon and Text
    QColor textColor;
    if (m_variant == ButtonVariant::Primary) {
        textColor = dark ? QColor(29, 29, 31) : QColor(255, 255, 255);
    } else {
        textColor = dark ? QColor(245, 245, 247) : QColor(29, 29, 31);
    }

    if (!isEnabled()) {
        textColor = dark ? QColor(110, 110, 115) : QColor(161, 161, 166);
    }

    painter.setPen(textColor);
    QFont f = font();
    f.setWeight(m_variant == ButtonVariant::Primary ? QFont::DemiBold : QFont::Medium);
    painter.setFont(f);

    QRect textRect = rect();
    if (!icon().isNull()) {
        int iconSize = 16;
        QRect iconRect(10, (height() - iconSize) / 2, iconSize, iconSize);
        if (text().isEmpty()) {
            iconRect = QRect((width() - iconSize) / 2, (height() - iconSize) / 2, iconSize, iconSize);
        }
        icon().paint(&painter, iconRect);
        if (!text().isEmpty()) {
            textRect.adjust(iconSize + 16, 0, -10, 0);
        }
    }

    if (!text().isEmpty()) {
        painter.drawText(textRect, Qt::AlignCenter, text());
    }
}

} // namespace Glass
} // namespace Ui
