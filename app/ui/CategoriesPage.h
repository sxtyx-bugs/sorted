#pragma once

#include <QWidget>
#include <QTableWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <unordered_map>
#include <vector>
#include "../models/FileRecord.h"
#include "Theme.h"

namespace Ui {

class CategoriesPage : public QWidget {
    Q_OBJECT

public:
    explicit CategoriesPage(QWidget* parent = nullptr);

    void setFiles(const std::vector<FileRecord>& files);
    void applyTheme();

signals:
    void categorySelected(const QString& category);

private slots:
    void onCellDoubleClicked(int row, int column);

private:
    void setupUi();

    QLabel* m_lblTitle = nullptr;
    QTableWidget* m_tableCategories = nullptr;
    std::vector<FileRecord> m_files;
};

} // namespace Ui
