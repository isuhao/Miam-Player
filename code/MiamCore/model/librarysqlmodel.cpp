#include "librarysqlmodel.h"
#include "filehelper.h"

#include <QtDebug>

LibrarySqlModel::LibrarySqlModel(QObject *parent) :
	QSqlTableModel(parent)
{
	_db = QSqlDatabase::addDatabase("QSQLITE");
	QString path = QStandardPaths::standardLocations(QStandardPaths::DataLocation).first();
	QString dbPath = QDir::toNativeSeparators(path + "/mmmmp.db");
	_db.setDatabaseName(dbPath);

	_musicSearchEngine = new MusicSearchEngine(this);

	connect(_musicSearchEngine, &MusicSearchEngine::scannedCover, this, &LibrarySqlModel::saveCoverRef);
	connect(_musicSearchEngine, &MusicSearchEngine::scannedFile, this, &LibrarySqlModel::saveFileRef);

	// When the scan is complete, save the model in the filesystem
	connect(_musicSearchEngine, &MusicSearchEngine::searchHasEnded, [=] () {
		// Close Connection
		_db.close();
		this->endResetModel();
	});
}

void LibrarySqlModel::loadFromFileDB()
{
	this->beginResetModel();
	_db.open();

	QSqlQuery qLoadFileDB;
	qLoadFileDB.exec("SELECT * FROM tracks");
	while (qLoadFileDB.next()) {
		emit trackExtractedFromDB(qLoadFileDB.record());
	}
	_db.close();
	this->endResetModel();
}

void LibrarySqlModel::rebuild()
{
	this->beginResetModel();

	// Open Connection
	_db.open();

	_db.exec("DROP TABLE tracks");
	QString createTable("CREATE TABLE tracks (artist varchar(255), artistAlbum varchar(255), album varchar(255), ");
	createTable.append("title varchar(255), discNumber INTEGER, year INTEGER, absPath varchar(255), path varchar(255), ");
	createTable.append("coverAbsPath varchar(255), internalCover INTEGER DEFAULT 0, externalCover INTEGER DEFAULT 0)");
	QSqlQuery qCreateTable = _db.exec(createTable);
	qDebug() << "create: " << qCreateTable.lastError().type();

	// Foreach file, insert tuple
	_musicSearchEngine->doSearch();
}

/** Reads an external picture which is close to multimedia files (same folder). */
void LibrarySqlModel::saveCoverRef(const QString &coverPath)
{
	QFileInfo fileInfo(coverPath);
	QSqlQuery updateCoverPath("UPDATE tracks SET coverAbsPath = ? WHERE path = ?");
	updateCoverPath.addBindValue(QDir::toNativeSeparators(fileInfo.absoluteFilePath()));
	updateCoverPath.addBindValue(QDir::toNativeSeparators(fileInfo.absolutePath()));
	updateCoverPath.exec();
	emit coverWasUpdated(fileInfo);
}

/** Reads a file from the filesystem and adds it into the library. */
void LibrarySqlModel::saveFileRef(const QString &absFilePath)
{
	FileHelper fh(absFilePath);
	if (fh.isValid()) {
		QSqlQuery insert;
		insert.prepare("INSERT INTO tracks (absPath, album, artist, artistAlbum, discNumber, internalCover, path, title, year) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?)");
		insert.addBindValue(QDir::toNativeSeparators(absFilePath));
		insert.addBindValue(fh.album());
		insert.addBindValue(fh.artist());
		insert.addBindValue(fh.artistAlbum());
		insert.addBindValue(fh.discNumber());
		Cover *cover = fh.extractCover();
		insert.addBindValue(cover != NULL);
		insert.addBindValue(QDir::toNativeSeparators(fh.fileInfo().absolutePath()));
		insert.addBindValue(fh.title());
		insert.addBindValue(fh.year());
		insert.exec();
		emit trackExtractedFromFS(fh);
	} else {
		//qDebug() << "INVALID FILE FOR:" << absFilePath;
	}
}