#ifndef SLAVETEST_H
#define SLAVETEST_H

#include <QObject>
#include <KIO/Job>
#include <QProcess>

class SlaveTest : public QObject
{
    Q_OBJECT

    public:
        SlaveTest();
        ~SlaveTest(){}

        enum NodeType {
            DirectoryNode,
            SymlinkNode,
            FileNode,
            InvalidNode
        };

    private slots:
        void initTestCase();

        void copyFileToStash();
        void copyStashToFile();
        void copyStashToStash();
        void copySymlinkFromStash();

        void moveToStashFromStash();
        void moveToFileFromStash();

        //void renameFileInStash(); //currently fails
        void statRoot();
        void statFileInRoot();
        void statDirectoryInRoot();
        void statSymlinkInRoot();
        void statFileInDirectory();

        void listRootDir();
        void listSubDir();

        void delRootFile();
        void delFileInDirectory();
        void delDirectory();

        void cleanupTestCase();

    private:
        QString tmpDirPath() const;
        bool statUrl(const QUrl &url, KIO::UDSEntry &entry);
        void stashFile(const QString &realPath, const QString &stashPath);
        void stashSymlink(const QString &realPath, const QString &stashPath);
        void stashDirectory(const QString &path);
        void stashCopy(const QUrl &src, const QUrl &dest);
        void createDirectory();
        void moveFromStash(const QUrl &src, const QUrl &dest);
        void deleteFromStash(const QUrl &url);
        void createTestFiles();

        QProcess *stashDaemonProcess;

        const QString tmpFolder;
        const QString m_fileTestFile;
        const QString m_fileTestFolder;
        const QString m_stashTestFolder;
        const QString m_stashTestSymlink;
        const QString m_stashTestFile;
        const QString m_stashTestFileInSubDirectory;
};

#endif
