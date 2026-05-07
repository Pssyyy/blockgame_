#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPainter>
#include <QMouseEvent>
#include <QVector>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    Ui::MainWindow *ui;
    static const int SIZE = 8;
    int grid[SIZE][SIZE];
    int score;
    int selectedShape;
    bool isDragging;
    QPoint dragPos;

    QVector<QVector<QPoint>> shapes;

    void drawGrid(QPainter &p);
    void drawShapesPanel(QPainter &p);
    void drawDraggingShape(QPainter &p);

    bool canPlace(int shapeIdx, int gridX, int gridY);
    void placeShape(int shapeIdx, int gridX, int gridY);
    void checkAndClearLines();
    bool isGameOver();

    QVector<QPoint> randomShape(int maxCell = 4);
    QVector<QPoint> rotateShapeRandom(const QVector<QPoint>& src);
    void refreshRandomShapes(int count = 7);
};

#endif // MAINWINDOW_H