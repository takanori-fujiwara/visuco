#include "StatView.h"

// TODO: convert a way for rendering to using shader

StatView::StatView(QWidget *parent)
{
    //fontSize = 10;
    fontSize = 12; // for paper
    scale = 1.0;
    translateX = 0.0;
    translateY = 0.0;

    valMin = 0;
    valMax = 0;
    displayedRankingStart = 0;
    displayedRankingEnd = -1;
    thresRatioShowLabel = 0.05; // if there is more than or equal to 5% show label

    chartMarginTop = 0.065;
    chartMarginLeft = 0.2;
    chartMarginRight = 0.1;
    chartMarginBottom = 0.1;

    median = 0.0;
    zeroValAxisPos = chartMarginLeft;
    medianAxisPos = chartMarginLeft;
    lowerQuantileAxisPos = chartMarginLeft;
    upperQuantileAxisPos = chartMarginLeft;

    mouseMode = "selection";

    selectedColorIndex = -1;
    setWindowTitle("Metric Rank View");
    this->setFixedWidth(250);
    this->setFixedHeight(750);

    comboBox = new QComboBox(this);
    comboBox->setFixedWidth(250);
    comboBox->addItem(modeRouteByte);
    comboBox->addItem(modeRouteLength);
    comboBox->addItem(modeRouteHopbyte);
    comboBox->addItem(modeEdgeLoad);
    comboBox->addItem(modeNodeInDeg);
    comboBox->addItem(modeNodeOutDeg);
    comboBox->addItem(modeNodeInMinusOutDeg);

    connect(comboBox, SIGNAL(activated(QString)),
            this, SLOT(resetDisplayedRankingStartEndFromComboBox(QString)));
    connect(comboBox, SIGNAL(activated(QString)),
            this, SLOT(updateAllDrawingInfoFromComboBox(QString)));
    connect(comboBox, SIGNAL(activated(int)),
            this, SLOT(emitCurrentComboBoxIndexChanged(int)));
}

void StatView::initializeGL()
{
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glEnable(GL_POINT_SMOOTH);
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_POLYGON_SMOOTH);
    glAlphaFunc(GL_GREATER, 0.02);
}

void StatView::resizeGL(int width, int height)
{
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, 1.0, 0.0, 1.0, -1.0, 1.0);
    this->update();
}

void StatView::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA , GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_ALPHA_TEST);

    glPushMatrix();
    {
        glScalef(scale, scale, scale);
        glTranslatef(translateX, translateY, 0.0);
        drawAxis();
        drawDistri();
        drawSelectedArea();
        if (comboBox->currentText() == modeEdgeLoad) {
            drawColorMap(chartMarginLeft, chartMarginBottom - 0.03, (1.0 - chartMarginLeft -chartMarginRight), 0.01, color->edgeColorOfPoints);
        } else if (comboBox->currentText() == modeRouteLength) {
            drawColorMap(chartMarginLeft, chartMarginBottom - 0.03, (1.0 - chartMarginLeft -chartMarginRight), 0.01, color->getRouteColorOfPointsForMetric("length"));
        } else if (comboBox->currentText() == modeRouteByte) {
            drawColorMap(chartMarginLeft, chartMarginBottom - 0.03, (1.0 - chartMarginLeft -chartMarginRight), 0.01, color->getRouteColorOfPointsForMetric("byte"));
        } else if (comboBox->currentText() == modeRouteHopbyte) {
            drawColorMap(chartMarginLeft, chartMarginBottom - 0.03, (1.0 - chartMarginLeft -chartMarginRight), 0.01, color->getRouteColorOfPointsForMetric("hopbyte"));
        } else if (comboBox->currentText() == modeNodeInDeg) {
            drawColorMap(chartMarginLeft, chartMarginBottom - 0.03, (1.0 - chartMarginLeft -chartMarginRight), 0.01, color->vertexColorOfPoints);
        } else if (comboBox->currentText() == modeNodeOutDeg) {
            drawColorMap(chartMarginLeft, chartMarginBottom - 0.03, (1.0 - chartMarginLeft -chartMarginRight), 0.014, color->vertexColorOfPoints);
        }
        drawColorPoints();
    }
    glPopMatrix();
    glClear(GL_DEPTH_BUFFER_BIT);
    paintDistriLabels();
}

void StatView::drawDistri()
{
    for (int i = 0; i < (int)distriVbo.size() / 8; ++i) {
        glColor4d(distriCbo.at(i * 4), distriCbo.at(i * 4 + 1), distriCbo.at(i * 4 + 2), distriCbo.at(i * 4 + 3));
        glBegin(GL_TRIANGLE_FAN);
        glVertex2d(distriVbo.at(i * 8    ), distriVbo.at(i * 8 + 1));
        glVertex2d(distriVbo.at(i * 8 + 2), distriVbo.at(i * 8 + 3));
        glVertex2d(distriVbo.at(i * 8 + 4), distriVbo.at(i * 8 + 5));
        glVertex2d(distriVbo.at(i * 8 + 6), distriVbo.at(i * 8 + 7));
        glEnd();
    }

    // outline
    glLineWidth(1.0 * this->devicePixelRatioF());
    glColor4d(0.3, 0.3, 0.7, 1.0);
    glBegin(GL_LINE_STRIP);
    for (int i = 0; i < (int)distriVbo.size() / 8; ++i) {
        glVertex2d(distriVbo.at(i * 8 + 2), distriVbo.at(i * 8 + 3));
        glVertex2d(distriVbo.at(i * 8 + 4), distriVbo.at(i * 8 + 5));
    }
    glEnd();
}

void StatView::drawAxis()
{
    float x0 = chartMarginLeft; // start
    float x2 = 1.0 - chartMarginRight; // end
    float x1 = (x0 + x2) / 2; // middle
    float y0 = 1.0 - chartMarginTop;
    float y1 = chartMarginBottom;

    glLineWidth(2.0);
    glBegin(GL_LINES);
    glColor4f(0.4, 0.4, 0.4, 1.0);
    glVertex2f(x1, y0);
    glVertex2f(x1, y1);
    glVertex2f(x2, y0);
    glVertex2f(x2, y1);
    glVertex2f(zeroValAxisPos, y0);
    glVertex2f(zeroValAxisPos, y1);
    glVertex2f(medianAxisPos, y0);
    glVertex2f(medianAxisPos, y1);
    glEnd();

    glLineWidth(3.0);
    glBegin(GL_LINES);
    glColor4f(0.0, 0.0, 0.0, 1.0);
    glVertex2f(x0, y0);
    glVertex2f(x0, y1);
    glEnd();
}

void StatView::drawColorMap(float x, float y, float w, float h, QList< QPair<float, QColor> > colorOfPoints)
{
    int numOfSplit = 100;
    float deltaW = w / (float)numOfSplit;
    float deltaRatio = 1.0 / (float)numOfSplit;

    for (int i = 0; i < numOfSplit; ++i) {
        glBegin(GL_TRIANGLE_FAN);
        QColor qColor = color->ratioToHeatMapColor(i * deltaRatio, colorOfPoints);
        glColor4d(qColor.redF(), qColor.greenF(), qColor.blueF(), qColor.alphaF());
        glVertex2d(x + i * deltaW,     y);
        glVertex2d(x + (i+1) * deltaW, y);
        glVertex2d(x + (i+1) * deltaW, y + h);
        glVertex2d(x + i * deltaW,     y + h);
        glEnd();
    }

    // outline
    glLineWidth(1.0);
    glBegin(GL_LINE_LOOP);
    glColor4d(0.0, 0.0, 0.0, 1.0);
    glVertex2d(x,     y);
    glVertex2d(x + w, y);
    glVertex2d(x + w, y + h);
    glVertex2d(x,     y + h);
    glEnd();
}
void StatView::paintDistriLabels()
{
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);

    int marginLeft = 10;
    int marginTop = this->height() * chartMarginTop;

    int fontW;
    int fontH;

    painter.setPen(Qt::black);

    // ylabels
    painter.setFont(QFont("San Francisco", fontSize + 1));
    fontW = painter.fontMetrics().ascent();
    fontH = painter.fontMetrics().height();
    painter.drawText(marginLeft, marginTop - fontH * 1.5, 10 * fontW, fontH,
                     Qt::AlignVCenter | Qt::AlignLeft, "Rank");

    painter.setFont(QFont("San Francisco", fontSize));
    fontW = painter.fontMetrics().ascent();
    fontH = painter.fontMetrics().height();

    int infoLength = painter.fontMetrics().width("123456");

    float prevLabelYPos = 0.0; // to avoid too many labels in limited space
    float prevXPos = std::numeric_limits<float>::max(); // to avoid too many labels in limited space
    float finalYPos = this->height() * (1.0 - chartMarginBottom) - fontH / 2.0; // to avoid overwapping with final count
    for (int i = 0; i < distriLabels.size(); ++i) {
        // last label is used for total number
        if (i < distriLabels.size() - 1) {
            float labelYPos = this->height() * (1.0 - distriVbo.at(i * 8 + 1)) - fontH / 2.0;
            float xPos = distriVbo.at(i * 8 + 2);

            if (labelYPos - prevLabelYPos >= 0.8 * fontH &&
                    finalYPos - labelYPos >= 0.8 * fontH &&
                    xPos < (1.0 - thresRatioShowLabel) * prevXPos) {
                painter.drawText(marginLeft - 5, labelYPos, infoLength, fontH, // - 5 for paper
                             Qt::AlignVCenter | Qt::AlignRight, distriLabels.at(i) + " -");
                prevLabelYPos = labelYPos;
                prevXPos = xPos;
            }
        } else {
            painter.drawText(marginLeft - 5, finalYPos, infoLength, fontH, // - 5 for paper
                         Qt::AlignVCenter | Qt::AlignRight, distriLabels.at(i) + " -");
        }
    }

    painter.setFont(QFont("San Francisco", fontSize + 2));
    fontW = painter.fontMetrics().ascent();
    fontH = painter.fontMetrics().height();
    // xlabels
    int x0 = this->width() * chartMarginLeft; // start
    int x2 = this->width() * (1.0 - chartMarginRight); // end
    int x1 = (x0 + x2) / 2; // middle
    int y = this->height() * (1.0 - chartMarginBottom) + 2;
    if (comboBox->currentText() == modeEdgeLoad) {
        painter.drawText(x1 - fontW * 5, this->height() - fontH * 2, fontW * 10, fontH,
                         Qt::AlignVCenter | Qt::AlignCenter, "Message Size (byte)");
    } else if (comboBox->currentText() == modeRouteLength) {
        painter.drawText(x1 - fontW * 5, this->height() - fontH * 2, fontW * 10, fontH,
                         Qt::AlignVCenter | Qt::AlignCenter, "Length");
    } else if (comboBox->currentText() == modeRouteByte) {
        painter.drawText(x1 - fontW * 5, this->height() - fontH * 2, fontW * 10, fontH,
                         Qt::AlignVCenter | Qt::AlignCenter, "Message Size (byte)");
    } else if (comboBox->currentText() == modeRouteHopbyte) {
        painter.drawText(x1 - fontW * 5, this->height() - fontH * 2, fontW * 10, fontH,
                         Qt::AlignVCenter | Qt::AlignCenter, "Hop-Byte");
    } else {
        painter.drawText(x1 - fontW * 5, this->height() - fontH * 2, fontW * 10, fontH,
                         Qt::AlignVCenter | Qt::AlignCenter, "Degree");
    }
    painter.setFont(QFont("San Francisco", fontSize));
    fontW = painter.fontMetrics().ascent();
    fontH = painter.fontMetrics().height();
    painter.drawText(x0, y, fontW * 5, fontH,
                     Qt::AlignVCenter | Qt::AlignLeft, QString::number(valMin, 'g', 3));
    painter.drawText(x1 - fontW * 2.5, y, fontW * 5, fontH,
                     Qt::AlignVCenter | Qt::AlignCenter, QString::number((valMax + valMin) / 2.0, 'g', 3));
    painter.drawText(x2 - fontW * 2.5, y, fontW * 5, fontH,
                     Qt::AlignVCenter | Qt::AlignCenter, QString::number(valMax, 'g', 3));
    if (valMin != 0) {
        painter.drawText(this->width() * zeroValAxisPos, y, fontW * 10, fontH,
                         Qt::AlignVCenter | Qt::AlignLeft, QString::number(0));
    }
    painter.drawText(this->width() * medianAxisPos, y - fontH - fontH, fontW * 10, fontH,
                     Qt::AlignVCenter | Qt::AlignLeft, "median");
    painter.drawText(this->width() * medianAxisPos, y - fontH, fontW * 10, fontH,
                     Qt::AlignVCenter | Qt::AlignLeft, QString::number(median, 'g', 3));

    painter.end();
}

void StatView::drawSelectedArea()
{
    if (selectedYRange.size() >= 2) {
        glColor4d(color->selectionColor.redF(),color->selectionColor.greenF(), color->selectionColor.blueF(), color->selectionColor.alphaF());
        glBegin(GL_TRIANGLE_FAN);
        glVertex2d(chartMarginLeft, selectedYRange.at(0));
        glVertex2d(chartMarginLeft, selectedYRange.at(1));
        glVertex2d(1.0 - chartMarginRight, selectedYRange.at(1));
        glVertex2d(1.0 - chartMarginRight, selectedYRange.at(0));
        glEnd();
    }
}

void StatView::updateColorPointsVbos()
{
    colorPointsVbosX.clear();
    colorPointsInsideVbosX.clear();

    QList< QPair<float, QColor> > colorPoints;

    if (comboBox->currentText() == modeEdgeLoad) {
        colorPoints = color->edgeColorOfPoints;
    } else if (comboBox->currentText() == modeRouteLength) {
        colorPoints = color->getRouteColorOfPointsForMetric("length");
    } else if (comboBox->currentText() == modeRouteByte) {
        colorPoints = color->getRouteColorOfPointsForMetric("byte");
    } else if (comboBox->currentText() == modeRouteHopbyte) {
        colorPoints = color->getRouteColorOfPointsForMetric("hopbyte");
    } else if (comboBox->currentText() == modeNodeInDeg) {
        colorPoints = color->vertexColorOfPoints;
    } else if (comboBox->currentText() == modeNodeOutDeg) {
        colorPoints = color->vertexColorOfPoints;
    }

    float chartW = 1.0 - chartMarginLeft - chartMarginRight;
    float y = chartMarginBottom - 0.03;
    float ax = 0.05; // point size
    float ay = ax * this->width() / (float)this->height();

    for(int i = 0; i < colorPoints.size(); ++i) {
        QPair<float, QColor> point = colorPoints.at(i);
        float ratio = point.first;
        float x = chartW * ratio + chartMarginLeft;

        colorPointsVbosX.push_back(vector<float>({x, y,
                                                float(x - 0.5 * ax), float(y - 0.5 * ay),
                                                float(x - 0.5 * ax), float(y - 1.5 * ay),
                                                float(x + 0.5 * ax), float(y - 1.5 * ay),
                                                float(x + 0.5 * ax), float(y - 0.5 * ay)}));
        colorPointsInsideVbosX.push_back(vector<float>({float(x - 0.35 * ax), float(y - 0.65 * ay),
                                                      float(x - 0.35 * ax), float(y - 1.35 * ay),
                                                      float(x + 0.35 * ax), float(y - 1.35 * ay),
                                                      float(x + 0.35 * ax), float(y - 0.65 * ay)}));
    }
}

void StatView::updateColorPointsCbos()
{
    colorPointsCbosX.clear();

    QList< QPair<float, QColor> > colorPoints;

    if (comboBox->currentText() == modeEdgeLoad) {
        colorPoints = color->edgeColorOfPoints;
    } else if (comboBox->currentText() == modeRouteLength) {
        colorPoints = color->getRouteColorOfPointsForMetric("length");
    } else if (comboBox->currentText() == modeRouteByte) {
        colorPoints = color->getRouteColorOfPointsForMetric("byte");
    } else if (comboBox->currentText() == modeRouteHopbyte) {
        colorPoints = color->getRouteColorOfPointsForMetric("hopbyte");
    } else if (comboBox->currentText() == modeNodeInDeg) {
        colorPoints = color->vertexColorOfPoints;
    } else if (comboBox->currentText() == modeNodeOutDeg) {
        colorPoints = color->vertexColorOfPoints;
    }

    for(int i = 0; i < colorPoints.size(); ++i) {
        QPair<float, QColor> point = colorPoints.at(i);
        QColor qColor = point.second;
        colorPointsCbosX.push_back(vector<float>({(float)qColor.redF(), (float)qColor.greenF(), (float)qColor.blueF(), (float)qColor.alphaF()}));
    }
}

void StatView::drawColorPoints()
{
    glLineWidth(1.0 * this->devicePixelRatioF());
    glColor4d(0.6, 0.6, 0.6, 1.0);

    foreach(vector<float> vbo, colorPointsVbosX) {
        glBegin(GL_LINE_LOOP);
        glVertex2f(vbo.at(0), vbo.at(1));
        glVertex2f(vbo.at(2), vbo.at(3));
        glVertex2f(vbo.at(4), vbo.at(5));
        glVertex2f(vbo.at(6), vbo.at(7));
        glVertex2f(vbo.at(8), vbo.at(9));
        glEnd();
    }

    for (int i = 0; i < (int)colorPointsInsideVbosX.size(); ++i) {
        vector<float> vbo = colorPointsInsideVbosX.at(i);
        vector<float> cbo = colorPointsCbosX.at(i);
        glColor4d(cbo.at(0), cbo.at(1), cbo.at(2), cbo.at(3));
        glBegin(GL_TRIANGLE_FAN);
        glVertex2f(vbo.at(0), vbo.at(1));
        glVertex2f(vbo.at(2), vbo.at(3));
        glVertex2f(vbo.at(4), vbo.at(5));
        glVertex2f(vbo.at(6), vbo.at(7));
        glEnd();
    }
}

QPointF StatView::qtPosToGlPos(QPoint qtPos)
{
    float resultX = ((float)qtPos.x() / (float)this->width()) / scale - translateX;
    float resultY = (1.0 - (float)qtPos.y() / (float)this->height()) / scale - translateY;
    return QPointF(resultX, resultY);
}

bool StatView::isInsidePolygon(QPointF point, vector<GLfloat> selectionAreaGlBuffer)
{
    bool isInside = false;

    float x = point.x();
    float y = point.y();

    int i;
    int j = selectionAreaGlBuffer.size() - 2;
    for (i = 0; i < (int)selectionAreaGlBuffer.size(); i += 2) {
        if ((selectionAreaGlBuffer.at(i + 1) < y && selectionAreaGlBuffer.at(j + 1) >= y || selectionAreaGlBuffer.at(j + 1) < y && selectionAreaGlBuffer.at(i + 1) >= y)
             && (selectionAreaGlBuffer.at(i) <= x || selectionAreaGlBuffer.at(j) <= x))
        {
            isInside ^= (selectionAreaGlBuffer.at(i) + (y - selectionAreaGlBuffer.at(i + 1)) / (selectionAreaGlBuffer.at(j + 1) - selectionAreaGlBuffer.at(i + 1)) * (selectionAreaGlBuffer.at(j) - selectionAreaGlBuffer.at(i))) < x;
        }
        j = i;
    }

    return isInside;
}

int StatView::selectColorPoints()
{
    int result = -1;

    QPointF mousePressPosF = qtPosToGlPos(mousePressPos);

    for (int i = 0; i < (int)colorPointsVbosX.size(); ++i) {
        if (isInsidePolygon(mousePressPosF, colorPointsVbosX.at(i))) {
            result = i;
            break;
        }
    }

    return result;
}

void StatView::mousePressEvent(QMouseEvent *event)
{
    mousePressPos = event->pos();
    selectedYRange.clear();

    selectedColorIndex = selectColorPoints();
    update();
}

void StatView::mouseMoveEvent(QMouseEvent *event)
{
    QPoint mousePosDelta = event->pos() - mousePressPos;

    if(mouseMode == "selection") {
        float pressX = qtPosToGlPos(mousePressPos).x();
        float pressY = qtPosToGlPos(mousePressPos).y();
        float moveY = qtPosToGlPos(event->pos()).y();

        if(mousePosDelta.manhattanLength() > 5 &&
                pressX > chartMarginLeft && pressX < (1.0 - chartMarginRight) &&
                pressY > chartMarginBottom * 0.8 && pressY < (1.0 - chartMarginTop * 0.5)) {
            vector<GLfloat> yPositions;    
            yPositions.push_back( qMax(chartMarginBottom, qMin(pressY, moveY)) );
            yPositions.push_back( qMin((float)(1.0 - chartMarginTop), qMax(pressY, moveY)) );
            selectedYRange = yPositions;
        }
    }

    if (selectedColorIndex >= 0 && mousePosDelta.manhattanLength() > 5) {
        changeColorPointsPos(selectedColorIndex);
    }
    mouseMovePos = event->pos();

    update();
}

void StatView::mouseReleaseEvent(QMouseEvent *event)
{
    QPoint mousePosDelta = event->pos() - mousePressPos;

    if (event->button() == Qt::LeftButton) {
        if (selectedYRange.size() > 0) {
            showContextMenu(event->pos());
        } else if (mousePosDelta.manhattanLength() < 5) {
            if (selectedColorIndex >= 0) {
                selectColor(selectedColorIndex);
            }
        }
    } else if (event->button() == Qt::RightButton) {
        showContextMenu(event->pos());
    }

    mouseReleasePos = event->pos();
    selectedColorIndex = -1;

    update();
}

void StatView::showContextMenu(const QPoint &pos)
{
    QMenu contextMenu(tr("Context menu"), this);
    QAction actZoomInSelectedRange("Zoom in selected range", this);
    QAction actDisplaySelectedElementsInOtherViews("Display selected vertices in other views", this);
    QAction actResetDisplayedRange("Reset displayed range", this);

    if (comboBox->currentText() == modeEdgeLoad) {
        actDisplaySelectedElementsInOtherViews.setText("Display elements related with the selected links in other views");
    } else if (comboBox->currentText() == modeRouteLength ||
               comboBox->currentText() == modeRouteByte ||
               comboBox->currentText() == modeRouteHopbyte) {
        actDisplaySelectedElementsInOtherViews.setText("Display elements related with the selected routes in other views");
    }

    connect(&actZoomInSelectedRange, SIGNAL(triggered()), this, SLOT(zoomInSelectedRange()));
    connect(&actResetDisplayedRange, SIGNAL(triggered()), this, SLOT(resetDisplayedRange()));
    connect(&actDisplaySelectedElementsInOtherViews, SIGNAL(triggered()), this, SLOT(displaySelectedElementsInOtherViews()));

    if (selectedYRange.size() > 0) {
        contextMenu.addAction(&actZoomInSelectedRange);
        contextMenu.addAction(&actDisplaySelectedElementsInOtherViews);
    }
    if (displayedRankingStart != 0 && displayedRankingEnd != -1) {
        contextMenu.addAction(&actResetDisplayedRange);
    }

    contextMenu.exec(mapToGlobal(pos));
}

void StatView::zoomInSelectedRange()
{
    if (selectedYRange.size() >= 2) {
        double ratioEnd = (selectedYRange.at(0) - chartMarginBottom) / (1.0 - chartMarginTop - chartMarginBottom);
        double ratioStart = (selectedYRange.at(1) - chartMarginBottom) / (1.0 - chartMarginTop - chartMarginBottom);
        displayedRankingStart = ratioStart * displayedRankingStart + (1.0 - ratioStart) * displayedRankingEnd;
        displayedRankingEnd = ratioEnd * displayedRankingStart + (1.0 - ratioEnd) * displayedRankingEnd;
    }

    selectedYRange.clear();
    updateAllDrawingInfo();
    update();
}

void StatView::resetDisplayedRange()
{
    displayedRankingStart = 0;
    displayedRankingEnd = -1;

    selectedYRange.clear();
    updateAllDrawingInfo();
    update();
}

void StatView::displaySelectedElementsInOtherViews()
{
    QString elemType = "";
    QVector<int> selectedIndices;
    int numOfAllElements;

    if (comboBox->currentText() == modeEdgeLoad) {
        elemType = "edge";
        numOfAllElements = displayedGraph->getEdges().size();
    } else if (comboBox->currentText() == modeRouteLength ||
               comboBox->currentText() == modeRouteByte ||
               comboBox->currentText() == modeRouteHopbyte) {
        elemType = "route";
        numOfAllElements = displayedGraph->getRoutes().size();
    } else if (comboBox->currentText() == modeNodeInDeg ||
               comboBox->currentText() == modeNodeOutDeg ||
               comboBox->currentText() == modeNodeInMinusOutDeg) {
        elemType = "vertex";
        numOfAllElements = displayedGraph->getVertices().size();
    }

    int indicesStart = displayedRankingStart;
    int indicesEnd = displayedRankingEnd;

    if (selectedYRange.size() >= 2) {
        double ratioEnd = (selectedYRange.at(0) - chartMarginBottom) / (1.0 - chartMarginTop - chartMarginBottom);
        double ratioStart = (selectedYRange.at(1) - chartMarginBottom) / (1.0 - chartMarginTop - chartMarginBottom);
        int rankingStart = ratioStart * displayedRankingStart + (1.0 - ratioStart) * displayedRankingEnd;
        int rankingEnd = ratioEnd * displayedRankingStart + (1.0 - ratioEnd) * displayedRankingEnd;
        indicesStart = numOfAllElements - 1 - rankingEnd;
        indicesEnd = numOfAllElements - 1 - rankingStart;
    }

    if (elemType == "") {
        qWarning() << "selected combo box cannot handle in displaySelectedElementsInOtherViews";
        return;
    }

    if (elemType == "vertex") {
        for (int i = indicesStart; i <= indicesEnd; ++i) {
            selectedIndices.push_back(sortedVertices[i].getIndex());
            qSort(selectedIndices.begin(), selectedIndices.end());
        }
        emit displaySelectedElementsInOtherViewsCalled(elemType, selectedIndices);
    } else {
        if (elemType == "route") {

            // index in statview to route index
            for (int i = indicesStart; i <= indicesEnd; ++i) {
                selectedIndices.push_back(sortedRoutes[i].getIndex());
            }

            QVector<int> passedVertices;
            foreach(int rtIndex, selectedIndices) {
                QVector<int> arcs = displayedGraph->getRouteAt(rtIndex).getArcs();
                foreach(int a, arcs) {
                    passedVertices.push_back(a);
                }
            }
            passedVertices = passedVertices.toList().toSet().toList().toVector();

            emit displaySelectedElementsInOtherViewsCalled("vertex", passedVertices);
            emit displaySelectedElementsInOtherViewsCalled("route", selectedIndices); // added on June 21st

        } else if (elemType == "edge") {
            // edges have been sorted and reordred index
            for (int i = indicesStart; i <= indicesEnd; ++i) {
                selectedIndices.push_back(i);
            }

            QVector<int> routeIndicesIncludingDisplayedEdge;

            QVector<Route> routes = displayedGraph->getRoutes();
            QVector<Edge> edges = displayedGraph->getEdges();

            foreach(Route rt, routes) {
                bool includesDisplayedEdge = false;
                foreach(int eIndex, selectedIndices) {
                    if (rt.includesEdge(edges.at(eIndex))) {
                        includesDisplayedEdge = true;
                        break;
                    }
                }

                if (includesDisplayedEdge) routeIndicesIncludingDisplayedEdge.push_back(rt.getIndex());
            }

            QVector<int> passedVertices;
            foreach(int rtIndex, routeIndicesIncludingDisplayedEdge) {
                QVector<int> arcs = displayedGraph->getRouteAt(rtIndex).getArcs();
                foreach(int a, arcs) {
                    passedVertices.push_back(a);
                }
            }
            passedVertices = passedVertices.toList().toSet().toList().toVector();
            emit displaySelectedElementsInOtherViewsCalled("vertex", passedVertices);
            emit displaySelectedElementsInOtherViewsCalled("route", routeIndicesIncludingDisplayedEdge);
        }

        qSort(selectedIndices.begin(), selectedIndices.end(), qLess<int>());
        emit displaySelectedElementsInOtherViewsCalled(elemType, selectedIndices);
    }
}

void StatView::selectColor(int colorPointIndex)
{
    QPair<float, QColor> colorPoint;
    QColor currentColor;
    QColor selectedColor;
    if (comboBox->currentText() == modeEdgeLoad) {
        colorPoint = color->edgeColorOfPoints.at(colorPointIndex);
        currentColor = colorPoint.second;
        selectedColor = QColorDialog::getColor(currentColor, __null, QString(), QColorDialog::ShowAlphaChannel);
        colorPoint.second = selectedColor;
        color->edgeColorOfPoints[colorPointIndex] = colorPoint;
    } else if (comboBox->currentText() == modeRouteLength) {
        colorPoint = color->getRouteColorOfPointsForMetric("length").at(colorPointIndex);
        currentColor = colorPoint.second;
        selectedColor = QColorDialog::getColor(currentColor, __null, QString(), QColorDialog::ShowAlphaChannel);
        colorPoint.second = selectedColor;
        color->routeColorOfPoints1[colorPointIndex] = colorPoint;
    } else if (comboBox->currentText() == modeRouteByte) {
        colorPoint = color->getRouteColorOfPointsForMetric("byte").at(colorPointIndex);
        currentColor = colorPoint.second;
        selectedColor = QColorDialog::getColor(currentColor, __null, QString(), QColorDialog::ShowAlphaChannel);
        colorPoint.second = selectedColor;
        color->routeColorOfPoints2[colorPointIndex] = colorPoint;
    } else if (comboBox->currentText() == modeRouteHopbyte) {
        colorPoint = color->getRouteColorOfPointsForMetric("hopbyte").at(colorPointIndex);
        currentColor = colorPoint.second;
        selectedColor = QColorDialog::getColor(currentColor, __null, QString(), QColorDialog::ShowAlphaChannel);
        colorPoint.second = selectedColor;
        color->routeColorOfPoints3[colorPointIndex] = colorPoint;
    } else if (comboBox->currentText() == modeNodeInDeg) {
        colorPoint = color->vertexColorOfPoints.at(colorPointIndex);
        currentColor = colorPoint.second;
        selectedColor = QColorDialog::getColor(currentColor, __null, QString(), QColorDialog::ShowAlphaChannel);
        colorPoint.second = selectedColor;
        color->vertexColorOfPoints[colorPointIndex] = colorPoint;
    } else if (comboBox->currentText() == modeNodeOutDeg) {
        colorPoint = color->vertexColorOfPoints.at(colorPointIndex);
        currentColor = colorPoint.second;
        selectedColor = QColorDialog::getColor(currentColor, __null, QString(), QColorDialog::ShowAlphaChannel);
        colorPoint.second = selectedColor;
        color->vertexColorOfPoints[colorPointIndex] = colorPoint;
    }

    if (currentColor != selectedColor) {
        updateAllDrawingInfo();
        emit colorChanged();
    }
}

void StatView::changeColorPointsPos(int colorPointIndex)
{
    QPointF mouseMovePosF = qtPosToGlPos(mouseMovePos);
    float ratio = (mouseMovePosF.x() - chartMarginLeft) / ((1.0 - chartMarginRight) - chartMarginLeft);

    if (ratio > 1.0) ratio = 1.0;
    if (ratio < 0.0) ratio = 0.0;

    if (comboBox->currentText() == modeEdgeLoad) {
        color->edgeColorOfPoints[colorPointIndex].first = ratio;
        color->sortColorPoints(color->edgeColorOfPoints);
    } else if (comboBox->currentText() == modeRouteLength) {
        color->routeColorOfPoints1[colorPointIndex].first = ratio;
        color->sortColorPoints(color->routeColorOfPoints1);
    } else if (comboBox->currentText() == modeRouteByte) {
        color->routeColorOfPoints2[colorPointIndex].first = ratio;
        color->sortColorPoints(color->routeColorOfPoints2);
    } else if (comboBox->currentText() == modeRouteHopbyte) {
        color->routeColorOfPoints3[colorPointIndex].first = ratio;
        color->sortColorPoints(color->routeColorOfPoints3);
    } else if (comboBox->currentText() == modeNodeInDeg) {
        color->vertexColorOfPoints[colorPointIndex].first = ratio;
        color->sortColorPoints(color->vertexColorOfPoints);
    } else if (comboBox->currentText() == modeNodeOutDeg) {
        color->vertexColorOfPoints[colorPointIndex].first = ratio;
        color->sortColorPoints(color->vertexColorOfPoints);
    }

    updateAllDrawingInfo();
    emit colorChanged();
}

void StatView::updateDistriVbo()
{
    distriVbo.clear();
    float chartW = 1.0 - chartMarginLeft - chartMarginRight;
    float chartH = 1.0 - chartMarginTop - chartMarginBottom;

    if (comboBox->currentText() == modeEdgeLoad) {
        // Edges are sorted by ascending order with load when loaded from file
        QVector<Edge> edges = displayedGraph->getEdges();

        if (displayedRankingEnd == -1) displayedRankingEnd = edges.size() - 1;

        valMax = edges[edges.size() - 1 - displayedRankingStart].getByte();
        valMin = 0;
        zeroValAxisPos = chartMarginLeft;

        float unitH = chartH / (float)(displayedRankingEnd - displayedRankingStart + 1);

        float prevValRatio;

        int numElements = displayedRankingEnd - displayedRankingStart;
        if (numElements % 2 != 0) {
            int medianIndex = numElements / 2 + 1;
            median = edges[medianIndex].getByte();
        } else {
            int medianIndex1 = numElements / 2;
            int medianIndex2 = numElements / 2 + 1;
            median = (edges[medianIndex1].getByte() + edges[medianIndex2].getByte()) * 0.5;
        }
        medianAxisPos = chartMarginLeft + chartW * (median - valMin) / (valMax - valMin);

        for (int i = displayedRankingStart; i <= displayedRankingEnd; ++i) {
            int index = edges.size() - 1 - i;
            float valRatio = chartW * (edges[index].getByte() - valMin) / (float)(valMax - valMin);

            if (i == displayedRankingStart) {
                distriVbo.push_back(chartMarginLeft);
                distriVbo.push_back(1.0 - chartMarginTop - (i - displayedRankingStart) * unitH);
                distriVbo.push_back(chartMarginLeft + valRatio);
                distriVbo.push_back(1.0 - chartMarginTop - (i - displayedRankingStart) * unitH);
            } else if (valRatio < prevValRatio) {
                distriVbo.push_back(chartMarginLeft + prevValRatio);
                distriVbo.push_back(1.0 - chartMarginTop - (i - displayedRankingStart) * unitH);
                distriVbo.push_back(chartMarginLeft);
                distriVbo.push_back(1.0 - chartMarginTop - (i - displayedRankingStart) * unitH);

                distriVbo.push_back(chartMarginLeft);
                distriVbo.push_back(1.0 - chartMarginTop - (i - displayedRankingStart) * unitH);
                distriVbo.push_back(chartMarginLeft + valRatio);
                distriVbo.push_back(1.0 - chartMarginTop - (i - displayedRankingStart) * unitH);
            }

            if (i == displayedRankingEnd) {
                distriVbo.push_back(chartMarginLeft + valRatio);
                distriVbo.push_back(1.0 - chartMarginTop - (i + 1 - displayedRankingStart) * unitH);
                distriVbo.push_back(chartMarginLeft);
                distriVbo.push_back(1.0 - chartMarginTop - (i + 1 - displayedRankingStart) * unitH);
            }

            prevValRatio = valRatio;
        }
    } else if (comboBox->currentText() == modeRouteLength ||
               comboBox->currentText() == modeRouteByte ||
               comboBox->currentText() == modeRouteHopbyte) {
        // Routes are sorted by ascending order with length when loaded from file
        QString metric = "";
        if (comboBox->currentText() == modeRouteLength) metric = "length";
        else if (comboBox->currentText() == modeRouteByte) metric = "byte";
        else if (comboBox->currentText() == modeRouteHopbyte) metric = "hopbyte";

        sortedRoutes = displayedGraph->getRoutes();
        Graph::sortRoutesByMetric(sortedRoutes, metric);

        if (displayedRankingEnd == -1) displayedRankingEnd = sortedRoutes.size() - 1;
        valMax = sortedRoutes[sortedRoutes.size() - 1 - displayedRankingStart].getMetric(metric);
        valMin = 0.0;
        zeroValAxisPos = chartMarginLeft;

        float unitH = chartH / (float)(displayedRankingEnd - displayedRankingStart + 1);

        int numElements = displayedRankingEnd - displayedRankingStart;
        if (numElements % 2 != 0) {
            int medianIndex = numElements / 2 + 1;
            median = sortedRoutes[medianIndex].getMetric(metric);
        } else {
            int medianIndex1 = numElements / 2;
            int medianIndex2 = numElements / 2 + 1;
            median = (sortedRoutes[medianIndex1].getMetric(metric) + sortedRoutes[medianIndex2].getMetric(metric)) * 0.5;
        }
        medianAxisPos = chartMarginLeft + chartW * (median - valMin) / (float)(valMax - valMin);

        float prevValRatio;
        for (int i = displayedRankingStart; i <= displayedRankingEnd; ++i) {
            int index = sortedRoutes.size() - 1 - i;
            float valRatio = chartW * (sortedRoutes[index].getMetric(metric) - valMin) / (float)(valMax - valMin);

            if (i == displayedRankingStart) {
                distriVbo.push_back(chartMarginLeft);
                distriVbo.push_back(1.0 - chartMarginTop - (i - displayedRankingStart) * unitH);
                distriVbo.push_back(chartMarginLeft + valRatio);
                distriVbo.push_back(1.0 - chartMarginTop - (i - displayedRankingStart) * unitH);
            } else if (valRatio < prevValRatio) {
                distriVbo.push_back(chartMarginLeft + prevValRatio);
                distriVbo.push_back(1.0 - chartMarginTop - (i - displayedRankingStart) * unitH);
                distriVbo.push_back(chartMarginLeft);
                distriVbo.push_back(1.0 - chartMarginTop - (i - displayedRankingStart) * unitH);

                distriVbo.push_back(chartMarginLeft);
                distriVbo.push_back(1.0 - chartMarginTop - (i - displayedRankingStart) * unitH);
                distriVbo.push_back(chartMarginLeft + valRatio);
                distriVbo.push_back(1.0 - chartMarginTop - (i - displayedRankingStart) * unitH);
            }

            if (i == displayedRankingEnd) {
                distriVbo.push_back(chartMarginLeft + valRatio);
                distriVbo.push_back(1.0 - chartMarginTop - (i + 1 - displayedRankingStart) * unitH);
                distriVbo.push_back(chartMarginLeft);
                distriVbo.push_back(1.0 - chartMarginTop - (i + 1 - displayedRankingStart) * unitH);
            }

            prevValRatio = valRatio;
        }
    } else if (comboBox->currentText() == modeNodeInDeg) {
        sortedVertices = displayedGraph->genSortedVerticesByInDegOfEdgesLoad();

        if (displayedRankingEnd == -1) displayedRankingEnd = sortedVertices.size() - 1;
        valMax = sortedVertices[sortedVertices.size() - 1 - displayedRankingStart].getInDegOfEdgesLoad();
        valMin = 0;
        zeroValAxisPos = chartMarginLeft;

        float unitH = chartH / (float)(displayedRankingEnd - displayedRankingStart + 1);

        int numElements = displayedRankingEnd - displayedRankingStart;
        if (numElements % 2 != 0) {
            int medianIndex = numElements / 2 + 1;
            median = sortedVertices[medianIndex].getInDegOfEdgesLoad();
        } else {
            int medianIndex1 = numElements / 2;
            int medianIndex2 = numElements / 2 + 1;
            median = (sortedVertices[medianIndex1].getInDegOfEdgesLoad() + sortedVertices[medianIndex2].getInDegOfEdgesLoad()) * 0.5;
        }
        medianAxisPos = chartMarginLeft + chartW * (median - valMin) / (float)(valMax - valMin);

        float prevValRatio;

        for (int i = displayedRankingStart; i <= displayedRankingEnd; ++i) {
            int index = sortedVertices.size() - 1 - i;
            float valRatio = chartW * (sortedVertices[index].getInDegOfEdgesLoad() - valMin) / (float)(valMax - valMin);

            if (i == displayedRankingStart) {
                distriVbo.push_back(chartMarginLeft);
                distriVbo.push_back(1.0 - chartMarginTop - (i - displayedRankingStart) * unitH);
                distriVbo.push_back(chartMarginLeft + valRatio);
                distriVbo.push_back(1.0 - chartMarginTop - (i - displayedRankingStart) * unitH);
            } else if (valRatio < prevValRatio) {
                distriVbo.push_back(chartMarginLeft + prevValRatio);
                distriVbo.push_back(1.0 - chartMarginTop - (i - displayedRankingStart) * unitH);
                distriVbo.push_back(chartMarginLeft);
                distriVbo.push_back(1.0 - chartMarginTop - (i - displayedRankingStart) * unitH);

                distriVbo.push_back(chartMarginLeft);
                distriVbo.push_back(1.0 - chartMarginTop - (i - displayedRankingStart) * unitH);
                distriVbo.push_back(chartMarginLeft + valRatio);
                distriVbo.push_back(1.0 - chartMarginTop - (i - displayedRankingStart) * unitH);
            }

            if (i == displayedRankingEnd) {
                distriVbo.push_back(chartMarginLeft + valRatio);
                distriVbo.push_back(1.0 - chartMarginTop - (i + 1 - displayedRankingStart) * unitH);
                distriVbo.push_back(chartMarginLeft);
                distriVbo.push_back(1.0 - chartMarginTop - (i + 1 - displayedRankingStart) * unitH);
            }

            prevValRatio = valRatio;
        }
    } else if (comboBox->currentText() == modeNodeOutDeg) {
        sortedVertices = displayedGraph->genSortedVerticesByOutDegOfEdgesLoad();

        if (displayedRankingEnd == -1) displayedRankingEnd = sortedVertices.size() - 1;
        valMax = sortedVertices[sortedVertices.size() - 1 - displayedRankingStart].getOutDegOfEdgesLoad();
        valMin = 0;
        zeroValAxisPos = chartMarginLeft;

        float unitH = chartH / (float)(displayedRankingEnd - displayedRankingStart + 1);

        int numElements = displayedRankingEnd - displayedRankingStart;
        if (numElements % 2 != 0) {
            int medianIndex = numElements / 2 + 1;
            median = sortedVertices[medianIndex].getOutDegOfEdgesLoad();
        } else {
            int medianIndex1 = numElements / 2;
            int medianIndex2 = numElements / 2 + 1;
            median = (sortedVertices[medianIndex1].getOutDegOfEdgesLoad() + sortedVertices[medianIndex2].getOutDegOfEdgesLoad()) * 0.5;
        }
        medianAxisPos = chartMarginLeft + chartW * (median - valMin) / (float)(valMax - valMin);

        float prevValRatio;

        for (int i = displayedRankingStart; i <= displayedRankingEnd; ++i) {
            int index = sortedVertices.size() - 1 - i;
            float valRatio = chartW * (sortedVertices[index].getOutDegOfEdgesLoad() - valMin) / (float)(valMax - valMin);

            if (i == displayedRankingStart) {
                distriVbo.push_back(chartMarginLeft);
                distriVbo.push_back(1.0 - chartMarginTop - (i - displayedRankingStart) * unitH);
                distriVbo.push_back(chartMarginLeft + valRatio);
                distriVbo.push_back(1.0 - chartMarginTop - (i - displayedRankingStart) * unitH);
            } else if (valRatio < prevValRatio) {
                distriVbo.push_back(chartMarginLeft + prevValRatio);
                distriVbo.push_back(1.0 - chartMarginTop - (i - displayedRankingStart) * unitH);
                distriVbo.push_back(chartMarginLeft);
                distriVbo.push_back(1.0 - chartMarginTop - (i - displayedRankingStart) * unitH);

                distriVbo.push_back(chartMarginLeft);
                distriVbo.push_back(1.0 - chartMarginTop - (i - displayedRankingStart) * unitH);
                distriVbo.push_back(chartMarginLeft + valRatio);
                distriVbo.push_back(1.0 - chartMarginTop - (i - displayedRankingStart) * unitH);
            }

            if (i == displayedRankingEnd) {
                distriVbo.push_back(chartMarginLeft + valRatio);
                distriVbo.push_back(1.0 - chartMarginTop - (i + 1 - displayedRankingStart) * unitH);
                distriVbo.push_back(chartMarginLeft);
                distriVbo.push_back(1.0 - chartMarginTop - (i + 1 - displayedRankingStart) * unitH);
            }

            prevValRatio = valRatio;
        }
    } else if (comboBox->currentText() == modeNodeInMinusOutDeg) {
        sortedVertices = displayedGraph->genSortedVerticesByInMinusOutDegOfEdgesLoad();

        if (displayedRankingEnd == -1) displayedRankingEnd = sortedVertices.size() - 1;
        valMax = sortedVertices[sortedVertices.size() - 1 - displayedRankingStart].getInMinusOutDegOfEdgesLoad();
        valMin = sortedVertices[sortedVertices.size() - 1 - displayedRankingEnd].getInMinusOutDegOfEdgesLoad();;

        float maxValRatio = chartW * sortedVertices[sortedVertices.size() - 1 - displayedRankingStart].getInMinusOutDegOfEdgesLoad() / (float)valMax;
        float minValRatio = chartW * sortedVertices[sortedVertices.size() - 1 - displayedRankingEnd].getInMinusOutDegOfEdgesLoad() / (float)valMax;
        float zeroValRatio = 0.0;
        if (maxValRatio - minValRatio > 0) {
            zeroValRatio = (0.0 - minValRatio) / (maxValRatio - minValRatio);
        }
        zeroValAxisPos = chartMarginLeft + zeroValRatio * chartW;

        float adjustSizeRatio = 0.0;
        if (maxValRatio - minValRatio > 0) {
            adjustSizeRatio = chartW / (maxValRatio - minValRatio);
        }

        float unitH = chartH / (float)(displayedRankingEnd - displayedRankingStart + 1);

        int numElements = displayedRankingEnd - displayedRankingStart;
        if (numElements % 2 != 0) {
            int medianIndex = numElements / 2 + 1;
            median = sortedVertices[medianIndex].getInMinusOutDegOfEdgesLoad();
        } else {
            int medianIndex1 = numElements / 2;
            int medianIndex2 = numElements / 2 + 1;
            median = (sortedVertices[medianIndex1].getInMinusOutDegOfEdgesLoad() + sortedVertices[medianIndex2].getInMinusOutDegOfEdgesLoad()) * 0.5;
        }

        medianAxisPos = chartMarginLeft + zeroValRatio * chartW + adjustSizeRatio * chartW * median / (float)(valMax);

        float prevValRatio;

        for (int i = displayedRankingStart; i <= displayedRankingEnd; ++i) {
            int index = sortedVertices.size() - 1 - i;
            float valRatio = chartW * sortedVertices[index].getInMinusOutDegOfEdgesLoad() / (float)valMax;

            if (i == displayedRankingStart) {
                distriVbo.push_back(chartMarginLeft + zeroValRatio * chartW);
                distriVbo.push_back(1.0 - chartMarginTop - (i - displayedRankingStart) * unitH);
                distriVbo.push_back(chartMarginLeft + zeroValRatio * chartW + adjustSizeRatio * valRatio);
                distriVbo.push_back(1.0 - chartMarginTop - (i - displayedRankingStart) * unitH);
            } else if (valRatio < prevValRatio) {
                distriVbo.push_back(chartMarginLeft + zeroValRatio * chartW + adjustSizeRatio * prevValRatio);
                distriVbo.push_back(1.0 - chartMarginTop - (i - displayedRankingStart) * unitH);
                distriVbo.push_back(chartMarginLeft + zeroValRatio * chartW);
                distriVbo.push_back(1.0 - chartMarginTop - (i - displayedRankingStart) * unitH);

                distriVbo.push_back(chartMarginLeft + zeroValRatio * chartW);
                distriVbo.push_back(1.0 - chartMarginTop - (i - displayedRankingStart) * unitH);
                distriVbo.push_back(chartMarginLeft + zeroValRatio * chartW + adjustSizeRatio * valRatio);
                distriVbo.push_back(1.0 - chartMarginTop - (i - displayedRankingStart) * unitH);
            }

            if (i == displayedRankingEnd) {
                distriVbo.push_back(chartMarginLeft + zeroValRatio * chartW + adjustSizeRatio * valRatio);
                distriVbo.push_back(1.0 - chartMarginTop - (i + 1 - displayedRankingStart) * unitH);
                distriVbo.push_back(chartMarginLeft + zeroValRatio * chartW);
                distriVbo.push_back(1.0 - chartMarginTop - (i + 1 - displayedRankingStart) * unitH);
            }

            prevValRatio = valRatio;
        }
    }
}

void StatView::updateDistriCbo()
{
    distriCbo.clear();

    if (comboBox->currentText() == modeEdgeLoad) {
        // Edges are sorted by ascending order with load when loaded from file
        QVector<Edge> edges = displayedGraph->getEdges();

        if (displayedRankingEnd == -1) displayedRankingEnd = edges.size() - 1;

        valMax = edges[edges.size() - 1].getByte(); // max value for color map is fixed val
        valMin = 0;

        float prevValRatio;

        for (int i = displayedRankingStart; i <= displayedRankingEnd; ++i) {
            int index = edges.size() - 1 - i;
            float valRatio = (edges[index].getByte() - valMin) / (float)(valMax - valMin);

            if (i == displayedRankingStart) {
                QColor qColor = color->ratioToHeatMapColor(valRatio, color->edgeColorOfPoints);
                distriCbo.push_back(qColor.redF());
                distriCbo.push_back(qColor.greenF());
                distriCbo.push_back(qColor.blueF());
                distriCbo.push_back(qColor.alphaF());
            } else if (valRatio < prevValRatio) {
                QColor qColor = color->ratioToHeatMapColor(valRatio, color->edgeColorOfPoints);
                distriCbo.push_back(qColor.redF());
                distriCbo.push_back(qColor.greenF());
                distriCbo.push_back(qColor.blueF());
                distriCbo.push_back(qColor.alphaF());
            }

            if (i == displayedRankingEnd) {
                QColor qColor = color->ratioToHeatMapColor(valRatio, color->edgeColorOfPoints);
                distriCbo.push_back(qColor.redF());
                distriCbo.push_back(qColor.greenF());
                distriCbo.push_back(qColor.blueF());
                distriCbo.push_back(qColor.alphaF());
            }

            prevValRatio = valRatio;
        }
    } else if (comboBox->currentText() == modeRouteLength ||
               comboBox->currentText() == modeRouteByte ||
               comboBox->currentText() == modeRouteHopbyte) {

        QString metric = "";
        if (comboBox->currentText() == modeRouteLength) metric = "length";
        else if (comboBox->currentText() == modeRouteByte) metric = "byte";
        else if (comboBox->currentText() == modeRouteHopbyte) metric = "hopbyte";

        if (displayedRankingEnd == -1) displayedRankingEnd = sortedRoutes.size() - 1;

        valMax = sortedRoutes[sortedRoutes.size() - 1].getMetric(metric); // max value for color map is fixed val
        valMin = 0;

        float prevValRatio;

        for (int i = displayedRankingStart; i <= displayedRankingEnd; ++i) {
            int index = sortedRoutes.size() - 1 - i;
            float valRatio = (sortedRoutes[index].getMetric(metric) - valMin) / (float)(valMax - valMin);

            if (i == displayedRankingStart) {
                QColor qColor = color->ratioToHeatMapColor(valRatio, color->getRouteColorOfPointsForMetric(metric));
                distriCbo.push_back(qColor.redF());
                distriCbo.push_back(qColor.greenF());
                distriCbo.push_back(qColor.blueF());
                distriCbo.push_back(qColor.alphaF());
            } else if (valRatio < prevValRatio) {
                QColor qColor = color->ratioToHeatMapColor(valRatio, color->getRouteColorOfPointsForMetric(metric));
                distriCbo.push_back(qColor.redF());
                distriCbo.push_back(qColor.greenF());
                distriCbo.push_back(qColor.blueF());
                distriCbo.push_back(qColor.alphaF());
            }

            if (i == displayedRankingEnd) {
                QColor qColor = color->ratioToHeatMapColor(valRatio, color->getRouteColorOfPointsForMetric(metric));
                distriCbo.push_back(qColor.redF());
                distriCbo.push_back(qColor.greenF());
                distriCbo.push_back(qColor.blueF());
                distriCbo.push_back(qColor.alphaF());
            }

            prevValRatio = valRatio;
        }
    } else if (comboBox->currentText() == modeNodeInDeg) {
        sortedVertices = displayedGraph->genSortedVerticesByInDegOfEdgesLoad();

        if (displayedRankingEnd == -1) displayedRankingEnd = sortedVertices.size() - 1;

        valMax = sortedVertices[sortedVertices.size() - 1].getInDegOfEdgesLoad(); // max value for color map is fixed val
        valMin = 0;

        float prevValRatio;

        for (int i = displayedRankingStart; i <= displayedRankingEnd; ++i) {
            int index = sortedVertices.size() - 1 - i;
            float valRatio = (sortedVertices[index].getInDegOfEdgesLoad() - valMin) / (float)(valMax - valMin);

            if (i == displayedRankingStart) {
                QColor qColor = color->ratioToHeatMapColor(valRatio, color->vertexColorOfPoints);
                distriCbo.push_back(qColor.redF());
                distriCbo.push_back(qColor.greenF());
                distriCbo.push_back(qColor.blueF());
                distriCbo.push_back(qColor.alphaF());
            } else if (valRatio < prevValRatio) {
                QColor qColor = color->ratioToHeatMapColor(valRatio, color->vertexColorOfPoints);
                distriCbo.push_back(qColor.redF());
                distriCbo.push_back(qColor.greenF());
                distriCbo.push_back(qColor.blueF());
                distriCbo.push_back(qColor.alphaF());
            }

            if (i == displayedRankingEnd) {
                QColor qColor = color->ratioToHeatMapColor(valRatio, color->vertexColorOfPoints);
                distriCbo.push_back(qColor.redF());
                distriCbo.push_back(qColor.greenF());
                distriCbo.push_back(qColor.blueF());
                distriCbo.push_back(qColor.alphaF());
            }

            prevValRatio = valRatio;
        }
    } else if (comboBox->currentText() == modeNodeOutDeg) {
        sortedVertices = displayedGraph->genSortedVerticesByOutDegOfEdgesLoad();

        if (displayedRankingEnd == -1) displayedRankingEnd = sortedVertices.size() - 1;

        valMax = sortedVertices[sortedVertices.size() - 1].getOutDegOfEdgesLoad(); // max value for color map is fixed val
        valMin = 0;

        float prevValRatio;

        for (int i = displayedRankingStart; i <= displayedRankingEnd; ++i) {
            int index = sortedVertices.size() - 1 - i;
            float valRatio = (sortedVertices[index].getOutDegOfEdgesLoad() - valMin) / (float)(valMax - valMin);

            if (i == displayedRankingStart) {
                QColor qColor = color->ratioToHeatMapColor(valRatio, color->vertexColorOfPoints);
                distriCbo.push_back(qColor.redF());
                distriCbo.push_back(qColor.greenF());
                distriCbo.push_back(qColor.blueF());
                distriCbo.push_back(qColor.alphaF());
            } else if (valRatio < prevValRatio) {
                QColor qColor = color->ratioToHeatMapColor(valRatio, color->vertexColorOfPoints);
                distriCbo.push_back(qColor.redF());
                distriCbo.push_back(qColor.greenF());
                distriCbo.push_back(qColor.blueF());
                distriCbo.push_back(qColor.alphaF());
            }

            if (i == displayedRankingEnd) {
                QColor qColor = color->ratioToHeatMapColor(valRatio, color->vertexColorOfPoints);
                distriCbo.push_back(qColor.redF());
                distriCbo.push_back(qColor.greenF());
                distriCbo.push_back(qColor.blueF());
                distriCbo.push_back(qColor.alphaF());
            }

            prevValRatio = valRatio;
        }
    } else if (comboBox->currentText() == modeNodeInMinusOutDeg) {
        sortedVertices = displayedGraph->genSortedVerticesByInMinusOutDegOfEdgesLoad();

        if (displayedRankingEnd == -1) displayedRankingEnd = sortedVertices.size() - 1;

        valMax = sortedVertices[sortedVertices.size() - 1].getInMinusOutDegOfEdgesLoad(); // max value for color map is fixed val
        valMin = 0;

        float prevValRatio;

        for (int i = displayedRankingStart; i <= displayedRankingEnd; ++i) {
            int index = sortedVertices.size() - 1 - i;
            float valRatio = (sortedVertices[index].getInMinusOutDegOfEdgesLoad() - valMin) / (float)(valMax - valMin);

            if (i == displayedRankingStart) {
                QColor qColor;
                if (valRatio >= 0) qColor = color->ratioToHeatMapColor(valRatio, color->vertexColorOfPoints);
                else qColor = color->ratioToHeatMapColor(-valRatio, color->vertexColorOfPoints);

                distriCbo.push_back(qColor.redF());
                distriCbo.push_back(qColor.greenF());
                distriCbo.push_back(qColor.blueF());
                distriCbo.push_back(qColor.alphaF());
            } else if (valRatio < prevValRatio) {
                QColor qColor;
                if (valRatio >= 0) qColor = color->ratioToHeatMapColor(valRatio, color->vertexColorOfPoints);
                else qColor = color->ratioToHeatMapColor(-valRatio, color->vertexColorOfPoints);
                distriCbo.push_back(qColor.redF());
                distriCbo.push_back(qColor.greenF());
                distriCbo.push_back(qColor.blueF());
                distriCbo.push_back(qColor.alphaF());
            }

            if (i == displayedRankingEnd) {
                QColor qColor;
                if (valRatio >= 0) qColor = color->ratioToHeatMapColor(valRatio, color->vertexColorOfPoints);
                else qColor = color->ratioToHeatMapColor(-valRatio, color->vertexColorOfPoints);
                distriCbo.push_back(qColor.redF());
                distriCbo.push_back(qColor.greenF());
                distriCbo.push_back(qColor.blueF());
                distriCbo.push_back(qColor.alphaF());
            }

            prevValRatio = valRatio;
        }
    }
}

void StatView::updateDistriLabels()
{
    distriLabels.clear();

    if (comboBox->currentText() == modeEdgeLoad) {
        // Edges are sorted by ascending order with load when loaded from file
        QVector<Edge> edges = displayedGraph->getEdges();

        if (displayedRankingEnd == -1) displayedRankingEnd = edges.size() - 1;

        valMax = edges[edges.size() - 1 - displayedRankingStart].getByte();
        valMin = 0;

        float prevValRatio;
        int count = 0;
        int total = 0;
        for (int i = displayedRankingStart; i <= displayedRankingEnd; ++i) {
            int index = edges.size() - 1 - i;
            float valRatio = (edges[index].getByte() - valMin) / (valMax - valMin);

            if (i == 0) {

            } else if (valRatio < prevValRatio) {
                distriLabels.push_back(QString::number(total - count + 1 + displayedRankingStart));
                count = 0;
            }

            if (i == displayedRankingEnd) {
                distriLabels.push_back(QString::number(total - count + 1 + displayedRankingStart));
                distriLabels.push_back(QString::number(total + 1));
            }

            prevValRatio = valRatio;
            count++;
            total++;
        }
    } else if (comboBox->currentText() == modeRouteLength ||
               comboBox->currentText() == modeRouteByte ||
               comboBox->currentText() == modeRouteHopbyte) {

        QString metric = "";
        if (comboBox->currentText() == modeRouteLength) metric = "length";
        else if (comboBox->currentText() == modeRouteByte) metric = "byte";
        else if (comboBox->currentText() == modeRouteHopbyte) metric = "hopbyte";

        if (displayedRankingEnd == -1) displayedRankingEnd = sortedRoutes.size() - 1;

        valMax = sortedRoutes[sortedRoutes.size() - 1 - displayedRankingStart].getMetric(metric);
        valMin = 0;

        float prevValRatio;
        int count = 0;
        int total = 0;
        for (int i = displayedRankingStart; i <= displayedRankingEnd; ++i) {
            int index = sortedRoutes.size() - 1 - i;
            float valRatio = (sortedRoutes[index].getMetric(metric) - valMin) / (float)(valMax - valMin);

            if (i == 0) {

            } else if (valRatio < prevValRatio) {
                distriLabels.push_back(QString::number(total - count + 1));
                count = 0;
            }

            if (i == displayedRankingEnd) {
                distriLabels.push_back(QString::number(total - count + 1));
                distriLabels.push_back(QString::number(total + 1));
            }

            prevValRatio = valRatio;
            count++;
            total++;
        }
    } else if (comboBox->currentText() == modeNodeInDeg) {
        sortedVertices = displayedGraph->genSortedVerticesByInDegOfEdgesLoad();

        if (displayedRankingEnd == -1) displayedRankingEnd = sortedVertices.size() - 1;

        valMax = sortedVertices[sortedVertices.size() - 1 - displayedRankingStart].getInDegOfEdgesLoad();
        valMin = 0;

        float prevValRatio;
        int count = 0;
        int total = 0;
        for (int i = displayedRankingStart; i <= displayedRankingEnd; ++i) {
            int index = sortedVertices.size() - 1 - i;
            float valRatio = (sortedVertices[index].getInDegOfEdgesLoad() - valMin) / (float)(valMax - valMin);

            if (i == 0) {

            } else if (valRatio < prevValRatio) {
                distriLabels.push_back(QString::number(total - count + 1));
                count = 0;
            }

            if (i == displayedRankingEnd) {
                distriLabels.push_back(QString::number(total - count + 1));
                distriLabels.push_back(QString::number(total + 1));
            }

            prevValRatio = valRatio;
            count++;
            total++;
        }
    } else if (comboBox->currentText() == modeNodeOutDeg) {
        sortedVertices = displayedGraph->genSortedVerticesByOutDegOfEdgesLoad();

        if (displayedRankingEnd == -1) displayedRankingEnd = sortedVertices.size() - 1;

        valMax = sortedVertices[sortedVertices.size() - 1 - displayedRankingStart].getOutDegOfEdgesLoad();
        valMin = 0;

        float prevValRatio;
        int count = 0;
        int total = 0;
        for (int i = displayedRankingStart; i <= displayedRankingEnd; ++i) {
            int index = sortedVertices.size() - 1 - i;
            float valRatio = (sortedVertices[index].getOutDegOfEdgesLoad() - valMin) / (float)(valMax - valMin);

            if (i == 0) {

            } else if (valRatio < prevValRatio) {
                distriLabels.push_back(QString::number(total - count + 1));
                count = 0;
            }

            if (i == displayedRankingEnd) {
                distriLabels.push_back(QString::number(total - count + 1));
                distriLabels.push_back(QString::number(total + 1));
            }

            prevValRatio = valRatio;
            count++;
            total++;
        }
    } else if (comboBox->currentText() == modeNodeInMinusOutDeg) {
        sortedVertices = displayedGraph->genSortedVerticesByInMinusOutDegOfEdgesLoad();

        if (displayedRankingEnd == -1) displayedRankingEnd = sortedVertices.size() - 1;

        valMax = sortedVertices[sortedVertices.size() - 1 - displayedRankingStart].getInMinusOutDegOfEdgesLoad();
        valMin = sortedVertices[sortedVertices.size() - 1 - displayedRankingEnd].getInMinusOutDegOfEdgesLoad();;

        float prevValRatio;
        int count = 0;
        int total = 0;
        for (int i = displayedRankingStart; i <= displayedRankingEnd; ++i) {
            int index = sortedVertices.size() - 1 - i;
            float valRatio = sortedVertices[index].getInMinusOutDegOfEdgesLoad() / (float)valMax;

            if (i == 0) {

            } else if (valRatio < prevValRatio) {
                distriLabels.push_back(QString::number(total - count + 1));
                count = 0;
            }

            if (i == displayedRankingEnd) {
                distriLabels.push_back(QString::number(total - count + 1));
                distriLabels.push_back(QString::number(total + 1));
            }

            prevValRatio = valRatio;
            count++;
            total++;
        }
    }
}

void StatView::updateAllDrawingInfo()
{
    updateDistriVbo();
    updateDistriCbo();
    updateDistriLabels();
    updateColorPointsVbos();
    updateColorPointsCbos();
}

void StatView::updateAllDrawingInfoFromComboBox(QString comboBoxText)
{
    updateAllDrawingInfo();
    update();
}

void StatView::resetDisplayedRankingStartEnd()
{
    displayedRankingStart = 0;
    displayedRankingEnd = -1;
}

void StatView::resetDisplayedRankingStartEndFromComboBox(QString comboBoxText)
{
    displayedRankingStart = 0;
    displayedRankingEnd = -1;
}

void StatView::setColor(Color *color) { this->color = color; }
void StatView::setGraph(Graph *graph) { this->graph = graph; }
void StatView::setAggregatedGraph(Graph* aggregatedGraph) { this->aggregatedGraph = aggregatedGraph; }
void StatView::setDisplayedGraph(Graph* graph) { this->displayedGraph = graph; }
void StatView::switchGraph()
{
    if (displayedGraph == graph) {
        displayedGraph = aggregatedGraph;
    } else {
        displayedGraph = graph;
    }

    displayedRankingStart = 0;
    displayedRankingEnd = -1;

    updateAllDrawingInfo();

    update();
}

void StatView::setCurrentComboBoxIndex(int index)
{
    comboBox->setCurrentIndex(index);
}

void StatView::emitCurrentComboBoxIndexChanged(int index)
{
    emit currentComboBoxIndexChanged(index);
}

void StatView::changeRouteColorMapSettings(QString metric, float min, float val)
{

}
