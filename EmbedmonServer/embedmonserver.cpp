#include <QRandomGenerator> //RNG handler
#include <QMessageBox> //Send message
#include <QTouchEvent> //Touch screen handler
#include <QMovie> //Sprite .gif handler
#include <QTcpServer> //Server handler
#include <QTcpSocket> //Socket handler
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
    if (!server->listen(QHostAddress::Any, 9999)) {
        qDebug() << "Server could not start";
    } else {
        qDebug() << "Server started";
    }

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
    QMovie *player1Movie = new QMovie(":/sprites/FreezionIdle.gif");
    QMovie *player2Movie = new QMovie(":/sprites/IgnisIdle.gif");
    //Set movie to Qlabel objects
    player1Sprite->setMovie(player1Movie);
    player2Sprite->setMovie(player2Movie);
    //Start animation
    player1Movie->start();
    player2Movie->start();
    player1Sprite->move(130, 150);
    player2Sprite->move(260, 150);



    /* Pixmap implementation */
    //QPixmap player1Pixmap(":/sprites/ignis_sprite0.png");
    //QPixmap player2Pixmap(":/sprites/freezon_sprite0.png");

    // Set the pixmap to the QLabel objects
    //player1Sprite->setPixmap(player1Pixmap);
    //player2Sprite->setPixmap(player2Pixmap);

    // Position the sprites (adjust the coordinates as needed)
    //player1Sprite->move(140, 190);
    //player2Sprite->move(280, 190);

    // Frame initialization for updateSprites() function
    //player1Frame = 0;
    //player2Frame = 0;

    // Initialize and start the animation timer
    //QTimer *spriteTimer = new QTimer(this);
    //spriteTimer->setInterval(200);
    //connect(spriteTimer, &QTimer::timeout, this, &Widget::updateSprites);
    //spriteTimer->start();
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

        // Process the received data
        QString action(data);
        if (action == "lattackP2") {
            lattackP2_clicked();
        } else if (action == "sattackP2") {
            sattackP2_clicked();
        } else if (action == "blockP2") {
            blockP2_clicked();
        } else if (action == "potionP2") {
            potionP2_clicked();
        } else {
            qWarning() << "Unknown action received:" << action;
        }
        sendDataToClient();
    }
}

void Widget::sendDataToClient() {
    if (clientConnection && clientConnection->state() == QAbstractSocket::ConnectedState) {
        QByteArray data;
        QDataStream stream(&data, QIODevice::WriteOnly);
        stream << player1.health << player2.health;
        clientConnection->write(data);
    }
}


/* Possible Actions */
// Player 1 Actions
void Widget::lattackP1_clicked() {
    player1Action = Action::LightAttack;
    checkBothPlayersReady();
}

void Widget::sattackP1_clicked() {
    player1Action = Action::StrongAttack;
    checkBothPlayersReady();
}

void Widget::blockP1_clicked() {
    player1Action = Action::Block;
    checkBothPlayersReady();
}

void Widget::potionP1_clicked() {
    player1Action = Action::Potion;
    checkBothPlayersReady();
}

// Player 2 Actions
void Widget::lattackP2_clicked() {
    player2Action = Action::LightAttack;
    checkBothPlayersReady();
}

void Widget::sattackP2_clicked() {
    player2Action = Action::StrongAttack;
    checkBothPlayersReady();
}

void Widget::blockP2_clicked() {
    player2Action = Action::Block;
    checkBothPlayersReady();
}

void Widget::potionP2_clicked() {
    player2Action = Action::Potion;
    checkBothPlayersReady();
}

/* This function checks if both players have chosen their move before executing the round */
void Widget::checkBothPlayersReady() {
    if (player1Action != Action::None && player2Action != Action::None) {
        // Execute the round with the chosen actions.
        executeRound();

        // Reset the actions for the next round.
        player1Action = Action::None;
        player2Action = Action::None;
    }
}

/* This function executes the round, handles damage calculations */
void Widget::executeRound() {
    int player1Damage = 0;
    int player2Damage = 0;

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

    // Update health bars and check for game over.
    updateHealthBars();
    checkGameOver();
}

void Widget::updateHealthBars() {
    ui->healthbarP1->setValue(player1.health);
    ui->healthbarP2->setValue(player2.health);
}

void Widget::checkGameOver() {
    if (player1.health <= 0 || player2.health <= 0) {
        QMessageBox::information(this, "Game Over", "The game is over.");
        // Reset the game state or close the application.
    }
}
void Widget::updateSprites() {
    // Update player 1's sprite frame
    player1Frame = (player1Frame + 1) % 3;
    QPixmap player1Pixmap(QString(":/sprites/ignis_sprite%1.png").arg(player1Frame));
    player1Sprite->setPixmap(player1Pixmap);

    // Update player 2's sprite frame
    player2Frame = (player2Frame + 1) % 3;
    QPixmap player2Pixmap(QString(":/sprites/freezon_sprite%1.png").arg(player2Frame));
    player2Sprite->setPixmap(player2Pixmap);
}

Widget::~Widget()
{
    delete ui;
}
