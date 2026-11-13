#include "GlassDialog.h"
#include "../Theme.h"
#include <QPainter>
#include <QPainterPath>
#include <QLinearGradient>

namespace Ui {
namespace Glass {

GlassDialog::GlassDialog(const QString& title, const QString& message, QWidget* parent)
    : QDialog(parent) {
    setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground, true);
    setMinimumWidth(440);
    setMinimumHeight(180);
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);

    QString textPri = Theme::getTextPrimary();
    QString textSec = Theme::getTextSecondary();

    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setContentsMargins(28, 28, 28, 24);
    m_mainLayout->setSpacing(14);
    m_mainLayout->setSizeConstraint(QLayout::SetMinimumSize);

    m_lblTitle = new QLabel(title, this);
    m_lblTitle->setStyleSheet(QString("font-size: 16px; font-weight: 700; color: %1; border: none;").arg(textPri));
    m_mainLayout->addWidget(m_lblTitle);

    m_lblMessage = new QLabel(message, this);
    m_lblMessage->setWordWrap(true);
    m_lblMessage->setStyleSheet(QString("font-size: 13px; color: %1; border: none;").arg(textPri));
    m_mainLayout->addWidget(m_lblMessage);

    m_lblInfo = new QLabel(this);
    m_lblInfo->setWordWrap(true);
    m_lblInfo->setStyleSheet(QString("font-size: 11px; color: %1; border: none;").arg(textSec));
    m_lblInfo->setVisible(false);
    m_mainLayout->addWidget(m_lblInfo);

    m_mainLayout->addStretch();

    m_buttonLayout = new QHBoxLayout();
    m_buttonLayout->setSpacing(10);
    m_buttonLayout->addStretch();
    m_mainLayout->addLayout(m_buttonLayout);

    adjustSize();
}

void GlassDialog::setInformativeText(const QString& infoText) {
    m_lblInfo->setText(infoText);
    m_lblInfo->setVisible(!infoText.isEmpty());
    adjustSize();
}

void GlassDialog::addActionButton(const QString& text, ButtonVariant variant, bool isAccept) {
    auto btn = new GlassButton(text, variant, this);
    if (isAccept) {
        connect(btn, &QPushButton::clicked, this, &QDialog::accept);
    } else {
        connect(btn, &QPushButton::clicked, this, &QDialog::reject);
    }
    m_buttonLayout->addWidget(btn);
    adjustSize();
}

void GlassDialog::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    bool dark = Theme::isDark();
    QRectF r = rect().adjusted(0.5, 0.5, -0.5, -0.5);

    QPainterPath path;
    path.addRoundedRect(r, 16, 16);

    // 1. Frosted Liquid Glass Card Fill
    QLinearGradient grad(r.topLeft(), r.bottomRight());
    if (dark) {
        grad.setColorAt(0.0, QColor(44, 44, 52, 240));
        grad.setColorAt(1.0, QColor(28, 28, 34, 230));
    } else {
        grad.setColorAt(0.0, QColor(255, 255, 255, 245));
        grad.setColorAt(1.0, QColor(246, 246, 242, 235));
    }
    painter.fillPath(path, grad);

    // 2. Soft Inner Rim Highlight
    QPainterPath innerPath;
    innerPath.addRoundedRect(r.adjusted(1, 1, -1, -1), 15, 15);
    QLinearGradient innerRim(r.topLeft(), r.bottomLeft());
    innerRim.setColorAt(0.0, QColor(255, 255, 255, dark ? 50 : 200));
    innerRim.setColorAt(0.3, QColor(255, 255, 255, dark ? 15 : 50));
    innerRim.setColorAt(1.0, QColor(255, 255, 255, 0));
    painter.strokePath(innerPath, QPen(QBrush(innerRim), 1.0));

    // 3. Border Stroke
    QPen borderPen(dark ? QColor(255, 255, 255, 40) : QColor(229, 229, 224, 220), 1.0);
    painter.strokePath(path, borderPen);
}

} // namespace Glass
} // namespace Ui
