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
    setFixedSize(440, 240);

    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setContentsMargins(28, 28, 28, 24);
    m_mainLayout->setSpacing(14);

    m_lblTitle = new QLabel(title, this);
    m_lblTitle->setStyleSheet("font-size: 16px; font-weight: 700; border: none;");
    m_mainLayout->addWidget(m_lblTitle);

    m_lblMessage = new QLabel(message, this);
    m_lblMessage->setWordWrap(true);
    m_lblMessage->setStyleSheet("font-size: 13px; border: none;");
    m_mainLayout->addWidget(m_lblMessage);

    m_lblInfo = new QLabel(this);
    m_lblInfo->setWordWrap(true);
    m_lblInfo->setStyleSheet("font-size: 11px; color: #86868B; border: none;");
    m_lblInfo->setVisible(false);
    m_mainLayout->addWidget(m_lblInfo);

    m_mainLayout->addStretch();

    m_buttonLayout = new QHBoxLayout();
    m_buttonLayout->setSpacing(10);
    m_buttonLayout->addStretch();
    m_mainLayout->addLayout(m_buttonLayout);
}

void GlassDialog::setInformativeText(const QString& infoText) {
    m_lblInfo->setText(infoText);
    m_lblInfo->setVisible(!infoText.isEmpty());
}

void GlassDialog::addActionButton(const QString& text, ButtonVariant variant, bool isAccept) {
    auto btn = new GlassButton(text, variant, this);
    if (isAccept) {
        connect(btn, &QPushButton::clicked, this, &QDialog::accept);
    } else {
        connect(btn, &QPushButton::clicked, this, &QDialog::reject);
    }
    m_buttonLayout->addWidget(btn);
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
