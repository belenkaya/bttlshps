#include "new_game.h"
#include "ui_new_game.h"
#include <utility>
#include <qmessagebox.h>
#include <random>
#include <chrono>
#include <vector>
#include <set>
#include <algorithm>
bool operator<(const Point& lhs, const Point& rhs){
    return (lhs.x < rhs.x) || (lhs.y < rhs.y);
}
bool cmp(const Point& a, const Point& b) {
    return (a.x == b.x && a.y == b.y);
}
Battleships GenVec(){
    int t1, t2;
    Battleships b;
    b.resize(10);
    for (int i = 0; i < 10; i++) {
        b[i].resize(10, false);
    }
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::mt19937 generator(seed);  // mt19937 is a standard mersenne_twister_engine
    int s = 0;
    while (s != 20) {
        t1 = generator() % 10;
        t2 = generator() % 10;
        if (b[t1][t2]) {
            continue;
        } else {
            b[t1][t2] = true;
            s++;
        }
    }

    return b;
}
NewGame::NewGame(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewGame)
{
    ui->setupUi(this);
    player_1_.resize(10);
    player_2_.resize(10);
    QVector<bool> temp(10, false);
    for(int i = 0; i < 10; i++) {
        player_1_[i] = temp;
        player_2_[i] = temp;
    }
//    QTableWidgetItem* proto  = new QTableWidgetItem("");
//    proto->setTextAlignment(Qt::AlignCenter);
//    ui->tableWidget->setItemPrototype(proto);
//    for (int i = 0; i < ui->tableWidget->rowCount(); i++) {
//        for (int j = 0; j < ui->tableWidget->columnCount(); j++) {
//            ui->tableWidget->setItem(i, j, new QTableWidgetItem(""));
//        }
//    }
}

NewGame::~NewGame()
{
    delete ui;
}

QString NewGame::GetPlayer1Name() const
{
    if (player_1_name_.size()) {
        return player_1_name_;
    } else {
        return "player 1";
    }
}
QString NewGame::GetPlayer2Name() const
{
    if (player_2_name_.size()) {
        return player_2_name_;
    } else {
        return "player 2";
    }
}

Battleships NewGame::GetShips(int player) const
{
    //assert(player==1||player==2);

    switch (player) {
    case 1:
        return player_1_;
        break;
    case 2:
        return player_2_;
        break;
    }
}

bool NewGame::CheckShip(const Battleships & vec)
{
    bool res = true;
    int counter = 0;
    for (const auto& v : vec) {
        for (const auto i : v) {
            counter += ( i ? 1 : 0 );
        }
    }
    if (counter < 20) {
        QMessageBox msg;
        msg.setWindowTitle("You have not enough ships");
        msg.setText("Place more ships");
        msg.setStandardButtons(QMessageBox::Ok);
        msg.setStyleSheet("QLabel{min-width: 170px;}");
        msg.exec();
        //QMessageBox::information(this, "You have not enough ships", "Place more ships", QMessageBox::Ok);
        return false;
    }
    if (counter > 20) {
        //QMessageBox::information(this, "You have too much ships", "Delete some ships", QMessageBox::Ok);
        QMessageBox msg;
        msg.setWindowTitle("You have too much ships");
        msg.setText("Delete some ships");
        msg.setStandardButtons(QMessageBox::Ok);
        msg.setStyleSheet("QLabel{min-width: 170px;}");
        msg.exec();
        return false;
    }
    return res;
}
void NewGame::on_radioButton_2_toggled(bool checked)//vs who
{
    if (checked) {//person
        ui->spinBox->setEnabled(0);
        ui->lineEdit_2->setEnabled(1);
        if (state_ == NewGameState::PLAYER1_READY) {
            ui->pushButton->setText("Save and start");
        } else {
            ui->pushButton->setText("Save");
        }
        ui->radioButton_3->setEnabled(1);
        ui->radioButton_4->setEnabled(1);
    } else {
        ui->spinBox->setEnabled(1);
        ui->lineEdit_2->setEnabled(0);
        if (state_ == NewGameState::PLAYER1_READY) {
            ui->pushButton->setText("Start");
        } else {
            ui->pushButton->setText("Save and start");
        }
        ui->radioButton_3->setEnabled(0);
        ui->radioButton_4->setEnabled(0);
        ui->radioButton_4->setChecked(1);
    }
}


void NewGame::on_lineEdit_textChanged(const QString &arg1)
{
    if (arg1 != GetPlayer1Name()) {
        player_1_name_ = ui->lineEdit->text();
        if (state_ == NewGameState::ONE_PLAYER) {
            ui->label_4->setText(GetPlayer1Name());
        }

    }
}

void NewGame::on_lineEdit_2_textChanged(const QString &arg1)
{
    if (arg1 != GetPlayer2Name()) {
        player_2_name_ = ui->lineEdit_2->text();
        if (state_ == NewGameState::TWO_PLAYERS || state_ == NewGameState::PLAYER1_READY) {
            ui->label_4->setText(GetPlayer2Name());
        }
    }
}

void NewGame::on_pushButton_clicked()//save btn
{
    switch (state_) {
        case NewGameState::ONE_PLAYER:
            if (CheckShip(GetShips(1))) {
                ui->radioButton_3->setEnabled(0);
                ui->radioButton_4->setEnabled(0);
                if (ui->radioButton_2->isChecked()) {
                    state_ = NewGameState::PLAYER1_READY;
                    //emit SendGameState(static_cast<int>(state_));
                    ui->tableWidget->clear();
                    QMessageBox::information(this, "You have to turn away", GetPlayer2Name() + " time to place his ships", QMessageBox::Ok);
                    ui->label_4->setText(GetPlayer2Name());

                    emit SendShip(player_1_, 1);
                    ui->pushButton->setText("Save and start");
                } else {
                    //start vs computer
                    //generate computer
                    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
                    std::mt19937 generator(seed);  // mt19937 is a standard mersenne_twister_engine
                    QVector<bool> temp(10, false);
                    for(int i = 0; i < 10; i++) {
                        player_2_[i] = temp;
                    }
                    player_2_ = GenVec();

                    int cheater_ = ui->spinBox->value();
                    std::string str = "computer" + std::to_string(cheater_);
                    emit SendShip(player_2_, 2);
                    emit SendName(QString::fromStdString(str), 2);
                    emit SendShip(player_1_, 1);
                    emit SendName(GetPlayer1Name(), 1);
                    emit SendGameState(1);
                    this->close();
                }
            }
            break;
        case NewGameState::PLAYER1_READY:
            if (ui->radioButton_2->isChecked()) {
                //goto case NewGameState::TWO_PLAYERS
            } else {//computer
                //generate computer
                int cheater_ = ui->spinBox->value();
                std::string str = "computer" + std::to_string(cheater_);
                unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
                std::mt19937 generator(seed);  // mt19937 is a standard mersenne_twister_engine
                QVector<bool> temp(10, false);
                for(int i = 0; i < 10; i++) {
                    player_2_[i] = temp;
                }
                player_2_= GenVec();

                emit SendShip(player_2_, 2);
                emit SendName(QString::fromStdString(str), 2);
                emit SendName(GetPlayer1Name(), 1);
                emit SendGameState(1);
                //emit SendShip(player_1_, 1);
             this->hide();
                break;
            }
        case NewGameState::TWO_PLAYERS:
            if (CheckShip(GetShips(2))) {
                ui->tableWidget->clear();
//                for (int i = 0; i < ui->tableWidget->rowCount(); i++) {
//                    for (int j = 0; j < ui->tableWidget->columnCount(); j++) {
//                        //player_1_[i][j] = ui->tableWidget->item(i, j)->text().size();
//                        ui->tableWidget->setItem(i, j, new QTableWidgetItem(""));
//                    }
//                }
                QMessageBox::information(this, "", "the game is about to start", QMessageBox::Ok);

                emit SendName(GetPlayer1Name(), 1);
                emit SendShip(player_2_, 2);
                emit SendName(GetPlayer2Name(), 2);
                emit SendGameState(2);
                this->hide();
                //start 1v1

            }
            break;
    }

}

void NewGame::on_pushButton_2_clicked()//get random ships
{
    switch (rules_) {
        case RuleSet::EASY:{
            QVector<bool> temp(10, false);
            for(int i = 0; i < 10; i++) {
                switch (state_) {
                    case NewGameState::ONE_PLAYER:
                        player_1_[i] = temp;
                        break;
                    case NewGameState::PLAYER1_READY:
                        player_2_[i] = temp;
                        break;
                }
            }
            ui->tableWidget->clear();

            //for(int k = 0; k < 20; k++) {
                switch (state_) {
                    case NewGameState::ONE_PLAYER:
                        player_1_ = GenVec();
                        break;
                    case NewGameState::PLAYER1_READY:
                        player_2_ = GenVec();
                        break;
                }

            for (int i = 0; i < 10; i++) {
                for (int j = 0; j < 10; j++ ){
                    switch (state_) {
                        case NewGameState::ONE_PLAYER:
                            if (player_1_[i][j]) {

                                QTableWidgetItem* proto  = new QTableWidgetItem("X");
                                proto->setTextAlignment(Qt::AlignCenter);
                                ui->tableWidget->setItem(i, j, proto);

                            }
                            break;
                        case NewGameState::PLAYER1_READY:
                            if (player_2_[i][j]) {
                                QTableWidgetItem* proto  = new QTableWidgetItem("X");
                                proto->setTextAlignment(Qt::AlignCenter);
                                ui->tableWidget->setItem(i, j, proto);
                            }
                            break;
                    }

                }

            }
            break;}
        case RuleSet::CLASSICAL:
        //todo
            break;
    }
}

void NewGame::on_radioButton_4_toggled(bool checked)//rules
{
    if (checked) {
        rules_ = RuleSet::EASY;
        ui->pushButton_2->setEnabled(1);
        ui->radioButton->setEnabled(1);
        ui->radioButton_2->setEnabled(1);
        //ui->radioButton_2->setChecked(1);
    } else {
        rules_ = RuleSet::CLASSICAL;
        ui->pushButton_2->setEnabled(0);
        ui->radioButton->setEnabled(0);
        ui->radioButton_2->setEnabled(0);
        ui->radioButton_2->setChecked(1);
    }
}

void NewGame::on_tableWidget_cellDoubleClicked(int row, int column)
{
    switch (state_) {
        case NewGameState::ONE_PLAYER:
            if (player_1_[row][column]) {
                player_1_[row][column] = false;
                QTableWidgetItem* proto  = new QTableWidgetItem("");
                proto->setTextAlignment(Qt::AlignCenter);
                ui->tableWidget->setItem(row, column, proto);
            } else {
                player_1_[row][column] = true;
                QTableWidgetItem* proto  = new QTableWidgetItem("X");
                proto->setTextAlignment(Qt::AlignCenter);
                ui->tableWidget->setItem(row, column, proto);
            }
            break;
        case NewGameState::PLAYER1_READY:
            if (player_2_[row][column]) {
                player_2_[row][column] = false;
                QTableWidgetItem* proto  = new QTableWidgetItem("");
                proto->setTextAlignment(Qt::AlignCenter);
                ui->tableWidget->setItem(row, column, proto);
            } else {
                player_2_[row][column] = true;
                QTableWidgetItem* proto  = new QTableWidgetItem("X");
                proto->setTextAlignment(Qt::AlignCenter);
                ui->tableWidget->setItem(row, column, proto);
            }
            break;
        case NewGameState::TWO_PLAYERS:
            break;

    }
}
