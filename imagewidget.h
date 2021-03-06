#ifndef IMAGEWIDGET_H
#define IMAGEWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QPixmap>
#include <QSize>
#include "targetlistwindow.h"
#include <QMouseEvent>
#include <QTabWidget>
#include <QThread>

#include "lifesupport.h"

class MainWindow;

namespace Ui {
class ImageWidget;
}

class ImageWidget : public QWidget
{
    Q_OBJECT

public:
    // LOADER THREAD
    class ImageLoader : public QThread
    {
    public:
        explicit ImageLoader(ImageWidget *imageWidget, QString filePath)
            : imageWidget(imageWidget), filePath(filePath) {}
        static int numLoaders;
        void run();
    private:
        ImageWidget *imageWidget;
        QString filePath;
    };

    explicit ImageWidget(LifeSupport* dataPackage, MainWindow *parent, bool initTargetList = true);
    ImageWidget(ImageWidget &toCopy);

    // Setters
    void setTitle(QString name);
    void setImage(QString imagePath);
    void setImage(QPixmap &resizedImage);
    void setFolderPath(QString folderPath) ;
    void setFilePath(QString filePath) ;
    void setImagePath(QString imagePath);
    void setTabWidget(QTabWidget* tabWidget ) ;
    void setNumTargets(int numTargets);
    void setSeen(bool seen);
    void setLatLon(const LatLon &latlon);
    void setTargetData(const QList<TargetData> &targetData);

    // Getters
    QString getTitle() const;
    QString getImagePath() const;
    QPixmap& getImage();
    QString getFolderPath() const;
    QString getFilePath() const;
    TargetListWindow* getTargetList();
    int getNumTargets() const;
    bool isInitialized() const;
    bool getSeen() const;
    QList<TargetData>& getTargetData();
    LatLon getLatLon() const;

    TargetData &getTarget(const QString &targetName);

    void deleteTargetListWindow();

    void changeTargetListWindow(TargetListWindow* targetList, bool alreadyInitialized = true);
    void setImagePathString(QString imagePath);
    void finishLoading();

    friend class ImageLoader;

    ~ImageWidget();

protected:
    void mouseDoubleClickEvent(QMouseEvent *event);
    QString title;
    int numTargets;
    QString filePath ;
    QString imagePath;
    QString folderPath;
    QPixmap image;
    TargetListWindow *targetList;
    bool targetListInitialized;
    bool seen;
    ImageLoader* loader;
    bool loadingFinished;
    QList<TargetData> targetData;

    LatLon latlon;

private slots:
    void on_pinButton_clicked();

private:
    LifeSupport* dataPackage ;
    Ui::ImageWidget *ui;
    MainWindow* mainWindow ;
};

#endif // IMAGEWIDGET_H
