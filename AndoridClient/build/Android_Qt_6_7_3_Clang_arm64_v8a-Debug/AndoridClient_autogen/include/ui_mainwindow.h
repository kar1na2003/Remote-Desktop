/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.7.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QPushButton *backSpace;
    QLineEdit *ip;
    QPushButton *Console;
    QPushButton *connectButton;
    QLabel *imageLabel;
    QPushButton *Desktop;
    QPushButton *consoleSend;
    QPushButton *Enter;
    QPushButton *sendButton;
    QPushButton *menu;
    QTextEdit *textEdit;
    QTextEdit *consoleOutput;
    QLineEdit *consoleInput;
    QLineEdit *lineEdit;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(1056, 467);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        backSpace = new QPushButton(centralwidget);
        backSpace->setObjectName("backSpace");
        backSpace->setGeometry(QRect(790, 200, 31, 31));
        ip = new QLineEdit(centralwidget);
        ip->setObjectName("ip");
        ip->setGeometry(QRect(360, 120, 113, 21));
        Console = new QPushButton(centralwidget);
        Console->setObjectName("Console");
        Console->setGeometry(QRect(170, 120, 171, 141));
        connectButton = new QPushButton(centralwidget);
        connectButton->setObjectName("connectButton");
        connectButton->setGeometry(QRect(360, 190, 111, 71));
        imageLabel = new QLabel(centralwidget);
        imageLabel->setObjectName("imageLabel");
        imageLabel->setGeometry(QRect(30, 30, 341, 231));
        Desktop = new QPushButton(centralwidget);
        Desktop->setObjectName("Desktop");
        Desktop->setGeometry(QRect(500, 120, 171, 141));
        consoleSend = new QPushButton(centralwidget);
        consoleSend->setObjectName("consoleSend");
        consoleSend->setGeometry(QRect(670, 280, 111, 51));
        Enter = new QPushButton(centralwidget);
        Enter->setObjectName("Enter");
        Enter->setGeometry(QRect(790, 240, 31, 31));
        sendButton = new QPushButton(centralwidget);
        sendButton->setObjectName("sendButton");
        sendButton->setGeometry(QRect(670, 200, 111, 71));
        menu = new QPushButton(centralwidget);
        menu->setObjectName("menu");
        menu->setGeometry(QRect(670, 50, 111, 24));
        textEdit = new QTextEdit(centralwidget);
        textEdit->setObjectName("textEdit");
        textEdit->setGeometry(QRect(360, 160, 111, 21));
        consoleOutput = new QTextEdit(centralwidget);
        consoleOutput->setObjectName("consoleOutput");
        consoleOutput->setGeometry(QRect(230, 120, 391, 201));
        consoleInput = new QLineEdit(centralwidget);
        consoleInput->setObjectName("consoleInput");
        consoleInput->setGeometry(QRect(230, 50, 391, 41));
        lineEdit = new QLineEdit(centralwidget);
        lineEdit->setObjectName("lineEdit");
        lineEdit->setGeometry(QRect(670, 170, 113, 24));
        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 1056, 21));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName("statusbar");
        MainWindow->setStatusBar(statusbar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
        backSpace->setText(QCoreApplication::translate("MainWindow", "<-", nullptr));
        Console->setText(QCoreApplication::translate("MainWindow", "Console", nullptr));
        connectButton->setText(QCoreApplication::translate("MainWindow", "Connect", nullptr));
        imageLabel->setText(QCoreApplication::translate("MainWindow", "TextLabel", nullptr));
        Desktop->setText(QCoreApplication::translate("MainWindow", "Desktop", nullptr));
        consoleSend->setText(QCoreApplication::translate("MainWindow", "Send", nullptr));
        Enter->setText(QCoreApplication::translate("MainWindow", "->", nullptr));
        sendButton->setText(QCoreApplication::translate("MainWindow", "Send ", nullptr));
        menu->setText(QCoreApplication::translate("MainWindow", "Menu", nullptr));
        consoleInput->setText(QCoreApplication::translate("MainWindow", "dir", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
