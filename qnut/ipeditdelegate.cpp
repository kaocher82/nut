//
// C++ Implementation: ipeditdelegate
//
// Description: 
//
//
// Author: Oliver Groß <z.o.gross@gmx.de>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "ipeditdelegate.h"

namespace qnut {
	CIPEditDelegate::CIPEditDelegate(QObject * parent) : QItemDelegate(parent) {
	}
	
	CIPEditDelegate::~CIPEditDelegate() {
	}
	
	QWidget * CIPEditDelegate::createEditor(QWidget * parent, const QStyleOptionViewItem & option, const QModelIndex & index) const {
		QLineEdit * editor = new QLineEdit(parent);
		editor->setInputMask("000.000.000.000;_");
		return editor;
	}
	
	void CIPEditDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const {
		int value = index.model()->data(index, Qt::DisplayRole).toInt();
		
		QLineEdit * lineEdit = static_cast<QLineEdit *>(editor);
		lineEdit->setText(value);
	}
	
	void CIPEditDelegate::setModelData(QWidget * editor, QAbstractItemModel * model, const QModelIndex & index) const {
		QLineEdit * lineEdit = static_cast<QLineEdit *>(editor);
		QString value = lineEdit->text();
		
		model->setData(index, value, Qt::EditRole);
	}
	
	void CIPEditDelegate::updateEditorGeometry(QWidget * editor, const QStyleOptionViewItem & option, const QModelIndex & index) const {
		editor->setGeometry(option.rect);
	}
};
