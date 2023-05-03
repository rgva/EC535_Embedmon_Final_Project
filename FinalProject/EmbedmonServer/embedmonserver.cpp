#include <QRandomGenerator> //RNG handler
#include <QMessageBox> //Send message
#include <QTouchEvent> //Touch screen handler
#include <QMovie> //Sprite .gif handler
#include <QTcpServer> //Server handler
#include <QTcpSocket> //Socket handler
/*Background Image Handler*/
#include <QPalette>
#include <QBrush>
/*Handle Battle Animations*/
#include <QPropertyAnimation>
#include <QTimer>
/* Header files */
#include "embedmonserver.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    /*Declare server*/
    server = new QTcpServer(this);
    connect(server, &QTcpServer::newConnection, this, &Widget::handleNewConnection);
    /*Listen for incoming connections to hard-coded port */
    if (!server->listen(QHostAddress::Any, 9999)) { //Change QHostAddress::LocalHost to Any before deploying to BBB
        qDebug() << "Server could not start";
    } else {
        qDebug() << "Server started";
    }

    /*Setup background image*/
    QPixmap backgroundPixmap(":/images/battlebackground2.png");
    QPalette palette;
    palette.setBrush(QPalette::Window, backgroundPixmap);
    this->setAutoFillBackground(true);
    this->setPalette(palette);

    /*Declare player actions*/
    player1Action = Action::None;
    player2Action = Action::None;

    /*Manual signal-slot connection */
    connect(ui->lattackP1, &QPushButton::clicked, this, &Widget::lattackP1_clicked);
    connect(ui->sattackP1, &QPushButton::clicked, this, &Widget::sattackP1_clicked);
    connect(ui->blockP1, &QPushButton::clicked, this, &Widget::blockP1_clicked);
    connect(ui->potionP1, &QPushButton::clicked, this, &Widget::potionP1_clicked);

    /*QMovie implementation */
    // Initialize QLabel pointers for the sprites
    player1Sprite = new QLabel(this);
    player2Sprite = new QLabel(this);
    //Load gif animations
    QMovie *player1Movie = new QMovie(":/sprites/RockyIdle_fr.gif");
    QMovie *player2Movie = new QMovie(":/sprites/FreezeIdle_fl.gif");
    //Set movie to Qlabel objects
    player1Sprite->setMovie(player1Movie);
    player2Sprite->setMovie(player2Movie);
    //Start animation
    player1Movie->start();
    player2Movie->start();
    player1Sprite->move(130, 150);
    player2Sprite->move(260, 150);

    /*Reset game*/
    connect(this, &Widget::gameReset, this, &Widget::resetGame);

}

/*Touch Screen Implementation */
bool Widget::event(QEvent *event) {
    if (event->type() == QEvent::TouchBegin ||
        event->type() == QEvent::TouchUpdate ||
        event->type() == QEvent::TouchEnd) {
        handleTouchEvent(static_cast<QTouchEvent *>(event));
        return true;
    }
    return QWidget::event(event);
}

void Widget::handleTouchEvent(QTouchEvent *event) {
    const QList<QTouchEvent::TouchPoint> &touchPoints = event->touchPoints();

    for (const QTouchEvent::TouchPoint &touchPoint : touchPoints) {
        QPointF pos = touchPoint.pos();

        if (touchPoint.state() & Qt::TouchPointPressed) {
            // Check if the touch point is within the bounding rectangle of each QPushButton
            if (ui->lattackP1->geometry().contains(pos.toPoint())) {
                ui->lattackP1->click();
            } else if (ui->sattackP1->geometry().contains(pos.toPoint())) {
                ui->sattackP1->click();
            } else if (ui->blockP1->geometry().contains(pos.toPoint())) {
                ui->blockP1->click();
            } else if (ui->potionP1->geometry().contains(pos.toPoint())) {
                ui->potionP1->click();
            }
        }
    }
}

/* Handler for server-client connection */
void Widget::handleNewConnection() {
    clientConnection = server->nextPendingConnection();

    // Connect the necessary signals and slots for data handling and disconnection
    connect(clientConnection, &QTcpSocket::readyRead, this, &Widget::handleDataFromClient);
    connect(clientConnection, &QTcpSocket::disconnected, clientConnection, &QTcpSocket::deleteLater);
}

void Widget::handleDataFromClient() {
    // Check if there is a valid connection
    QTcpSocket *clientConnection = qobject_cast<QTcpSocket *>(sender());
    if (clientConnection && clientConnection->state() == QAbstractSocket::ConnectedState) {
        // Read the data from the client
        QByteArray data = clientConnection->readAll();

        qDebug() << "Received data from client:" << data; // Add this debug statement

        QDataStream stream(&data, QIODevice::ReadOnly);
        int p2Action;
        stream >> p2Action;
        qDebug() << "Deserialized actions from client:" << p2Action;
        player2Action = static_cast<Action>(p2Action);

        updateHealthBars();
        checkGameOver();

        // Execute the round based on the received actions
        executeRound(player1Action, player2Action);

        // Send updated health and actions to the client
        sendDataToClient();
    }
}

void Widget::sendDataToClient() {
    if (clientConnection && clientConnection->state() == QAbstractSocket::ConnectedState) {
        QByteArray data;
        QDataStream stream(&data, QIODevice::WriteOnly);
        stream << player1.health << player2.health << static_cast<int>(player1Action);
        clientConnection->write(data);
        qDebug() << "Sending data to client:" << data;
    }
}


/* Possible Actions */
// Player 1 Actions
void Widget::lattackP1_clicked() {
    player1Action = Action::LightAttack;
    checkBothPlayersReady();
    //qDebug() << "Player 1 clicked Light Attack";
}

void Widget::sattackP1_clicked() {
    player1Action = Action::StrongAttack;
    checkBothPlayersReady();
    //qDebug() << "Player 1 clicked Strong Attack";

}

void Widget::blockP1_clicked() {
    player1Action = Action::Block;
    checkBothPlayersReady();
    //qDebug() << "Player 1 clicked Block";

}

void Widget::potionP1_clicked() {
    player1Action = Action::Potion;
    checkBothPlayersReady();
    //qDebug() << "Player 1 clicked Potion";

}

// Player 2 Actions
void Widget::lattackP2_clicked() {
    player2Action = Action::LightAttack;
    checkBothPlayersReady();
    //qDebug() << "Player 2 clicked Light Attack";
}

void Widget::sattackP2_clicked() {
    player2Action = Action::StrongAttack;
    checkBothPlayersReady();
    //qDebug() << "Player 2 clicked Strong Attack";
}

void Widget::blockP2_clicked() {
    player2Action = Action::Block;
    checkBothPlayersReady();
    //qDebug() << "Player 2 clicked Block";
}

void Widget::potionP2_clicked() {
    player2Action = Action::Potion;
    checkBothPlayersReady();
    //qDebug() << "Player 2 clicked Potion";
}

/* This function checks if both players have chosen their move before executing the round */
void Widget::checkBothPlayersReady() {
    if (player1Action != Action::None && player2Action != Action::None) {
        // Execute the round with the chosen actions.
        executeRound(player1Action, player2Action);

        // Reset the actions for the next round.
        player1Action = Action::None;
        player2Action = Action::None;
    }
}

/*Play the attack animation */
void Widget::playAttackAnimation(int player) {
    if (player == 1) {
        QMovie *player1AttackMovie = new QMovie(":/sprites/RockyAttack_fr.gif");
        player1Sprite->setMovie(player1AttackMovie);
        player1AttackMovie->start();
    } else if (player == 2) {
        QMovie *player2AttackMovie = new QMovie(":/sprites/FreezeAttack_fl.gif");
        player2Sprite->setMovie(player2AttackMovie);
        player2AttackMovie->start();
    }
    QTimer::singleShot(2000, this, &Widget::resetIdleAnimation);
}

void Widget::resetIdleAnimation() {
    QMovie *player1Movie = new QMovie(":/sprites/RockyIdle_fr.gif");
    QMovie *player2Movie = new QMovie(":/sprites/FreezeIdle_fl.gif");
    player1Sprite->setMovie(player1Movie);
    player2Sprite->setMovie(player2Movie);
    player1Movie->start();
    player2Movie->start();
}

/* This function executes the round, handles damage calculations */
void Widget::executeRound(Action player1Action, Action player2Action) {
    qDebug() << "Player 1 action:" << static_cast<int>(player1Action);
    qDebug() << "Player 2 action:" << static_cast<int>(player2Action);
    int player1Damage = 0;
    int player2Damage = 0;
    qDebug() << "Round is executing";
    // Handle player 1 actions.
    switch (player1Action) {
    case Action::LightAttack:
        if (rand() % 100 >= 10) { // 90% chance to hit
            player1Damage = 8 + rand() % 5; // Range from 8 - 12
        }
        break;
    case Action::StrongAttack:
        if (rand() % 100 >= 50) { // 50% chance to hit
            player1Damage = 15 + rand() % 11; //Range from 15 to 25
        }
        break;
    case Action::Block:
        break;
    case Action::Potion:
        player1.health = qMin(player1.health + 25, 100);
        break;
    default:
        break;
    }

    // Handle player 2 actions.
    switch (player2Action) {
    case Action::LightAttack:
        if (rand() % 100 >= 10) { // 90% chance to hit
            player2Damage = 8 + rand() % 5;
        }
        break;
    case Action::StrongAttack:
        if (rand() % 100 >= 50) { // 50% chance to hit
            player2Damage = 15 + rand() % 11;
        }
        break;
    case Action::Block:
        break;
    case Action::Potion:
        player2.health = qMin(player2.health + 25, 100);
        break;
    default:
        break;
    }

    // Apply block logic.
    if (player1Action == Action::Block) {
        player2Damage /= 2;
    }
    if (player2Action == Action::Block) {
        player1Damage /= 2;
    }

    // Apply damage to the players.
    player1.health -= player2Damage;
    player2.health -= player1Damage;

    // Hold health values to the [0, 100] range.
    player1.health = qMax(0, qMin(player1.health, 100));
    player2.health = qMax(0, qMin(player2.health, 100));

    // Delay rounds by 2 second, update health bars and check for game over.
    QTimer::singleShot(2000, this, [this](){
        updateHealthBars();
        checkGameOver();
    });
}

void Widget::updateHealthBars() {
    ui->healthbarP1->setValue(player1.health);
    ui->healthbarP2->setValue(player2.health);
}

void Widget::checkGameOver() {
    if (player1.health <= 0 || player2.health <= 0) {
        QString winner = player1.health <= 0 ? "Player 2" : "Player 1";
        QMessageBox gameOverMessageBox;
        gameOverMessageBox.setWindowTitle("Game Over");
        gameOverMessageBox.setText(winner + " wins!");
        gameOverMessageBox.setStandardButtons(QMessageBox::Ok);
        gameOverMessageBox.exec();

        emit gameReset();
    }
}


void Widget::resetGame() {
    // Reset the game state (e.g., health bars, player actions, etc.)
    player1.health = 100;
    player2.health = 100;
    ui->healthbarP1->setValue(player1.health);
    ui->healthbarP2->setValue(player2.health);

    // Send a game reset message to the client
    if (clientConnection && clientConnection->state() == QAbstractSocket::ConnectedState) {
        QByteArray data;
        QDataStream stream(&data, QIODevice::WriteOnly);
        stream << -1 << -1 ; // Send -1 as health values to indicate a game reset
        clientConnection->write(data);
        qDebug() << "Sending game reset data to client:" << data;
    }
}

Widget::~Widget()
{
    delete ui;
}
