#pragma once

#include <QStyledItemDelegate>
#include <functional>

namespace Ui {
namespace Glass {

class GlassNavigationItem : public QStyledItemDelegate {
    Q_OBJECT

public:
    explicit GlassNavigationItem(QObject* parent = nullptr,
                                std::function<bool()> isCollapsedFn = nullptr);
    ~GlassNavigationItem() override = default;

    QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const override;
    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;

private:
    std::function<bool()> m_isCollapsedFn;
};

} // namespace Glass
} // namespace Ui
