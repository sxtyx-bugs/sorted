#pragma once

#include <QDialog>
#include <QVBoxLayout>
#include <QLabel>
#include "GlassButton.h"

namespace Ui {
namespace Glass {

class GlassDialog : public QDialog {
    Q_OBJECT

public:
    explicit GlassDialog(const QString& title, const QString& message, QWidget* parent = nullptr);
    ~GlassDialog() override = default;

    void setInformativeText(const QString& infoText);
    void addActionButton(const QString& text, ButtonVariant variant = ButtonVariant::Subtle, bool isAccept = true);

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    QVBoxLayout* m_mainLayout = nullptr;
    QHBoxLayout* m_buttonLayout = nullptr;
    QLabel* m_lblTitle = nullptr;
    QLabel* m_lblMessage = nullptr;
    QLabel* m_lblInfo = nullptr;
};

} // namespace Glass
} // namespace Ui
