#include "GlassNavigationItem.h"
#include "../Theme.h"
#include <QPainter>
#include <QPainterPath>
#include <QLinearGradient>
#include <QStyle>

namespace Ui {
namespace Glass {

GlassNavigationItem::GlassNavigationItem(QObject* parent, std::function<bool()> isCollapsedFn)
    : QStyledItemDelegate(parent), m_isCollapsedFn(std::move(isCollapsedFn)) {}

QSize GlassNavigationItem::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const {
    Q_UNUSED(option);
    Q_UNUSED(index);
    bool collapsed = m_isCollapsedFn ? m_isCollapsedFn() : false;
    return collapsed ? QSize(52, 42) : QSize(212, 42);
}

void GlassNavigationItem::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const {
    painter->save();
    painter->setRenderHint(QPainter::Antialiasing);

    bool dark = Theme::isDark();
    bool collapsed = m_isCollapsedFn ? m_isCollapsedFn() : false;
    bool isSelected = option.state & QStyle::State_Selected;
    bool isHover = option.state & QStyle::State_MouseOver;

    QRectF rect = option.rect.adjusted(2, 2, -2, -2);
    QPainterPath rowPath;
    rowPath.addRoundedRect(rect, 8, 8);

    // 1. Draw Liquid Glass Pill Selection / Hover
    if (isSelected) {
        QLinearGradient selGrad(rect.topLeft(), rect.bottomRight());
        if (dark) {
            selGrad.setColorAt(0.0, QColor(255, 255, 255, 45));
            selGrad.setColorAt(1.0, QColor(255, 255, 255, 25));
        } else {
            selGrad.setColorAt(0.0, QColor(29, 29, 31, 25));
            selGrad.setColorAt(1.0, QColor(29, 29, 31, 15));
        }
        painter->fillPath(rowPath, selGrad);

        // Subtle Bevel Border
        QPen selPen(dark ? QColor(255, 255, 255, 40) : QColor(29, 29, 31, 30), 1.0);
        painter->strokePath(rowPath, selPen);
    } else if (isHover) {
        QColor hoverColor = dark ? QColor(255, 255, 255, 18) : QColor(0, 0, 0, 10);
        painter->fillPath(rowPath, hoverColor);
    }

    // 2. Active Indicator Pill for Collapsed State
    if (collapsed && isSelected) {
        QRectF pillRect(rect.left() + 2, rect.center().y() - 9, 3, 18);
        QPainterPath pillPath;
        pillPath.addRoundedRect(pillRect, 1.5, 1.5);
        painter->fillPath(pillPath, dark ? QColor(255, 255, 255) : QColor(29, 29, 31));
    }

    // 3. Draw Text / Icon
    QString rawText = index.data(Qt::DisplayRole).toString();
    QColor textColor;
    if (isSelected) {
        textColor = dark ? QColor(255, 255, 255) : QColor(29, 29, 31);
    } else if (isHover) {
        textColor = dark ? QColor(235, 235, 240) : QColor(40, 40, 42);
    } else {
        textColor = dark ? QColor(161, 161, 166) : QColor(119, 122, 115);
    }

    painter->setPen(textColor);
    QFont f = painter->font();
    f.setPointSize(10);
    f.setWeight(isSelected ? QFont::DemiBold : QFont::Normal);
    painter->setFont(f);

    if (collapsed) {
        // Centered single glyph
        QString glyph = rawText.trimmed().left(1);
        painter->drawText(rect, Qt::AlignCenter, glyph);
    } else {
        // Label with left padding
        QRect textRect = rect.toRect().adjusted(12, 0, -12, 0);
        painter->drawText(textRect, Qt::AlignVCenter | Qt::AlignLeft, rawText);
    }

    painter->restore();
}

} // namespace Glass
} // namespace Ui
