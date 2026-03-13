#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QPainter>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFont>
#include <QPen>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , currentStep(-1)
    , autoPlaying(false)
{
    ui->setupUi(this);

    rectFillColors = {
        QColor(120, 200, 120, 100),   // green
        QColor(220, 130, 130, 100),   // red
        QColor(130, 150, 220, 100),   // blue
        QColor(220, 200, 100, 100),   // yellow
        QColor(200, 130, 220, 100),   // purple
        QColor(100, 210, 210, 100),   // turquoise
        QColor(220, 170, 100, 100),   // orange
        QColor(180, 180, 180, 100),   // grey
    };
    rectBorderColors = {
        QColor(80, 160, 80),    QColor(180, 80, 80),
        QColor(80, 100, 180),   QColor(180, 160, 50),
        QColor(160, 80, 180),   QColor(50, 170, 170),
        QColor(180, 130, 50),   QColor(130, 130, 130),
    };

    setupControls();
    txtInput->setText("[0,0,2,2],[1,1,2,3],[1,0,3,1]");
    onLoadInput();

    setMinimumSize(1050, 680);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupControls()
{
    controlBar  = new QWidget(this);
    txtInput    = new QLineEdit(controlBar);
    btnLoad     = new QPushButton("Load", controlBar);
    btnStepBack = new QPushButton("◀ Back", controlBar);
    btnStep     = new QPushButton("▶ Step", controlBar);
    btnAuto     = new QPushButton("⏵ Auto", controlBar);
    btnReset    = new QPushButton("↺ Reset", controlBar);
    speedSlider = new QSlider(Qt::Horizontal, controlBar);
    lblSpeed    = new QLabel("Speed:", controlBar);
    lblDesc     = new QLabel("", this);
    autoTimer   = new QTimer(this);

    speedSlider->setRange(1, 20);
    speedSlider->setValue(5);
    speedSlider->setFixedWidth(120);

    txtInput->setPlaceholderText("[x1,y1,x2,y2],[x1,y1,x2,y2],...");
    txtInput->setMinimumWidth(300);

    QFont btnFont("Monospace", 10, QFont::Bold);
    for (auto* b : {btnLoad, btnStepBack, btnStep, btnAuto, btnReset}) {
        b->setFont(btnFont);
        b->setFixedWidth(90);
    }

    auto *hbox = new QHBoxLayout(controlBar);
    hbox->setContentsMargins(10, 4, 10, 4);
    hbox->addWidget(txtInput, 1);
    hbox->addWidget(btnLoad);
    hbox->addSpacing(12);
    hbox->addWidget(btnStepBack);
    hbox->addWidget(btnStep);
    hbox->addWidget(btnAuto);
    hbox->addWidget(btnReset);
    hbox->addSpacing(8);
    hbox->addWidget(lblSpeed);
    hbox->addWidget(speedSlider);

    lblDesc->setWordWrap(true);
    lblDesc->setStyleSheet(
        "color: #cccccc; font-size: 12px; font-family: monospace;"
        "background: #1a1a28; border: 1px solid #2a2a3a;"
        "border-radius: 4px; padding: 6px 10px;"
        );

    connect(btnLoad,     &QPushButton::clicked,  this, &MainWindow::onLoadInput);
    connect(txtInput,    &QLineEdit::returnPressed, this, &MainWindow::onLoadInput);
    connect(btnStepBack, &QPushButton::clicked,  this, &MainWindow::onStepBack);
    connect(btnStep,     &QPushButton::clicked,  this, &MainWindow::onStep);
    connect(btnAuto,     &QPushButton::clicked,  this, &MainWindow::onAutoPlay);
    connect(btnReset,    &QPushButton::clicked,  this, &MainWindow::onReset);
    connect(speedSlider, &QSlider::valueChanged, this, &MainWindow::onSpeedChanged);
    connect(autoTimer,   &QTimer::timeout,       this, &MainWindow::onTimerTick);

}

std::vector<std::vector<int>> MainWindow::parseInput(const QString& text)
{
    std::vector<std::vector<int>> result;

    QRegularExpression numRe("-?\\d+");
    auto it = numRe.globalMatch(text);

    std::vector<int> allNums;
    while (it.hasNext()) {
        auto match = it.next();
        allNums.push_back(match.captured().toInt());
    }

    for (int i = 0; i + 3 < (int)allNums.size(); i += 4) {
        result.push_back({allNums[i], allNums[i+1], allNums[i+2], allNums[i+3]});
    }

    return result;
}

void MainWindow::onLoadInput()
{
    auto rects = parseInput(txtInput->text());

    if (rects.empty()) {
        lblDesc->setText("Invalid input. Format: [x1,y1,x2,y2],[x1,y1,x2,y2],...");
        return;
    }

    loadRectangles(rects);
}

void MainWindow::loadRectangles(const std::vector<std::vector<int>>& rects)
{
    currentStep = -1;
    autoPlaying = false;
    autoTimer->stop();
    btnAuto->setText("⏵ Auto");

    algo.solve(rects);

    updateDescription();
    update();
}


void MainWindow::updateDescription()
{
    if (currentStep >= 0 && currentStep < (int)algo.states.size())
        lblDesc->setText(QString::fromStdString(algo.states[currentStep].description));
    else
        lblDesc->setText("Press 'Step' to start the visualization step by step");
}


QRectF MainWindow::canvasRect() const
{
    double w = width() * 0.63;
    double h = height() - 140;
    return QRectF(50, 35, w, h);
}

double MainWindow::toScreenX(double wx) const
{
    QRectF c = canvasRect();
    return c.left() + (wx - algo.wxMin) / (double)(algo.wxMax - algo.wxMin) * c.width();
}

double MainWindow::toScreenY(double wy) const
{
    QRectF c = canvasRect();
    return c.bottom() - (wy - algo.wyMin) / (double)(algo.wyMax - algo.wyMin) * c.height();
}

QRectF MainWindow::toScreenRect(double x1, double y1, double x2, double y2) const
{
    double sx1 = toScreenX(x1), sy1 = toScreenY(y2);
    double sx2 = toScreenX(x2), sy2 = toScreenY(y1);
    return QRectF(sx1, sy1, sx2 - sx1, sy2 - sy1);
}


void MainWindow::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    p.fillRect(rect(), QColor(25, 25, 35));

    if (algo.rectangles.empty()) return;

    drawGrid(p);
    drawActiveStrip(p);
    drawRectangles(p);
    drawSweepLine(p);
    drawCntPanel(p);
    drawLegend(p);

    int descY = height() - 130;
    int ctrlY = height() - 55;
    lblDesc->setGeometry(20, descY, width() - 40, 40);
    controlBar->setGeometry(0, ctrlY, width(), 50);
}

void MainWindow::drawGrid(QPainter &p)
{
    QRectF c = canvasRect();
    QPen gridPen(QColor(50, 50, 65), 0.5, Qt::DotLine);

    // Compute a nice grid step so we get ~5-12 lines per axis.
    // For small ranges (e.g. 0..5) step=1, for huge (0..10^9) step=200000000, etc.
    auto niceStep = [](long long range) -> long long {
        if (range <= 0) return 1;
        // Target ~8 grid lines
        double raw = range / 8.0;
        // Round to nearest 1, 2, or 5 × 10^k
        double mag = std::pow(10.0, std::floor(std::log10(raw)));
        double norm = raw / mag;  // 1.0 .. 9.99
        long long step;
        if      (norm < 1.5) step = (long long)mag;
        else if (norm < 3.5) step = (long long)(2 * mag);
        else if (norm < 7.5) step = (long long)(5 * mag);
        else                 step = (long long)(10 * mag);
        return std::max(step, 1LL);
    };

    long long stepX = niceStep((long long)algo.wxMax - algo.wxMin);
    long long stepY = niceStep((long long)algo.wyMax - algo.wyMin);

    // First grid line at or after wxMin, aligned to stepX
    long long startX = ((long long)algo.wxMin / stepX) * stepX;
    if (startX < algo.wxMin) startX += stepX;

    long long startY = ((long long)algo.wyMin / stepY) * stepY;
    if (startY < algo.wyMin) startY += stepY;

    p.setFont(QFont("Monospace", 9));

    for (long long x = startX; x <= algo.wxMax; x += stepX) {
        double sx = toScreenX(x);
        p.setPen(gridPen);
        p.drawLine(QPointF(sx, c.top()), QPointF(sx, c.bottom()));
        p.setPen(QColor(140, 140, 160));
        p.drawText(QRectF(sx - 30, c.bottom() + 3, 60, 18), Qt::AlignCenter, QString::number(x));
    }
    for (long long y = startY; y <= algo.wyMax; y += stepY) {
        double sy = toScreenY(y);
        p.setPen(gridPen);
        p.drawLine(QPointF(c.left(), sy), QPointF(c.right(), sy));
        p.setPen(QColor(140, 140, 160));
        p.drawText(QRectF(c.left() - 55, sy - 9, 50, 18), Qt::AlignRight | Qt::AlignVCenter, QString::number(y));
    }

    p.setPen(QPen(QColor(80, 80, 100), 1));
    p.drawRect(c);
}

void MainWindow::drawRectangles(QPainter &p)
{
    for (int i = 0; i < (int)algo.rectangles.size(); i++) {
        auto& r = algo.rectangles[i];
        QRectF sr = toScreenRect(r[0], r[1], r[2], r[3]);

        QColor fill = rectFillColors[i % rectFillColors.size()];
        QColor border = rectBorderColors[i % rectBorderColors.size()];

        p.setBrush(fill);
        p.setPen(QPen(border, 2));
        p.drawRect(sr);

        p.setPen(QColor(220, 220, 220));
        p.setFont(QFont("Monospace", 9, QFont::Bold));
        p.drawText(sr, Qt::AlignCenter, QString("R%1").arg(i));
    }
}


//  Sweep line + active Y segments (green)
void MainWindow::drawSweepLine(QPainter &p)
{
    if (currentStep < 0 || currentStep >= (int)algo.states.size()) return;
    const AlgorithmState& st = algo.states[currentStep];

    double sx = toScreenX(st.sweepX);
    QRectF c = canvasRect();

    // Red sweep line
    p.setPen(QPen(QColor(255, 60, 60), 2.5));
    p.drawLine(QPointF(sx, c.top()), QPointF(sx, c.bottom()));

    QPolygonF arrow;
    arrow << QPointF(sx, c.top() - 8)
          << QPointF(sx - 6, c.top() - 1)
          << QPointF(sx + 6, c.top() - 1);
    p.setBrush(QColor(255, 60, 60));
    p.setPen(Qt::NoPen);
    p.drawPolygon(arrow);

    p.setPen(QColor(255, 100, 100));
    p.setFont(QFont("Monospace", 9, QFont::Bold));
    p.drawText(QRectF(sx - 25, c.top() - 24, 50, 16),
               Qt::AlignCenter, QString("x=%1").arg(st.sweepX));

    // Green active segnets on swwep line
    for (int i = 0; i < algo.numSegments && i < (int)st.cnt.size(); i++) {
        if (st.cnt[i] > 0) {
            double sy1 = toScreenY(algo.yVals[i + 1]);
            double sy2 = toScreenY(algo.yVals[i]);
            p.setBrush(QColor(0, 255, 100, 120));
            p.setPen(QPen(QColor(0, 200, 80), 1.5));
            p.drawRect(QRectF(sx - 4, sy1, 8, sy2 - sy1));
        }
    }
}

// Active strip (yellow)
void MainWindow::drawActiveStrip(QPainter &p)
{
    if (currentStep < 0 || currentStep >= (int)algo.states.size()) return;
    const AlgorithmState& st = algo.states[currentStep];

    if (!st.isCompute || st.prevX >= st.sweepX) return;

    for (int i = 0; i < algo.numSegments && i < (int)st.cnt.size(); i++) {
        if (st.cnt[i] > 0) {
            QRectF sr = toScreenRect(st.prevX, algo.yVals[i],
                                     st.sweepX, algo.yVals[i + 1]);
            p.setBrush(QColor(255, 220, 50, 70));
            p.setPen(QPen(QColor(255, 200, 0, 150), 1, Qt::DashLine));
            p.drawRect(sr);
        }
    }
}


// cnt[] panel (desni deo prozora)
void MainWindow::drawCntPanel(QPainter &p)
{
    if (algo.yVals.size() < 2) return;

    double panelX = width() * 0.66;
    double panelY = 35;
    double panelW = width() * 0.32;
    double panelH = canvasRect().height();

    p.setPen(QColor(200, 200, 220));
    p.setFont(QFont("Monospace", 11, QFont::Bold));
    p.drawText(QRectF(panelX, panelY - 2, panelW, 20),
               Qt::AlignLeft, "cnt[] — Y segments:");

    double startY = panelY + 28;
    double segH = std::min(38.0, (panelH - 60.0) / algo.numSegments);

    // Current cnt[]
    std::vector<int> curCnt;
    if (currentStep >= 0 && currentStep < (int)algo.states.size())
        curCnt = algo.states[currentStep].cnt;
    else
        curCnt.assign(algo.numSegments, 0);

    for (int i = 0; i < algo.numSegments; i++) {
        double y = startY + i * segH;
        bool active = curCnt[i] > 0;

        QColor bg = active ? QColor(0, 180, 80, 60) : QColor(40, 40, 55);
        p.setBrush(bg);
        p.setPen(QPen(active ? QColor(0, 140, 60, 100) : QColor(60, 60, 80), 1));
        p.drawRoundedRect(QRectF(panelX, y, panelW - 15, segH - 4), 4, 4);

        p.setPen(QColor(190, 190, 210));
        p.setFont(QFont("Monospace", 9));
        p.drawText(QRectF(panelX + 10, y, panelW * 0.6, segH - 4),
                   Qt::AlignVCenter,
                   QString("seg[%1] = [%2, %3)")
                       .arg(i).arg(algo.yVals[i]).arg(algo.yVals[i + 1]));

        p.setFont(QFont("Monospace", 13, QFont::Bold));
        p.setPen(active ? QColor(100, 255, 150) : QColor(100, 100, 120));
        p.drawText(QRectF(panelX + panelW * 0.62, y, panelW * 0.33, segH - 4),
                   Qt::AlignVCenter | Qt::AlignCenter,
                   QString("cnt = %1").arg(curCnt[i]));
    }

    // Total area
    double infoY = startY + algo.numSegments * segH + 12;
    long long total = (currentStep >= 0 && currentStep < (int)algo.states.size())
                          ? algo.states[currentStep].totalAreaMod : 0;

    p.setBrush(QColor(30, 30, 45));
    p.setPen(QPen(QColor(80, 70, 40), 1));
    p.drawRoundedRect(QRectF(panelX, infoY, panelW - 15, 50), 6, 6);

    p.setPen(QColor(255, 220, 80));
    p.setFont(QFont("Monospace", 14, QFont::Bold));
    p.drawText(QRectF(panelX + 10, infoY + 4, panelW, 24),
               Qt::AlignLeft, QString("Total Area: %1").arg(total));

    p.setPen(QColor(140, 140, 160));
    p.setFont(QFont("Monospace", 10));
    p.drawText(QRectF(panelX + 10, infoY + 28, panelW, 18),
               Qt::AlignLeft,
               QString("Step: %1 / %2")
                   .arg(std::max(0, currentStep + 1))
                   .arg(algo.states.size()));
}

void MainWindow::drawLegend(QPainter &p)
{
    if (algo.rectangles.empty()) return;

    double panelX = width() * 0.66;
    double panelW = width() * 0.32;

    // Racunamo gde se zavrsava Total Area box
    double startY = 35 + 28;
    double segH = std::min(38.0, (canvasRect().height() - 60.0) / algo.numSegments);
    double infoY = startY + algo.numSegments * segH + 12;
    double legendY = infoY + 65;  // odmah ispod Total Area boxa (50 visina + 15 razmak)

    p.setPen(QColor(160, 160, 180));
    p.setFont(QFont("Monospace", 9, QFont::Bold));
    p.drawText(QRectF(panelX, legendY, 200, 16), Qt::AlignLeft, "Legend:");
    legendY += 20;

    p.setFont(QFont("Monospace", 9));
    for (int i = 0; i < (int)algo.rectangles.size(); i++) {
        auto& r = algo.rectangles[i];
        double y = legendY + i * 22;

        QColor c = rectFillColors[i % rectFillColors.size()]; c.setAlpha(200);
        p.setBrush(c);
        p.setPen(QPen(rectBorderColors[i % rectBorderColors.size()], 1.5));
        p.drawRoundedRect(QRectF(panelX, y, 16, 16), 2, 2);

        p.setPen(QColor(180, 180, 200));
        p.drawText(QRectF(panelX + 22, y - 1, 250, 18), Qt::AlignVCenter,
                   QString("R%1 = [%2, %3, %4, %5]").arg(i).arg(r[0]).arg(r[1]).arg(r[2]).arg(r[3]));
    }
}

void MainWindow::onStep()
{
    if (currentStep < (int)algo.states.size() - 1) {
        currentStep++;
        updateDescription();
        update();
    }
}

void MainWindow::onStepBack()
{
    if (currentStep > 0) {
        currentStep--;
        updateDescription();
        update();
    }
}

void MainWindow::onAutoPlay()
{
    autoPlaying = !autoPlaying;
    if (autoPlaying) {
        btnAuto->setText("⏸ Pause");
        autoTimer->start(1100 - speedSlider->value() * 50);
    } else {
        btnAuto->setText("⏵ Auto");
        autoTimer->stop();
    }
}

void MainWindow::onReset()
{
    currentStep = -1;
    autoPlaying = false;
    autoTimer->stop();
    btnAuto->setText("⏵ Auto");
    updateDescription();
    update();
}

void MainWindow::onSpeedChanged(int val)
{
    if (autoTimer->isActive())
        autoTimer->setInterval(1100 - val * 50);
}

void MainWindow::onTimerTick()
{
    if (currentStep < (int)algo.states.size() - 1) {
        currentStep++;
        updateDescription();
        update();
    } else {
        autoPlaying = false;
        autoTimer->stop();
        btnAuto->setText("⏵ Auto");
    }
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);
    update();
}
