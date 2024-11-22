#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>


QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
    QTimer *timer;

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void UpdateLabel(const QString text);
    Ui::MainWindow *ui;
private slots:
    void onConnectButtonClicked();    // Слот для кнопки підключення
    void onDataReceived();
    void mousePressEvent(QMouseEvent *event);
    void TimerSlot();
    void updateImageLabelSize();
    void resizeEvent(QResizeEvent* event);
    void onOrientationChanged(Qt::ScreenOrientation orientation);
    void onSendButtonClicked();
    void onEnterButtonClicked();
    void onBackSpaceButtonClicked();
    void mainMenuButtonClicked();
    void ConsoleButtonClicked();
    void DesktopButtonClicked();
    void ConsoleSendButtonClicked();
    void ConsoleDataReceived();

private:
    bool console;
    QTcpSocket *socket;
    QPixmap pixmap;
};
#endif // MAINWINDOW_H
