#include "mainwindow.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle(tr("Visual Perception Toolbox"));

    //Initializing local variables
    windowList = QVector<Window*>();
    activeWindow = NULL;
}

MainWindow::~MainWindow()
{
    delete ui;
    for(int i = 0; i < windowList.length(); i++)
        if (windowList[i] != NULL)
        {
            windowList[i]->~Window();
            windowList[i] = NULL;
        }
}
void MainWindow::on_actionClose_triggered()
{
    int idx = windowList.indexOf(activeWindow);
    if (activeWindow != NULL && idx != -1)
    {

        windowList.removeAt(idx);

        //                activeWindow->~Window();

        activeWindow->close();
        if ((typeid(*activeWindow) != typeid(VideoWindow)) || (typeid(*activeWindow) == typeid(VideoWindow) && static_cast<VideoWindow*>(activeWindow)->camera_or_video()))
            activeWindow->deleteLater();

        if (windowList.length() != 0)
            activeWindow = windowList.last();

        activeWindow->activateWindow();
    }
    //    qDebug()<<windowList.length();
}

void MainWindow::on_actionExit_triggered()
{
    QCoreApplication::quit();
}
void MainWindow::closeEvent( QCloseEvent *event )
{
    on_actionExit_triggered();
}

void MainWindow::on_actionOpen_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("Open Image"), "", tr("Image Files (*.png *.jpg *.bmp)"));

    //    QString fileName = QString("C:/Users/Mins/Documents/C++/vp/build-vp-kit1-Debug/George06.jpg");

    //exit function if no image selected
    if (fileName.isEmpty())
        return;

    // Load the image through opencv

    activeWindow = new Window(fileName, this);
    windowList.append(activeWindow);

    activeWindow->setWindowTitle(fileName);
    //    this->setMinimumSize(wdg->size());
    connect(activeWindow, SIGNAL(windowActivated()), this, SLOT(on_focusChanged()));
    connect(activeWindow, SIGNAL(windowClosed()), this, SLOT(on_actionClose_triggered()));
}

void MainWindow::on_actionMean_triggered()
{//Mean filter
    if (windowList.isEmpty()) return;

    bool ok;
    int num = QInputDialog::getInt(this, tr("QInputDialog::getInteger()"),
                                   tr("New Kernel Size:"), 3, 1, 1000, 1, &ok, Qt::WindowStaysOnTopHint);

    if (ok && (num > 0))
    {
        int  kernel_size = num;
        Blur blur_(kernel_size);

        if (typeid(*activeWindow) == typeid(VideoWindow))
        {
            if (!static_cast<VideoWindow*>(activeWindow)->camera_or_video())
            {
                activeWindow = new VideoWindow(this);
                activeWindow->setWindowTitle(QString("Camera"));
            }
        }
        else
        {
            Mat img = activeWindow->getImageMat();

            activeWindow = new Window(img, this);
        }

        windowList.append(activeWindow);
        connect(activeWindow, SIGNAL(windowActivated()), this, SLOT(on_focusChanged()));
        connect(activeWindow, SIGNAL(windowClosed()), this, SLOT(on_actionClose_triggered()));

        activeWindow->applyFilter(blur_);
        if (typeid(*activeWindow) == typeid(Window)) activeWindow->removeFilter();
    }
}

void MainWindow::on_actionMedian_triggered()
{//Median filter

    if (windowList.isEmpty()) return;
    Mat img;

    bool ok;
    int num = QInputDialog::getInt(this, tr("QInputDialog::getInteger()"),
                                   tr("New Kernel Size:"), 3, 3, 1000, 1, &ok, Qt::WindowStaysOnTopHint);
    if (ok && (num > 0))
    {
        int  kernel_size = num;

        if (typeid(*activeWindow) == typeid(VideoWindow))
        {
            if (!static_cast<VideoWindow*>(activeWindow)->camera_or_video())
            {
                activeWindow = new VideoWindow(this);
                activeWindow->setWindowTitle(QString("Camera"));
            }
        }
        else
        {
            Mat img = activeWindow->getImageMat();

            activeWindow = new Window(img, this);
        }
        windowList.append(activeWindow);
        connect(activeWindow, SIGNAL(windowActivated()), this, SLOT(on_focusChanged()));
        connect(activeWindow, SIGNAL(windowClosed()), this, SLOT(on_actionClose_triggered()));

        if (kernel_size % 2 == 0)
        {
            MedianBlur medianBlur_(kernel_size + 1);
            activeWindow->applyFilter(medianBlur_);
        }
        else
        {
            MedianBlur medianBlur_(kernel_size);
            activeWindow->applyFilter(medianBlur_);
        }
        if (typeid(*activeWindow) == typeid(Window)) activeWindow->removeFilter();
    }
}

void MainWindow::on_actionGaussian_triggered()
{
    if (windowList.isEmpty()) return;
    QDialog dialog(this);
    dialog.setWindowFlags(this->windowFlags() | Qt::WindowStaysOnTopHint);

    dialog.setWindowTitle(tr("Gaussian Filter"));

    QGridLayout* groupLayout = new QGridLayout(&dialog);

    //Kernel size input group
    QGroupBox* kernel_size_group = new QGroupBox(tr("Kernel Size"));
    QLabel *kernelX_label = new QLabel(tr("Width: "));
    QSpinBox *kernelX = new QSpinBox;
    kernelX->setRange(0, 100);
    kernelX->setSingleStep(2);
    kernelX->setValue(3);
    kernelX->findChild<QLineEdit*>()->setReadOnly(true);

    QLabel *kernelY_label = new QLabel(tr("Height: "));
    QSpinBox *kernelY = new QSpinBox;
    kernelY->setRange(0, 100);
    kernelY->setSingleStep(2);
    kernelY->setValue(3);
    kernelY->findChild<QLineEdit*>()->setReadOnly(true);

    QHBoxLayout* kernel_layout = new QHBoxLayout;
    kernel_layout->addWidget(kernelX_label);
    kernel_layout->addWidget(kernelX);
    kernel_layout->addWidget(kernelY_label);
    kernel_layout->addWidget(kernelY);

    kernel_size_group->setLayout(kernel_layout);

    //Sigmas input group
    QGroupBox* sigma_group = new QGroupBox(tr("Standard Deviation"));

    QLabel *sigmaX_label = new QLabel(tr("SigmaX: "));
    QSpinBox *sigmaX = new QSpinBox;
    sigmaX->setRange(0, 100);
    sigmaX->setSingleStep(1);
    sigmaX->setValue(1);

    QLabel *sigmaY_label = new QLabel(tr("SigmaY: "));
    QSpinBox *sigmaY = new QSpinBox;
    sigmaY->setRange(0, 100);
    sigmaY->setSingleStep(1);
    sigmaY->setValue(1);

    QHBoxLayout* sigma_layout = new QHBoxLayout;
    sigma_layout->addWidget(sigmaX_label);
    sigma_layout->addWidget(sigmaX);
    sigma_layout->addWidget(sigmaY_label);
    sigma_layout->addWidget(sigmaY);

    sigma_group->setLayout(sigma_layout);

    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
                                                       Qt::Horizontal, &dialog);

    groupLayout->addWidget(buttonBox, 3, 0,1,2);
    groupLayout->addWidget(kernel_size_group, 0, 0);
    groupLayout->addWidget(sigma_group, 1, 0);

    groupLayout->setAlignment(buttonBox, Qt::AlignCenter);

    connect(buttonBox, SIGNAL(accepted()), &dialog, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), &dialog, SLOT(reject()));
    //    kernel_size_group->adjustSize();

    if (dialog.exec() == QDialog::Accepted)
    {
        GaussianBlurFunctor gaussianBlurFunctor_(cv::Size(kernelX->value(), kernelY->value()),sigmaX->value(), sigmaY->value());
        if (typeid(*activeWindow) == typeid(VideoWindow))
        {
            if (!static_cast<VideoWindow*>(activeWindow)->camera_or_video())
            {
                activeWindow = new VideoWindow(this);
                activeWindow->setWindowTitle(QString("Camera"));
            }
        }
        else
        {
            Mat img = activeWindow->getImageMat();

            activeWindow = new Window(img, this);
        }
        windowList.append(activeWindow);
        connect(activeWindow, SIGNAL(windowActivated()), this, SLOT(on_focusChanged()));
        connect(activeWindow, SIGNAL(windowClosed()), this, SLOT(on_actionClose_triggered()));

        activeWindow->applyFilter(gaussianBlurFunctor_);
        if (typeid(*activeWindow) == typeid(Window)) activeWindow->removeFilter();
    }
}

void MainWindow::on_actionSalt_and_Pepper_triggered()
{
    if (windowList.isEmpty()) return;

    QDialog dialog(this);

    QFormLayout form(&dialog);


    form.addRow(new QLabel("Salt And Pepper Noise"));

    SliderWidget* slider = new SliderWidget(1, 100, 50, QString("Quantity"),true, &dialog);
    form.addRow(slider);

    QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
                               Qt::Horizontal, &dialog);
    form.addRow(&buttonBox);
    QObject::connect(&buttonBox, SIGNAL(accepted()), &dialog, SLOT(accept()));
    QObject::connect(&buttonBox, SIGNAL(rejected()), &dialog, SLOT(reject()));

    if (dialog.exec() == QDialog::Accepted) {
        int p = slider->getValue();

        SaltPaperFunctor saltPepper_(p);

        if (typeid(*activeWindow) == typeid(VideoWindow))
        {
            if (!static_cast<VideoWindow*>(activeWindow)->camera_or_video())
            {
                activeWindow = new VideoWindow(this);
                activeWindow->setWindowTitle(QString("Camera"));
            }
        }
        else
        {
            Mat img = activeWindow->getImageMat();

            activeWindow = new Window(img, this);
        }
        windowList.append(activeWindow);
        connect(activeWindow, SIGNAL(windowActivated()), this, SLOT(on_focusChanged()));
        connect(activeWindow, SIGNAL(windowClosed()), this, SLOT(on_actionClose_triggered()));

        activeWindow->applyFilter(saltPepper_);
        if (typeid(*activeWindow) == typeid(Window)) activeWindow->removeFilter();
    }
}

void MainWindow::on_actionSave_triggered()
{
    if (windowList.isEmpty()) return;

    if (activeWindow->getFileName().isEmpty())
        on_actionSave_As_triggered();
    else
        activeWindow->save();
}

void MainWindow::on_actionSave_As_triggered()
{
    if (windowList.isEmpty()) return;
    QFileDialog* dlg = new QFileDialog;
    dlg->setModal(true);
    dlg->setWindowFlags(Qt::WindowStaysOnTopHint);

    QString fileName = dlg->getSaveFileName(this, tr("Save As"),activeWindow->getFileName(),
                                            tr("Images (*.png *.xpm *.jpg)"));

    if (fileName.isEmpty()) {
        return;
    } else {
        activeWindow->save(fileName);
    }
}

void MainWindow::on_actionNew_triggered()
{

    activeWindow = new Window(this);
    windowList.append(activeWindow);
    activeWindow->setWindowTitle("NewFile");

    connect(activeWindow, SIGNAL(windowActivated()), this, SLOT(on_focusChanged()));
    connect(activeWindow, SIGNAL(windowClosed()), this, SLOT(on_actionClose_triggered()));
    connect(activeWindow, SIGNAL(windowClosed()), this, SLOT(on_actionClose_triggered()));
}
void MainWindow::on_focusChanged()
{
    Window* w = qobject_cast<Window*>(sender());
    if( w != NULL )
    {
        activeWindow = w;
    }
}

void MainWindow::on_actionFlip_Horizontally_triggered()
{
    if (windowList.isEmpty()) return;


    FlipFunctor flip_(1);

    if (typeid(*activeWindow) == typeid(VideoWindow))
    {
        if (!static_cast<VideoWindow*>(activeWindow)->camera_or_video())
        {
            activeWindow = new VideoWindow(this);
            activeWindow->setWindowTitle(QString("Camera"));
        }
    }
    else
    {
        Mat img = activeWindow->getImageMat();

        activeWindow = new Window(img, this);
    }
    windowList.append(activeWindow);
    connect(activeWindow, SIGNAL(windowActivated()), this, SLOT(on_focusChanged()));
    connect(activeWindow, SIGNAL(windowClosed()), this, SLOT(on_actionClose_triggered()));

    activeWindow->applyFilter(flip_);
    if (typeid(*activeWindow) == typeid(Window)) activeWindow->removeFilter();
}

void MainWindow::on_actionFlip_Vertically_triggered()
{
    if (windowList.isEmpty()) return;

    FlipFunctor flip_(0);
    if (typeid(*activeWindow) == typeid(VideoWindow))
    {
        if (!static_cast<VideoWindow*>(activeWindow)->camera_or_video())
        {
            activeWindow = new VideoWindow(this);
            activeWindow->setWindowTitle(QString("Camera"));
        }
    }
    else
    {
        Mat img = activeWindow->getImageMat();

        activeWindow = new Window(img, this);
    }
    windowList.append(activeWindow);
    connect(activeWindow, SIGNAL(windowActivated()), this, SLOT(on_focusChanged()));
    connect(activeWindow, SIGNAL(windowClosed()), this, SLOT(on_actionClose_triggered()));

    activeWindow->applyFilter(flip_);
    if (typeid(*activeWindow) == typeid(Window)) activeWindow->removeFilter();
}

void MainWindow::on_actionRotate_Clockwise_triggered()
{
    if (windowList.isEmpty()) return;

    RotateFunctor rotate_(1);
    if (typeid(*activeWindow) == typeid(VideoWindow))
    {
        if (!static_cast<VideoWindow*>(activeWindow)->camera_or_video())
        {
            activeWindow = new VideoWindow(this);
            activeWindow->setWindowTitle(QString("Camera"));
        }
    }
    else
    {
        Mat img = activeWindow->getImageMat();

        activeWindow = new Window(img, this);
    }
    windowList.append(activeWindow);
    connect(activeWindow, SIGNAL(windowActivated()), this, SLOT(on_focusChanged()));
    connect(activeWindow, SIGNAL(windowClosed()), this, SLOT(on_actionClose_triggered()));

    activeWindow->applyFilter(rotate_);
    if (typeid(*activeWindow) == typeid(Window)) activeWindow->removeFilter();
}

void MainWindow::on_actionRotate_Anti_Clockwise_triggered()
{
    if (windowList.isEmpty()) return;
    RotateFunctor rotate_(0);
    if (typeid(*activeWindow) == typeid(VideoWindow))
    {
        if (!static_cast<VideoWindow*>(activeWindow)->camera_or_video())
        {
            activeWindow = new VideoWindow(this);
            activeWindow->setWindowTitle(QString("Camera"));
        }
    }
    else
    {
        Mat img = activeWindow->getImageMat();

        activeWindow = new Window(img, this);
    }
    windowList.append(activeWindow);
    connect(activeWindow, SIGNAL(windowActivated()), this, SLOT(on_focusChanged()));
    connect(activeWindow, SIGNAL(windowClosed()), this, SLOT(on_actionClose_triggered()));

    activeWindow->applyFilter(rotate_);
    if (typeid(*activeWindow) == typeid(Window)) activeWindow->removeFilter();
}

void MainWindow::on_actionHistogram_Equalization_triggered()
{
    if (windowList.isEmpty()) return;

    HisteqFunctor histeq_;

    if (typeid(*activeWindow) == typeid(VideoWindow))
    {
        if (!static_cast<VideoWindow*>(activeWindow)->camera_or_video())
        {
            activeWindow = new VideoWindow(this);
            activeWindow->setWindowTitle(QString("Camera"));
        }
    }
    else
    {
        Mat img = activeWindow->getImageMat();

        activeWindow = new Window(img, this);
    }
    windowList.append(activeWindow);
    connect(activeWindow, SIGNAL(windowActivated()), this, SLOT(on_focusChanged()));
    connect(activeWindow, SIGNAL(windowClosed()), this, SLOT(on_actionClose_triggered()));

    activeWindow->applyFilter(histeq_);
    if (typeid(*activeWindow) == typeid(Window)) activeWindow->removeFilter();
}

void MainWindow::on_actionInvert_triggered()
{
    if (windowList.isEmpty()) return;

    InvertFunctor invert_;
    if (typeid(*activeWindow) == typeid(VideoWindow))
    {
        if (!static_cast<VideoWindow*>(activeWindow)->camera_or_video())
        {
            activeWindow = new VideoWindow(this);
            activeWindow->setWindowTitle(QString("Camera"));
        }
    }
    else
    {
        Mat img = activeWindow->getImageMat();

        activeWindow = new Window(img, this);
    }
    windowList.append(activeWindow);
    connect(activeWindow, SIGNAL(windowActivated()), this, SLOT(on_focusChanged()));
    connect(activeWindow, SIGNAL(windowClosed()), this, SLOT(on_actionClose_triggered()));

    activeWindow->applyFilter(invert_);
    if (typeid(*activeWindow) == typeid(Window)) activeWindow->removeFilter();
}

void MainWindow::on_actionCanny_triggered()
{//http://docs.opencv.org/2.4/doc/tutorials/imgproc/imgtrans/canny_detector/canny_detector.html

    if (windowList.isEmpty()) return;

    QDialog dialog(this);
    QFormLayout form(&dialog);
    form.addRow(new QLabel("Canny Edge Detection Settings"));

    SliderWidget* slider1 = new SliderWidget(1, 100, 80, QString("Threshold "),false, &dialog);
    form.addRow(slider1);

    SliderWidget* slider2 = new SliderWidget(1, 10, 3,QString("Ratio "),false, &dialog);
    form.addRow(slider2);

    SliderWidget* slider3 = new SliderWidget(2, 50, 3,QString("Kernel Size "),false, &dialog);
    form.addRow(slider3);

    QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
                               Qt::Horizontal, &dialog);
    form.addRow(&buttonBox);
    QObject::connect(&buttonBox, SIGNAL(accepted()), &dialog, SLOT(accept()));
    QObject::connect(&buttonBox, SIGNAL(rejected()), &dialog, SLOT(reject()));
    if (dialog.exec() == QDialog::Accepted) {
        int ts = slider1->getValue();
        int ratio = slider2->getValue();
        int kernel_size = slider2->getValue();

        CannyFunctor canny_(ts, ratio, kernel_size);

        if (typeid(*activeWindow) == typeid(VideoWindow))
        {
            if (!static_cast<VideoWindow*>(activeWindow)->camera_or_video())
            {
                activeWindow = new VideoWindow(this);
                activeWindow->setWindowTitle(QString("Camera"));
            }
        }
        else
        {
            Mat img = activeWindow->getImageMat();

            activeWindow = new Window(img, this);
        }
        windowList.append(activeWindow);
        connect(activeWindow, SIGNAL(windowActivated()), this, SLOT(on_focusChanged()));
        connect(activeWindow, SIGNAL(windowClosed()), this, SLOT(on_actionClose_triggered()));

        activeWindow->applyFilter(canny_);
        if (typeid(*activeWindow) == typeid(Window)) activeWindow->removeFilter();
    }
}

void MainWindow::on_actionSobel_triggered()
{//http://docs.opencv.org/2.4/doc/tutorials/imgproc/imgtrans/sobel_derivatives/sobel_derivatives.html

    if (windowList.isEmpty()) return;

    QDialog dialog(this);
    QFormLayout form(&dialog);
    form.addRow(new QLabel("Sobel Edge Detection Settings"));

    SliderWidget* slider1 = new SliderWidget(1, 100, 1, QString("Scale "),false, &dialog);
    form.addRow(slider1);

    SliderWidget* slider2 = new SliderWidget(1, 100, 0, QString("Delta "),true, &dialog);
    form.addRow(slider2);

    QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
                               Qt::Horizontal, &dialog);
    form.addRow(&buttonBox);
    QObject::connect(&buttonBox, SIGNAL(accepted()), &dialog, SLOT(accept()));
    QObject::connect(&buttonBox, SIGNAL(rejected()), &dialog, SLOT(reject()));
    if (dialog.exec() == QDialog::Accepted) {
        int scale = slider1->getValue();
        int delta = slider2->getValue();

        SobelFunctor sobel_(scale, delta);

        if (typeid(*activeWindow) == typeid(VideoWindow))
        {
            if (!static_cast<VideoWindow*>(activeWindow)->camera_or_video())
            {
                activeWindow = new VideoWindow(this);
                activeWindow->setWindowTitle(QString("Camera"));
            }
        }
        else
        {
            Mat img = activeWindow->getImageMat();

            activeWindow = new Window(img, this);
        }
        windowList.append(activeWindow);
        connect(activeWindow, SIGNAL(windowActivated()), this, SLOT(on_focusChanged()));
        connect(activeWindow, SIGNAL(windowClosed()), this, SLOT(on_actionClose_triggered()));

        activeWindow->applyFilter(sobel_);
        if (typeid(*activeWindow) == typeid(Window)) activeWindow->removeFilter();
    }
}

void MainWindow::on_actionLaplacian_triggered()
{
    if (windowList.isEmpty()) return;

    Mat src = activeWindow->getImageMat();


    LaplacianFunctor laplacian_;
    if (typeid(*activeWindow) == typeid(VideoWindow))
    {
        if (!static_cast<VideoWindow*>(activeWindow)->camera_or_video())
        {
            activeWindow = new VideoWindow(this);
            activeWindow->setWindowTitle(QString("Camera"));
        }
    }
    else
    {
        Mat img = activeWindow->getImageMat();

        activeWindow = new Window(img, this);
    }
    windowList.append(activeWindow);
    connect(activeWindow, SIGNAL(windowActivated()), this, SLOT(on_focusChanged()));
    connect(activeWindow, SIGNAL(windowClosed()), this, SLOT(on_actionClose_triggered()));

    activeWindow->applyFilter(laplacian_);
    if (typeid(*activeWindow) == typeid(Window)) activeWindow->removeFilter();
}

void MainWindow::on_actionDilation_triggered()
{
    if (windowList.isEmpty()) return;

    QDialog dialog(this);

    QFormLayout form(&dialog);


    form.addRow(new QLabel("Dilation Settings"));

    SliderWidget* slider1 = new SliderWidget(0, 2, 0,QString("Structure Element: 0-Rect, 1-Cross, 2-Elliplse"),false, &dialog);
    form.addRow(slider1);

    SliderWidget* slider2 = new SliderWidget(1, 20, 1,QString("Kernel Size: 2n+1"),false, &dialog);
    form.addRow(slider2);

    QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
                               Qt::Horizontal, &dialog);
    form.addRow(&buttonBox);
    QObject::connect(&buttonBox, SIGNAL(accepted()), &dialog, SLOT(accept()));
    QObject::connect(&buttonBox, SIGNAL(rejected()), &dialog, SLOT(reject()));
    if (dialog.exec() == QDialog::Accepted) {
        int dilation_elem = slider1->getValue();
        int dilation_size = slider2->getValue();
        int dilation_type;
        if( dilation_elem == 0 ){ dilation_type = MORPH_RECT; }
        else if( dilation_elem == 1 ){ dilation_type = MORPH_CROSS; }
        else if( dilation_elem == 2) { dilation_type = MORPH_ELLIPSE; }

        Mat element = getStructuringElement( dilation_type,
                                             Size( 2*dilation_size + 1, 2*dilation_size+1 ),
                                             Point( dilation_size, dilation_size ) );
        // Apply the dilation operation

        DilateFunctor dilate_(element);
        if (typeid(*activeWindow) == typeid(VideoWindow))
        {
            if (!static_cast<VideoWindow*>(activeWindow)->camera_or_video())
            {
                activeWindow = new VideoWindow(this);
                activeWindow->setWindowTitle(QString("Camera"));
            }
        }
        else
        {
            Mat img = activeWindow->getImageMat();

            activeWindow = new Window(img, this);
        }
        windowList.append(activeWindow);
        connect(activeWindow, SIGNAL(windowActivated()), this, SLOT(on_focusChanged()));
        connect(activeWindow, SIGNAL(windowClosed()), this, SLOT(on_actionClose_triggered()));

        activeWindow->applyFilter(dilate_);
        if (typeid(*activeWindow) == typeid(Window)) activeWindow->removeFilter();
    }
}


void MainWindow::on_actionErosion_triggered()
{//http://docs.opencv.org/2.4/doc/tutorials/imgproc/erosion_dilatation/erosion_dilatation.html
    if (windowList.isEmpty()) return;

    QDialog dialog(this);

    QFormLayout form(&dialog);


    form.addRow(new QLabel("Erosion Settings"));

    SliderWidget* slider1 = new SliderWidget(0, 2, 0,QString("Structure Element: 0-Rect, 1-Cross, 2-Elliplse"),false, &dialog);
    form.addRow(slider1);

    SliderWidget* slider2 = new SliderWidget(1, 20, 1,QString("Kernel Size: 2n+1"),false, &dialog);
    form.addRow(slider2);

    QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
                               Qt::Horizontal, &dialog);
    form.addRow(&buttonBox);
    QObject::connect(&buttonBox, SIGNAL(accepted()), &dialog, SLOT(accept()));
    QObject::connect(&buttonBox, SIGNAL(rejected()), &dialog, SLOT(reject()));
    if (dialog.exec() == QDialog::Accepted) {
        int erosion_elem = slider1->getValue();
        int erosion_size = slider2->getValue();
        int erosion_type;
        if( erosion_elem == 0 ){ erosion_type = MORPH_RECT; }
        else if( erosion_elem == 1 ){ erosion_type = MORPH_CROSS; }
        else if( erosion_elem == 2) { erosion_type = MORPH_ELLIPSE; }

        Mat element = getStructuringElement( erosion_type,
                                             Size( 2*erosion_size + 1, 2*erosion_size+1 ),
                                             Point( erosion_size, erosion_size ) );

        ErodeFunctor erode_(element);
        if (typeid(*activeWindow) == typeid(VideoWindow))
        {
            if (!static_cast<VideoWindow*>(activeWindow)->camera_or_video())
            {
                activeWindow = new VideoWindow(this);
                activeWindow->setWindowTitle(QString("Camera"));
            }
        }
        else
        {
            Mat img = activeWindow->getImageMat();

            activeWindow = new Window(img, this);
        }
        windowList.append(activeWindow);
        connect(activeWindow, SIGNAL(windowActivated()), this, SLOT(on_focusChanged()));
        connect(activeWindow, SIGNAL(windowClosed()), this, SLOT(on_actionClose_triggered()));

        activeWindow->applyFilter(erode_);
        if (typeid(*activeWindow) == typeid(Window)) activeWindow->removeFilter();
    }
}

void MainWindow::on_actionOpening_triggered()
{
    if (windowList.isEmpty()) return;

    QDialog dialog(this);

    QFormLayout form(&dialog);


    form.addRow(new QLabel("Opening Settings"));

    SliderWidget* slider1 = new SliderWidget(0, 2, 0,QString("Structure Element: 0-Rect, 1-Cross, 2-Elliplse"),false, &dialog);
    form.addRow(slider1);

    SliderWidget* slider2 = new SliderWidget(1, 20, 1,QString("Kernel Size: 2n+1"),false, &dialog);
    form.addRow(slider2);

    QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
                               Qt::Horizontal, &dialog);
    form.addRow(&buttonBox);
    QObject::connect(&buttonBox, SIGNAL(accepted()), &dialog, SLOT(accept()));
    QObject::connect(&buttonBox, SIGNAL(rejected()), &dialog, SLOT(reject()));
    if (dialog.exec() == QDialog::Accepted) {
        int elem = slider1->getValue();
        int elem_size = slider2->getValue();
        int elem_type;
        if( elem == 0 ){ elem_type = MORPH_RECT; }
        else if( elem == 1 ){ elem_type = MORPH_CROSS; }
        else if( elem == 2) { elem_type = MORPH_ELLIPSE; }

        Mat element = getStructuringElement( elem_type,
                                             Size( 2*elem_size + 1, 2*elem_size+1 ),
                                             Point( elem_size, elem_size ) );

        OpeningFunctor opening_(element);
        if (typeid(*activeWindow) == typeid(VideoWindow))
        {
            if (!static_cast<VideoWindow*>(activeWindow)->camera_or_video())
            {
                activeWindow = new VideoWindow(this);
                activeWindow->setWindowTitle(QString("Camera"));
            }
        }
        else
        {
            Mat img = activeWindow->getImageMat();

            activeWindow = new Window(img, this);
        }
        windowList.append(activeWindow);
        connect(activeWindow, SIGNAL(windowActivated()), this, SLOT(on_focusChanged()));
        connect(activeWindow, SIGNAL(windowClosed()), this, SLOT(on_actionClose_triggered()));

        activeWindow->applyFilter(opening_);
        if (typeid(*activeWindow) == typeid(Window)) activeWindow->removeFilter();

    }
}

void MainWindow::on_actionClosing_triggered()
{
    if (windowList.isEmpty()) return;

    QDialog dialog(this);

    QFormLayout form(&dialog);


    form.addRow(new QLabel("Closing Settings"));

    SliderWidget* slider1 = new SliderWidget(0, 2, 0,QString("Structure Element: 0-Rect, 1-Cross, 2-Elliplse"),false, &dialog);
    form.addRow(slider1);

    SliderWidget* slider2 = new SliderWidget(1, 20, 1,QString("Kernel Size: 2n+1"),false, &dialog);
    form.addRow(slider2);

    QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
                               Qt::Horizontal, &dialog);
    form.addRow(&buttonBox);
    QObject::connect(&buttonBox, SIGNAL(accepted()), &dialog, SLOT(accept()));
    QObject::connect(&buttonBox, SIGNAL(rejected()), &dialog, SLOT(reject()));
    if (dialog.exec() == QDialog::Accepted) {
        int elem = slider1->getValue();
        int elem_size = slider2->getValue();
        int elem_type;
        if( elem == 0 ){ elem_type = MORPH_RECT; }
        else if( elem == 1 ){ elem_type = MORPH_CROSS; }
        else if( elem == 2) { elem_type = MORPH_ELLIPSE; }

        Mat element = getStructuringElement( elem_type,
                                             Size( 2*elem_size + 1, 2*elem_size+1 ),
                                             Point( elem_size, elem_size ) );

        ClosingFunctor closing_(element);
        if (typeid(*activeWindow) == typeid(VideoWindow))
        {
            if (!static_cast<VideoWindow*>(activeWindow)->camera_or_video())
            {
                activeWindow = new VideoWindow(this);
                activeWindow->setWindowTitle(QString("Camera"));
            }
        }
        else
        {
            Mat img = activeWindow->getImageMat();

            activeWindow = new Window(img, this);
        }
        windowList.append(activeWindow);
        connect(activeWindow, SIGNAL(windowActivated()), this, SLOT(on_focusChanged()));
        connect(activeWindow, SIGNAL(windowClosed()), this, SLOT(on_actionClose_triggered()));

        activeWindow->applyFilter(closing_);
        if (typeid(*activeWindow) == typeid(Window)) activeWindow->removeFilter();
    }
}

void MainWindow::on_actionUniform_triggered()
{
    if (windowList.isEmpty()) return;

}

void MainWindow::on_actionCamera_Calibration_triggered()
{
    QString pwd("");
    char * PWD;
    PWD = getenv ("PWD");
    pwd.append(PWD);
    qDebug() << pwd;

    char *s = "../visual_perception_toolbox/cameraCalibration/default.xml";
    camera_calib(1, &s);
}

void MainWindow::on_actionConvolution_Mask_triggered()
{
}

void MainWindow::on_actionHistogram_triggered()
{//http://docs.opencv.org/2.4/doc/tutorials/imgproc/histograms/histogram_calculation/histogram_calculation.html

    if (windowList.isEmpty()) return;

    // Load image
    Mat src = activeWindow->getImageMat();

    if( !src.data )
    { return; }

    /// Separate the image in 3 places ( B, G and R )
    vector<Mat> bgr_planes;
    split( src, bgr_planes );

    /// Establish the number of bins
    int histSize = 256;

    /// Set the ranges ( for B,G,R) )
    float range[] = { 0, 256 } ;
    const float* histRange = { range };

    bool uniform = true; bool accumulate = false;

    Mat b_hist, g_hist, r_hist;

    // Compute the histograms:
    calcHist( &bgr_planes[0], 1, 0, Mat(), b_hist, 1, &histSize, &histRange, uniform, accumulate );
    calcHist( &bgr_planes[1], 1, 0, Mat(), g_hist, 1, &histSize, &histRange, uniform, accumulate );
    calcHist( &bgr_planes[2], 1, 0, Mat(), r_hist, 1, &histSize, &histRange, uniform, accumulate );

    // Draw the histograms for B, G and R
    int hist_w = 512; int hist_h = 400;
    int bin_w = cvRound( (double) hist_w/histSize );

    Mat histImage( hist_h, hist_w, CV_8UC3, Scalar( 0,0,0) );

    /// Normalize the result to [ 0, histImage.rows ]
    normalize(b_hist, b_hist, 0, histImage.rows, NORM_MINMAX, -1, Mat() );
    normalize(g_hist, g_hist, 0, histImage.rows, NORM_MINMAX, -1, Mat() );
    normalize(r_hist, r_hist, 0, histImage.rows, NORM_MINMAX, -1, Mat() );

    /// Draw for each channel
    for( int i = 1; i < histSize; i++ )
    {
        line( histImage, Point( bin_w*(i-1), hist_h - cvRound(b_hist.at<float>(i-1)) ) ,
              Point( bin_w*(i), hist_h - cvRound(b_hist.at<float>(i)) ),
              Scalar( 255, 0, 0), 2, 8, 0  );
        line( histImage, Point( bin_w*(i-1), hist_h - cvRound(g_hist.at<float>(i-1)) ) ,
              Point( bin_w*(i), hist_h - cvRound(g_hist.at<float>(i)) ),
              Scalar( 0, 255, 0), 2, 8, 0  );
        line( histImage, Point( bin_w*(i-1), hist_h - cvRound(r_hist.at<float>(i-1)) ) ,
              Point( bin_w*(i), hist_h - cvRound(r_hist.at<float>(i)) ),
              Scalar( 0, 0, 255), 2, 8, 0  );
    }

    activeWindow = new Window(histImage, this);
    activeWindow->setWindowTitle("Histogram");
    windowList.append(activeWindow);
    connect(activeWindow, SIGNAL(windowActivated()), this, SLOT(on_focusChanged()));
    connect(activeWindow, SIGNAL(windowClosed()), this, SLOT(on_actionClose_triggered()));
}

void MainWindow::on_actionSIFT_triggered()
{//http://stackoverflow.com/questions/5461148/sift-implementation-with-opencv-2-2
    if (windowList.isEmpty()) return;


    SiftFunctor sift_;
    if (typeid(*activeWindow) == typeid(VideoWindow))
    {
        if (!static_cast<VideoWindow*>(activeWindow)->camera_or_video())
        {
            activeWindow = new VideoWindow(this);
            activeWindow->setWindowTitle(QString("Camera"));
        }
    }
    else
    {
        Mat img = activeWindow->getImageMat();

        activeWindow = new Window(img, this);
    }
    windowList.append(activeWindow);
    connect(activeWindow, SIGNAL(windowActivated()), this, SLOT(on_focusChanged()));
    connect(activeWindow, SIGNAL(windowClosed()), this, SLOT(on_actionClose_triggered()));

    activeWindow->applyFilter(sift_);
    if (typeid(*activeWindow) == typeid(Window)) activeWindow->removeFilter();
}

void MainWindow::on_actionSURF_triggered()
{
    if (windowList.isEmpty()) return;


    SurfFunctor surf_;
    if (typeid(*activeWindow) == typeid(VideoWindow))
    {
        if (!static_cast<VideoWindow*>(activeWindow)->camera_or_video())
        {
            activeWindow = new VideoWindow(this);
            activeWindow->setWindowTitle(QString("Camera"));
        }
    }
    else
    {
        Mat img = activeWindow->getImageMat();

        activeWindow = new Window(img, this);
    }
    windowList.append(activeWindow);
    connect(activeWindow, SIGNAL(windowActivated()), this, SLOT(on_focusChanged()));
    connect(activeWindow, SIGNAL(windowClosed()), this, SLOT(on_actionClose_triggered()));

    activeWindow->applyFilter(surf_);
    if (typeid(*activeWindow) == typeid(Window)) activeWindow->removeFilter();

}
void MainWindow::on_actionHarris_triggered()
{
    if (windowList.isEmpty()) return;


    HarrisFunctor harris_;
    if (typeid(*activeWindow) == typeid(VideoWindow))
    {
        if (!static_cast<VideoWindow*>(activeWindow)->camera_or_video())
        {
            activeWindow = new VideoWindow(this);
            activeWindow->setWindowTitle(QString("Camera"));
        }
        else
            static_cast<VideoWindow*>(activeWindow)->setDelay(30);
    }
    else
    {
        Mat img = activeWindow->getImageMat();

        activeWindow = new Window(img, this);
    }
    windowList.append(activeWindow);
    connect(activeWindow, SIGNAL(windowActivated()), this, SLOT(on_focusChanged()));
    connect(activeWindow, SIGNAL(windowClosed()), this, SLOT(on_actionClose_triggered()));

    activeWindow->applyFilter(harris_);
    if (typeid(*activeWindow) == typeid(Window)) activeWindow->removeFilter();
}

void MainWindow::on_actionOpen_Video_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("Open Video"), "", tr("Video Files (*.avi *.mp4 *.flv)"));

    //exit function if no image selected
    if (fileName.isEmpty())
        return;

    // Load the image through opencv

    activeWindow = new VideoWindow(fileName, this);
    windowList.append(activeWindow);

    activeWindow->setWindowTitle(fileName);
    connect(activeWindow, SIGNAL(windowActivated()), this, SLOT(on_focusChanged()));
    connect(activeWindow, SIGNAL(windowClosed()), this, SLOT(on_actionClose_triggered()));
}

void MainWindow::on_actionDraw_Epipolar_Lines_triggered()
{
    QDialog dialog(this);
    dialog.setWindowFlags(this->windowFlags() | Qt::WindowStaysOnTopHint);

    dialog.setWindowTitle(tr("Settings"));

    QGridLayout* groupLayout = new QGridLayout(&dialog);

    //Feature descriptor type
    QGroupBox* feature_type_group = new QGroupBox(tr("Feature Descriptor"));

    QRadioButton *surfRadio = new QRadioButton(tr("SURF"));
    QRadioButton *siftRadio = new QRadioButton(tr("SIFT"));

    surfRadio->setChecked(true);

    QVBoxLayout *vbox = new QVBoxLayout;
    vbox->addWidget(siftRadio);
    vbox->addWidget(surfRadio);

    vbox->addStretch(1);
    feature_type_group->setLayout(vbox);

    groupLayout->addWidget(feature_type_group);

    QGroupBox* fileGroup = new QGroupBox(tr("Images"));

    QRadioButton *viewRadio = new QRadioButton(tr("Use Open Windows"));
    QRadioButton *fileRadio = new QRadioButton(tr("Browse.."));

    viewRadio->setChecked(true);

    QVBoxLayout *vbox2 = new QVBoxLayout;
    vbox2->addWidget(viewRadio);
    vbox2->addWidget(fileRadio);

    vbox2->addStretch(1);
    fileGroup->setLayout(vbox2);
    groupLayout->addWidget(fileGroup);


    QGroupBox* methodGroup = new QGroupBox(tr("Fundamental Matrix Method"));

    QRadioButton *sevenRadio = new QRadioButton(tr("7-Point algorithm"));
    QRadioButton *eightRadio = new QRadioButton(tr("8-Point algorithm"));
    QRadioButton *ransacRadio = new QRadioButton(tr("RANSAC"));

    eightRadio->setChecked(true);

    QVBoxLayout *vbox3 = new QVBoxLayout;
    vbox3->addWidget(sevenRadio);
    vbox3->addWidget(eightRadio);
    vbox3->addWidget(ransacRadio);

    vbox3->addStretch(1);
    methodGroup->setLayout(vbox3);
    groupLayout->addWidget(methodGroup);

    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
                                                       Qt::Horizontal, &dialog);

    groupLayout->addWidget(buttonBox);

    groupLayout->setAlignment(buttonBox, Qt::AlignCenter);

    connect(buttonBox, SIGNAL(accepted()), &dialog, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), &dialog, SLOT(reject()));

    if (dialog.exec() == QDialog::Accepted)
    {
        Mat input_gray1;
        Mat input_gray2;
        Mat input1;
        Mat input2;
        Mat output1;
        Mat output2;

        if (fileRadio->isChecked())
        {
            QStringList filenames = QFileDialog::getOpenFileNames(this,
                                                                  tr("Open Images"), "", tr("Image Files (*.png *.jpg *.bmp)"));
            if(filenames.isEmpty()) return;

            input1 = cv::imread(filenames.at(0).toStdString());
            input2 = cv::imread(filenames.at(1).toStdString());
        }
        else
            if (viewRadio->isChecked())
            {
                if (windowList.isEmpty()) return;
                activeWindow->getImageMat().copyTo(input1);

                if (windowList.length() < 2) return;
                Window* secondView = (windowList.last() != activeWindow)? windowList.last() : windowList[windowList.length()-2];
                secondView->getImageMat().copyTo(input2);

            }
        if (input1.channels() == 3)
            cvtColor(input1, input_gray1, CV_BGR2GRAY);
        else input_gray1 = input1;
        if (input2.channels() == 3)
            cvtColor(input2, input_gray2, CV_BGR2GRAY);
        else input_gray2 = input2;

        input1.copyTo(output1);
        input2.copyTo(output2);

        int minHessian = 400;

        Mat descriptors_1, descriptors_2;
        std::vector<KeyPoint> keypoints_1, keypoints_2;

        if (siftRadio->isChecked()) {
            SiftFeatureDetector detector(minHessian);
            detector.detect(input_gray1, keypoints_1);
            detector.detect(input_gray2, keypoints_2);
            SiftDescriptorExtractor extractor;
            extractor.compute(input_gray1, keypoints_1, descriptors_1 );
            extractor.compute(input_gray2, keypoints_2, descriptors_2 );

        }
        if (surfRadio->isChecked())
        {
            SurfFeatureDetector detector(minHessian);
            detector.detect(input_gray1, keypoints_1);
            detector.detect(input_gray2, keypoints_2);
            SurfDescriptorExtractor extractor;
            extractor.compute(input_gray1, keypoints_1, descriptors_1 );
            extractor.compute(input_gray2, keypoints_2, descriptors_2 );

        }

        std::vector<std::vector<cv::DMatch>> matches;
        cv::BFMatcher matcher;
        matcher.knnMatch(descriptors_1, descriptors_2, matches, 2);  // Find two nearest matches

        vector<cv::DMatch> good_matches;

        for (int i = 0; i < matches.size(); ++i)
        {

            if (matches[i][0].distance < 0.8 * matches[i][1].distance)
            {
                good_matches.push_back(matches[i][0]);
            }
        }

        std::vector<int> pointIndexes1;
        std::vector<int> pointIndexes2;
        for (std::vector<cv::DMatch>::const_iterator it= good_matches.begin();
             it!= good_matches.end(); ++it) {

            // Get the indexes of the selected matched keypoints
            pointIndexes1.push_back(it->queryIdx);
            pointIndexes2.push_back(it->trainIdx);
        }

        std::vector<cv::Point2f> selPoints1, selPoints2;
        cv::KeyPoint::convert(keypoints_1,selPoints1,pointIndexes1);
        cv::KeyPoint::convert(keypoints_2,selPoints2,pointIndexes2);
        Mat fundamental;

        if (sevenRadio->isChecked())
        {
            if (int(selPoints1.size()) < 7) {qDebug()<<"Too few points found"; return;}
            fundamental = cv::findFundamentalMat(cv::Mat(selPoints1), cv::Mat(selPoints2),CV_FM_7POINT);
        }
        if (eightRadio->isChecked())
        {
            if (int(selPoints1.size()) < 8) {qDebug()<<"Too few points found"; return;}
            fundamental = cv::findFundamentalMat(cv::Mat(selPoints1), cv::Mat(selPoints2),CV_FM_8POINT);
        }
        if (ransacRadio->isChecked())
        {
            if (int(selPoints1.size()) < 8) {qDebug()<<"Too few points found"; return;}
            fundamental = cv::findFundamentalMat(cv::Mat(selPoints1), cv::Mat(selPoints2),CV_FM_RANSAC);
        }


        // draw the left points corresponding epipolar lines in right image
        std::vector<cv::Vec3f> lines1;
        cv::computeCorrespondEpilines(cv::Mat(selPoints1),1,fundamental,lines1);
        for (vector<cv::Vec3f>::const_iterator it= lines1.begin();
             it!=lines1.end(); ++it) {

            // draw the epipolar line between first and last column
            cv::line(output2,cv::Point(0,-(*it)[2]/(*it)[1]),
                    cv::Point(output2.cols,-((*it)[2]+(*it)[0]*output2.cols)/(*it)[1]),
                    cv::Scalar(255,255,255));
        }

        // draw the left points corresponding epipolar lines in left image
        std::vector<cv::Vec3f> lines2;
        cv::computeCorrespondEpilines(cv::Mat(selPoints2),2,fundamental,lines2);
        for (vector<cv::Vec3f>::const_iterator it= lines2.begin();
             it!=lines2.end(); ++it) {

            // draw the epipolar line between first and last column
            cv::line(output1,cv::Point(0,-(*it)[2]/(*it)[1]),
                    cv::Point(output1.cols,-((*it)[2]+(*it)[0]*output1.cols)/(*it)[1]),
                    cv::Scalar(255,255,255));
        }

        activeWindow = new Window(output1, this);
        windowList.append(activeWindow);
        activeWindow->setWindowTitle(QString("Image Epipolar Lines"));
        connect(activeWindow, SIGNAL(windowActivated()), this, SLOT(on_focusChanged()));
        connect(activeWindow, SIGNAL(windowClosed()), this, SLOT(on_actionClose_triggered()));

        activeWindow = new Window(output2, this);

        windowList.append(activeWindow);
        activeWindow->setWindowTitle(QString("Image Epipolar Lines"));
        connect(activeWindow, SIGNAL(windowActivated()), this, SLOT(on_focusChanged()));
        connect(activeWindow, SIGNAL(windowClosed()), this, SLOT(on_actionClose_triggered()));

    }
}

void MainWindow::on_actionMatch_Features_triggered()
{
    QDialog dialog(this);
    dialog.setWindowFlags(this->windowFlags() | Qt::WindowStaysOnTopHint);

    dialog.setWindowTitle(tr("Settings"));

    QGridLayout* groupLayout = new QGridLayout(&dialog);

    //Feature descriptor type
    QGroupBox* feature_type_group = new QGroupBox(tr("Feature Descriptor"));

    QRadioButton *surfRadio = new QRadioButton(tr("SURF"));
    QRadioButton *siftRadio = new QRadioButton(tr("SIFT"));

    surfRadio->setChecked(true);

    QVBoxLayout *vbox = new QVBoxLayout;
    vbox->addWidget(siftRadio);
    vbox->addWidget(surfRadio);

    vbox->addStretch(1);
    feature_type_group->setLayout(vbox);

    groupLayout->addWidget(feature_type_group);

    QGroupBox* fileGroup = new QGroupBox(tr("Images"));

    QRadioButton *viewRadio = new QRadioButton(tr("Use Open Views"));
    QRadioButton *fileRadio = new QRadioButton(tr("Browse.."));

    viewRadio->setChecked(true);

    QVBoxLayout *vbox2 = new QVBoxLayout;
    vbox2->addWidget(viewRadio);
    vbox2->addWidget(fileRadio);

    vbox2->addStretch(1);
    fileGroup->setLayout(vbox2);
    groupLayout->addWidget(fileGroup);

    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
                                                       Qt::Horizontal, &dialog);

    groupLayout->addWidget(buttonBox);

    groupLayout->setAlignment(buttonBox, Qt::AlignCenter);

    connect(buttonBox, SIGNAL(accepted()), &dialog, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), &dialog, SLOT(reject()));

    if (dialog.exec() == QDialog::Accepted)
    {
        Mat input_gray1;
        Mat input_gray2;
        Mat input1;
        Mat input2;
        if (fileRadio->isChecked())
        {
            QStringList filenames = QFileDialog::getOpenFileNames(this,
                                                                  tr("Open Images"), "", tr("Image Files (*.png *.jpg *.bmp)"));
            if(filenames.isEmpty()) return;

            input1 = cv::imread(filenames.at(0).toStdString());
            input2 = cv::imread(filenames.at(1).toStdString());
        }

        if (viewRadio->isChecked())
        {
            if (windowList.isEmpty()) return;
            input1 = activeWindow->getImageMat();

            if (windowList.length() < 2) return;
            Window* secondView = (windowList.last() != activeWindow)? windowList.last() : windowList[windowList.length()-2];
            input2 = secondView->getImageMat();

        }
        cvtColor(input1, input_gray1, CV_BGR2GRAY);
        cvtColor(input2, input_gray2, CV_BGR2GRAY);
        int minHessian = 400;

        Mat descriptors_1, descriptors_2;
        std::vector<KeyPoint> keypoints_1, keypoints_2;

        if (siftRadio->isChecked()) {
            SiftFeatureDetector detector(minHessian);
            detector.detect(input_gray1, keypoints_1);
            detector.detect(input_gray2, keypoints_2);
            SiftDescriptorExtractor extractor;
            extractor.compute(input_gray1, keypoints_1, descriptors_1 );
            extractor.compute(input_gray2, keypoints_2, descriptors_2 );
        }
        if (surfRadio->isChecked()) {
            SurfFeatureDetector detector(minHessian);
            detector.detect(input_gray1, keypoints_1);
            detector.detect(input_gray2, keypoints_2);
            SurfDescriptorExtractor extractor;
            extractor.compute(input_gray1, keypoints_1, descriptors_1 );
            extractor.compute(input_gray2, keypoints_2, descriptors_2 );
        }

        FlannBasedMatcher matcher;
        std::vector< DMatch > matches;
        matcher.match( descriptors_1, descriptors_2, matches );

        double max_dist = 0; double min_dist = 100;

        //-- Quick calculation of max and min distances between keypoints
        for( int i = 0; i < descriptors_1.rows; i++ )
        { double dist = matches[i].distance;
            if( dist < min_dist ) min_dist = dist;
            if( dist > max_dist ) max_dist = dist;
        }

        std::vector< DMatch > good_matches;

        for( int i = 0; i < descriptors_1.rows; i++ )
        { if( matches[i].distance <= max(2*min_dist, 0.02) )
            { good_matches.push_back( matches[i]); }
        }

        //-- Draw only "good" matches
        Mat img_matches;
        drawMatches( input1, keypoints_1, input2, keypoints_2,
                     good_matches, img_matches, Scalar::all(-1), Scalar::all(-1),
                     vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS );


        activeWindow = new Window(this);
        windowList.append(activeWindow);

        activeWindow->setWindowTitle(QString("Matched Features"));
        connect(activeWindow, SIGNAL(windowActivated()), this, SLOT(on_focusChanged()));
        connect(activeWindow, SIGNAL(windowClosed()), this, SLOT(on_actionClose_triggered()));

        activeWindow->setImage(img_matches);
    }
}

void MainWindow::on_actionFAST_triggered()
{
    if (windowList.isEmpty()) return;
    bool ok;

    int num = QInputDialog::getInt(this, tr("FAST feature detection settings"),tr("Threshold"), 50, 0, 255, 1, &ok, Qt::WindowStaysOnTopHint);

    if(!ok) return;


    FastFunctor fast_(num);
    if (typeid(*activeWindow) == typeid(VideoWindow))
    {
        if (!static_cast<VideoWindow*>(activeWindow)->camera_or_video())
        {
            activeWindow = new VideoWindow(this);
            activeWindow->setWindowTitle(QString("Camera"));
        }
    }
    else
    {
        Mat img = activeWindow->getImageMat();

        activeWindow = new Window(img, this);
    }
    windowList.append(activeWindow);
    connect(activeWindow, SIGNAL(windowActivated()), this, SLOT(on_focusChanged()));
    connect(activeWindow, SIGNAL(windowClosed()), this, SLOT(on_actionClose_triggered()));

    activeWindow->applyFilter(fast_);
    if (typeid(*activeWindow) == typeid(Window)) activeWindow->removeFilter();
}

void MainWindow::on_actionOpen_Camera_triggered()
{
    bool ok;
    int num = QInputDialog::getInt(this, tr("Open Camera"),tr("Camera:"), 0, 0, 10, 1, &ok, Qt::WindowStaysOnTopHint);

    if (ok)
    {
        activeWindow = new VideoWindow(num, this);
        windowList.append(activeWindow);

        activeWindow->setWindowTitle(QString("Camera"));
        connect(activeWindow, SIGNAL(windowActivated()), this, SLOT(on_focusChanged()));
        connect(activeWindow, SIGNAL(windowClosed()), this, SLOT(on_actionClose_triggered()));
    }
}

void MainWindow::on_actionAdd_Logo_triggered()
{
    if (windowList.isEmpty()) return;

    QDialog dialog(this);
    dialog.setWindowFlags(this->windowFlags() | Qt::WindowStaysOnTopHint);

    dialog.setWindowTitle(tr("Logo Settings"));

    QGridLayout* groupLayout = new QGridLayout(&dialog);

    //Feature descriptor type
    QGroupBox* logo_loc = new QGroupBox(tr("Logo Location"));

    QRadioButton *tlRadio = new QRadioButton(tr("Top-Left"));
    QRadioButton *trRadio = new QRadioButton(tr("Top-Right"));
    QRadioButton *blRadio = new QRadioButton(tr("Bottom-Left"));
    QRadioButton *brRadio = new QRadioButton(tr("Bottom-Right"));

    blRadio->setChecked(true);

    QVBoxLayout *vbox = new QVBoxLayout;
    vbox->addWidget(tlRadio);
    vbox->addWidget(trRadio);
    vbox->addWidget(blRadio);
    vbox->addWidget(brRadio);

    vbox->addStretch(1);
    logo_loc->setLayout(vbox);

    groupLayout->addWidget(logo_loc);

    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
                                                       Qt::Horizontal, &dialog);

    groupLayout->addWidget(buttonBox);

    groupLayout->setAlignment(buttonBox, Qt::AlignCenter);

    connect(buttonBox, SIGNAL(accepted()), &dialog, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), &dialog, SLOT(reject()));

    if (dialog.exec() == QDialog::Accepted)
    {
        QString file = QFileDialog::getOpenFileName(this,
                                                    tr("Open Image"), "", tr("Image Files (*.png *.jpg *.bmp)"));

        if (file.isEmpty())
            return;
        Mat logo = cv::imread(file.toStdString());
        if (logo.empty())
            return;
        const Mat image = activeWindow->getImageMat();

        if (logo.size().width > image.size().width  || logo.size().height > image.size().height)
            return;

        int location = 3;
        if (tlRadio->isChecked())
            location = 1;
        if (trRadio->isChecked())
            location = 2;
        if (blRadio->isChecked())
            location = 3;
        if (brRadio->isChecked())
            location = 4;

        AddLogoFunctor addLogo_(logo, location);

        if (typeid(*activeWindow) == typeid(VideoWindow))
        {
            if (!static_cast<VideoWindow*>(activeWindow)->camera_or_video())
            {
                activeWindow = new VideoWindow(this);
                activeWindow->setWindowTitle(QString("Camera"));
            }
        }
        else
        {
            Mat img = activeWindow->getImageMat();

            activeWindow = new Window(img, this);
        }
        windowList.append(activeWindow);
        connect(activeWindow, SIGNAL(windowActivated()), this, SLOT(on_focusChanged()));
        connect(activeWindow, SIGNAL(windowClosed()), this, SLOT(on_actionClose_triggered()));

        activeWindow->applyFilter(addLogo_);
        if (typeid(*activeWindow) == typeid(Window)) activeWindow->removeFilter();
    }
}
void MainWindow::on_actionCompute_Fundamental_Matrix_triggered()
{
    QDialog dialog(this);
    dialog.setWindowFlags(this->windowFlags() | Qt::WindowStaysOnTopHint);

    dialog.setWindowTitle(tr("Settings"));

    QGridLayout* groupLayout = new QGridLayout(&dialog);

    //Feature descriptor type
    QGroupBox* feature_type_group = new QGroupBox(tr("Feature Descriptor"));

    QRadioButton *surfRadio = new QRadioButton(tr("SURF"));
    QRadioButton *siftRadio = new QRadioButton(tr("SIFT"));

    surfRadio->setChecked(true);

    QVBoxLayout *vbox = new QVBoxLayout;
    vbox->addWidget(siftRadio);
    vbox->addWidget(surfRadio);

    vbox->addStretch(1);
    feature_type_group->setLayout(vbox);

    groupLayout->addWidget(feature_type_group);

    QGroupBox* fileGroup = new QGroupBox(tr("Images"));

    QRadioButton *viewRadio = new QRadioButton(tr("Use Open Windows"));
    QRadioButton *fileRadio = new QRadioButton(tr("Browse.."));

    viewRadio->setChecked(true);

    QVBoxLayout *vbox2 = new QVBoxLayout;
    vbox2->addWidget(viewRadio);
    vbox2->addWidget(fileRadio);

    vbox2->addStretch(1);
    fileGroup->setLayout(vbox2);
    groupLayout->addWidget(fileGroup);


    QGroupBox* methodGroup = new QGroupBox(tr("Fundamental Matrix Method"));

    QRadioButton *sevenRadio = new QRadioButton(tr("7-Point algorithm"));
    QRadioButton *eightRadio = new QRadioButton(tr("8-Point algorithm"));
    QRadioButton *ransacRadio = new QRadioButton(tr("RANSAC"));

    eightRadio->setChecked(true);

    QVBoxLayout *vbox3 = new QVBoxLayout;
    vbox3->addWidget(sevenRadio);
    vbox3->addWidget(eightRadio);
    vbox3->addWidget(ransacRadio);

    vbox3->addStretch(1);
    methodGroup->setLayout(vbox3);
    groupLayout->addWidget(methodGroup);

    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
                                                       Qt::Horizontal, &dialog);

    groupLayout->addWidget(buttonBox);

    groupLayout->setAlignment(buttonBox, Qt::AlignCenter);

    connect(buttonBox, SIGNAL(accepted()), &dialog, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), &dialog, SLOT(reject()));

    if (dialog.exec() == QDialog::Accepted)
    {
        Mat input_gray1;
        Mat input_gray2;
        Mat input1;
        Mat input2;
        Mat output1;
        Mat output2;

        if (fileRadio->isChecked())
        {
            QStringList filenames = QFileDialog::getOpenFileNames(this,
                                                                  tr("Open Images"), "", tr("Image Files (*.png *.jpg *.bmp)"));
            if(filenames.isEmpty()) return;

            input1 = cv::imread(filenames.at(0).toStdString());
            input2 = cv::imread(filenames.at(1).toStdString());
        }
        else
            if (viewRadio->isChecked())
            {
                if (windowList.isEmpty()) return;
                activeWindow->getImageMat().copyTo(input1);

                if (windowList.length() < 2) return;
                Window* secondView = (windowList.last() != activeWindow)? windowList.last() : windowList[windowList.length()-2];
                secondView->getImageMat().copyTo(input2);

            }
        if (input1.channels() == 3)
            cvtColor(input1, input_gray1, CV_BGR2GRAY);
        else input_gray1 = input1;
        if (input2.channels() == 3)
            cvtColor(input2, input_gray2, CV_BGR2GRAY);
        else input_gray2 = input2;

        input1.copyTo(output1);
        input2.copyTo(output2);

        int minHessian = 400;

        Mat descriptors_1, descriptors_2;
        std::vector<KeyPoint> keypoints_1, keypoints_2;

        if (siftRadio->isChecked()) {
            SiftFeatureDetector detector(minHessian);
            detector.detect(input_gray1, keypoints_1);
            detector.detect(input_gray2, keypoints_2);
            SiftDescriptorExtractor extractor;
            extractor.compute(input_gray1, keypoints_1, descriptors_1 );
            extractor.compute(input_gray2, keypoints_2, descriptors_2 );

        }
        if (surfRadio->isChecked())
        {
            SurfFeatureDetector detector(minHessian);
            detector.detect(input_gray1, keypoints_1);
            detector.detect(input_gray2, keypoints_2);
            SurfDescriptorExtractor extractor;
            extractor.compute(input_gray1, keypoints_1, descriptors_1 );
            extractor.compute(input_gray2, keypoints_2, descriptors_2 );

        }

        std::vector<std::vector<cv::DMatch>> matches;
        cv::BFMatcher matcher;
        matcher.knnMatch(descriptors_1, descriptors_2, matches, 2);  // Find two nearest matches

        vector<cv::DMatch> good_matches;

        for (int i = 0; i < matches.size(); ++i)
        {

            if (matches[i][0].distance < 0.8 * matches[i][1].distance)
            {
                good_matches.push_back(matches[i][0]);
            }
        }

        std::vector<int> pointIndexes1;
        std::vector<int> pointIndexes2;
        for (std::vector<cv::DMatch>::const_iterator it= good_matches.begin();
             it!= good_matches.end(); ++it) {

            // Get the indexes of the selected matched keypoints
            pointIndexes1.push_back(it->queryIdx);
            pointIndexes2.push_back(it->trainIdx);
        }

        std::vector<cv::Point2f> selPoints1, selPoints2;
        cv::KeyPoint::convert(keypoints_1,selPoints1,pointIndexes1);
        cv::KeyPoint::convert(keypoints_2,selPoints2,pointIndexes2);
        Mat fundamental;

        if (sevenRadio->isChecked())
        {
            if (int(selPoints1.size()) < 7) {qDebug()<<"Too few points found"; return;}
            fundamental = cv::findFundamentalMat(cv::Mat(selPoints1), cv::Mat(selPoints2),CV_FM_7POINT);
        }
        if (eightRadio->isChecked())
        {
            if (int(selPoints1.size()) < 8) {qDebug()<<"Too few points found"; return;}
            fundamental = cv::findFundamentalMat(cv::Mat(selPoints1), cv::Mat(selPoints2),CV_FM_8POINT);
        }
        if (ransacRadio->isChecked())
        {
            if (int(selPoints1.size()) < 8) {qDebug()<<"Too few points found"; return;}
            fundamental = cv::findFundamentalMat(cv::Mat(selPoints1), cv::Mat(selPoints2),CV_FM_RANSAC);
        }
        qDebug()<<"Fundamental Matrix:";
        qDebug()<<"["<<fundamental.at<double>(0, 0)<<" "<<fundamental.at<double>(0, 1)<<" "<<fundamental.at<double>(0, 2);
        qDebug()<<fundamental.at<double>(1, 0)<<" "<<fundamental.at<double>(1, 1)<<" "<<fundamental.at<double>(1, 2);
        qDebug()<<fundamental.at<double>(2, 0)<<" "<<fundamental.at<double>(2, 1)<<" "<<fundamental.at<double>(2, 2)<<"]";

    }
}
void MainWindow::on_actionStitch_Images_triggered()
{//http://study.marearts.com/2013/11/opencv-stitching-example-stitcher-class.html
    QDialog dialog(this);
    dialog.setWindowFlags(this->windowFlags() | Qt::WindowStaysOnTopHint);

    dialog.setWindowTitle(tr("Image Stitching Settings"));

    QGridLayout* groupLayout = new QGridLayout(&dialog);

    QGroupBox* fileGroup = new QGroupBox(tr("Images"));

    QRadioButton *viewRadio = new QRadioButton(tr("Use Open Views"));
    QRadioButton *fileRadio = new QRadioButton(tr("Browse.."));

    viewRadio->setChecked(true);

    QVBoxLayout *vbox2 = new QVBoxLayout;
    vbox2->addWidget(viewRadio);
    vbox2->addWidget(fileRadio);

    vbox2->addStretch(1);
    fileGroup->setLayout(vbox2);
    groupLayout->addWidget(fileGroup);

    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
                                                       Qt::Horizontal, &dialog);

    groupLayout->addWidget(buttonBox);

    groupLayout->setAlignment(buttonBox, Qt::AlignCenter);

    connect(buttonBox, SIGNAL(accepted()), &dialog, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), &dialog, SLOT(reject()));

    if (dialog.exec() == QDialog::Accepted)
    {

        vector< Mat > vImg;
        if (fileRadio->isChecked())
        {
            QStringList filenames = QFileDialog::getOpenFileNames(this,
                                                                  tr("Open Images"), "", tr("Image Files (*.png *.jpg *.bmp)"));
            if(filenames.isEmpty()) return;
            for(int i = 0; i < filenames.length(); i++)
                vImg.push_back(imread(filenames.at(i).toStdString()));
        }

        if (viewRadio->isChecked())
        {
            if (windowList.isEmpty()) return;
            if (windowList.length() < 2) return;

            for(int i = 0; i < windowList.length(); i++)
                vImg.push_back(windowList[i]->getImageMat());
        }

        Mat rImg;

        Stitcher stitcher = Stitcher::createDefault();

        Stitcher::Status status = stitcher.stitch(vImg, rImg);
        if (Stitcher::OK != status)
        {
            qDebug()<<"Stitching failed.";
            return;
        }

        activeWindow = new Window(rImg, this);
        windowList.append(activeWindow);
        activeWindow->setWindowTitle(QString("Panorama"));
        connect(activeWindow, SIGNAL(windowActivated()), this, SLOT(on_focusChanged()));
        connect(activeWindow, SIGNAL(windowClosed()), this, SLOT(on_actionClose_triggered()));
    }
}
void MainWindow::on_actionCompute_Homography_triggered()
{//http://docs.opencv.org/2.4/doc/tutorials/features2d/feature_homography/feature_homography.html
    QDialog dialog(this);
    dialog.setWindowFlags(this->windowFlags() | Qt::WindowStaysOnTopHint);

    dialog.setWindowTitle(tr("Homography Settings"));

    QGridLayout* groupLayout = new QGridLayout(&dialog);

    QGroupBox* fileGroup = new QGroupBox(tr("Images"));

    QRadioButton *viewRadio = new QRadioButton(tr("Use Open Views"));
    QRadioButton *fileRadio = new QRadioButton(tr("Browse.."));

    viewRadio->setChecked(true);

    QVBoxLayout *vbox2 = new QVBoxLayout;
    vbox2->addWidget(viewRadio);
    vbox2->addWidget(fileRadio);

    vbox2->addStretch(1);
    fileGroup->setLayout(vbox2);
    groupLayout->addWidget(fileGroup);

    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
                                                       Qt::Horizontal, &dialog);

    groupLayout->addWidget(buttonBox);

    groupLayout->setAlignment(buttonBox, Qt::AlignCenter);

    connect(buttonBox, SIGNAL(accepted()), &dialog, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), &dialog, SLOT(reject()));

    if (dialog.exec() == QDialog::Accepted)
    {
        Mat input_gray1;
        Mat input_gray2;
        Mat input1;
        Mat input2;

        if (fileRadio->isChecked())
        {
            QStringList filenames = QFileDialog::getOpenFileNames(this,
                                                                  tr("Open Images"), "", tr("Image Files (*.png *.jpg *.bmp)"));
            if(filenames.isEmpty()) return;

            input1 = cv::imread(filenames.at(0).toStdString());
            input2 = cv::imread(filenames.at(1).toStdString());
        }
        else
            if (viewRadio->isChecked())
            {
                if (windowList.isEmpty()) return;
                activeWindow->getImageMat().copyTo(input1);

                if (windowList.length() < 2) return;
                Window* secondView = (windowList.last() != activeWindow)? windowList.last() : windowList[windowList.length()-2];
                secondView->getImageMat().copyTo(input2);

            }
        if (input1.channels() == 3)
            cvtColor(input1, input_gray1, CV_BGR2GRAY);
        else input_gray1 = input1;
        if (input2.channels() == 3)
            cvtColor(input2, input_gray2, CV_BGR2GRAY);
        else input_gray2 = input2;

        Mat img_object;
        Mat img_scene;
        input1.copyTo(img_object);
        input2.copyTo(img_scene);

        if( !img_object.data || !img_scene.data )
        { qDebug()<< " --(!) Error reading images "; return; }

        //-- Step 1: Detect the keypoints using SURF Detector
        int minHessian = 400;

        SurfFeatureDetector detector( minHessian );

        std::vector<KeyPoint> keypoints_object, keypoints_scene;

        detector.detect( img_object, keypoints_object );
        detector.detect( img_scene, keypoints_scene );

        //-- Step 2: Calculate descriptors (feature vectors)
        SurfDescriptorExtractor extractor;

        Mat descriptors_object, descriptors_scene;

        extractor.compute( img_object, keypoints_object, descriptors_object );
        extractor.compute( img_scene, keypoints_scene, descriptors_scene );

        //-- Step 3: Matching descriptor vectors using FLANN matcher
        FlannBasedMatcher matcher;
        std::vector< DMatch > matches;
        matcher.match( descriptors_object, descriptors_scene, matches );

        double max_dist = 0; double min_dist = 100;

        //-- Quick calculation of max and min distances between keypoints
        for( int i = 0; i < descriptors_object.rows; i++ )
        { double dist = matches[i].distance;
            if( dist < min_dist ) min_dist = dist;
            if( dist > max_dist ) max_dist = dist;
        }



        //-- Draw only "good" matches (i.e. whose distance is less than 3*min_dist )
        std::vector< DMatch > good_matches;

        for( int i = 0; i < descriptors_object.rows; i++ )
        { if( matches[i].distance < 3*min_dist )
            { good_matches.push_back( matches[i]); }
        }

        Mat img_matches;
        drawMatches( img_object, keypoints_object, img_scene, keypoints_scene,
                     good_matches, img_matches, Scalar::all(-1), Scalar::all(-1),
                     vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS );

        //-- Localize the object
        std::vector<Point2f> obj;
        std::vector<Point2f> scene;

        for( int i = 0; i < good_matches.size(); i++ )
        {
            //-- Get the keypoints from the good matches
            obj.push_back( keypoints_object[ good_matches[i].queryIdx ].pt );
            scene.push_back( keypoints_scene[ good_matches[i].trainIdx ].pt );
        }

        Mat H = findHomography( obj, scene, CV_RANSAC );

        //-- Get the corners from the image_1 ( the object to be "detected" )
        std::vector<Point2f> obj_corners(4);
        obj_corners[0] = cvPoint(0,0); obj_corners[1] = cvPoint( img_object.cols, 0 );
        obj_corners[2] = cvPoint( img_object.cols, img_object.rows ); obj_corners[3] = cvPoint( 0, img_object.rows );
        std::vector<Point2f> scene_corners(4);

        perspectiveTransform( obj_corners, scene_corners, H);

        //-- Draw lines between the corners (the mapped object in the scene - image_2 )
        line( img_matches, scene_corners[0] + Point2f( img_object.cols, 0), scene_corners[1] + Point2f( img_object.cols, 0), Scalar(0, 255, 0), 4 );
        line( img_matches, scene_corners[1] + Point2f( img_object.cols, 0), scene_corners[2] + Point2f( img_object.cols, 0), Scalar( 0, 255, 0), 4 );
        line( img_matches, scene_corners[2] + Point2f( img_object.cols, 0), scene_corners[3] + Point2f( img_object.cols, 0), Scalar( 0, 255, 0), 4 );
        line( img_matches, scene_corners[3] + Point2f( img_object.cols, 0), scene_corners[0] + Point2f( img_object.cols, 0), Scalar( 0, 255, 0), 4 );


        activeWindow = new Window(img_matches, this);
        windowList.append(activeWindow);
        activeWindow->setWindowTitle(QString("Homography Result"));
        connect(activeWindow, SIGNAL(windowActivated()), this, SLOT(on_focusChanged()));
        connect(activeWindow, SIGNAL(windowClosed()), this, SLOT(on_actionClose_triggered()));
    }
}
void MainWindow::on_actionHough_Transformaton_Circles_triggered()
{
    if (windowList.isEmpty()) return;

    HoughCirclesFunctor houghCircles_;


    //////////////////////
    if (typeid(*activeWindow) == typeid(VideoWindow))
    {
        if (!static_cast<VideoWindow*>(activeWindow)->camera_or_video())
        {
            activeWindow = new VideoWindow(this);
            windowList.append(activeWindow);
            connect(activeWindow, SIGNAL(windowActivated()), this, SLOT(on_focusChanged()));
            connect(activeWindow, SIGNAL(windowClosed()), this, SLOT(on_actionClose_triggered()));
        }
    }
    else
    {
        activeWindow = new Window(activeWindow->getImageMat(), this);
        windowList.append(activeWindow);
        connect(activeWindow, SIGNAL(windowActivated()), this, SLOT(on_focusChanged()));
        connect(activeWindow, SIGNAL(windowClosed()), this, SLOT(on_actionClose_triggered()));
    }


    activeWindow->applyFilter(houghCircles_);
    if (typeid(*activeWindow) == typeid(Window)) activeWindow->removeFilter();
}


void MainWindow::on_actionHough_transformation_triggered()
{
    if (windowList.isEmpty()) return;

    HoughLinesFunctor houghLines_;

    if (typeid(*activeWindow) == typeid(VideoWindow))
    {
        if (!static_cast<VideoWindow*>(activeWindow)->camera_or_video())
        {
            activeWindow = new VideoWindow(this);
            windowList.append(activeWindow);
            connect(activeWindow, SIGNAL(windowActivated()), this, SLOT(on_focusChanged()));
            connect(activeWindow, SIGNAL(windowClosed()), this, SLOT(on_actionClose_triggered()));
        }
    }
    else
    {
        activeWindow = new Window(activeWindow->getImageMat(), this);
        windowList.append(activeWindow);
        connect(activeWindow, SIGNAL(windowActivated()), this, SLOT(on_focusChanged()));
        connect(activeWindow, SIGNAL(windowClosed()), this, SLOT(on_actionClose_triggered()));
    }


    activeWindow->applyFilter(houghLines_);
    if (typeid(*activeWindow) == typeid(Window)) activeWindow->removeFilter();

}

void MainWindow::on_actionScale_Image_triggered()
{

    if (windowList.isEmpty()) return;

    Mat img = activeWindow->getImageMat();

    QDialog dialog(this);
    dialog.setWindowTitle(tr("Scale Image"));
    QFormLayout form(&dialog);


    QGroupBox* newSizeGr = new QGroupBox(tr("Enter New Size"));

    QHBoxLayout* widthGr = new QHBoxLayout;
    QLabel* label_width = new QLabel(tr("New Width: "));
    QLineEdit* in_width = new QLineEdit(QString::number(img.size().width));
    widthGr->addWidget(label_width);
    widthGr->addWidget(in_width);
    form.addRow(widthGr);

    QHBoxLayout* heightGr = new QHBoxLayout;
    QLabel* label_height = new QLabel(tr("New Height: "));
    QLineEdit* in_height = new QLineEdit(QString::number(img.size().height));
    heightGr->addWidget(label_height);
    heightGr->addWidget(in_height);
    form.addRow(heightGr);

    QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
                               Qt::Horizontal, &dialog);
    form.addRow(&buttonBox);
    QObject::connect(&buttonBox, SIGNAL(accepted()), &dialog, SLOT(accept()));
    QObject::connect(&buttonBox, SIGNAL(rejected()), &dialog, SLOT(reject()));
    if (dialog.exec() == QDialog::Accepted)
    {
        bool ok1;
        bool ok2;
        int w = in_width->text().toInt(&ok1);
        int h = in_height->text().toInt(&ok2);
        if (ok1 && ok2)
        {
            ResizeFunctor resize_(h, w);

            if (typeid(*activeWindow) == typeid(VideoWindow))
            {
                if (!static_cast<VideoWindow*>(activeWindow)->camera_or_video())
                {
                    activeWindow = new VideoWindow(this);
                    windowList.append(activeWindow);
                    connect(activeWindow, SIGNAL(windowActivated()), this, SLOT(on_focusChanged()));
                    connect(activeWindow, SIGNAL(windowClosed()), this, SLOT(on_actionClose_triggered()));
                }
            }
            else
            {
                activeWindow = new Window(activeWindow->getImageMat(), this);
                windowList.append(activeWindow);
                connect(activeWindow, SIGNAL(windowActivated()), this, SLOT(on_focusChanged()));
                connect(activeWindow, SIGNAL(windowClosed()), this, SLOT(on_actionClose_triggered()));
            }


            activeWindow->applyFilter(resize_);
            if (typeid(*activeWindow) == typeid(Window)) activeWindow->removeFilter();
        }

    }
}

void MainWindow::on_actionAbout_triggered()
{
    QMessageBox::about(this, tr("About"), tr("Visual Perception Toolbox(2016).\n Author: Armine Vardazaryan"));
}

void MainWindow::on_actionConvert_to_Binary_triggered()
{
    if (windowList.isEmpty()) return;

    bool ok;
    int num = QInputDialog::getInt(this, tr("QInputDialog::getInteger()"),
                                   tr("Threshold: "), 100, 0, 255, 1, &ok, Qt::WindowStaysOnTopHint);

    if (ok && (num > 0))
    {
        BinaryFunctor binary_(num);

        if (typeid(*activeWindow) == typeid(VideoWindow))
        {
            if (!static_cast<VideoWindow*>(activeWindow)->camera_or_video())
            {
                activeWindow = new VideoWindow(this);
                activeWindow->setWindowTitle(QString("Camera"));
            }
        }
        else
        {
            Mat img = activeWindow->getImageMat();

            activeWindow = new Window(img, this);
        }
        windowList.append(activeWindow);
        connect(activeWindow, SIGNAL(windowActivated()), this, SLOT(on_focusChanged()));
        connect(activeWindow, SIGNAL(windowClosed()), this, SLOT(on_actionClose_triggered()));

        activeWindow->applyFilter(binary_);
        if (typeid(*activeWindow) == typeid(Window)) activeWindow->removeFilter();
    }
}

void MainWindow::on_actionConvert_To_Grayscale_triggered()
{
    if (windowList.isEmpty()) return;

    GrayFunctor gray_;

    if (typeid(*activeWindow) == typeid(VideoWindow))
    {
        if (!static_cast<VideoWindow*>(activeWindow)->camera_or_video())
        {
            activeWindow = new VideoWindow(this);
            activeWindow->setWindowTitle(QString("Camera"));
        }
    }
    else
    {
        Mat img = activeWindow->getImageMat();

        activeWindow = new Window(img, this);
    }
    windowList.append(activeWindow);
    connect(activeWindow, SIGNAL(windowActivated()), this, SLOT(on_focusChanged()));
    connect(activeWindow, SIGNAL(windowClosed()), this, SLOT(on_actionClose_triggered()));

    activeWindow->applyFilter(gray_);
    if (typeid(*activeWindow) == typeid(Window)) activeWindow->removeFilter();
}

void MainWindow::on_actionGaussian_noise_triggered()
{
    if (windowList.isEmpty()) return;

    bool ok;
    int num = QInputDialog::getInt(this, tr("Additive Gaussian Noise Settings"),
                                   tr("Sigma: "), 10, 0, 255, 1, &ok, Qt::WindowStaysOnTopHint);

    if (ok && (num > 0))
    {
        GaussianNoiseFunctor gaussNoise_(num);

        if (typeid(*activeWindow) == typeid(VideoWindow))
        {
            if (!static_cast<VideoWindow*>(activeWindow)->camera_or_video())
            {
                activeWindow = new VideoWindow(this);
                activeWindow->setWindowTitle(QString("Camera"));
            }
        }
        else
        {
            Mat img = activeWindow->getImageMat();

            activeWindow = new Window(img, this);
        }
        windowList.append(activeWindow);
        connect(activeWindow, SIGNAL(windowActivated()), this, SLOT(on_focusChanged()));
        connect(activeWindow, SIGNAL(windowClosed()), this, SLOT(on_actionClose_triggered()));

        activeWindow->applyFilter(gaussNoise_);
        if (typeid(*activeWindow) == typeid(Window)) activeWindow->removeFilter();
    }
}
