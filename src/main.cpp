#include <QApplication>
#include <QStyleFactory>
#include <QIcon>
#include "ui/Theme.h"
#include "ui/MainWindow.h"

int main(int argc, char* argv[]) {
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
#endif

    QApplication app(argc, argv);

    QApplication::setApplicationName("SORTED");
    QApplication::setApplicationDisplayName("SORTED");
    QApplication::setApplicationVersion("1.0.0");
    QApplication::setOrganizationName("Satydev");
    QApplication::setOrganizationDomain("satydev.com");

    // Set window icon from bundled Qt resource
    app.setWindowIcon(QIcon(":/icons/SORTED.png"));

    // Set modern clean UI style
    app.setStyle(QStyleFactory::create("Fusion"));

    // Apply centralized theme
    Ui::Theme::applyTheme(Ui::ColorTheme::Light);

    Ui::MainWindow mainWindow;
    mainWindow.show();

    return app.exec();
}
