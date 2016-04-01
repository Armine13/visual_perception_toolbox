#include "mainwindow.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QIcon::setThemeName("Tango");//custom theme for icons

    // Add icons to buttons
    //File
    ui->actionNew->setIcon(QIcon::fromTheme("document-new"));
    ui->actionOpen->setIcon(QIcon::fromTheme("image-x-generic"));
    ui->actionSave_As->setIcon(QIcon::fromTheme("document-save-as"));
    ui->actionSave->setIcon(QIcon::fromTheme("document-save"));
    ui->actionClose->setIcon(QIcon::fromTheme("document-close"));
    ui->actionExit->setIcon(QIcon::fromTheme("exit"));
    //Edit
    ui->actionUndo->setIcon(QIcon::fromTheme("edit-undo"));
    ui->actionRedo->setIcon(QIcon::fromTheme("edit-redo"));
    ui->actionHistory->setIcon(QIcon::fromTheme("appointment-new"));
    //Help
    ui->actionAbout->setIcon(QIcon::fromTheme("help-browser"));


    //Initializing local variables
    kernel_size = 3; // Default kernel side size
    windowList = QVector<Window*>();
    activeWindow = NULL;
    vPlayer = new Player();
}

MainWindow::~MainWindow()
{
    delete vPlayer;
    delete ui;
//    for(int i = 0; i < windowList.length(); i++)
//        if (windowList[i] != NULL)
//        {
//            windowList[i]->~Window();
//            windowList[i] = NULL;
//        }
    qDebug()<<"mainwindow descructor";

}
void MainWindow::on_actionClose_triggered()
{
    if (activeWindow != NULL)
    {
        activeWindow->~Window();
        activeWindow = NULL;
    }
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
    //exit function if no image selected
    if (fileName.isEmpty())
        return;

    // Load the image through opencv
    activeWindow = new Window(fileName);
    windowList.append(activeWindow);

    activeWindow->setWindowTitle(fileName);
    //    this->setMinimumSize(wdg->size());
    connect(activeWindow, SIGNAL(windowActivated()), this, SLOT(on_focusChanged()));
    connect(activeWindow, SIGNAL(windowClosed()), this, SLOT(on_actionClose_triggered()));
    connect(vPlayer, SIGNAL(processedImage(QImage)),this, SLOT(updatePlayerUI(QImage)));
}
void MainWindow::on_actionOpen_Video_triggered()
{
    QString filename = QFileDialog::getOpenFileName(this,
                                             tr("Open Video"), ".",
                                             tr("Video Files (*.avi *.mpg *.mp4)"));

       if (!filename.isEmpty()){
           if (!vPlayer->loadVideo(filename.toLatin1().data()))
           {

               QMessageBox msgBox;
               msgBox.setText("The selected video could not be opened!");
               msgBox.exec();
           }

           activeWindow = new Window();
           windowList.append(activeWindow);
           vPlayer->Play();
           connect(vPlayer, SIGNAL(processedImage(QImage)),this, SLOT(updatePlayerUI(QImage)));

           setAttribute(Qt::WA_DeleteOnClose, true);
       }
//       QObject::connect(myPlayer, SIGNAL(processedImage(QImage)),
//                                     this, SLOT(updatePlayerUI(QImage)));
}
void MainWindow::on_actionSet_Kernel_Size_triggered()
{
    bool ok;
    int num = QInputDialog::getInt(this, tr("QInputDialog::getInteger()"),
                                   tr("New Kernel Size:"), kernel_size, 1, 1000, 1, &ok, Qt::WindowStaysOnTopHint);
    if (ok && (num > 0))
        kernel_size = num;
}

void MainWindow::on_actionMean_triggered()
{//Mean filter
    if (activeWindow == NULL) return;
    qDebug()<<"mean filter";
    cv::Mat img;

    blur(activeWindow->getImageMat(), img, cv::Size( kernel_size, kernel_size), cv::Point(-1,-1) );
    activeWindow->setImage(img);
}

void MainWindow::on_actionMedian_triggered()
{//Median filter
    if (activeWindow == NULL) return;
    Mat img;
    //kernel size has to be odd for median filter
    if (kernel_size % 2 == 0)
        medianBlur(activeWindow->getImageMat(), img, kernel_size + 1);
    else
        medianBlur(activeWindow->getImageMat(), img, kernel_size);
    activeWindow->setImage(img);
}

void MainWindow::on_actionGaussian_triggered()
{
    if (activeWindow == NULL) return;
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
    sigmaX->setSingleStep(0.1);
    sigmaX->setValue(1);

    QLabel *sigmaY_label = new QLabel(tr("SigmaY: "));
    QSpinBox *sigmaY = new QSpinBox;
    sigmaY->setRange(0, 100);
    sigmaY->setSingleStep(0.1);
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

    groupLayout->setRowMinimumHeight(2, 160);
    groupLayout->setColumnMinimumWidth(1, 160);

    groupLayout->setAlignment(buttonBox, Qt::AlignCenter);
    groupLayout->setVerticalSpacing(20);

    connect(buttonBox, SIGNAL(accepted()), &dialog, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), &dialog, SLOT(reject()));
    //    kernel_size_group->adjustSize();

    Mat img;
    if (dialog.exec() == QDialog::Accepted)
    {
        GaussianBlur(activeWindow->getImageMat(), img, cv::Size(kernelX->value(), kernelY->value()),sigmaX->value(), sigmaY->value());
        activeWindow->setImage(img);
    }
}

void MainWindow::on_actionSalt_and_Pepper_triggered()
{
    if (activeWindow == NULL) return;
}

void MainWindow::on_actionSave_triggered()
{
    if (activeWindow == NULL) return;

    if (activeWindow->getFileName().isEmpty())
        on_actionSave_As_triggered();
    else
        activeWindow->save();
}

void MainWindow::on_actionSave_As_triggered()
{
    if (activeWindow == NULL) return;
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
    activeWindow = new Window();
    windowList.append(activeWindow);
    activeWindow->setWindowTitle("NewFile");

    connect(activeWindow, SIGNAL(windowActivated()), this, SLOT(on_focusChanged()));
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
void MainWindow::updatePlayerUI(QImage img)
{
//    if (activeWindow == NULL) return;
//    qDebug()<<"mean filter";
//    cv::Mat img;

//    blur(activeWindow->getImageMat(), img, cv::Size( kernel_size, kernel_size), cv::Point(-1,-1) );
//    activeWindow->setImage(img);

    if (activeWindow != NULL)
    {
//        ui->label->setAlignment(Qt::AlignCenter);
//        ui->label->setPixmap(QPixmap::fromImage(img).scaled(ui->label->size(),
//                                           Qt::KeepAspectRatio, Qt::FastTransformation));
        activeWindow->setImage(img);
    }
}

void MainWindow::on_stopVideo()
{
//    if (vPlayer->isStopped())
//    {
//        vPlayer->Play();
//        ui->pushButton_2->setText(tr("Stop"));
//    }else
//    {
//        vPlayer->Stop();
//        ui->pushButton_2->setText(tr("Play"));
//    }
}
