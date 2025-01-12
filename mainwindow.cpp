#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFile>
#include <QTextStream>
#include <QMessageBox>

using namespace std;

bool Check_numbers(vector<QLineEdit*> line_edit_vector, QRegularExpression regular_for_number, QRegularExpression regular_for_number_2, QString number_1, QVector<QString>& numbers){
    if(regular_for_number.match(number_1).hasMatch() or regular_for_number_2.match(number_1).hasMatch()){
        if(number_1 != ""){
            numbers.push_back(number_1);
        }
        for(int i = 0; i < line_edit_vector.size(); i++){
            if(regular_for_number.match(line_edit_vector[i]->text()).hasMatch() or regular_for_number_2.match(line_edit_vector[i]->text()).hasMatch()){
                if(line_edit_vector[i]->text() != ""){
                    numbers.push_back(line_edit_vector[i]->text());
                }
            }
            else{
                return false;
            }
        }
        return true;
    }
    else{
        return false;
    }
}
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //Чтение данных из файла
    // QFile file(path);
    // if(!file.open(QFile::ReadOnly | QFile::Text)){
    //     qDebug() << "Ошибка открытия файла для чтения";
    // }
    // else{
    //     qDebug() << "Файл открыт для чтения";

    //     QTextStream fin(&file);
    //     if(!fin.atEnd()){
    //         size_t size;
    //         fin >> size;
    //         for(int i = 0; i < size; i++){
    //             Human human;
    //             human.read(fin);
    //             book.emplace(human.second_name, human);
    //         }
    //     }
    //     qDebug() << "Данные считаны";
    // }
    // file.flush();
    // file.close();

    {
        QString second_name;
        QString first_name;
        QString third_name;
        QString address;
        QString date_of_birth;
        QString email;
        QSqlDatabase db;
        db = QSqlDatabase::addDatabase("QSQLITE", "Telephone_book");
        db.setDatabaseName("/Users/ilya/Documents/kursach_1/users_DB.db");
        if(db.open()){
            qDebug("open db for read");
        }
        else{
            qDebug("no open for read");
        }
        QSqlQuery query(db);
        QSqlQuery query_2(db);

        query.exec("SELECT Second_name, First_name, Third_name, Address, Date_of_birth, Email FROM Telephone_book;");
        while(query.next()){
            second_name = query.value(0).toString();
            first_name = query.value(1).toString();
            third_name = query.value(2).toString();
            address = query.value(3).toString();
            date_of_birth = query.value(4).toString();
            email = query.value(5).toString();

            query_2.prepare("SELECT number FROM Numbers WHERE second_name = :second_name;");
            query_2.bindValue(":second_name", second_name);
            query_2.exec();
            while(query_2.next()){
                numbers.push_back(query_2.value(0).toString());
            }
            Human human(first_name, second_name, third_name, address, date_of_birth, email, numbers);
            book.emplace(second_name, human);
            numbers.clear();
            human.Print();
        }

        db.close();
    }
    QSqlDatabase::removeDatabase("Telephone_book");
}

MainWindow::~MainWindow()
{
    QFile file(path);
    if(!file.open(QFile::WriteOnly | QFile::Text)){
        qDebug() << "Ошибка открытия файла для записи ";
    }
    else{
        qDebug() << "Файл открыт для записи";

        //Запись данных в файл
        QTextStream fout(&file);
        size_t l = book.size();
        fout << l << " ";
        for(auto element : book){
            element.second.write(fout);
        }
         qDebug() << "Данные записаны";
    }
    file.flush();
    file.close();

    delete ui;
}
//Добавление контакта
void MainWindow::on_pushButton_clicked()
{
    QRegularExpression regular_for_name("(^[A-Za-z]{1}[-0-9A-z\\.]{0,}[0-9A-Za-z]{1}$)"); //для фио
    QRegularExpression regular_for_number("(^((8|\\+7)[\\- ]?)?(\\(?\\d{3}\\)?[\\- ]?)?[\\d\\- ]{7,10}$)");// для телефона
    QRegularExpression regular_for_number_2("(^$)");// для удаления телефона
    QRegularExpression regular_for_email("(^((([0-9A-Za-z]{1}[-0-9A-z\\.]{1,}[0-9A-Za-z]{1}))@([-A-Za-z]{1,}\\.){1,2}[-A-Za-z]{2,})$)");// для email

    QString first_name = ui->first_name->text();
    QString second_name = ui->second_name->text();
    QString third_name = ui->third_name->text();
    QString address = ui->address->text();
    QString date_of_birth = ui->dateEdit->date().toString("dd.MM.yyyy");
    QString email = ui->email->text();
    QString number_1 = ui->phone->text();

    if(regular_for_name.match(first_name).hasMatch()){
        if(regular_for_name.match(second_name).hasMatch()){
            if(regular_for_name.match(third_name).hasMatch()){
                if(ui->dateEdit->date() < current_date){
                    if(regular_for_email.match(email).hasMatch()){
                        if(Check_numbers(line_edit_vector, regular_for_number, regular_for_number_2, number_1, numbers)){
                            //Запись в map
                            Human tmp(first_name, second_name, third_name, address, date_of_birth, email, numbers);
                            book.emplace(second_name, tmp);

                            //Запись в базу данных
                            {
                                QSqlDatabase db;
                                db = QSqlDatabase::addDatabase("QSQLITE", "Telephone_book");
                                db.setDatabaseName("/Users/ilya/Documents/kursach_1/users_DB.db");
                                if(db.open()){
                                    qDebug("open db for insert");
                                }
                                else{
                                    qDebug("no open for insert");
                                }
                                QSqlQuery query(db);

                                query.prepare("INSERT INTO Telephone_Book(Second_name, First_name, Third_name, Address,"
                                              " Date_of_birth, Email) VALUES(:Second_name, :First_name, :Third_name, "
                                              ":Address, :Date_of_birth, :Email);");
                                query.bindValue(":Second_name", second_name);
                                query.bindValue(":First_name", first_name);
                                query.bindValue(":Third_name", third_name);
                                query.bindValue(":Address", address);
                                query.bindValue(":Date_of_birth", date_of_birth);
                                query.bindValue(":Email", email);
                                query.exec();

                                for(int i = 0; i < numbers.size(); i++){
                                    query.prepare("INSERT INTO Numbers(second_name, number, id) VALUES(:Second_name, :Number, :id);");
                                    query.bindValue(":Second_name", second_name);
                                    query.bindValue(":Number", numbers[i]);
                                    query.bindValue(":id", i);
                                    query.exec();
                                }

                                db.close();
                            }
                            QSqlDatabase::removeDatabase("Telephone_book");

                            ui->statusbar->showMessage("Контакт зарегистрирован!");

                            //отчистка полей ввода
                            ui->first_name->clear();
                            ui->second_name->clear();
                            ui->third_name->clear();
                            ui->dateEdit->setDate(QDate::fromString("01.01.2000","dd.MM.yyyy"));
                            ui->address->clear();
                            ui->email->clear();
                            ui->phone->clear();
                            ui->lineEdit->clear();

                            for(int i = 0; i < line_edit_vector.size(); i++){
                                line_edit_vector[i]->clear();
                                delete line_edit_vector[i];
                                delete layout_vector[i];
                                delete label_vector[i];
                            }

                            line_edit_vector.clear();
                            layout_vector.clear();
                            label_vector.clear();
                            numbers.clear();
                        }
                        else{
                            ui->statusbar->showMessage("Телефон введен некорректно!");
                            numbers.clear();
                        }
                    }
                    else{
                        ui->statusbar->showMessage("email введен некорректно!");
                    }
                }
                else{
                    ui->statusbar->showMessage("Дата рождения не может быть больше сегодняшнего дня!");
                }
            }
            else{
                ui->statusbar->showMessage("Отчество введено некорректно!");
            }
        }
        else{
            ui->statusbar->showMessage("Фамилия введена некорректно!");
        }
    }
    else{
        ui->statusbar->showMessage("Имя введено некорректно!");
    }
    emit signal_for_update_table();
}



//Добавление дополнительного телефона
void MainWindow::on_insert_number_clicked()
{
    QHBoxLayout* new_layout = new QHBoxLayout;
    QLabel* label_1 = new QLabel("Телефон:");
    QLineEdit* new_line_edit = new QLineEdit;

    layout_vector.push_back(new_layout);
    label_vector.push_back(label_1);
    line_edit_vector.push_back(new_line_edit);

    new_layout->addWidget(label_1);
    new_layout->addWidget(new_line_edit);

    ui->verticalLayout->addLayout(new_layout);
}

//Вывод данных найденного пользователя на экран
void MainWindow::on_pushButton_2_clicked()
{
    second_name_for_update = ui->lineEdit->text();

    if(book.find(second_name_for_update) != book.end()){
        ui->first_name->setText(book[second_name_for_update].first_name);
        ui->second_name->setText(book[second_name_for_update].second_name);
        ui->third_name->setText(book[second_name_for_update].third_name);
        ui->address->setText(book[second_name_for_update].address);
        ui->dateEdit->setDate(QDate::fromString(book[second_name_for_update].date_of_birth,"dd.MM.yyyy"));
        ui->email->setText(book[second_name_for_update].email);
        ui->phone->setText(book[second_name_for_update].number[0]);

        for(int i = 1; i < book[second_name_for_update].number.size(); i++){

            QHBoxLayout* new_layout = new QHBoxLayout;
            QLabel* label_1 = new QLabel("Телефон:");
            QLineEdit* new_line_edit = new QLineEdit;

            layout_vector.push_back(new_layout);
            label_vector.push_back(label_1);
            line_edit_vector.push_back(new_line_edit);

            new_layout->addWidget(label_1);
            new_layout->addWidget(new_line_edit);
            new_line_edit->setText(book[second_name_for_update].number[i]);

            ui->verticalLayout->addLayout(new_layout);
        }

    }
    else{
        QMessageBox::warning(0, "Предупреждение", "Контакта с такой фамилией нет!");
        ui->lineEdit->clear();
    }
}

//Обновление данных пользователя
void MainWindow::on_pushButton_3_clicked()
{
    QRegularExpression regular_for_name("(^[A-Za-z]{1}[-0-9A-z\\.]{0,}[0-9A-Za-z]{1}$)"); //для фио
    QRegularExpression regular_for_number("(^((8|\\+7)[\\- ]?)?(\\(?\\d{3}\\)?[\\- ]?)?[\\d\\- ]{7,10}$)");// для телефона
    QRegularExpression regular_for_number_2("(^$)");// для телефона
    QRegularExpression regular_for_email("(^((([0-9A-Za-z]{1}[-0-9A-z\\.]{1,}[0-9A-Za-z]{1}))@([-A-Za-z]{1,}\\.){1,2}[-A-Za-z]{2,})$)");// для email

    QString first_name = ui->first_name->text();
    QString second_name = ui->second_name->text();
    QString third_name = ui->third_name->text();
    QString address = ui->address->text();
    QString date_of_birth = ui->dateEdit->date().toString("dd.MM.yyyy");
    QString email = ui->email->text();
    QString number_1 = ui->phone->text();

    if(regular_for_name.match(first_name).hasMatch()){
        if(regular_for_name.match(second_name).hasMatch()){
            if(regular_for_name.match(third_name).hasMatch()){
                if(ui->dateEdit->date() < current_date){
                    if(regular_for_email.match(email).hasMatch()){
                        if(Check_numbers(line_edit_vector, regular_for_number, regular_for_number_2, number_1, numbers)){

                            book[second_name_for_update].first_name = first_name;
                            book[second_name_for_update].second_name = second_name;
                            book[second_name_for_update].third_name = third_name;
                            book[second_name_for_update].address = address;
                            book[second_name_for_update].date_of_birth = date_of_birth;
                            book[second_name_for_update].email = email;
                            book[second_name_for_update].number = numbers;

                            if(second_name_for_update != ui->second_name->text()){
                                auto it = book.find(second_name_for_update);
                                book[ui->second_name->text()] = (*it).second;
                                book.erase(it);
                            }

                            {
                                QSqlDatabase db;
                                db = QSqlDatabase::addDatabase("QSQLITE", "Telephone_book");
                                db.setDatabaseName("/Users/ilya/Documents/kursach_1/users_DB.db");
                                if(db.open()){
                                    qDebug("open db for update");
                                }
                                else{
                                    qDebug("no open for update");
                                }
                                QSqlQuery query(db);
                                query.prepare("UPDATE Telephone_Book SET Second_name = :Second_name, First_name = :First_name,"
                                              " Third_name = :Third_name, Address = :Address, Date_of_birth = :Date_of_birth,"
                                              " Email = :Email WHERE Second_name = :Second_name_for_update;");
                                query.bindValue(":Second_name_for_update", second_name_for_update);
                                query.bindValue(":Second_name", second_name);
                                query.bindValue(":First_name", first_name);
                                query.bindValue(":Third_name", third_name);
                                query.bindValue(":Address", address);
                                query.bindValue(":Date_of_birth", date_of_birth);
                                query.bindValue(":Email", email);
                                query.exec();

                                query.prepare("SELECT MAX(id) FROM Numbers WHERE second_name = :Second_name_for_update;");
                                query.bindValue(":Second_name_for_update", second_name_for_update);
                                query.exec();
                                query.next();

                                int size = query.value(0).toInt() + 1;
                                qDebug() << size;
                                if(size == numbers.size()){
                                    qDebug() << "1 slach";
                                    for(int i = 0; i < numbers.size(); i++){
                                        query.prepare("UPDATE Numbers SET second_name = :Second_name,"
                                                      "number = :Number, id = :id WHERE second_name = :Second_name_for_update"
                                                      " AND id = :id_for_update;");
                                        query.bindValue(":Second_name_for_update", second_name_for_update);
                                        query.bindValue(":id_for_update", i);
                                        query.bindValue(":Second_name", second_name);
                                        query.bindValue(":Number", numbers[i]);
                                        query.bindValue(":id", i);
                                        query.exec();
                                    }
                                }
                                else if(size > numbers.size()){
                                    qDebug() << "2 slach";
                                    for(int i = 0; i < numbers.size(); i++){
                                        query.prepare("UPDATE Numbers SET second_name = :Second_name,"
                                                      "number = :Number, id = :id WHERE second_name = :Second_name_for_update"
                                                      " AND id = :id_for_update;");
                                        query.bindValue(":Second_name_for_update", second_name_for_update);
                                        query.bindValue(":id_for_update", i);
                                        query.bindValue(":Second_name", second_name);
                                        query.bindValue(":Number", numbers[i]);
                                        query.bindValue(":id", i);
                                        query.exec();
                                    }
                                    query.prepare("DELETE FROM Numbers WHERE second_name = :Second_name_for_update AND id > :id;");
                                    query.bindValue(":Second_name_for_update", second_name_for_update);
                                    query.bindValue(":id", numbers.size()-1);
                                    query.exec();
                                }
                                else if(size < numbers.size()){
                                    qDebug() << "3 slach";
                                    for(int i = 0; i < size; i++){
                                        query.prepare("UPDATE Numbers SET second_name = :Second_name,"
                                                      "number = :Number, id = :id WHERE second_name = :Second_name_for_update"
                                                      " AND id = :id_for_update;");
                                        query.bindValue(":Second_name_for_update", second_name_for_update);
                                        query.bindValue(":id_for_update", i);
                                        query.bindValue(":Second_name", second_name);
                                        query.bindValue(":Number", numbers[i]);
                                        query.bindValue(":id", i);
                                        query.exec();
                                    }
                                    for(int i = size; i < numbers.size(); i++){
                                        query.prepare("INSERT INTO Numbers(second_name, number, id) VALUES(:Second_name, :Number, :id);");
                                        query.bindValue(":Second_name", second_name);
                                        query.bindValue(":Number", numbers[i]);
                                        query.bindValue(":id", i);
                                        query.exec();
                                    }
                                }

                                db.close();
                            }
                            QSqlDatabase::removeDatabase("Telephone_book");


                            ui->statusbar->showMessage("Информация о контакте обновлена!");
                            ui->first_name->clear();
                            ui->second_name->clear();
                            ui->third_name->clear();
                            ui->dateEdit->setDate(QDate::fromString("01.01.2000","dd.MM.yyyy"));
                            ui->address->clear();
                            ui->email->clear();
                            ui->phone->clear();
                            ui->lineEdit->clear();

                            for(int i = 0; i < line_edit_vector.size(); i++){
                                line_edit_vector[i]->clear();
                                delete line_edit_vector[i];
                                delete layout_vector[i];
                                delete label_vector[i];
                            }

                            line_edit_vector.clear();
                            layout_vector.clear();
                            label_vector.clear();
                            numbers.clear();
                        }
                        else{
                            ui->statusbar->showMessage("Телефон введен некорректно!");
                            numbers.clear();
                        }
                    }
                    else{
                        ui->statusbar->showMessage("email введен некорректно!");
                    }
                }
                else{
                    ui->statusbar->showMessage("Дата рождения не может быть больше сегодняшнего дня!");
                }
            }
            else{
                ui->statusbar->showMessage("Отчество введено некорректно!");
            }
        }
        else{
            ui->statusbar->showMessage("Фамилия введена некорректно!");
        }
    }
    else{
        ui->statusbar->showMessage("Имя введено некорректно!");
    }
    emit signal_for_update_table();
}

