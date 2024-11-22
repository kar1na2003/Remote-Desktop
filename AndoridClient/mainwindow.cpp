#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QTcpSocket>
#include <QDebug>
#include <QHostAddress>
#include <QPixmap>
#include <QMouseEvent>
#include <QTimer>
#include <QScreen>
#include <QGuiApplication>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , socket(new QTcpSocket(this))
{
    ui->setupUi(this);



    pixmap.load(":/myimage.png");
    if (!pixmap.isNull()) {
        ui->imageLabel->setPixmap(pixmap.scaled(ui->imageLabel->size(), Qt::KeepAspectRatio));
    } else {
        qDebug() << "Не вдалося завантажити зображення";
    }
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::TimerSlot );
    connect(ui->connectButton, &QPushButton::clicked, this, &MainWindow::onConnectButtonClicked);
    connect(socket, &QTcpSocket::readyRead, this, &MainWindow::onDataReceived);
    connect(socket, &QTcpSocket::readyRead, this, &MainWindow::ConsoleDataReceived);

    QScreen *screen = QGuiApplication::primaryScreen();
    connect(screen, &QScreen::primaryOrientationChanged, this, &MainWindow::onOrientationChanged);

    connect(ui->sendButton, &QPushButton::clicked, this, &MainWindow::onSendButtonClicked);
    connect(ui->backSpace, &QPushButton::clicked, this, &MainWindow::onBackSpaceButtonClicked);
    connect(ui->Enter, &QPushButton::clicked, this, &MainWindow::onEnterButtonClicked);
    connect(ui->menu, &QPushButton::clicked, this, &MainWindow::mainMenuButtonClicked);
    connect(ui->Desktop, &QPushButton::clicked, this, &MainWindow::DesktopButtonClicked);
    connect(ui->Console, &QPushButton::clicked, this, &MainWindow::ConsoleButtonClicked);
    connect(ui->consoleSend, &QPushButton::clicked, this, &MainWindow:: ConsoleSendButtonClicked);



    ui->imageLabel->setVisible(false);
    ui->lineEdit->setVisible(false);
    ui->backSpace->setVisible(false);
    ui->Enter->setVisible(false);
    ui->menu->setVisible(false);
    ui->sendButton->setVisible(false);
    ui->Console->setVisible(false);
    ui->Desktop->setVisible(false);
    ui->consoleInput->setVisible(false);
    ui->consoleOutput->setVisible(false);
    ui->consoleSend->setVisible(false);

    ui->ip->setText("192.168.1.5");
}

void MainWindow::ConsoleButtonClicked(){
    ui->consoleInput->setVisible(true);
    ui->consoleInput->update();
    ui->consoleOutput->setVisible(true);
    ui->menu->setVisible(true);
    ui->consoleSend->setVisible(true);

    ui->Desktop->setVisible(false);
    ui->Console->setVisible(false);
    console = true;
    QString message = QString("CONSOLE");
    socket->write(message.toUtf8());
}

void MainWindow::DesktopButtonClicked(){
    ui->imageLabel->setVisible(true);
    ui->lineEdit->setVisible(true);
    ui->backSpace->setVisible(true);
    ui->Enter->setVisible(true);
    ui->menu->setVisible(true);
    ui->sendButton->setVisible(true);

    console = false;
    ui->Console->setVisible(false);
    ui->Desktop->setVisible(false);

    QString message = QString("DESKTOP");
    socket->write(message.toUtf8());
}

void MainWindow::ConsoleSendButtonClicked() {
    QString message = ui->consoleInput->text();
    QByteArray data = message.toUtf8();

    qint32 length = data.size();
    QByteArray lengthArray;
    QDataStream lengthStream(&lengthArray, QIODevice::WriteOnly);
    lengthStream << length;

    socket->write(lengthArray);
    socket->write(data);

    qDebug() << "Sent message:" << message << "with length:" << length;

    ui->consoleInput->clear();
}
unsigned int fromNetworkByteOrder(unsigned int value) {
    unsigned char b1 = (value >> 24) & 0xFF;
    unsigned char b2 = (value >> 16) & 0xFF;
    unsigned char b3 = (value >> 8) & 0xFF;
    unsigned char b4 = value & 0xFF;
    return (b1 << 24) | (b2 << 16) | (b3 << 8) | b4;
}
void MainWindow::ConsoleDataReceived(){
    if (console) {
        static QByteArray accumulatedData;
        static qint32 expectedSize = -1;  // The expected size of the string message

        QByteArray data = socket->readAll();
        accumulatedData.append(data);

        qDebug() << "Accumulated data size:" << accumulatedData.size();

        if (expectedSize == -1 && accumulatedData.size() >= sizeof(qint32)) {
            unsigned int rawSize;
            memcpy(&rawSize, accumulatedData.data(), sizeof(rawSize));

            expectedSize = fromNetworkByteOrder(rawSize);

            accumulatedData.remove(0, sizeof(qint32));

            qDebug() << "Expected message size:" << expectedSize;
        }

        if (expectedSize != -1 && accumulatedData.size() >= expectedSize) {
            qDebug() << "Received message data size:" << accumulatedData.size();

            QByteArray messageData = accumulatedData.left(expectedSize);

            QString message = QString::fromUtf8(messageData);

            qDebug() << "Received message:" << message;

            ui->consoleOutput->setText(message);

            accumulatedData.remove(0, expectedSize);
            expectedSize = -1;
        }
    }
}
MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::TimerSlot(){
    ui->imageLabel->setPixmap(pixmap.scaled(ui->imageLabel->size(), Qt::KeepAspectRatio));
    timer->start(33);
}

void MainWindow::mainMenuButtonClicked(){
    QString message = QString("MENU");
    socket->write(message.toUtf8());


    qDebug() << "Disconnected from server!";
    ui->textEdit->append("Disconnected from server!");
    ui->textEdit->setVisible(false);
    ui->connectButton->setVisible(false);
    ui->imageLabel->setVisible(false);
    ui->lineEdit->setVisible(false);
    ui->backSpace->setVisible(false);
    ui->Enter->setVisible(false);
    ui->menu->setVisible(false);
    ui->sendButton->setVisible(false);
    ui->consoleInput->setVisible(false);
    ui->consoleOutput->setVisible(false);
    ui->consoleSend->setVisible(false);


    ui->Console->setVisible(true);
    ui->Desktop->setVisible(true);
}

void MainWindow::onEnterButtonClicked(){
    QString text = "ENTER";
    if (!text.isEmpty() && socket->state() == QAbstractSocket::ConnectedState) {
        socket->write(text.toUtf8());
        socket->flush();
        qDebug() << "Sent text to server:" << text;
    } else {
        qDebug() << "Not connected to server; cannot send text.";
    }
}

void MainWindow::onBackSpaceButtonClicked(){
    QString text = "BACK_SPACE";

    if (!text.isEmpty() && socket->state() == QAbstractSocket::ConnectedState) {
        socket->write(text.toUtf8());
        socket->flush();
        qDebug() << "Sent text to server:" << text;
    } else {
        qDebug() << "Not connected to server; cannot send text.";
    }
}

void MainWindow::onConnectButtonClicked()
{

    socket->connectToHost(ui->ip->text(), 8080);

    if (socket->waitForConnected()) {
        qDebug() << "Connected to server!";
        ui->textEdit->append("Connected to server!");

        ui->connectButton->setVisible(false);
        ui->textEdit->setVisible(false);
        ui->ip->setVisible(false);
        ui->ip->update();

        ui->Console->setVisible(true);
        ui->Desktop->setVisible(true);
    } else {
        qDebug() << "Connection failed!";
        ui->textEdit->append("Connection failed!");
    }
    ui->Console->update();

}

void MainWindow::onDataReceived() {
    if(!console){
        static QByteArray accumulatedData;
        static qint32 expectedSize = -1;

        QByteArray data = socket->readAll();
        accumulatedData.append(data);

        if (expectedSize == -1 && accumulatedData.size() >= sizeof(qint32)) {
            QDataStream stream(&accumulatedData, QIODevice::ReadOnly);
            stream >> expectedSize;
            accumulatedData.remove(0, sizeof(qint32));
            qDebug() << "Expected image size:" << expectedSize;
        }

        if (expectedSize != -1 && accumulatedData.size() >= expectedSize) {
            qDebug() << "Received image data size:" << accumulatedData.size();

            if (pixmap.loadFromData(accumulatedData.left(expectedSize))) {

                ui->imageLabel->setPixmap(pixmap.scaled(ui->imageLabel->size(), Qt::KeepAspectRatio));
                qDebug() << "Image loaded successfully.";
                timer->start(100);
                accumulatedData.remove(0, expectedSize);
                expectedSize = -1;

            }

        }
        updateImageLabelSize();
    }

}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    if (!console){
        QPoint clickPosition = event->pos();
        QRect labelRect = ui->imageLabel->geometry();

        if (labelRect.contains(clickPosition)) {
            int x = clickPosition.x() - labelRect.x();
            int y = clickPosition.y() - labelRect.y();
            QString message = QString("x: %1, y: %2").arg(x).arg(y);
            qDebug() << "Sending coordinates:" << message;

            if (socket->state() == QAbstractSocket::ConnectedState) {
                socket->write(message.toUtf8());
            }
        } else {
            QString message = QString( "");
            socket->write(message.toUtf8());
        }
    }
}

void MainWindow::onSendButtonClicked() {
    QString text = ui->lineEdit->text();

    if (!text.isEmpty() && socket->state() == QAbstractSocket::ConnectedState) {
        socket->write(text.toUtf8());
        socket->flush();
        qDebug() << "Sent text to server:" << text;

        ui->lineEdit->clear();
    } else if (text.isEmpty()) {
        qDebug() << "No text entered to send.";
    } else {
        qDebug() << "Not connected to server; cannot send text.";
    }
}

void MainWindow::resizeEvent(QResizeEvent* event) {
    QMainWindow::resizeEvent(event);
    updateImageLabelSize();
}

void MainWindow::updateImageLabelSize() {
    QScreen *screen = QGuiApplication::primaryScreen();
    QSize screenSize = screen->size();

    int targetHeight = static_cast<int>(screenSize.height() * 0.8);
    int targetWidth = targetHeight * pixmap.width() / pixmap.height();

    ui->imageLabel->setFixedSize(targetWidth, targetHeight);

    ui->imageLabel->move((width() - targetWidth) / 2, (height() - targetHeight) / 2);

    ui->imageLabel->setPixmap(pixmap.scaled(ui->imageLabel->size(), Qt::KeepAspectRatio));
}

void MainWindow::onOrientationChanged(Qt::ScreenOrientation orientation){
    updateImageLabelSize();
}
