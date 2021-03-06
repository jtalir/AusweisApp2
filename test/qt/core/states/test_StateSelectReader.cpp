/*!
 * \brief Tests the StateSelectReader
 *
 * \copyright Copyright (c) 2018-2020 Governikus GmbH & Co. KG, Germany
 */

#include "states/StateSelectReader.h"

#include "Env.h"
#include "ReaderManager.h"
#include "states/StateBuilder.h"

#include <QtTest>

using namespace governikus;


class test_StateSelectReader
	: public QObject
{
	Q_OBJECT
	QScopedPointer<StateSelectReader> mState;
	QSharedPointer<WorkflowContext> mContext;

	private Q_SLOTS:
		void init()
		{
			mContext.reset(new WorkflowContext());
			mState.reset(StateBuilder::createState<StateSelectReader>(mContext));
		}


		void cleanup()
		{
			mState.reset();
			mContext.clear();
		}


		void test_OnReaderInfoChangedNoSelectableReaders()
		{
			mContext->setReaderPlugInTypes({ReaderManagerPlugInType::PCSC, ReaderManagerPlugInType::UNKNOWN, ReaderManagerPlugInType::REMOTE});
			mContext->setStateApproved();
			mState->onStateApprovedChanged();
			const auto readerManager = Env::getSingleton<ReaderManager>();

			QTest::ignoreMessage(QtDebugMsg, "No selectable reader detected");
			Q_EMIT readerManager->fireReaderAdded(QString());
		}


		void test_OnReaderDeviceError()
		{
			QSignalSpy spyAbort(mState.data(), &StateSelectReader::fireAbort);
			mContext->setStateApproved();
			mState->onStateApprovedChanged();
			const auto readerManager = Env::getSingleton<ReaderManager>();

			Q_EMIT readerManager->fireReaderDeviceError(GlobalStatus::Code::No_Error);
			QCOMPARE(spyAbort.count(), 0);

			Q_EMIT readerManager->fireReaderDeviceError(GlobalStatus::Code::Workflow_Reader_Device_Scan_Error);
			QCOMPARE(spyAbort.count(), 0);

			Q_EMIT readerManager->fireReaderDeviceError(GlobalStatus::Code::Card_Communication_Error);
			QCOMPARE(spyAbort.count(), 1);
			QCOMPARE(mContext->getStatus().getStatusCode(), GlobalStatus::Code::Card_Communication_Error);
		}


		void test_OnEntry()
		{
			QSignalSpy spyRetry(mState.data(), &StateSelectReader::fireRetry);

			mState->onEntry(nullptr);

			Q_EMIT mContext->fireReaderPlugInTypesChanged();
			QCOMPARE(spyRetry.count(), 1);
		}


};

QTEST_GUILESS_MAIN(test_StateSelectReader)
#include "test_StateSelectReader.moc"
