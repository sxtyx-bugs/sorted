#include <QApplication>
#include <QStyleFactory>
#include "ui/Theme.h"
#include "ui/MainWindow.h"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    QCoreApplication::setApplicationName("SORTED");
    QCoreApplication::setApplicationVersion("1.0.0");
    QCoreApplication::setOrganizationName("SORTED Project");

    // Set modern clean UI style
    app.setStyle(QStyleFactory::create("Fusion"));

    // Apply centralized Apple x Notion Cobalt Blue theme
    Ui::Theme::applyTheme(Ui::ColorTheme::Light);

    Ui::MainWindow mainWindow;
    mainWindow.show();

    return app.exec();
}
