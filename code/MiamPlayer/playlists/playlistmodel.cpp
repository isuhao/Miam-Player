#include "playlistmodel.h"

#include <filehelper.h>
#include <settings.h>
#include "starrating.h"

#include <QFile>
#include <QTime>
#include <QUrl>

#include <algorithm>
#include <functional>

#include <QtDebug>

PlaylistModel::PlaylistModel(QObject *parent) :
	QStandardItemModel(0, 7, parent), _mediaPlaylist(new QMediaPlaylist(this))
{}

/** Clear the content of playlist. */
void PlaylistModel::clear()
{
	if (rowCount() > 0) {
		removeRows(0, rowCount());
	}
}

void PlaylistModel::insertMedias(int rowIndex, const QList<QMediaContent> &tracks)
{
	bool b = _mediaPlaylist->insertMedia(rowIndex, tracks);
	qDebug() << "media was inserted" << b;
	foreach (QMediaContent track, tracks) {
		FileHelper f(track);
		if (f.isValid()) {
			this->insertMedia(rowIndex++, f);
		} else {
			qDebug() << "f is invalid" << track.canonicalUrl();
		}
	}
}

void PlaylistModel::insertMedia(int rowIndex, const FileHelper &fileHelper)
{
	QString title(fileHelper.title());

	// Then, construct a new row with correct informations
	QStandardItem *trackItem = new QStandardItem(fileHelper.trackNumber());
	QStandardItem *titleItem = new QStandardItem(title);
	QStandardItem *albumItem = new QStandardItem(fileHelper.album());
	QStandardItem *lengthItem = new QStandardItem(fileHelper.length());
	QStandardItem *artistItem = new QStandardItem(fileHelper.artist());
	QStandardItem *ratingItem = new QStandardItem();
	int rating = fileHelper.rating();
	if (rating > 0) {
		StarRating r(rating);
		ratingItem->setData(QVariant::fromValue(r), Qt::DisplayRole);
	}
	QStandardItem *yearItem = new QStandardItem(fileHelper.year());

	trackItem->setTextAlignment(Qt::AlignCenter);
	lengthItem->setTextAlignment(Qt::AlignCenter);
	ratingItem->setTextAlignment(Qt::AlignCenter);
	yearItem->setTextAlignment(Qt::AlignCenter);

	QList<QStandardItem *> widgetItems;
	widgetItems << trackItem << titleItem << albumItem << lengthItem << artistItem << ratingItem << yearItem;
	this->insertRow(rowIndex, widgetItems);
}

/** Moves rows from various positions to a new one (discontiguous rows are grouped). */
QList<QStandardItem*> PlaylistModel::internalMove(QModelIndex dest, QModelIndexList selectedIndexes)
{
	// After moving rows, selection is lost. We need to keep a track on previously selected indexes
	QList<QStandardItem*> rowsToHiglight;
	QList<QMediaContent> mediasToMove;

	// Sort in reverse lexical order for correctly taking rows
	std::sort(selectedIndexes.begin(), selectedIndexes.end(), [](const QModelIndex &a, const QModelIndex &b) { return b < a; });
	QList<QList<QStandardItem*>> removedRows;
	_mediaPlaylist->blockSignals(true);
	int currentPlayingTrack = _mediaPlaylist->currentIndex();
	foreach (QModelIndex selectedIndex, selectedIndexes) {
		int rowNumber = selectedIndex.row();
		QList<QStandardItem*> row = this->takeRow(rowNumber);
		rowsToHiglight << row.at(0);
		removedRows.append(row);
		mediasToMove.prepend(_mediaPlaylist->media(rowNumber));
		_mediaPlaylist->removeMedia(rowNumber);
	}

	// Dest equals -1 when rows are dropped at the bottom of the playlist
	int insertPoint = (dest.isValid() || dest.row() >= 0) ? dest.row() : rowCount();
	for (int i = 0; i < removedRows.count(); i++) {
		this->insertRow(insertPoint, removedRows.at(i));
	}
	// Finally, reorder the inner QMediaPlaylist
	_mediaPlaylist->insertMedia(insertPoint, mediasToMove);
	qDebug() << Q_FUNC_INFO << currentPlayingTrack << insertPoint;

	int offset = 0;
	foreach (QModelIndex selectedIndex, selectedIndexes) {
		int rowNumber = selectedIndex.row();
		if (rowNumber > currentPlayingTrack && currentPlayingTrack > insertPoint) {
			offset++;
		} else if (rowNumber < currentPlayingTrack && currentPlayingTrack < insertPoint) {
			offset--;
		} else if (currentPlayingTrack == rowNumber) {
			offset = -rowNumber;
		}
	}
	if (offset < 0) {
		//_mediaPlaylist->setCurrentIndex(-offset);
	} else {
		_mediaPlaylist->setCurrentIndex(currentPlayingTrack + offset);
	}

	_mediaPlaylist->blockSignals(false);

	return rowsToHiglight;
}

void PlaylistModel::insertRow(int row, const QList<QStandardItem*> &items)
{
	QFont font = Settings::getInstance()->font(Settings::FF_Playlist);
	for (int i=0; i < items.length(); i++) {
		QStandardItem *item = items.at(i);
		item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsDragEnabled);
		item->setFont(font);
	}
	QStandardItemModel::insertRow(row, items);
}

void PlaylistModel::removeTrack(int row)
{
	QStandardItemModel::removeRow(row);
	_mediaPlaylist->removeMedia(row);
}
