#pragma once

#include <QLineEdit>
#include <QPropertyAnimation>

namespace Ui {
namespace Glass {

class GlassSearchField : public QLineEdit {
    Q_OBJECT
    Q_PROPERTY(qreal focusProgress READ focusProgress WRITE setFocusProgress)

public:
    explicit GlassSearchField(QWidget* parent = nullptr);
    ~GlassSearchField() override = default;

    qreal focusProgress() const { return m_focusProgress; }
    void setFocusProgress(qreal progress);

protected:
    void paintEvent(QPaintEvent* event) override;
    void focusInEvent(QFocusEvent* event) override;
    void focusOutEvent(QFocusEvent* event) override;

private:
    qreal m_focusProgress = 0.0;
    QPropertyAnimation* m_focusAnim = nullptr;
};

} // namespace Glass
} // namespace Ui
