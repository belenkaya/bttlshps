#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <qdebug.h>
#include <qmessagebox.h>
#include <set>
#include <vector>
#include <random>
#include <chrono>
#include <thread>
#include <future>



MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->actionexit, SIGNAL(triggered()), this, SLOT(close()));
    connect(ui->actionnew_game, SIGNAL(triggered()), this, SLOT(OpenNewGame()));
    mov1_.resize(10);
    for (int i = 0; i < 10; i++) {
        mov1_[i].resize(10, false);
    }
    mov2_.resize(10);
    for (int i = 0; i < 10; i++) {
        mov2_[i].resize(10, false);
    }

    MainWindow::setFixedSize(871, 490);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::OpenNewGame()
{
    if (current_state_ == GameState::GAME_VS_COMPUTER || current_state_ == GameState::GAME_VS_PERSON) {
        QMessageBox::StandardButton reply;
          reply = QMessageBox::question(this, "Game is in progress", "You want to restart?",
                                        QMessageBox::Yes|QMessageBox::No);
          if (reply == QMessageBox::Yes) {
              ui->tableWidget->clear();
              ui->tableWidget_2->clear();
              current_state_ = GameState::BEFORE_GAME;
              new_game_ = new NewGame();
              connect(new_game_, SIGNAL(SendGameState(int)), this, SLOT(GetState(int)));
              connect(new_game_, SIGNAL(SendShip(Battleships&, int)), this, SLOT(GetShip(Battleships&,int)));
              connect(new_game_, SIGNAL(SendName(QString, int)), this, SLOT(GetName(QString,int)));
              new_game_->isModal();
              new_game_->show();
          }
    } else {
        new_game_ = new NewGame();
        connect(new_game_, SIGNAL(SendGameState(int)), this, SLOT(GetState(int)));
        connect(new_game_, SIGNAL(SendShip(Battleships&, int)), this, SLOT(GetShip(Battleships&,int)));
        connect(new_game_, SIGNAL(SendName(QString, int)), this, SLOT(GetName(QString,int)));
        new_game_->isModal();
        new_game_->show();
    }

    //qDebug() << "test";
}

void MainWindow::GetState(int i)
{
    num_players_ = i;
    current_state_ = static_cast<GameState>(i);
    switch (i) {
        case 1:
            StartGameComputer();
            break;
        case 2:
            StartGamePerson();
            break;
    }

}

void MainWindow::GetShip(Battleships & ship, int i)
{
    switch(i) {
        case 1:
            player_1_ = std::move(ship);
            break;
        case 2:
            player_2_ = std::move(ship);
            break;
    }
}

void MainWindow::GetName(QString str, int i)
{
    switch(i) {
        case 1:
            player1_name_ = std::move(str);
            break;
        case 2:
            player2_name_ = std::move(str);

            break;
    }
}

Point MainWindow::Peek()
{
    for (int i = 0; i < 10; ++i) {
        for (int j = 0; j < 10; j++) {
            if (player_1_[i][j]) {
                return {i, j};
            }
        }
    }
    //return {};
}

Point MainWindow::MakeMove()
{

    int t1, t2;
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::mt19937 generator(seed);  // mt19937 is a standard mersenne_twister_engine
    while (1) {
        t1 = generator() % 10;
        t2 = generator() % 10;
        if (mov2_[t1][t2]) {
            continue;
        } else {
            mov2_[t1][t2] = true;
            break;
        }
    }
    return {t1, t2};
}

void MainWindow::Congaratulate(int i)
{
    QString str;
    if (i == 1) {
        str = player1_name_;
    } else {
        str = player2_name_;
    }
    QMessageBox msg;
    msg.setWindowTitle("Congratulatoins!");
    msg.setText("Gratz on your win," + str);
    msg.setStandardButtons(QMessageBox::Ok);
    msg.setStyleSheet("QLabel{min-width: 200px;}");
    msg.exec();
}

void MainWindow::StartGameComputer()
{
    qDebug() << "vs comp";

    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 10; j++) {
            mov1_[i][j] = false;
            mov2_[i][j] = false;
        }
    }
    move_count_ = 0;
    alive1_ = 20;
    alive2_ = 20;
    //cheater_ =
    QString tmp = player2_name_[player2_name_.size()-1];
    cheater_ = tmp.toInt();
    qDebug() << cheater_;
    QMessageBox::information(this, "", player1_name_ + " moves first", QMessageBox::Ok);
    ui->label_4->setText(player1_name_);
    ui->label_5->setText(player2_name_);
    std::string str(std::to_string(alive1_) + " : " + std::to_string(alive2_));
    QString qstr = QString::fromStdString(str);
    ui->label_6->setText(qstr);
    ui->tableWidget->setEnabled(1);
    ui->tableWidget_2->setEnabled(1);
    ui->statusbar->showMessage(player1_name_ +" attacks "+ player2_name_);
}

void MainWindow::StartGamePerson()
{
    qDebug() << "vs person";
    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 10; j++) {
            mov1_[i][j] = false;
            mov2_[i][j] = false;
        }
    }

    move_count_ = 0;
    alive1_ = 20;
    alive2_ = 20;
    std::string str(std::to_string(alive1_) + " : " + std::to_string(alive2_));
    QString qstr = QString::fromStdString(str);
    ui->label_6->setText(qstr);
    QMessageBox::information(this, "", player1_name_ + " moves first", QMessageBox::Ok);
    ui->label_4->setText(player1_name_);
    ui->label_5->setText(player2_name_);
    ui->tableWidget_2->setEnabled(1);
    ui->statusbar->showMessage(player1_name_ +" attacks "+ player2_name_);
}

void MainWindow::on_tableWidget_cellDoubleClicked(int row, int column)
{
    //left p2 moves
    //qDebug() << row<<" " << column;
    if (current_state_ == GameState::GAME_VS_PERSON) {
        //
        if (!mov2_[row][column]) {//there wasnt such move before
            move_count_++;
            if (player_1_[row][column]) {//on target
                player_1_[row][column] = false;
                alive1_--;
                std::string str(std::to_string(alive1_) + " : " + std::to_string(alive2_));
                QString qstr = QString::fromStdString(str);
                ui->label_6->setText(qstr);
                QTableWidgetItem* proto  = new QTableWidgetItem("X");
                proto->setTextAlignment(Qt::AlignCenter);
                ui->tableWidget->setItem(row, column, proto);
                if (!HasAliveShip(player_1_)) {
                    //end game
                    Congaratulate(2);
                    qDebug() << player2_name_<< " won!";
                    ui->statusbar->showMessage(player2_name_ +" won on move " + QString::number(move_count_));
                    ui->tableWidget->setEnabled(0);
                    current_state_ = GameState::BEFORE_GAME;
                }
            } else { //missed
                //player_2_[row][column] = false;
                current_move_ = Move::PLAYER1;
                ui->tableWidget->setEnabled(0);
                ui->tableWidget_2->setEnabled(1);
                ui->statusbar->showMessage(player1_name_ +" attacks "+ player2_name_);
                QTableWidgetItem* proto  = new QTableWidgetItem("0");
                proto->setTextAlignment(Qt::AlignCenter);
                ui->tableWidget->setItem(row, column, proto);
            }
            mov2_[row][column] = true;
        }
    }
}

void MainWindow::on_tableWidget_2_cellDoubleClicked(int row, int column)
{
    //right//p1 moves
    //qDebug() << row<<" " << column;
    if (current_state_ == GameState::GAME_VS_PERSON) {
        //logic

        if (!mov1_[row][column]) {//there wasnt such move before
            move_count_++;
            if (player_2_[row][column]) {//on target
                player_2_[row][column] = false;
                alive2_--;
                std::string str(std::to_string(alive1_) + " : " + std::to_string(alive2_));
                QString qstr = QString::fromStdString(str);
                ui->label_6->setText(qstr);
                QTableWidgetItem* proto  = new QTableWidgetItem("X");
                proto->setTextAlignment(Qt::AlignCenter);
                ui->tableWidget_2->setItem(row, column, proto);
                if (!HasAliveShip(player_2_)) {
                    //end game
                    Congaratulate(1);
                    qDebug() << player1_name_<< " won!";
                    ui->statusbar->showMessage(player1_name_ +" won on move " + QString::number(move_count_));
                    ui->tableWidget_2->setEnabled(0);
                    current_state_ = GameState::BEFORE_GAME;
                }
            } else { //missed
                //player_2_[row][column] = false;
                current_move_ = Move::PLAYER2;
                ui->tableWidget->setEnabled(1);
                ui->tableWidget_2->setEnabled(0);
                ui->statusbar->showMessage(player2_name_ +" attacks "+ player1_name_);
                QTableWidgetItem* proto  = new QTableWidgetItem("0");
                proto->setTextAlignment(Qt::AlignCenter);
                ui->tableWidget_2->setItem(row, column, proto);
            }
            mov1_[row][column] = true;
        }
    }
    if (current_state_ == GameState::GAME_VS_COMPUTER) {
        if (!mov1_[row][column]) {//there wasnt such move before
            move_count_++;
            mov1_[row][column] = true;
            if (player_2_[row][column]) {//human on target
                player_2_[row][column] = false;
                alive2_--;
                std::string str(std::to_string(alive1_) + " : " + std::to_string(alive2_));
                QString qstr = QString::fromStdString(str);
                ui->label_6->setText(qstr);
                QTableWidgetItem* proto  = new QTableWidgetItem("X");
                proto->setTextAlignment(Qt::AlignCenter);
                ui->tableWidget_2->setItem(row, column, proto);
                if (!HasAliveShip(player_2_)) {
                    //end game
                    //qDebug() << player1_name_<< " won!";
                    Congaratulate(1);
                    ui->statusbar->showMessage(player1_name_ +" won on move " + QString::number(move_count_));
                    ui->tableWidget_2->setEnabled(0);
                    ui->tableWidget->setEnabled(0);
                    current_state_ = GameState::BEFORE_GAME;
                }

            } else { //human missed
                QTableWidgetItem* proto  = new QTableWidgetItem("0");
                proto->setTextAlignment(Qt::AlignCenter);
                ui->tableWidget_2->setItem(row, column, proto);
                current_move_ = Move::PLAYER2;
                ui->statusbar->showMessage("Computer calculates his next move");
                auto p = MakeMove();//makes new move
                //cheat
                unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
                std::mt19937 generator(seed);  // mt19937 is a standard mersenne_twister_engine
                bool TrueFalse = (generator() % 100) < cheater_ * 5;
                if (TrueFalse) {
                    p = Peek();
                }
//                int tmp = (21 - alive2_) % (8 - cheater_);
//                if (tmp == 0) {
//                    p = Peek();
//                }

                move_count_++;
                while (player_1_[p.x][p.y]) {//on target

                    alive1_--;
                    std::string str(std::to_string(alive1_) + " : " + std::to_string(alive2_));
                    QString qstr = QString::fromStdString(str);
                    ui->label_6->setText(qstr);
                    player_1_[p.x][p.y] = false;
                    if (!HasAliveShip(player_1_)) {
                        //end game
                        //qDebug() << player1_name_<< " won!";
                        Congaratulate(2);
                        ui->statusbar->showMessage("Computer is superior, won on move " + QString::number(move_count_));
                        ui->tableWidget_2->setEnabled(0);
                        ui->tableWidget->setEnabled(0);

                        current_state_ = GameState::BEFORE_GAME;
                        break;
                    }
                    QTableWidgetItem* proto  = new QTableWidgetItem("X");
                    proto->setTextAlignment(Qt::AlignCenter);
                    ui->tableWidget->setItem(p.x, p.y, proto);
                    p = MakeMove();
                    ui->statusbar->showMessage("Computer calculates his next move");
                    move_count_++;
                }
                //computer missed
                QTableWidgetItem* proto2  = new QTableWidgetItem("0");
                proto2->setTextAlignment(Qt::AlignCenter);
                ui->tableWidget->setItem(p.x, p.y, proto2);
                current_move_ = Move::PLAYER1;
                ui->statusbar->clearMessage();
                ui->statusbar->showMessage(player1_name_ +" attacks "+ player2_name_);

            }
        }

    }
}

bool MainWindow::HasAliveShip(const Battleships & vec)
{
    int count = 0;
    bool res = false;
    for (const auto& v : vec) {
       for (bool b : v) {
           if (b) {
               count++;
               res = true;
           }
           if (count == 20) {
               return res;
           }
       }
    }
    return res;
}
