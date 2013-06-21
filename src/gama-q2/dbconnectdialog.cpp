#include "dbconnectdialog.h"
#include "ui_dbconnectdialog.h"
#include "constants.h"

#include <QFileDialog>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>

#include <QDebug>

DBConnectDialog::DBConnectDialog(QString connectionName, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DBConnectDialog),
    connection_name(connectionName)
{
    ui->setupUi(this);

    setWindowTitle(tr("DB Connection"));

    const QStringList drivers = QSqlDatabase::drivers();
    ui->comboBox_Driver ->addItems(drivers);
    ui->comboBox_Driver2->addItems(drivers);

    connect(ui->comboBox_Driver,  SIGNAL(currentIndexChanged(int)), ui->comboBox_Driver2, SLOT(setCurrentIndex(int)));
    connect(ui->comboBox_Driver2, SIGNAL(currentIndexChanged(int)), ui->comboBox_Driver,  SLOT(setCurrentIndex(int)));
    connect(ui->comboBox_Driver,  SIGNAL(currentIndexChanged(int)), this, SLOT(switchStackedWidgets()));

    switchStackedWidgets();

    this->adjustSize();
}

DBConnectDialog::~DBConnectDialog()
{
    delete ui;
}

void DBConnectDialog::switchStackedWidgets()
{
    if (ui->comboBox_Driver->currentText() == "QSQLITE")
        ui->stackedWidget->setCurrentIndex(1);
    else
        ui->stackedWidget->setCurrentIndex(0);
}

void DBConnectDialog::on_pushButton_OpenFileDialog_clicked()
{
    QFileDialog fileDialog(0,trUtf8("Opening Sqlite Database File"));
    fileDialog.setAcceptMode(QFileDialog::AcceptOpen);
    fileDialog.setFileMode(QFileDialog::AnyFile);     // a single file only
    fileDialog.setDefaultSuffix("db");
    //fileDialog.setNameFilter(tr("Sqlite DB (*.db)"));
    QStringList filters;
    filters << "Sqlite DB (*.db)"
            << "All files (*.*)";
    fileDialog.setNameFilters(filters);
    fileDialog.setViewMode(QFileDialog::Detail);

    if (!fileDialog.exec()) return;

    ui->lineEdit_DatabaseFile->setText( fileDialog.selectedFiles()[0] );
    on_buttonBox_accepted();
    hide();
}

void DBConnectDialog::create_missing_tables(QSqlDatabase& db)
{
    QSqlQuery query(db);

    db.transaction();
    for (QStringList::const_iterator
            i = GamaQ2::gama_local_schema.begin(),
            e = GamaQ2::gama_local_schema.end();   i!=e; ++i)
    {
        query.exec(*i);
        if (query.lastError().isValid())
        {
            QMessageBox::critical(this,
                tr("Database Error"),
                tr("Critical Database error occured during the "
                   "attempt to create missing schema tables.\n\n"
                   "%1").arg(query.lastError().databaseText()));
            db.rollback();
            db.close();
            emit input_data_open(false);
            close();
            return;
        }
    }
    db.commit();
}

void DBConnectDialog::on_buttonBox_rejected()
{
    emit input_data_open(false);
}

void DBConnectDialog::on_buttonBox_accepted()
{
    QString driver = ui->comboBox_Driver->currentText();
    QString database_name = ui->lineEdit_DatabaseName->text();
    if (driver == "QSQLITE") database_name = ui->lineEdit_DatabaseFile->text();

    QSqlDatabase::removeDatabase(connection_name);
    QSqlDatabase db = QSqlDatabase::addDatabase(driver, connection_name);
    db.setHostName    (ui->lineEdit_Hostname->text().simplified());
    db.setDatabaseName(database_name);
    db.setUserName    (ui->lineEdit_Username->text().simplified());
    db.setPassword    (ui->lineEdit_Password->text().simplified());
    if (!ui->lineEdit_Port->text().simplified().isEmpty())
    {
        db.setPort(ui->lineEdit_Port->text().toInt());
    }

    if (!db.open())
    {
        QMessageBox::critical(this, tr("Database Error"), db.lastError().databaseText() );
        emit input_data_open(false);
        return;
    }

    if (driver == "QSQLITE" && database_name != ":memory:")
    {
        // http://www.sqlite.org/fileformat.html
        QFile file(database_name);
        file.open(QIODevice::ReadOnly);
        QByteArray headerString = file.read(16);

        if ( headerString != QByteArray("SQLite format 3\000",16) &&
            !headerString.isEmpty())
        {
            QMessageBox::critical(this, tr("Database Error"),
                                  tr("Selected file ") + database_name +
                                  tr(" is not a SQLite database"));
            emit input_data_open(false);
            return;
        }


    }

    int missing = 0;
    QString missing_tables;
    if (database_name != ":memory:")
    {
        QStringList tables = db.tables();
        for (QStringList::const_iterator i=GamaQ2::gama_local_schema_table_names.begin(),
                                         e=GamaQ2::gama_local_schema_table_names.end(); i!=e; ++i)
            if (!tables.contains(*i, Qt::CaseInsensitive))
            {
                missing++;
                if (!missing_tables.isEmpty()) missing_tables += ", ";
                missing_tables += *i;
            }
    }

    if (database_name == ":memory:")
    {
        create_missing_tables(db);
    }
    else if (missing > 0 && missing < GamaQ2::gama_local_schema_table_names.size())
    {
        QMessageBox::critical(this, tr("Bad Database Schema Tables"),
                                    tr("One or more (but not all) of the Gnu Gama schema tables "
                                       "are missing in the database.\n\n"
                                       "Missing tables: %1").arg(missing_tables));
        db.close();

        emit input_data_open(false);
        on_buttonBox_rejected();
        return;
    }
    else if (missing == GamaQ2::gama_local_schema_table_names.size())
    {
        int ret = QMessageBox::question(this, tr("No Database Schema Tables"),
                                        tr("Gnu Gama schema tables are missing in the database.\n\n"
                                           "%1\n\n"
                                           "Do you want to create them?").arg(missing_tables),
                                        QMessageBox::No, QMessageBox::Yes);
        if (ret == QMessageBox::No)
        {
            db.close();

            emit input_data_open(false);
            on_buttonBox_rejected();
            return;
        }

        create_missing_tables(db);
    }

    emit input_data_open(true);
}
