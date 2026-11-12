#pragma once

#include <QWidget>
#include <QLineEdit>
#include <QComboBox>
#include <QTableWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <vector>
#include "../models/FileRecord.h"
#include "../core/MergeSort.h"
#include "Theme.h"

namespace Ui {

class FilesPage : public QWidget {
    Q_OBJECT

public:
    explicit FilesPage(QWidget* parent = nullptr);

    void setFiles(const std::vector<FileRecord>& files);
    void filterByCategory(const QString& category);
    void applyTheme();
    void focusSearch();

private slots:
    void onSearchChanged(const QString& query);
    void onCategoryChanged(int index);
    void onSortChanged(int index);
    void onCustomContextMenuRequested(const QPoint& pos);
    void openSelectedFileLocation();
    void copySelectedFilePath();
    void filterBySelectedCategory();

private:
    void setupUi();
    void applySortAndFilter();
    void renderTable();

    std::vector<FileRecord> m_allFiles;
    std::vector<FileRecord> m_displayedFiles;

    QLineEdit* m_searchEdit = nullptr;
    QComboBox* m_categoryCombo = nullptr;
    QComboBox* m_sortCombo = nullptr;
    QLabel* m_lblCount = nullptr;
    QTableWidget* m_tableWidget = nullptr;
};

} // namespace Ui
