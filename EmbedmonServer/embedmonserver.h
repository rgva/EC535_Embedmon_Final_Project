#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QRandomGenerator>
/*Sprite includes*/
#include <QLabel>
#include <QPixmap>
#include <QTimer>
/*Touch Functionality includes*/
#include <QTouchEvent>
/* Networking functionality includes */
#include <QTcpServer>
#include <QTcpSocket>
// Timer
#include <QTimer>

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

/* This class lists out all possible actions in a move */
enum class Action {
    None,
    LightAttack,
    StrongAttack,
    Block,
    Potion
};

/* This class defines character attributes */
class Character {
public:
    int health = 100;
    int healthPotion = 1;
    Action currentAction = Action::None;

    void setAction(Action action) {
        currentAction = action;
    }

    int lightAttack() {
        if (QRandomGenerator::global()->generateDouble() < 0.1) return 0; // 10% miss chance
        return QRandomGenerator::global()->bounded(8, 13); // 8-12 damage
    }

    int strongAttack() {
        if (QRandomGenerator::global()->generateDouble() < 0.5) return 0; // 50% miss chance
        return QRandomGenerator::global()->bounded(15, 26); // 15-25 damage
    }

    bool block() {
        return QRandomGenerator::global()->generateDouble() < 0.5; // 50% chance to increase miss chance
    }

    int useHealthPotion() {
        if (healthPotion > 0) {
            healthPotion--;
            return 25;
        }
        return 0;
    }
};


class Widget : public QWidget

{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();
public slots:
    void myfunction();

private:
    Ui::Widget *ui;
    /*Declare characters*/
    Character player1;
    Character player2;
    /*Character actions*/
    Action player1Action; //Player 1's action
    Action player2Action; //Player 2's action
    /*Slot functions for actions */
    void lattackP1_clicked();
    void sattackP1_clicked();
    void blockP1_clicked();
    void potionP1_clicked();
    void lattackP2_clicked();
    void sattackP2_clicked();
    void blockP2_clicked();
    void potionP2_clicked();
    /*Game mechanic functions*/
    void checkBothPlayersReady();
    void executeRound();
    void updateHealthBars();
    void checkGameOver();
    /*Sprite functions*/
    QLabel *player1Sprite;
    QLabel *player2Sprite;
    //Establishes delays between sprite frames
    QTimer *animationTimer;
    //Initialize the player sprite frames
    int player1Frame;
    int player2Frame;
    //Intialize networking
    QTcpServer *server;
    QTcpSocket *clientConnection;
    QTimer *timer; //Timer

private slots:
    //Function to update sprites
    void updateSprites();
    // Function to handle networking & data from clients
    void handleNewConnection();
    void handleDataFromClient();
    void sendDataToClient();

protected:
    bool event(QEvent *event) override;
    void handleTouchEvent(QTouchEvent *event);

};

#endif // WIDGET_H
