/*
 * \copyright Copyright (c) 2018-2020 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "Env.h"

#include <QAbstractListModel>
#include <QDateTime>
#include <QObject>
#include <QPoint>
#include <QString>
#include <QStringList>
#include <QTextStream>
#include <QUrl>

class test_LogModel;

namespace governikus
{

class LogModel
	: public QAbstractListModel
{
	Q_OBJECT
	friend class Env;

	enum LogModelRoles
	{
		OriginRole = Qt::UserRole + 1,
		MessageRole
	};

	Q_PROPERTY(QStringList logFiles READ getLogfiles NOTIFY fireLogFilesChanged)

	private:
		friend class ::test_LogModel;
		QStringList mLogFiles;
		int mSelectedLogFile;
		QStringList mLogEntries;

		void addLogEntry(const QString& pEntry);
		void setLogEntries(QTextStream& pTextStream);

	private Q_SLOTS:
		void onNewLogMsg(const QString& pMsg);

	protected:
		LogModel();
		static LogModel& getInstance();

	public:
		QStringList getLogfiles();
		Q_INVOKABLE QDateTime getCurrentLogfileDate() const;
		Q_INVOKABLE void removeOtherLogfiles();
		Q_INVOKABLE void removeCurrentLogfile();
		Q_INVOKABLE void setLogfile(int pIndex);
		Q_INVOKABLE void saveCurrentLogfile(const QUrl& pFilename) const;
#ifndef QT_NO_DEBUG
		Q_INVOKABLE void saveDummyLogfile() const;
#endif

		Q_INVOKABLE void mailLog(const QString& pEmail = tr("support.ausweisapp2@governikus.de"),
				const QString& pSubject = tr("Mobile logfile"),
				const QString& pMsg = tr("<Please describe the error>"));

		/// \a popupPosition will be used on an iPad as the origin of the share bubble
		Q_INVOKABLE void shareLog(QPoint popupPosition);

		int rowCount(const QModelIndex& pIndex = QModelIndex()) const override;
		QHash<int, QByteArray> roleNames() const override;
		QVariant data(const QModelIndex& pIndex, int pRole = Qt::DisplayRole) const override;
		Q_INVOKABLE static QString createLogFileName(const QDateTime& pDateTime = QDateTime::currentDateTime());

	Q_SIGNALS:
		void fireLogFilesChanged();
		void fireNewLogMsg();
};

} // namespace governikus
