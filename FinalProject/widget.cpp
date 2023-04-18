#include <QRandomGenerator>
#include <QMessageBox>
#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    /*Declare player actions*/
    player1Action = Action::None;
    player2Action = Action::None;

    /*Manual signal-slot connection */
    connect(ui->lattackP1, &QPushButton::clicked, this, &Widget::lattackP1_clicked);
    connect(ui->sattackP1, &QPushButton::clicked, this, &Widget::sattackP1_clicked);
    connect(ui->blockP1, &QPushButton::clicked, this, &Widget::blockP1_clicked);
    connect(ui->potionP1, &QPushButton::clicked, this, &Widget::potionP1_clicked);
    connect(ui->lattackP2, &QPushButton::clicked, this, &Widget::lattackP2_clicked);
    connect(ui->sattackP2, &QPushButton::clicked, this, &Widget::sattackP2_clicked);
    connect(ui->blockP2, &QPushButton::clicked, this, &Widget::blockP2_clicked);
    connect(ui->potionP2, &QPushButton::clicked, this, &Widget::potionP2_clicked);

    // Initialize QLabel pointers for the sprites
    player1Sprite = new QLabel(this);
    player2Sprite = new QLabel(this);

    // Load the first frame for each sprite
    QPixmap player1Pixmap(":/sprites/ignis_sprite0.png");
    QPixmap player2Pixmap(":/sprites/freezon_sprite0.png");

    // Set the pixmap to the QLabel objects
    player1Sprite->setPixmap(player1Pixmap);
    player2Sprite->setPixmap(player2Pixmap);

    // Position the sprites (adjust the coordinates as needed)
    player1Sprite->move(140, 190);
    player2Sprite->move(280, 190);

    // Frame initialization for updateSprites() function
    player1Frame = 0;
    player2Frame = 0;

    // Initialize and start the animation timer
    QTimer *spriteTimer = new QTimer(this);
    spriteTimer->setInterval(200);
    connect(spriteTimer, &QTimer::timeout, this, &Widget::updateSprites);
    spriteTimer->start();
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
