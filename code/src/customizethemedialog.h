#ifndef CUSTOMIZETHEMEDIALOG_H
#define CUSTOMIZETHEMEDIALOG_H

#include <QDialog>

#include "ui_customizetheme.h"
#include "mainwindow.h"

class CustomizeThemeDialog : public QDialog, public Ui::CustomizeThemeDialog
{
	Q_OBJECT

private:
	MainWindow *mainWindow;

public:
	CustomizeThemeDialog(QWidget *parent);

private slots:
	/** Changes the current theme and updates this dialog too. */
	void setThemeNameAndDialogButtons(QString);

	/** Displays covers or not in the library. */
	void displayCovers(bool);

	/** Displays alphabecical separators or not in the library. */
	void displayAlphabeticalSeparators(bool);

	/** Updates the font family of a specific component. */
	void updateFontFamily(const QFont&);

	/** Updates the font size of a specific component. */
	void updateFontSize(int);

public slots:
	/** Load theme at startup. */
	void loadTheme();

signals:
	void themeChanged();
};

#endif // CUSTOMIZETHEMEDIALOG_H
