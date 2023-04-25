#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QRandomGenerator>
/*Sprite includes*/
#include <QLabel>
#include <QPixmap>
#include <QTimer>
#include <QMovie>
/*Touch Functionality includes*/
#include <QEvent>
#include <QTouchEvent>
/* Networking functionality includes */
#include <QTcpSocket>
#include <QAbstractSocket>
/* UI includes */
#include <QPushButton>
#include <QProgressBar>


QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE


class Character {
public:
    int health = 100;
    int healthPotion = 1;
};

class Widget : public QWidget


{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();
    void connectToServer(const QString &serverIP, quint16 serverPort); //Initialize erver connection

protected:
    bool event(QEvent *event) override;
    void handleTouchEvent(QTouchEvent *event);

private:
    Ui::Widget *ui;
    /*Declare characters */
    Character player1;
    Character player2;
    /*Slot functions for actions */
    void lattackP2_clicked();
    void sattackP2_clicked();
    void blockP2_clicked();
    void potionP2_clicked();
    /*Sprite functions*/
    QLabel *player1Sprite;
    QLabel *player2Sprite;
    //Intialize networking
    QTcpSocket *clientSocket;

    void sendDataToServer(const QByteArray &data);

private slots:
    //Function to update sprites
    // Function to handle networking
    void onConnected();
    void handleDataFromServer();
    void onError(QAbstractSocket::SocketError socketError);

};

#endif // WIDGET_H
