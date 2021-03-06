#include "imagewidget.h"
#include "ui_imagewidget.h"

#include "mainwindow.h"

int ImageWidget::ImageLoader::numLoaders = 0;

ImageWidget::ImageWidget(LifeSupport *dataPackage, MainWindow *parent, bool initTargetList) :
    QWidget(parent),
    loader(NULL),
    loadingFinished(true),
    ui(new Ui::ImageWidget)
{
    imagePath = "";
    title = "";
    ui->setupUi(this);

    this->seen = false;
    mainWindow = parent ;
    this->dataPackage=dataPackage;

    if (initTargetList)
        targetList = new TargetListWindow(dataPackage, this);
    else
        targetList = NULL;
    targetListInitialized = false;

    ui->colourLabel->setStyleSheet("QLabel { background-color : yellow; color : blue; }");
}

ImageWidget::ImageWidget(ImageWidget &toCopy) :
    ImageWidget(toCopy.dataPackage, toCopy.mainWindow, false)
{
    // Copy all information over
    this->setTitle(toCopy.getTitle());
    this->setFilePath(toCopy.getFilePath());
    this->setFolderPath(toCopy.getFolderPath());
    this->setImagePath(toCopy.getImagePath());
    this->setImage(toCopy.getImage());
    this->setNumTargets(toCopy.getNumTargets());
    this->setSeen(toCopy.getSeen());
    this->setTargetData(toCopy.getTargetData());
    this->setLatLon(toCopy.getLatLon());
}

ImageWidget::~ImageWidget()
{
    if (loader)
        loader->wait();
    delete loader;
    delete targetList;
    delete ui;
}

QString ImageWidget::getTitle() const
{
    return this->title;
}

int ImageWidget::getNumTargets() const
{
    return this->numTargets;
}

QString ImageWidget::getImagePath() const
{
    return this->imagePath;
}

QPixmap& ImageWidget::getImage()
{
    return this->image;
}

QString ImageWidget::getFolderPath() const
{
    return this->folderPath;
}

QString ImageWidget::getFilePath() const
{
    return this->filePath;
}

bool ImageWidget::getSeen() const
{
    return this->seen;
}

LatLon ImageWidget::getLatLon() const
{
    return this->latlon;
}

QList<TargetData>& ImageWidget::getTargetData()
{
    return this->targetData;
}

TargetData &ImageWidget::getTarget(const QString &targetName)
{
    for (int i = 0; i < targetData.size(); ++i)
    {
        if (targetData[i].imagePath == targetName)
        {
            return targetData[i];
        }
    }
    return targetData[0];
}

void ImageWidget::setTitle(QString name)
{
    ui->imageCaption->setText(name);
    title = name;
}

void ImageWidget::setSeen(bool seen)
{
    if (seen)
        ui->colourLabel->setStyleSheet("QLabel { background-color : green; color : blue; }");
    else
        ui->colourLabel->setStyleSheet("QLabel { background-color : yellow; color : blue; }");
    this->seen = seen;
}

void ImageWidget::setTargetData(const QList<TargetData> &targetData)
{
    this->targetData = targetData;
}

void ImageWidget::setFolderPath(QString folderPath){
    this->folderPath = folderPath ;
}

void ImageWidget::setFilePath(QString filePath){
    this->filePath = filePath ;
}

void ImageWidget::setImagePath(QString imagePath)
{
    this->imagePath = imagePath;
}

void ImageWidget::setLatLon(const LatLon &latlon)
{
    this->latlon = latlon;
}

void ImageWidget::setNumTargets(int numTargets)
{
    QString s = QString::number(numTargets);
    QString numTargetsString = s + " targets";
    this->ui->numTargetDisplay->setText(numTargetsString);
    this->numTargets = numTargets;
}

void ImageWidget::finishLoading()
{
    if (!loadingFinished) qDebug() << "Not done";

    if (loader)
        loader->wait();
}

void ImageWidget::setImage(QString imagePath)
{
    this->imagePath = imagePath;
    QPixmap *pix = new QPixmap();
    pix->load(imagePath);
    image = pix->scaled(220, 220, Qt::KeepAspectRatioByExpanding, Qt::FastTransformation);
    ui->imageLabel->setPixmap(image);
    delete pix;
    return;

    // Experimental stuff
    /*
    ++ImageLoader::numLoaders;
    loadingFinished = false;
    this->imagePath = imagePath;
    loader = new ImageLoader(this, imagePath);
    connect(loader, &ImageLoader::finished, [=](){
        loadingFinished = true;
        --ImageLoader::numLoaders;
    });
    loader->start();
    */
}

void ImageWidget::setImage(QPixmap &resizedImage)
{
    if (resizedImage.size().width() == 0 && resizedImage.size().height() == 0)
    {
        QPixmap *pix = new QPixmap();
        pix->load(imagePath);
        this->image = pix->scaled(220, 220, Qt::KeepAspectRatioByExpanding, Qt::FastTransformation);
        this->ui->imageLabel->setPixmap(this->image);
        delete pix;
    }
    else
    {
        ui->imageLabel->setPixmap(resizedImage);
        image = resizedImage;
    }
}

bool ImageWidget::isInitialized() const
{
    return this->targetListInitialized;
}

void ImageWidget::deleteTargetListWindow()
{
    delete this->targetList;
    this->targetList = NULL;
}

void ImageWidget::changeTargetListWindow(TargetListWindow* targetList, bool alreadyInitialized)
{
    this->targetList = targetList;
    if (targetList)
    {
        this->targetList->parent = this;
    }
    this->targetListInitialized = alreadyInitialized;
}

TargetListWindow* ImageWidget::getTargetList()
{
    return this->targetList;
}

void ImageWidget::mouseDoubleClickEvent(QMouseEvent *event){
    if ( event->button() == Qt::LeftButton )
    {
        ui->colourLabel->setStyleSheet("QLabel { background-color : green; color : blue; }"); // Mark as seen
        this->seen = true;

        dataPackage->filePath=filePath;

        if (targetList == NULL)
        {
            qDebug() << "making new one";
            targetList = new TargetListWindow(dataPackage, this);
        }

        if (!targetListInitialized)
        {
            targetList->setMainPic(imagePath);
            targetList->loadTargets(folderPath, filePath) ;
            mainWindow->addTab(targetList, title) ;
            targetListInitialized = true;
        }
        else
        {
            if (targetList->parent != this)
            {
                qFatal("targetList->parent != this");
            }

            bool found = mainWindow->findTab(targetList) ;
            if (!found)
            {
                mainWindow->addTab(targetList, title);
            }
        }
    }
}

void ImageWidget::on_pinButton_clicked()
{
    QList<ImageWidget*>& itemsList = mainWindow->getItems();

    for (int i = 0; i < itemsList.size(); ++i)
    {
        if (itemsList[i] == this) {
            itemsList.removeAt(i);
            break;
        }
    }

    itemsList.insert(0, this);

    mainWindow->refreshTable();
}

void ImageWidget::ImageLoader::run()
{
    if (filePath != "")
    {
        QPixmap *pix = new QPixmap();
        pix->load(filePath);
        imageWidget->image = pix->scaled(220, 220, Qt::KeepAspectRatioByExpanding, Qt::FastTransformation);
        imageWidget->ui->imageLabel->setPixmap(imageWidget->image);
        delete pix;
    }
}
