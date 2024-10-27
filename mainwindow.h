#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
#include <QtMultimedia>
#include <QtCore>
#include <QtWidgets>
#include <QDebug>
#include <QtSql>
#include <QtGui>
#include <QSqlQuery>
#include <QFileInfo>
#include <QCloseEvent>
#include <QStringListModel>
#include <QFile>
#include <QMovie>
#include <QGraphicsScene>
#include <QMediaPlayer>
#include <QUrlQuery>





QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    QSqlDatabase db;
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();


private slots:
    void durationChanged(qint64 duration);

    void positionChanged(qint64 progress);

    void on_pushButton_volume_clicked();

    void on_pushButton_clicked();

    void on_horizontalSlider_Audio_File_Duration_triggered();

    void on_pushButton_Play_clicked();

    void on_pushButton_3_Pause_clicked();

    void on_pushButton_4_Stop_clicked();

    void on_pushButton_previous_clicked();

    void on_pushButton_3_next_clicked();

    void on_horizontalSlider_valueChanged(int value);

    void on_horizontalSlider_2_valueChanged(int value);

    void handleMediaPlayerStateChanged(QMediaPlayer::State state);

    void on_actionListe_de_lecture_triggered();

    void on_actionR_initialiser_la_biblioth_que_triggered();

    void on_actionQuitter_triggered();

    void on_actionRaffraichir_la_binlioth_que_triggered();

    void on_pushButton_2_clicked();

    void playSelectedSong(const QModelIndex &index);


    void on_pushButton_3_clicked();


private:
    void updateduration(qint64 duration);
    Ui::MainWindow *ui;
    bool IS_Muted = false;
    QMediaPlayer * MPlayer;
    qint64 Mduration;
    QGraphicsScene *scence;
    QTimer *timer;
    QMovie *movie;

protected:
    void closeEvent(QCloseEvent *event) override;

};
#endif // MAINWINDOW_H
