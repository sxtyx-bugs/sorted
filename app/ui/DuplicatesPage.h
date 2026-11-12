#pragma once

#include <QWidget>
#include <QTreeWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFrame>
#include <vector>
#include "../models/ScanResult.h"
#include "Theme.h"

namespace Ui {

class DuplicatesPage : public QWidget {
    Q_OBJECT

public:
    explicit DuplicatesPage(QWidget* parent = nullptr);

    void setDuplicateGroups(const std::vector<DuplicateGroup>& groups);
    void reset();
    void applyTheme();

private:
    void setupUi();

    QFrame* m_noticeBanner = nullptr;
    QLabel* m_lblNotice = nullptr;
    QTreeWidget* m_treeWidget = nullptr;
    std::vector<DuplicateGroup> m_groups;
};

} // namespace Ui
