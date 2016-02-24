#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "imageprocessing.h"
#include "thirdParty/cvMatToQImage.cpp"


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
    cv::Mat openImage(cv::Size(320,240),CV_8UC3);
    kernel_size = 3; // Default kernel side size
}

MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::on_actionClose_triggered()
{
    //triggered from File menu
}

void MainWindow::on_actionNew_triggered()
{
}


void MainWindow::on_actionExit_triggered()
{
    QCoreApplication::quit();
}

void MainWindow::on_actionOpen_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("Open Image"), "", tr("Image Files (*.png *.jpg *.bmp)"));
    //exit function if no image selected
    if (fileName.isEmpty())
        return;

    // Load the image through opencv
    openImage = cv::imread(fileName.toStdString());
    assert(!openImage.empty());

    //Load and display image in qt label
    QImage openQImage;

    if (!openQImage.load(fileName))
        return;

    ui->imageLabel->setPixmap(QPixmap::fromImage(openQImage));

    // Adjust sizes of labels to contained images
    ui->imageLabel->setMinimumSize(openQImage.size());
    this->setMinimumSize(openQImage.size());
}

void MainWindow::on_actionSet_Kernel_Size_triggered()
{
    bool ok;
    int num = QInputDialog::getInt(this, tr("QInputDialog::getInteger()"),
                                   tr("New Kernel Size:"), kernel_size, 1, 1000, 1, &ok);
    if (ok && (num > 0))
        kernel_size = num;
}

void MainWindow::on_actionMean_triggered()
{//Mean filter
    blur( openImage, openImage, cv::Size( kernel_size, kernel_size), cv::Point(-1,-1) );
    ui->imageLabel->setPixmap(QPixmap::fromImage(cvMatToQImage(openImage)));
}

void MainWindow::on_actionMedian_triggered()
{//Median filter

    //kernel size has to be odd for median filter
    if (kernel_size % 2 == 0)
        medianBlur(openImage, openImage, kernel_size + 1);
    else
        medianBlur(openImage, openImage, kernel_size);
    ui->imageLabel->setPixmap(QPixmap::fromImage(cvMatToQImage(openImage)));
}
void MainWindow::on_actionSalt_Pepper_triggered()
{

}



void MainWindow::on_actionGaussian_triggered()
{
    QDialog dialog(this);
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

    if (dialog.exec() == QDialog::Accepted)
    {
        GaussianBlur(openImage, openImage, cv::Size(kernelX->value(), kernelY->value()),sigmaX->value(), sigmaY->value());
        ui->imageLabel->setPixmap(QPixmap::fromImage(cvMatToQImage(openImage)));
    }
}
