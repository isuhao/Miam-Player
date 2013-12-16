#include "albumform.h"

AlbumForm::AlbumForm(QWidget *parent) :
	QWidget(parent)
{
	setupUi(this);
}

void AlbumForm::setArtist(const QString &artist)
{
	artistPushButton->setText(artist);
}

void AlbumForm::setAlbum(const QString &album, int year)
{
	QString text = album + " [" + QString::number(year) + "]";
	albumPushButton->setText(text);
}

void AlbumForm::setDiscNumber(int discNumber)
{
	if (discNumber == 0) {
		discPushButton->hide();
	} else {
		discPushButton->setText(QString::number(discNumber));
	}
}

void AlbumForm::appendTrack(const QString &track)
{
	tracksWidget->addItem(track);
}