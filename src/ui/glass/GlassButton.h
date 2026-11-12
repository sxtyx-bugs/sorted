#pragma once

#include <QPushButton>
#include <QPropertyAnimation>
#include <QPoint>

namespace Ui {
namespace Glass {

enum class ButtonVariant {
    Primary,    // High-contrast charcoal/white glass
    Subtle,     // Translucent frosted glass button
    IconOnly    // Square/circular icon button
};

class GlassButton : public QPushButton {
    Q_OBJECT
    Q_PROPERTY(qreal hoverProgress READ hoverProgress WRITE setHoverProgress)
    Q_PROPERTY(qreal pressProgress READ pressProgress WRITE setPressProgress)
    Q_PROPERTY(int cornerRadius READ cornerRadius WRITE setCornerRadius)

public:
    explicit GlassButton(QWidget* parent = nullptr);
    explicit GlassButton(const QString& text, QWidget* parent = nullptr);
    GlassButton(const QString& text, ButtonVariant variant, QWidget* parent = nullptr);
    ~GlassButton() override = default;

    ButtonVariant variant() const { return m_variant; }
    void setVariant(ButtonVariant variant);

    int cornerRadius() const { return m_cornerRadius; }
    void setCornerRadius(int radius);

    qreal hoverProgress() const { return m_hoverProgress; }
    void setHoverProgress(qreal progress);

    qreal pressProgress() const { return m_pressProgress; }
    void setPressProgress(qreal progress);

    QSize sizeHint() const override;

protected:
    void paintEvent(QPaintEvent* event) override;
    void enterEvent(QEnterEvent* event) override;
    void leaveEvent(QEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

private:
    void init();

    ButtonVariant m_variant = ButtonVariant::Subtle;
    int m_cornerRadius = 10;
    qreal m_hoverProgress = 0.0;
    qreal m_pressProgress = 0.0;
    bool m_isHovered = false;
    bool m_isPressed = false;
    QPoint m_mousePos;

    QPropertyAnimation* m_hoverAnim = nullptr;
    QPropertyAnimation* m_pressAnim = nullptr;
};

} // namespace Glass
} // namespace Ui
