#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("8×8 消除游戏");
    resize(600, 500);

    // 初始化网格
    for (int i = 0; i < SIZE; i++)
        for (int j = 0; j < SIZE; j++)
            grid[i][j] = 0;

    score = 0;
    selectedShape = -1;
    isDragging = false;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    drawGrid(p);
    drawShapesPanel(p);
    if (isDragging && selectedShape >= 0)
        drawDraggingShape(p);

    // 显示分数
    p.drawText(10, 420, QString("分数: %1").arg(score));

    if (isGameOver())
        p.drawText(10, 450, "游戏结束！");
}

// 画 8×8 网格
void MainWindow::drawGrid(QPainter &p)
{
    const int cellSize = 40;
    const int startX = 50;
    const int startY = 50;

    p.setPen(QPen(Qt::gray, 1));
    for (int i = 0; i <= SIZE; i++) {
        p.drawLine(startX, startY + i*cellSize, startX + SIZE*cellSize, startY + i*cellSize);
        p.drawLine(startX + i*cellSize, startY, startX + i*cellSize, startY + SIZE*cellSize);
    }

    // 画已填充方块
    p.setBrush(QBrush(QColor(100, 150, 255)));
    p.setPen(Qt::NoPen);
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            if (grid[i][j] == 1) {
                p.drawRect(startX + j*cellSize + 2,
                           startY + i*cellSize + 2,
                           cellSize - 4, cellSize - 4);
            }
        }
    }
}

// 画右侧形状面板
void MainWindow::drawShapesPanel(QPainter &p)
{
    const int panelX = 400;
    const int startY = 50;
    const int shapeSize = 20;

    p.drawText(panelX, 20, "可选形状（点选拖动）");

    p.setBrush(Qt::white);
    p.setPen(Qt::black);
    for (int s = 0; s < shapes.size(); s++) {
        int baseY = startY + s * 60;
        for (auto &pt : shapes[s]) {
            int x = panelX + pt.x() * shapeSize;
            int y = baseY + pt.y() * shapeSize;
            p.drawRect(x, y, shapeSize, shapeSize);
        }
    }
}

// 画正在拖动的形状预览
void MainWindow::drawDraggingShape(QPainter &p)
{
    const int cellSize = 40;
    const int startX = 50;
    const int startY = 50;

    if (selectedShape < 0 || selectedShape >= shapes.size())
        return;

    int gridX = (dragPos.y() - startY) / cellSize;
    int gridY = (dragPos.x() - startX) / cellSize;

    p.setBrush(QBrush(QColor(200, 200, 255)));
    p.setPen(Qt::NoPen);
    for (auto &pt : shapes[selectedShape]) {
        int nx = gridX + pt.x();
        int ny = gridY + pt.y();
        if (nx >= 0 && nx < SIZE && ny >= 0 && ny < SIZE) {
            p.drawRect(startX + ny*cellSize + 2,
                       startY + nx*cellSize + 2,
                       cellSize - 4, cellSize - 4);
        }
    }
}

bool MainWindow::canPlace(int shapeIdx, int gridX, int gridY)
{
    if (shapeIdx < 0 || shapeIdx >= shapes.size())
        return false;

    for (auto &pt : shapes[shapeIdx]) {
        int nx = gridX + pt.x();
        int ny = gridY + pt.y();
        if (nx < 0 || nx >= SIZE || ny < 0 || ny >= SIZE)
            return false;
        if (grid[nx][ny] == 1)
            return false;
    }
    return true;
}

void MainWindow::placeShape(int shapeIdx, int gridX, int gridY)
{
    for (auto &pt : shapes[shapeIdx]) {
        int nx = gridX + pt.x();
        int ny = gridY + pt.y();
        grid[nx][ny] = 1;
    }
}

void MainWindow::checkAndClearLines()
{
    int cleared = 0;

    // 消除行
    for (int i = 0; i < SIZE; i++) {
        bool full = true;
        for (int j = 0; j < SIZE; j++)
            if (grid[i][j] == 0) { full = false; break; }
        if (full) {
            for (int j = 0; j < SIZE; j++) grid[i][j] = 0;
            cleared++;
        }
    }

    // 消除列
    for (int j = 0; j < SIZE; j++) {
        bool full = true;
        for (int i = 0; i < SIZE; i++)
            if (grid[i][j] == 0) { full = false; break; }
        if (full) {
            for (int i = 0; i < SIZE; i++) grid[i][j] = 0;
            cleared++;
        }
    }

    if (cleared > 0)
        score += cleared * 100;
}

bool MainWindow::isGameOver()
{
    for (int s = 0; s < shapes.size(); s++) {
        for (int i = 0; i < SIZE; i++) {
            for (int j = 0; j < SIZE; j++) {
                if (canPlace(s, i, j))
                    return false;
            }
        }
    }
    return true;
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        const int panelX = 400;
        const int startY = 50;
        const int shapeSize = 20;

        for (int s = 0; s < shapes.size(); s++) {
            int baseY = startY + s * 60;
            for (auto &pt : shapes[s]) {
                QRect rect(panelX + pt.x()*shapeSize,
                           baseY + pt.y()*shapeSize,
                           shapeSize, shapeSize);
                if (rect.contains(event->pos())) {
                    selectedShape = s;
                    isDragging = true;
                    dragPos = event->pos();
                    break;
                }
            }
            if (isDragging) break;
        }
    }
    QMainWindow::mousePressEvent(event);
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    if (isDragging && selectedShape >= 0) {
        dragPos = event->pos();
        update();
    }
    QMainWindow::mouseMoveEvent(event);
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && isDragging && selectedShape >= 0) {
        const int cellSize = 40;
        const int startX = 50;
        const int startY = 50;

        int gridX = (event->pos().y() - startY) / cellSize;
        int gridY = (event->pos().x() - startX) / cellSize;

        if (canPlace(selectedShape, gridX, gridY)) {
            placeShape(selectedShape, gridX, gridY);
            checkAndClearLines();
        }

        isDragging = false;
        selectedShape = -1;
        update();
    }
    QMainWindow::mouseReleaseEvent(event);
}