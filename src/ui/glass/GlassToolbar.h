#pragma once

#include "GlassPanel.h"

namespace Ui {
namespace Glass {

class GlassToolbar : public GlassPanel {
    Q_OBJECT

public:
    explicit GlassToolbar(QWidget* parent = nullptr);
    ~GlassToolbar() override = default;

protected:
    void paintEvent(QPaintEvent* event) override;
};

} // namespace Glass
} // namespace Ui
