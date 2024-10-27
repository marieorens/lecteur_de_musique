#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    MPlayer = new QMediaPlayer();


    QSqlDatabase music=QSqlDatabase::addDatabase("QSQLITE");
    music.setDatabaseName("C:/Users/Orens_10/Documents/lecteur_musique/music.db");
    if (music.open()) {
        qDebug() << "Base de données 'music.db' en cours de création...........";
        qDebug() << "Base de données 'music.db' ouverte avec succès.";

        QSqlQuery query;


                QString createTableQuery = "CREATE TABLE IF NOT EXISTS chansons ("
                                    "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                                    "nom TEXT NOT NULL, "
                                    "chemin TEXT NOT NULL)";

                QString createPlaylistsTableQuery = "CREATE TABLE IF NOT EXISTS playlists ("
                                                     "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                                                     "nom TEXT NOT NULL, "
                                                     "chemin TEXT NOT NULL)";


                if (query.exec(createTableQuery) && query.exec(createPlaylistsTableQuery)) {
                    qDebug() << "Table 'chansons' et 'playlists' créée avec succès.";}
                else{
                      qDebug() << "Erreur lors de la création de la table : ";
                }
    } else {
        qDebug() << "Erreur lors de l'ouverture de la base de données : " << music.lastError().text();
    }



    ui->pushButton_Play->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
    ui->pushButton_3_Pause->setIcon(style()->standardIcon(QStyle::SP_MediaPause));
    ui->pushButton_4_Stop->setIcon(style()->standardIcon(QStyle::SP_MediaStop));
    ui->pushButton_previous->setIcon(style()->standardIcon(QStyle::SP_MediaSeekBackward));
    ui->pushButton_3_next->setIcon(style()->standardIcon(QStyle::SP_MediaSeekForward));
    ui->pushButton_volume->setIcon(style()->standardIcon(QStyle::SP_MediaVolume));
    ui->horizontalSlider_2->setMinimum(1);
    ui->horizontalSlider_2->setMaximum(100);
    ui->horizontalSlider_2->setValue(30);

    MPlayer->setVolume(ui->horizontalSlider_2->value());

    connect(MPlayer, &QMediaPlayer::durationChanged,this, &MainWindow::durationChanged);
    connect(MPlayer, &QMediaPlayer::positionChanged,this, &MainWindow::positionChanged);
    ui->horizontalSlider->setRange(0,MPlayer->duration() / 1000);


}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::durationChanged(qint64 duration)
{
   Mduration = duration / 1000;
   ui->horizontalSlider->setMaximum(Mduration);
}

void MainWindow::positionChanged(qint64 progress)
{
    if(!ui->horizontalSlider->isSliderDown()){

        ui->horizontalSlider->setValue(progress/1000);
    }
    updateduration(progress /1000);
}


void MainWindow::updateduration(qint64 duration)
{
    QString timestr;
    if(duration || Mduration){
        QTime CurrentTime((duration / 3600) % 60, (duration / 60) %60, duration % 60,(duration * 1000) % 1000);
        QTime totalTime((Mduration /3600) % 60, (Mduration / 60) % 60, Mduration % 60, (Mduration * 1000) % 1000);
        QString format = "mm:ss";
        if(Mduration > 3600)
            format = "hh:mm:ss";
        ui->label_start->setText(CurrentTime.toString(format));
        ui->label_end->setText(totalTime.toString(format));
    }
}

void MainWindow::on_pushButton_volume_clicked()
{
    if (IS_Muted == false)
    {
        ui->pushButton_volume->setIcon(style()->standardIcon(QStyle::SP_MediaVolume));
        IS_Muted = true;
        MPlayer->setMuted(true);

    }else{
        ui->pushButton_volume->setIcon(style()->standardIcon(QStyle::SP_MediaVolumeMuted));
        IS_Muted = false;
        MPlayer->setMuted(false);
    }
}


void MainWindow::on_pushButton_clicked()
{

    QString fileName = QFileDialog::getOpenFileName(this, tr("Sélectionner un fichier audio"), "", tr("Fichiers audio (*.mp3 *.ogg *.wav)"));


    if (!fileName.isEmpty()) {

        QFileInfo fileInfo(fileName);
        QString songName = fileInfo.fileName();
        QString songPath = fileInfo.absoluteFilePath();


        QSqlDatabase music = QSqlDatabase::database();
        if (music.isOpen()) {
            QSqlQuery query;
            query.prepare("INSERT INTO chansons (nom, chemin) VALUES (:nom, :chemin)");
            query.bindValue(":nom", songName);
            query.bindValue(":chemin", songPath);



            if (query.exec()) {
                QMessageBox::information(this, "Succès", "La chanson a été ajoutée à bibliothèque avec succès.");
                on_pushButton_2_clicked();
            } else {
                QMessageBox::critical(this, "Erreur", "Échec de l'ajout de la chanson : " + query.lastError().text());
            }
        } else {
            QMessageBox::critical(this, "Erreur", "Échec de l'ouverture de la base de données.");
        }
    } else {
        QMessageBox::warning(this, "Aucune sélection", "Aucun fichier sélectionné.");
    }
}






void MainWindow::on_horizontalSlider_Audio_File_Duration_triggered()
{

    QString FileName = QFileDialog::getOpenFileName(this, tr("Sélectionner le fichier audio"), "", tr("Fichiers MP3 (*.mp3 *.ogg *.wav)"));

    if (FileName.isEmpty()) {
        return;
    }


    MPlayer->setMedia(QUrl::fromLocalFile(FileName));


    QFileInfo fileinfo(FileName);
    QString fullSongName = fileinfo.fileName();
    QString songName = fileinfo.completeBaseName();


    ui->label_filename->setText(fullSongName);



}





void MainWindow::on_pushButton_Play_clicked()
{
    MPlayer->play();
}


void MainWindow::on_pushButton_3_Pause_clicked()
{
    MPlayer->pause();
}


void MainWindow::on_pushButton_4_Stop_clicked()
{
    MPlayer->stop();
}


void MainWindow::on_pushButton_previous_clicked()
{

}






void MainWindow::on_horizontalSlider_2_valueChanged(int value)
{
    MPlayer->setVolume(value);
}



void MainWindow::on_actionListe_de_lecture_triggered()
{

    QString folderPath = QFileDialog::getExistingDirectory(this, tr("Sélectionner un dossier contenant des fichiers audio"));


    if (!folderPath.isEmpty()) {

        QDir dir(folderPath);


        QStringList filters;
        filters << "*.mp3" << "*.ogg" << "*.wav";
        dir.setNameFilters(filters);

        QStringList fileList = dir.entryList(QDir::Files);


        if (!fileList.isEmpty()) {

            QString m3uFilePath = folderPath + "/playlist.m3u";
            QFile m3uFile(m3uFilePath);

            if (m3uFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
                QTextStream out(&m3uFile);
                out << "#EXTM3U\n";
                foreach (QString fileName, fileList) {
                    QString songPath = dir.filePath(fileName);
                    out << songPath << "\n";
                }

                m3uFile.close();
                QSqlDatabase music = QSqlDatabase::database();
                if (music.isOpen()) {
                    QSqlQuery query;
                    query.prepare("INSERT INTO playlists (nom, chemin) VALUES (:nom, :chemin)");
                    query.bindValue(":nom", "playlist");
                    query.bindValue(":chemin", m3uFilePath);

                    if (query.exec()) {
                        QMessageBox::information(this, "Succès", "La liste de lecture a été créée et ajoutée à la bibliothèque : " + m3uFilePath);
                    } else {
                        QMessageBox::critical(this, "Erreur", "Échec de l'ajout de la liste de lecture à la bibliothèque : " + query.lastError().text());
                    }
                } else {
                    QMessageBox::critical(this, "Erreur", "Échec de l'ouverture de la base de données.");
                }
            } else {
                QMessageBox::critical(this, "Erreur", "Échec de la création du fichier M3U.");
            }
        } else {
            QMessageBox::warning(this, "Aucun fichier trouvé", "Aucun fichier audio n'a été trouvé dans ce dossier.");
        }
    } else {
        QMessageBox::warning(this, "Aucun dossier sélectionné", "Aucun dossier n'a été sélectionné.");
    }
}


void MainWindow::on_actionR_initialiser_la_biblioth_que_triggered()
{

        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "Réinitialiser la bibliothèque",
                                      "Êtes-vous sûr de vouloir réinitialiser la bibliothèque ? Toutes les chansons et playlists seront supprimées.",
                                      QMessageBox::Yes | QMessageBox::No);

        if (reply == QMessageBox::Yes) {

            QSqlDatabase music = QSqlDatabase::database();
            if (music.isOpen()) {
                QSqlQuery query;


                if (!query.exec("DELETE FROM chansons")) {
                    QMessageBox::critical(this, "Erreur", "Erreur lors de la suppression des chansons : " + query.lastError().text());
                    return;
                }


                if (!query.exec("DELETE FROM playlists")) {
                    QMessageBox::critical(this, "Erreur", "Erreur lors de la suppression des playlists : " + query.lastError().text());
                    return;
                }


                QMessageBox::information(this, "Bibliothèque réinitialisée", "Toutes les chansons et playlists ont été supprimées.");
            } else {
                QMessageBox::critical(this, "Erreur", "Impossible d'ouvrir la base de données.");
            }
        } else {

            QMessageBox::information(this, "Opération annulée", "Aucune donnée n'a été supprimée.");
        }
}

void MainWindow::on_actionQuitter_triggered()
{

    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Quitter l'application",
                                  "Êtes-vous sûr de vouloir quitter le logiciel ?",
                                  QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {

        bool ok;
        QStringList ratings;
        ratings << "1 étoile"
                << "2 étoiles"
                << "3 étoiles"
                << "4 étoiles"
                << "5 étoiles";

        QString rating = QInputDialog::getItem(this, "Évaluation du logiciel",
                                               "svp évaluez mon logiciel MUSIKY avant de quitter :", ratings, 4, false, &ok);
        if (ok && !rating.isEmpty()) {

            QMessageBox::information(this, "Merci", "Merci pour votre évaluation : " + rating);
        }


        QApplication::quit();
    } else {


    }
}


void MainWindow::closeEvent(QCloseEvent *event)
{

    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Quitter l'application",
                                  "Êtes-vous sûr de vouloir quitter Musiky ?",
                                  QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {

        bool ok;
        QStringList ratings;
        ratings <<"1 étoile"
                << "2 étoiles"
                << "3 étoiles"
                << "4 étoiles"
                << "5 étoiles";

        QString rating = QInputDialog::getItem(this, "Évaluation du logiciel",
                                               "svp évaluez mon logiciel MUSIKY avant de quitter :", ratings, 4, false, &ok);
        if (ok && !rating.isEmpty()) {

            QMessageBox::information(this, "Merci", "Merci pour votre évaluation : " + rating);
        }


        event->accept();
    } else {

        event->ignore();

    }
}


void MainWindow::on_actionRaffraichir_la_binlioth_que_triggered()
{
       on_pushButton_2_clicked();
}


void MainWindow::on_pushButton_2_clicked()
{

        QSqlDatabase music = QSqlDatabase::database();
        if (music.isOpen()) {
            QSqlQuery query("SELECT nom, chemin FROM chansons");
            QStringList songNames;
            QMap<QString, QString> songMap;
            while (query.next()) {
                QString songName = query.value(0).toString();
                QString songPath = query.value(1).toString();
                songNames << songName;
                songMap[songName] = songPath;
            }


            QStringListModel *model = new QStringListModel(this);
            model->setStringList(songNames);


            ui->listView->setModel(model);


            connect(ui->listView, &QListView::clicked, [=](const QModelIndex &index) {
                QString selectedSongName = index.data(Qt::DisplayRole).toString();
                QString songPath = songMap[selectedSongName];


                if (QFile::exists(songPath)) {

                    MPlayer->setMedia(QUrl::fromLocalFile(songPath));


                    ui->label_filename->setText(selectedSongName);


                    MPlayer->play();

                } else {


                }
            });

        } else {
            QMessageBox::critical(this, "Erreur", "Échec de l'ouverture de la base de données.");
        }

}



void MainWindow::on_pushButton_3_clicked()
{

    ui->listView->setModel(new QStringListModel());


    QSqlQuery query("SELECT song_name FROM playlists");
    QStringList songList;

    while (query.next()) {
        QString songName = query.value(0).toString();
        songList << songName;
    }


    QStringListModel *model = new QStringListModel(songList, this);
    ui->listView->setModel(model);


    connect(ui->listView, &QListView::clicked, this, &MainWindow::playSelectedSong);
}


void MainWindow::playSelectedSong(const QModelIndex &index)
{
    if (index.isValid()) {
        QString songName = index.data().toString();


        QSqlQuery query;
        query.prepare("SELECT path FROM playlists WHERE song_name = :songName");
        query.bindValue(":songName", songName);

        if (query.exec() && query.next()) {
            QString songPath = query.value(0).toString();

            MPlayer->setMedia(QUrl::fromLocalFile(songPath));
            MPlayer->play();
        } else {
            qDebug() << "Chanson non trouvée dans la base de données.";
        }
    }
}



