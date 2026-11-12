#pragma once

#include <QFrame>
#include <QColor>
#include <QPoint>

namespace Ui {
namespace Glass {

class GlassPanel : public QFrame {
    Q_OBJECT
    Q_PROPERTY(int cornerRadius READ cornerRadius WRITE setCornerRadius)
    Q_PROPERTY(bool showSpecular READ showSpecular WRITE setShowSpecular)
    Q_PROPERTY(int specularRadius READ specularRadius WRITE setSpecularRadius)

public:
    explicit GlassPanel(QWidget* parent = nullptr);
    ~GlassPanel() override = default;

    int cornerRadius() const { return m_cornerRadius; }
    void setCornerRadius(int radius);

    bool showSpecular() const { return m_showSpecular; }
    void setShowSpecular(bool show);

    int specularRadius() const { return m_specularRadius; }
    void setSpecularRadius(int radius);

    void setCustomTint(const QColor& tint);
    void clearCustomTint();

protected:
    void paintEvent(QPaintEvent* event) override;
    void enterEvent(QEnterEvent* event) override;
    void leaveEvent(QEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;

private:
    int m_cornerRadius = 12;
    bool m_showSpecular = true;
    int m_specularRadius = 180;
    bool m_hasCustomTint = false;
    QColor m_customTint;

    bool m_isHovered = false;
    QPoint m_mousePos;
};

} // namespace Glass
} // namespace Ui
