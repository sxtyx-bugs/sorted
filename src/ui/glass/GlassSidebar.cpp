#include "GlassSidebar.h"
#include "../Theme.h"
#include <QPainter>
#include <QPainterPath>
#include <QLinearGradient>
#include <QRadialGradient>
#include <QEasingCurve>
#include <QGraphicsDropShadowEffect>

namespace Ui {
namespace Glass {

GlassSidebar::GlassSidebar(QWidget* parent) : GlassPanel(parent) {
    setObjectName("glassSidebar");
    setFixedWidth(m_sidebarWidth);
    setCornerRadius(20); // 20px rounded corners for floating appearance
    setShowSpecular(true);
    setSpecularRadius(180);

    // Floating drop shadow effect
    auto* shadow = new QGraphicsDropShadowEffect(this);
    shadow->setBlurRadius(24);
    shadow->setOffset(2, 6);
    shadow->setColor(QColor(0, 0, 0, 95));
    setGraphicsEffect(shadow);

    m_anim = new QPropertyAnimation(this, "sidebarWidth", this);
    m_anim->setDuration(180);
    m_anim->setEasingCurve(QEasingCurve::OutCubic);
}

void GlassSidebar::setSidebarWidth(int width) {
    m_sidebarWidth = width;
    setFixedWidth(width);
    update();
}

void GlassSidebar::setCollapsed(bool collapsed, bool animated) {
    if (m_isCollapsed == collapsed) return;
    m_isCollapsed = collapsed;
    int targetWidth = collapsed ? 72 : 244;

    if (animated) {
        m_anim->stop();
        m_anim->setStartValue(width());
        m_anim->setEndValue(targetWidth);
        m_anim->start();
    } else {
        setSidebarWidth(targetWidth);
    }
    emit collapseToggled(collapsed);
}

void GlassSidebar::refreshBackdrop() {
    if (m_isRenderingBackdrop) return;
    int w = width() > 0 ? width() : m_sidebarWidth;
    int h = height() > 0 ? height() : 700;

    QWidget* target = m_contentSourceFn ? m_contentSourceFn() : nullptr;
    QPixmap rawPix(w, h);
    bool dark = Theme::isDark();
    rawPix.fill(dark ? QColor(25, 25, 25) : QColor(247, 247, 245));

    if (target && target != this) {
        m_isRenderingBackdrop = true;
        target->render(&rawPix, QPoint(), QRegion(0, 0, w, h));
        m_isRenderingBackdrop = false;
    }

    // Multi-pass blur simulation
    QImage img = rawPix.toImage();
    QImage s1 = img.scaled(qMax(1, w / 8), qMax(1, h / 8), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    QImage s2 = s1.scaled(qMax(1, w / 14), qMax(1, h / 14), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    QImage blurred = s2.scaled(w, h, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

    // Subtle optical displacement refraction
    QImage displaced(w, h, QImage::Format_ARGB32_Premultiplied);
    displaced.fill(Qt::transparent);
    QPainter p(&displaced);
    p.setRenderHint(QPainter::Antialiasing);
    p.setRenderHint(QPainter::SmoothPixmapTransform);
    p.drawImage(QRectF(-4, -4, w + 8, h + 8), blurred);
    p.end();

    m_cachedBackdrop = QPixmap::fromImage(displaced);
    update();
}

void GlassSidebar::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);

    QRectF r = rect();
    if (r.isEmpty()) return;

    qreal radius = cornerRadius();
    QPainterPath path;
    path.addRoundedRect(r.adjusted(0.5, 0.5, -0.5, -0.5), radius, radius);

    // 1. Live Blurred Content Backdrop
    if (!m_cachedBackdrop.isNull()) {
        painter.save();
        painter.setClipPath(path);
        painter.drawPixmap(0, 0, m_cachedBackdrop);
        painter.restore();
    }

    // 2. Dark Smoked Translucent Liquid Glass Material Wash
    bool dark = Theme::isDark();
    QLinearGradient grad(r.topLeft(), r.bottomRight());
    if (dark) {
        grad.setColorAt(0.0, QColor(18, 18, 22, 195));
        grad.setColorAt(0.5, QColor(14, 14, 18, 185));
        grad.setColorAt(1.0, QColor(22, 22, 26, 195));
    } else {
        grad.setColorAt(0.0, QColor(24, 24, 28, 180));
        grad.setColorAt(0.5, QColor(20, 20, 24, 175));
        grad.setColorAt(1.0, QColor(26, 26, 30, 185));
    }
    painter.fillPath(path, grad);

    // 3. Brighter Top Edge Bevel
    QPainterPath innerPath;
    innerPath.addRoundedRect(r.adjusted(1.0, 1.0, -1.0, -1.0), qMax(0.0, radius - 1.0), qMax(0.0, radius - 1.0));
    QLinearGradient topGrad(r.topLeft(), QPointF(r.left(), r.top() + 45));
    topGrad.setColorAt(0.0, QColor(255, 255, 255, 85));
    topGrad.setColorAt(0.3, QColor(255, 255, 255, 25));
    topGrad.setColorAt(1.0, QColor(255, 255, 255, 0));
    painter.strokePath(innerPath, QPen(QBrush(topGrad), 1.0));

    // 4. Perimeter Edge Reflection Rim
    QPen borderPen(QColor(255, 255, 255, 38), 1.0);
    painter.strokePath(path, borderPen);
}

} // namespace Glass
} // namespace Ui
