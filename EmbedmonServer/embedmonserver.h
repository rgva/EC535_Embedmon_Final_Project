#ifndef EMBEDMONSERVER_H
#define EMBEDMONSERVER_H

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

    int lightAttack();
    int strongAttack();
    bool block();
    int useHealthPotion();
};

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();
public slots:
    void playAttackAnimation(int player);
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
    void executeRound(Action player1Action, Action player2Action);
    void updateHealthBars();
    void checkGameOver();
    /*Sprite functions*/
    QLabel *player1Sprite;
    QLabel *player2Sprite;
    void resetIdleAnimation();
    //Establishes delays between sprite frames
    QTimer *animationTimer;
    //Initialize the player sprite frames
    int player1Frame;
    int player2Frame;
    //Intialize networking
    QTcpServer *server;
    QTcpSocket *clientConnection;

private slots:
    // Function to handle networking & data from clients
    void handleNewConnection();
    void handleDataFromClient();
    void sendDataToClient();
    void resetGame();

protected:
    bool event(QEvent *event) override;
    void handleTouchEvent(QTouchEvent *event);

signals:
    void gameReset();
};

#endif // EMBEDMONSERVER_H
