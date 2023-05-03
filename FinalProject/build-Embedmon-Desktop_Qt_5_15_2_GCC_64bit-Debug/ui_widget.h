/********************************************************************************
** Form generated from reading UI file 'widget.ui'
**
** Created by: Qt User Interface Compiler version 5.15.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_WIDGET_H
#define UI_WIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QLabel>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Widget
{
public:
    QLabel *player1;
    QLabel *player2;
    QPushButton *lattackP1;
    QPushButton *sattackP1;
    QPushButton *blockP1;
    QPushButton *potionP1;
    QPushButton *sattackP2;
    QPushButton *lattackP2;
    QPushButton *blockP2;
    QPushButton *potionP2;
    QProgressBar *healthbarP1;
    QProgressBar *healthbarP2;

    void setupUi(QWidget *Widget)
    {
        if (Widget->objectName().isEmpty())
            Widget->setObjectName(QString::fromUtf8("Widget"));
        Widget->resize(480, 272);
        player1 = new QLabel(Widget);
        player1->setObjectName(QString::fromUtf8("player1"));
        player1->setGeometry(QRect(150, 80, 63, 20));
        QFont font;
        font.setFamily(QString::fromUtf8("MS Sans Serif"));
        font.setBold(true);
        player1->setFont(font);
        player2 = new QLabel(Widget);
        player2->setObjectName(QString::fromUtf8("player2"));
        player2->setGeometry(QRect(270, 80, 63, 20));
        player2->setFont(font);
        lattackP1 = new QPushButton(Widget);
        lattackP1->setObjectName(QString::fromUtf8("lattackP1"));
        lattackP1->setGeometry(QRect(10, 20, 83, 29));
        QFont font1;
        font1.setFamily(QString::fromUtf8("Tempus Sans ITC"));
        lattackP1->setFont(font1);
        sattackP1 = new QPushButton(Widget);
        sattackP1->setObjectName(QString::fromUtf8("sattackP1"));
        sattackP1->setGeometry(QRect(10, 60, 83, 29));
        sattackP1->setFont(font1);
        blockP1 = new QPushButton(Widget);
        blockP1->setObjectName(QString::fromUtf8("blockP1"));
        blockP1->setGeometry(QRect(10, 100, 83, 29));
        blockP1->setFont(font1);
        potionP1 = new QPushButton(Widget);
        potionP1->setObjectName(QString::fromUtf8("potionP1"));
        potionP1->setGeometry(QRect(10, 140, 83, 29));
        potionP1->setFont(font1);
        sattackP2 = new QPushButton(Widget);
        sattackP2->setObjectName(QString::fromUtf8("sattackP2"));
        sattackP2->setGeometry(QRect(380, 60, 83, 29));
        sattackP2->setFont(font1);
        lattackP2 = new QPushButton(Widget);
        lattackP2->setObjectName(QString::fromUtf8("lattackP2"));
        lattackP2->setGeometry(QRect(380, 20, 83, 29));
        lattackP2->setFont(font1);
        blockP2 = new QPushButton(Widget);
        blockP2->setObjectName(QString::fromUtf8("blockP2"));
        blockP2->setGeometry(QRect(380, 100, 83, 29));
        blockP2->setFont(font1);
        potionP2 = new QPushButton(Widget);
        potionP2->setObjectName(QString::fromUtf8("potionP2"));
        potionP2->setGeometry(QRect(380, 140, 83, 29));
        potionP2->setFont(font1);
        healthbarP1 = new QProgressBar(Widget);
        healthbarP1->setObjectName(QString::fromUtf8("healthbarP1"));
        healthbarP1->setGeometry(QRect(110, 110, 118, 23));
        healthbarP1->setFont(font1);
        healthbarP1->setAcceptDrops(false);
        healthbarP1->setAutoFillBackground(false);
        healthbarP1->setValue(100);
        healthbarP2 = new QProgressBar(Widget);
        healthbarP2->setObjectName(QString::fromUtf8("healthbarP2"));
        healthbarP2->setGeometry(QRect(250, 110, 118, 23));
        healthbarP2->setFont(font1);
        healthbarP2->setValue(100);

        retranslateUi(Widget);

        QMetaObject::connectSlotsByName(Widget);
    } // setupUi

    void retranslateUi(QWidget *Widget)
    {
        Widget->setWindowTitle(QCoreApplication::translate("Widget", "Widget", nullptr));
        player1->setText(QCoreApplication::translate("Widget", "Player 1", nullptr));
        player2->setText(QCoreApplication::translate("Widget", "Player 2", nullptr));
        lattackP1->setText(QCoreApplication::translate("Widget", "LAttack P1", nullptr));
        sattackP1->setText(QCoreApplication::translate("Widget", "SAttack P1", nullptr));
        blockP1->setText(QCoreApplication::translate("Widget", "Block P1", nullptr));
        potionP1->setText(QCoreApplication::translate("Widget", "Potion P1", nullptr));
        sattackP2->setText(QCoreApplication::translate("Widget", "SAttack P2", nullptr));
        lattackP2->setText(QCoreApplication::translate("Widget", "LAttack P2", nullptr));
        blockP2->setText(QCoreApplication::translate("Widget", "Block P2", nullptr));
        potionP2->setText(QCoreApplication::translate("Widget", "Potion P2", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Widget: public Ui_Widget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_WIDGET_H
