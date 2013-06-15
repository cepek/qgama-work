/*
  GNU Gama Qt based GUI
  Copyright (C) 2013 Ales Cepek <cepek@gnu.org>

  This file is part of GNU Gama.

  GNU Gama is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  GNU Gama is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with GNU Gama.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "pointeditor.h"
#include "pointtablemodel.h"
#include "ui_pointeditor.h"
#include "pointtypecombobox.h"

#include <QMenu>
#include <QMessageBox>

PointEditor::PointEditor(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PointEditor),
    model(0), readonly(true)
{
    ui->setupUi(this);

    PointTypeComboBox* combobox = new PointTypeComboBox(ui->tableView);
    ui->tableView->setItemDelegateForColumn(3, combobox);
    ui->tableView->setItemDelegateForColumn(5, combobox);

    enableEdit(false);

    ui->tableView->verticalHeader()->setContextMenuPolicy(Qt::CustomContextMenu);
    pointMenu = new QMenu(this);

    QAction* pdelete = new QAction(tr("delete point"), this);
    pointMenu->addAction(pdelete);
    connect(pdelete, SIGNAL(triggered()), this, SLOT(pointDelete()));

    connect(ui->tableView->verticalHeader(), SIGNAL(customContextMenuRequested(QPoint)),
            this, SLOT(pointContextMenu(QPoint)));
}

void PointEditor::pointContextMenu(QPoint p)
{
    if (readonly) return;

    pointLogicalIndex = ui->tableView->verticalHeader()->logicalIndexAt(p);
    pointMenu->exec(QCursor::pos());
}

void PointEditor::pointDelete()
{
    QModelIndex index = model->index(pointLogicalIndex, 0);
    if (!index.isValid()) return;

    QString ID = index.data().toString();
    if (ID.isEmpty()) return;   // last table row used to insert new points

    ui->tableView->selectRow(pointLogicalIndex);

    int q = QMessageBox::warning(this, tr("Deleting Point"),
             tr("Do you really want to delete point Id %1").arg(ID),
             QMessageBox::Ok|QMessageBox::Cancel);
    if (q != QMessageBox::Ok) return;

    model->removeRows(pointLogicalIndex,1, QModelIndex());
}

PointEditor::~PointEditor()
{
    delete ui;
}

void PointEditor::connectPointData(GNU_gama::local::PointData* pd)
{
    PointTableModel* old = model;
    model = new PointTableModel(*pd, this);
    ui->tableView->setModel(model);
    delete old;

    connect(model, SIGNAL(warning(QString)), this, SIGNAL(warning(QString)));
}

void PointEditor::enableEdit(bool edit)
{
    readonly = !edit;
    if (edit)
    {
        // implicit behaviour is "double click for editing"
        ui->tableView->setEditTriggers(QAbstractItemView::AllEditTriggers);
    }
    else
    {
        ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    }
}