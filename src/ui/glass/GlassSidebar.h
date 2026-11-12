#pragma once

#include "GlassPanel.h"
#include <QPropertyAnimation>
#include <functional>

namespace Ui {
namespace Glass {

class GlassSidebar : public GlassPanel {
    Q_OBJECT
    Q_PROPERTY(int sidebarWidth READ sidebarWidth WRITE setSidebarWidth)

public:
    explicit GlassSidebar(QWidget* parent = nullptr);
    ~GlassSidebar() override = default;

    int sidebarWidth() const { return m_sidebarWidth; }
    void setSidebarWidth(int width);

    bool isCollapsed() const { return m_isCollapsed; }
    void setCollapsed(bool collapsed, bool animated = true);

    void setContentSource(std::function<QWidget*()> fn) { m_contentSourceFn = fn; }
    void refreshBackdrop();

signals:
    void collapseToggled(bool isCollapsed);

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    int m_sidebarWidth = 244;
    bool m_isCollapsed = false;
    QPropertyAnimation* m_anim = nullptr;
    std::function<QWidget*()> m_contentSourceFn = nullptr;
    QPixmap m_cachedBackdrop;
    bool m_isRenderingBackdrop = false;
};

} // namespace Glass
} // namespace Ui
