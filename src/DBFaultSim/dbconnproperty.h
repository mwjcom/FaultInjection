#ifndef DBCONNPROPERTY_H
#define DBCONNPROPERTY_H

#include <QDialog>
#include "ui_dbconnproperty.h"

class DBConnProperty : public QDialog
{
	Q_OBJECT

public:
	DBConnProperty(QWidget *parent = 0);
	~DBConnProperty();

	QString GetConnName();
	void SetConnName( QString strConnName );

private:
	Ui::DBConnProperty ui;
};

#endif // DBCONNPROPERTY_H
