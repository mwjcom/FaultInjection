#include "dbconnproperty.h"

DBConnProperty::DBConnProperty(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);

	connect(ui.okButton,SIGNAL(clicked()),this,SLOT(accept()));
	connect(ui.cancelButton,SIGNAL(clicked()),this,SLOT(reject()));
}

DBConnProperty::~DBConnProperty()
{

}

QString DBConnProperty::GetConnName()
{
	return ui.dbConnName->text();
}

void DBConnProperty::SetConnName( QString strConnName )
{
	 ui.dbConnName->setText(strConnName);
}
