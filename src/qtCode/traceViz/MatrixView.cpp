#include "MatrixView.h"

// TODO: convert a way for rendering to using shader

MatrixView::MatrixView(QWidget *parent)
{
    edgeColorMetric = "byte";
    routeColorMetric = "byte";

    unitBlockSize = 1;
    unitBlockSizeForLens = 1;
    unitFoldingSize = 1;
    thresAdjustingResolution = 0.8 * 0.5 * this->width() / this->devicePixelRatio();
    isResolutionReductionTurnedOn = true;

    scale = 1.0;
    translateX = 0.0;
    translateY = 0.0;
    mouseMode = "Rect"; // "Rect", "Lens"
    leftButtonPressed = false;
    rightButtonPressed = false;
    lensSize = 0.3; // 0.26 // 0.3
    lensZoomLevel = 8; //8
    edgeDrawingMode = "none"; // "none", "straight"
    routeDrawingMode = "straight"; // "none", "straight", "catmull-rom"

    routingGuideType = "src-dest-fixed"; // "src-dest-fixed" or "src-fixed"
    routingGuidePriority = "lengthFirst"; // "lengthFirst" or "loadFirst"
    mappingGuideType = "all-ma"; // "all-ma", "selected-optimal", "selected-randomized"
    routingGuideLengthTolerance = 0;
    routingGuideLoadTolerance = 0.0;

    totalLengthWillBeChanged = 0;
    totalMaxEdgeLoadWillBeChanged = 0;
    autoUpdate = false;

    rscriptPath = "/usr/local/bin/Rscript";
    saQapPath = "";

    // in general
//    routeWidth = 3.0;
//    edgeWidth = 4.0;
//    routeWidth = 3.0;
//    edgeWidth = 6.0; // 6.0 // 4.5

    routeWidth = 3.0;
    edgeWidth = 4.0;
}

void MatrixView::initializeGL()
{
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_POLYGON_SMOOTH);
    glAlphaFunc(GL_GREATER, 0.02);
}

void MatrixView::resizeGL(int width, int height)
{
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, 1.0, 0.0, 1.0, -1.0, 1.0);

    this->update();
}

void MatrixView::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA , GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_ALPHA_TEST);

    glEnable(GL_LINE_SMOOTH);

    glPushMatrix();
    {
        glTranslatef(0.5, 0.5, 0.0);
        glScalef(scale, scale, scale);
        glTranslatef(-0.5, -0.5, 0.0);
        glTranslatef(translateX, translateY, 0.0);
        drawFoldedRegions(foldedRegionsVbos);
        drawAdjMat(adjMatVbo);
        drawEdges(edgesVbos, edgesCbo, edgeWidth);
        drawEdges(suggestedEdgesVbos, suggestedEdgesCbo, edgeWidth);
        drawRoutes(routesArcVbos, routesCurveVbos, routesCbo, routesSrcVbos, routesDestVbos, "displayed", routeWidth);
        drawRoutes(suggestedRoutesArcVbos, suggestedRoutesCurveVbos, suggestedRoutesCbo, suggestedRoutesSrcVbos, suggestedRoutesDestVbos, "suggested", routeWidth);
        drawSelectedArea();
        drawLensOutLine();
    }
    glPopMatrix();

    glPushMatrix();
    {
        drawColorMaps();
        drawColorSymbols();
    }
    glPopMatrix();

    glPushMatrix();
    {
        // TODO: why devicePixelRatio is needed? some weird thing is happend before here
        glViewport(this->devicePixelRatio() * mouseMovePos.x() - this->devicePixelRatio() * lensSize * width() / 2,
                   this->devicePixelRatio() * height() - this->devicePixelRatio() * mouseMovePos.y() - this->devicePixelRatio() * lensSize * height() / 2,
                   this->devicePixelRatio() * lensSize * width(),
                   this->devicePixelRatio() * lensSize * height());
        // TODO: displayed area is little different, especially corner areas
        QPointF mouseMovePosF = qtPosToGlPos(mouseMovePos);
        glTranslatef(mouseMovePosF.x(), mouseMovePosF.y(), 0.0);
        glScalef(scale * (float)lensZoomLevel / lensSize,
                 scale * (float)lensZoomLevel / lensSize,
                 scale * (float)lensZoomLevel / lensSize);
        glTranslatef(-mouseMovePosF.x(), -mouseMovePosF.y(), 0.0);
        glTranslatef(translateX, translateY, 0.0);
        drawLensContents();
    }
    glPopMatrix();

    paintLabels();

    if (autoUpdate) {
        if (suggestedRoutes.size() > 0) {
            if (totalLengthWillBeChanged == 0 && totalMaxEdgeLoadWillBeChanged == 0) turnOffAutoUpdate();
            applyAltRoutes();
        } else {
            suggestAltRoutesForDisplayedRoutes();
        }

    }
}

void MatrixView::drawFoldedRegions(vector<vector<GLfloat> > &foldedRegionsVbos)
{
    // background
    QColor col = color->foldedRegionBackgroundColor;
    glColor4f(col.redF(), col.greenF(), col.blueF(), col.alphaF());
    foreach(vector<GLfloat> vbo, foldedRegionsVbos) {
        glVertexPointer(2, GL_FLOAT, 0, &vbo[0]);
        glEnableClientState(GL_VERTEX_ARRAY);
        glDrawArrays(GL_TRIANGLE_FAN, 0, vbo.size() / 2);
        glDisableClientState(GL_VERTEX_ARRAY);
    }

    // outline
    col = color->foldedRegionStrokeColor;
    glColor4f(col.redF(), col.greenF(), col.blueF(), col.alphaF());
    glLineWidth(0.5 * this->devicePixelRatioF());
    foreach(vector<GLfloat> vbo, foldedRegionsVbos) {
        glVertexPointer(2, GL_FLOAT, 0, &vbo[0]);
        glEnableClientState(GL_VERTEX_ARRAY);
        glDrawArrays(GL_LINE_LOOP, 0, vbo.size() / 2);
        glDisableClientState(GL_VERTEX_ARRAY);
    }
}

void MatrixView::drawAdjMat(vector<GLfloat> &adjMatVbo)
{
    QColor col = color->adjMatrixColor;
    glColor4f(col.redF(), col.greenF(), col.blueF(), col.alphaF());
    glVertexPointer(2, GL_FLOAT, 0, &adjMatVbo[0]);
    glEnableClientState(GL_VERTEX_ARRAY);
    glDrawArrays(GL_TRIANGLES, 0, adjMatVbo.size() / 2);
    glDisableClientState(GL_VERTEX_ARRAY);
}

void MatrixView::drawEdges(vector< vector<GLfloat> > &edgesVbos, vector<GLfloat> &edgesCbo, float width)
{
    if (edgeDrawingMode == "straight") {
        glLineWidth(20.0 * this->devicePixelRatioF());
        float w2 = 0.003 * width * 0.5 / scale;

        for (int i = 0; i < (int)edgesVbos.size(); ++i) {
            bool isAllVerticesDisplayed = true;
            for (int j = 0; j < (int)edgesVbos.at(i).size() / 2; ++j) {
                if (edgesVbos.at(i).at(j * 2) < -1.0) {
                    isAllVerticesDisplayed = false;
                    break;
                }
            }
            if (isAllVerticesDisplayed) {
                glColor4f(edgesCbo.at(i * 4), edgesCbo.at(i * 4 + 1), edgesCbo.at(i * 4 + 2), edgesCbo.at(i * 4 + 3));                
                //                glVertexPointer(2, GL_FLOAT, 0, &edgesVbos[i][0]);
                //                glEnableClientState(GL_VERTEX_ARRAY);
                //                glDrawArrays(GL_LINE_STRIP, 0, edgesVbos[i].size() / 2);
                //                glDisableClientState(GL_VERTEX_ARRAY);
                vector<GLfloat> vbo = edgesVbos.at(i);

                glBegin(GL_TRIANGLE_FAN);
                glVertex2f(vbo.at(2) + w2, vbo.at(3) + w2);
                glVertex2f(vbo.at(0) - w2, vbo.at(1) + w2);
                glVertex2f(vbo.at(0) - w2, vbo.at(1) - w2);
                glVertex2f(vbo.at(2) - w2, vbo.at(3) - w2);
                glVertex2f(vbo.at(4) - w2, vbo.at(5) - w2);
                glVertex2f(vbo.at(4) + w2, vbo.at(5) - w2);
                glEnd();
            }
        }
    }
}

void MatrixView::drawEdgesForLens(vector< vector<GLfloat> > &edgesVbos, vector<GLfloat> &edgesCbo, float width)
{
    if (edgeDrawingMode == "straight") {
        glLineWidth(20.0 * this->devicePixelRatioF());
        float w2 = 0.003 * width * 0.5 / scale;

        for (int i = 0; i < (int)edgesVbos.size(); ++i) {
            bool isAllVerticesDisplayed = true;
            for (int j = 0; j < (int)edgesVbos.at(i).size() / 2; ++j) {
                if (edgesVbos.at(i).at(j * 2) < -1.0) {
                    isAllVerticesDisplayed = false;
                    break;
                }
            }
            if (isAllVerticesDisplayed) {

                vector<GLfloat> vbo = edgesVbos.at(i);

                if (vbo.at(0) >= lensTargetXRange.at(0) && vbo.at(0) <= lensTargetXRange.at(1) &&
                        vbo.at(1) >= lensTargetYRange.at(0) && vbo.at(1) <= lensTargetYRange.at(1) &&
                    vbo.at(4) >= lensTargetXRange.at(0) && vbo.at(4) <= lensTargetXRange.at(1) &&
                        vbo.at(5) >= lensTargetYRange.at(0) && vbo.at(5) <= lensTargetYRange.at(1)) {

                    glColor4f(edgesCbo.at(i * 4), edgesCbo.at(i * 4 + 1), edgesCbo.at(i * 4 + 2), edgesCbo.at(i * 4 + 3));
                    glBegin(GL_TRIANGLE_FAN);
                    glVertex2f(vbo.at(2) + w2, vbo.at(3) + w2);
                    glVertex2f(vbo.at(0) - w2, vbo.at(1) + w2);
                    glVertex2f(vbo.at(0) - w2, vbo.at(1) - w2);
                    glVertex2f(vbo.at(2) - w2, vbo.at(3) - w2);
                    glVertex2f(vbo.at(4) - w2, vbo.at(5) - w2);
                    glVertex2f(vbo.at(4) + w2, vbo.at(5) - w2);
                    glEnd();
                }
            }
        }
    }
}


void MatrixView::drawRoutes(vector< vector<GLfloat> > &routesArcVbos, vector< vector<GLfloat> > &routesCurveVbos, vector<GLfloat> &routesCbo, vector< vector<GLfloat> > &routesSrcVbos, vector< vector<GLfloat> > &routesDestVbos, QString type, float width)
{
    // draw srcs
    QColor srcColor = color->matrixSrcColor;
    for (int i = 0; i < (int)routesSrcVbos.size(); ++i) {
        bool isAllVerticesDisplayed = true;
        if (routesSrcVbos.at(i).at(0) < -1.0) isAllVerticesDisplayed = false;

        if (isAllVerticesDisplayed) {
            glColor4f(srcColor.redF(), srcColor.greenF(), srcColor.blueF(), srcColor.alphaF());
            glVertexPointer(2, GL_FLOAT, 0, &routesSrcVbos[i][0]);
            glEnableClientState(GL_VERTEX_ARRAY);
            glDrawArrays(GL_TRIANGLE_FAN, 0, routesSrcVbos[i].size() / 2);
            glDisableClientState(GL_VERTEX_ARRAY);

            // TODO: should be not included type in args. think later.
            if (type == "suggested" && routeDrawingMode == "straight") {
                glLineWidth(1.5 * this->devicePixelRatioF());
                QColor qColor = color->ratioToHeatMapColor(1.0, color->suggestedRouteColorOfPoints);
                glColor4f(qColor.redF(), qColor.greenF(), qColor.blueF(), qColor.alphaF());
                glVertexPointer(2, GL_FLOAT, 0, &routesSrcVbos[i][0]);
                glEnableClientState(GL_VERTEX_ARRAY);
                glDrawArrays(GL_LINE_LOOP, 0, routesSrcVbos[i].size() / 2);
                glDisableClientState(GL_VERTEX_ARRAY);
            }
        }
    }

    // draw dests
    QColor destColor = color->matrixDestColor;
    for (int i = 0; i < (int)routesDestVbos.size(); ++i) {
        bool isAllVerticesDisplayed = true;
        if (routesDestVbos.at(i).at(0) < -1.0) isAllVerticesDisplayed = false;

        if (isAllVerticesDisplayed) {
            glColor4f(destColor.redF(), destColor.greenF(), destColor.blueF(), destColor.alphaF());
            glVertexPointer(2, GL_FLOAT, 0, &routesDestVbos[i][0]);
            glEnableClientState(GL_VERTEX_ARRAY);
            glDrawArrays(GL_TRIANGLE_FAN, 0, routesDestVbos[i].size() / 2);
            glDisableClientState(GL_VERTEX_ARRAY);
        }

        // TODO: should be not included type in args. think later.
        if (type == "suggested" && routeDrawingMode == "straight") {
            glLineWidth(1.5 * this->devicePixelRatioF());
            QColor qColor = color->ratioToHeatMapColor(1.0, color->suggestedRouteColorOfPoints);
            glColor4f(qColor.redF(), qColor.greenF(), qColor.blueF(), qColor.alphaF());
            glVertexPointer(2, GL_FLOAT, 0, &routesDestVbos[i][0]);
            glEnableClientState(GL_VERTEX_ARRAY);
            glDrawArrays(GL_LINE_LOOP, 0, routesDestVbos[i].size() / 2);
            glDisableClientState(GL_VERTEX_ARRAY);
        }
    }


    if (routeDrawingMode == "straight") {
        // draw routes
        glLineWidth(width * this->devicePixelRatioF());
        float w2 = 0.003 * width * 0.5 / scale;

        for (int i = 0; i < (int)routesArcVbos.size(); ++i) {
            bool isAllVerticesDisplayed = true;
            for (int j = 0; j < (int)routesArcVbos.at(i).size() / 2; ++j) {
                if (routesArcVbos.at(i).at(j * 2) < -1.0) {
                    isAllVerticesDisplayed = false;
                    break;
                }
            }
            if (isAllVerticesDisplayed) {
                glColor4f(routesCbo.at(i * 4), routesCbo.at(i * 4 + 1), routesCbo.at(i * 4 + 2), routesCbo.at(i * 4 + 3));
                glVertexPointer(2, GL_FLOAT, 0, &routesArcVbos[i][0]);
                glEnableClientState(GL_VERTEX_ARRAY);
                glDrawArrays(GL_LINE_STRIP, 0, routesArcVbos[i].size() / 2);
                glDisableClientState(GL_VERTEX_ARRAY);
            }
        }
    } else if (routeDrawingMode == "catmull-rom") {
        // draw routes
        glLineWidth(width * this->devicePixelRatioF());

        for (int i = 0; i < (int)routesArcVbos.size(); ++i) {
            bool isAllVerticesDisplayed = true;
            for (int j = 0; j < (int)routesArcVbos.at(i).size() / 2; ++j) {
                if (routesArcVbos.at(i).at(j * 2) < -1.0) {
                    isAllVerticesDisplayed = false;
                    break;
                }
            }

            if (isAllVerticesDisplayed) {
                glColor4f(routesCbo.at(i * 4), routesCbo.at(i * 4 + 1), routesCbo.at(i * 4 + 2), routesCbo.at(i * 4 + 3));
                glLineWidth(width * this->devicePixelRatioF());
                vector<GLfloat> curveVbo = routesCurveVbos.at(i);

                glBegin(GL_LINE_STRIP);
                for (int j = 0; j < (int)curveVbo.size() / 2; ++j) {
                    glVertex2d(curveVbo.at(j * 2), curveVbo.at(j * 2 + 1));
                }
                glEnd();
            }
        }
    }

    glPointSize(width * this->devicePixelRatioF() * 2.0);
    for (int i = 0; i < (int)routesDestVbos.size(); ++i) {
        bool isAllVerticesDisplayed = true;
        if (routesDestVbos.at(i).at(0) < -1.0 || routesSrcVbos.at(i).at(0) < -1.0) isAllVerticesDisplayed = false;

        if (isAllVerticesDisplayed) {
            if (routesSrcVbos.at(i).at(0) + routesSrcVbos.at(i).at(4) ==
                routesDestVbos.at(i).at(0) + routesDestVbos.at(i).at(20)) {
                glColor4f(routesCbo.at(i * 4), routesCbo.at(i * 4 + 1), routesCbo.at(i * 4 + 2), routesCbo.at(i * 4 + 3));
                if (type == "suggested" && routeDrawingMode == "straight") {
                    QColor qColor = color->ratioToHeatMapColor(1.0, color->suggestedRouteColorOfPoints);
                    glColor4f(qColor.redF(), qColor.greenF(), qColor.blueF(), qColor.alphaF());
                }

                glBegin(GL_POINTS);
                double x = (routesSrcVbos.at(i).at(0) + routesSrcVbos.at(i).at(4)) * 0.5;
                double y = (routesSrcVbos.at(i).at(1) + routesSrcVbos.at(i).at(5)) * 0.5;
                glVertex2d(x, y);
                glEnd();
            }
        }
    }
}

void MatrixView::drawColorMap(float x, float y, float w, float h, QList< QPair<float, QColor> > colorOfPoints)
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

void MatrixView::drawColorMaps()
{
    float edgeLabelSpace = 0.034;
    if (edgeDrawingMode == "none") {
        edgeLabelSpace = 0.0;
    }
    if (edgeDrawingMode != "none") {
        drawColorMap(0.01, 0.015, 0.17, 0.015, color->edgeColorOfPoints);
    }
    if (routeDrawingMode != "none" && routeColorMetric != "distinguishable") {
        drawColorMap(0.01, 0.016 + edgeLabelSpace, 0.17, 0.015, color->getRouteColorOfPointsForMetric(routeColorMetric));
    }
}

void MatrixView::drawColorSymbols()
{
    float edgeLabelSpace = 0.034;
    if (edgeDrawingMode == "none") {
        edgeLabelSpace = 0.0;
    }

    QColor srcColor = color->matrixSrcColor;
    QColor destColor = color->matrixDestColor;
    QColor connectColor = color->adjMatrixColor;

    glPointSize(10.0 * this->devicePixelRatioF());
    glColor4f(connectColor.redF(), connectColor.greenF(), connectColor.blueF(), connectColor.alphaF());
    glBegin(GL_POINTS);
    glVertex2d(0.015, 0.100 + edgeLabelSpace);
    glEnd();
    glColor4f(srcColor.redF(), srcColor.greenF(), srcColor.blueF(), srcColor.alphaF());
    glBegin(GL_POINTS);
    glVertex2d(0.015, 0.078 + edgeLabelSpace);
    glEnd();

    glEnable(GL_POINT_SMOOTH);
    glColor4f(destColor.redF(), destColor.greenF(), destColor.blueF(), destColor.alphaF());
    glBegin(GL_POINTS);
    glVertex2d(0.015, 0.054 + edgeLabelSpace);
    glEnd();
    glDisable(GL_POINT_SMOOTH);
}

void MatrixView::paintLabels()
{
    float edgeLabelSpace = 2.0;
    if (edgeDrawingMode == "none") {
        edgeLabelSpace = 0.0;
    }

    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
    painter.setPen(QColor(60, 60, 60));

    painter.setFont(QFont("San Francisco", 8));
    painter.setBackgroundMode(Qt::TransparentMode);
    int fontW = painter.fontMetrics().ascent();
    int fontH = painter.fontMetrics().height();

    // labels for symbols
    QString blockSizeLabel = "size of displayed unit cell: ";
    painter.drawText(0.5 * fontW, this->height() - (edgeLabelSpace + 8.3) * fontH, 30 * fontW, fontH,
                     Qt::AlignVCenter | Qt::AlignLeft, blockSizeLabel + QString::number(unitBlockSize));
    QString connectedCellLabel = "connected pairs of nodes";
    painter.drawText(2.2 * fontW, this->height() - (edgeLabelSpace + 6.9) * fontH, 25 * fontW, fontH,
                     Qt::AlignVCenter | Qt::AlignLeft, connectedCellLabel);
    QString srcLabel = "source";
    QString destLabel = "destination";;
    painter.drawText(2.2 * fontW, this->height() - (edgeLabelSpace + 5.5) * fontH, 25 * fontW, fontH,
                     Qt::AlignVCenter | Qt::AlignLeft, srcLabel);
    painter.drawText(2.2 * fontW, this->height() - (edgeLabelSpace + 4.1) * fontH, 25 * fontW, fontH,
                     Qt::AlignVCenter | Qt::AlignLeft, destLabel);


    float edgeValMax = 0.0;
    float edgeValMin = 0.0;
    if (graph->getNumEdges() > 0) edgeValMax = graph->getEdgeAt(graph->getNumEdges() - 1).getMetric(edgeColorMetric);

    float routeValMax = 0.0;
    float routeValMin = 0.0;

    if (graph->getNumRoutes() > 0) {
        if (routeColorMetric == "byte") routeValMax = graph->getMaxRoutesByte();
        else if (routeColorMetric == "length") routeValMax = graph->getMaxLengthOfRoutes();
        else if (routeColorMetric == "hopbyte") routeValMax = graph->getMaxRoutesHopbyte();
    }

    if (edgeDrawingMode != "none") {
        painter.drawText(0.5 * fontW, this->height() - 1.1 * fontH, 2 * fontW, fontH,
                         Qt::AlignVCenter | Qt::AlignLeft, QString::number(edgeValMin, 'g', 3));
        painter.drawText(8.5 * fontW, this->height() - 1.1 * fontH, 5 * fontW, fontH,
                         Qt::AlignVCenter | Qt::AlignRight, QString::number(edgeValMax, 'g', 3));
        painter.drawText(14 * fontW, this->height() - 2.0 * fontH, 12.0 * fontW, fontH,
                         Qt::AlignVCenter | Qt::AlignLeft, "message size on link");
    }

    if (routeDrawingMode != "none" && routeColorMetric != "distinguishable") {
        QString title = "";
        if (routeColorMetric == "byte") title = "message size on route";
        else if (routeColorMetric == "length") title = "length of route";
        else if (routeColorMetric == "hop-byte") title = "hop-byte on route";

        painter.drawText(0.5 * fontW, this->height() - (edgeLabelSpace + 1.1) * fontH, 2 * fontW, fontH,
                         Qt::AlignVCenter | Qt::AlignLeft, QString::number(routeValMin, 'g', 3));
        painter.drawText(8.5 * fontW, this->height() - (edgeLabelSpace + 1.1) * fontH, 5 * fontW, fontH,
                         Qt::AlignVCenter | Qt::AlignRight, QString::number(routeValMax, 'g', 3));
        painter.drawText(14 * fontW, this->height() - (edgeLabelSpace + 1.9) * fontH, 12.0 * fontW, fontH,
                         Qt::AlignVCenter | Qt::AlignLeft, title);
    }

    painter.end();
}

void MatrixView::drawSelectedArea()
{
    if (selectedXRange.size() >= 2 && selectedYRange.size() >= 2) {
        glColor4d(color->selectionColor.redF(), color->selectionColor.greenF(), color->selectionColor.blueF(), color->selectionColor.alphaF());
        glBegin(GL_TRIANGLE_FAN);
        glVertex2d(selectedXRange.at(0), selectedYRange.at(0));
        glVertex2d(selectedXRange.at(0), selectedYRange.at(1));
        glVertex2d(selectedXRange.at(1), selectedYRange.at(1));
        glVertex2d(selectedXRange.at(1), selectedYRange.at(0));
        glEnd();
    }
}

void MatrixView::drawLensContents()
{
    if (mouseMode == "Lens" && lensTargetXRange.size() >= 2 && lensTargetYRange.size() >= 2) {

        glColor4d(1.0, 1.0, 1.0, 1.0);
        glBegin(GL_TRIANGLE_FAN);
        glVertex2d(0.0, 0.0);
        glVertex2d(1.0, 0.0);
        glVertex2d(1.0, 1.0);
        glVertex2d(0.0, 1.0);
        glEnd();

        drawFoldedRegions(foldedRegionsVbos);
        drawAdjMat(adjMatVboForLens);
        drawEdges(edgesVbosForLens, edgesCbo, 1.0);
        drawEdges(suggestedEdgesVbos, suggestedEdgesCbo, 1.0);
        drawRoutes(routesArcVbosForLens, routesCurveVbosForLens, routesCbo, routesSrcVbosForLens, routesDestVbosForLens, "displayed", 3.0);
        drawRoutes(suggestedRoutesArcVbosForLens, suggestedRoutesCurveVbosForLens, suggestedRoutesCbo, suggestedRoutesSrcVbosForLens, suggestedRoutesDestVbosForLens, "suggested", 3.0);
    }
}

void MatrixView::drawLensOutLine()
{
    if (mouseMode == "Lens") {
        QPointF mouseMovePosF = qtPosToGlPos(mouseMovePos);
        glColor4d(0.2, 0.2, 0.2, 1.0);
        glBegin(GL_TRIANGLE_FAN);
        glVertex2d(mouseMovePosF.x() - lensSize * 0.515 / scale, mouseMovePosF.y() + lensSize * 0.515 / scale);
        glVertex2d(mouseMovePosF.x() + lensSize * 0.515 / scale, mouseMovePosF.y() + lensSize * 0.515 / scale);
        glVertex2d(mouseMovePosF.x() + lensSize * 0.515 / scale, mouseMovePosF.y() - lensSize * 0.515 / scale);
        glVertex2d(mouseMovePosF.x() - lensSize * 0.515 / scale, mouseMovePosF.y() - lensSize * 0.515 / scale);
        glEnd();
    }
}

void MatrixView::zoomInSelectedRange()
{
    if (selectedXRange.size() >= 2 && selectedYRange.size() >= 2) {
        scale = 1.0 / qMax((selectedXRange.at(1) - selectedXRange.at(0)), (selectedYRange.at(1) - selectedYRange.at(0)));
        translateX = - (selectedXRange.at(0) + selectedXRange.at(1)) * 0.5 + 0.5;
        translateY = - (selectedYRange.at(0) + selectedYRange.at(1)) * 0.5 + 0.5;
    }

    selectedXRange.clear();
    selectedYRange.clear();
    updateAllDrawingInfo();
    update();
}

void MatrixView::resetDisplayedRange()
{
    scale = 1.0;
    translateX = 0.0;
    translateY = 0.0;
    selectedXRange.clear();
    selectedYRange.clear();
    updateAllDrawingInfo();
    update();
}

QPointF MatrixView::qtPosToGlPos(QPoint qtPos)
{
    float resultX = ((float)qtPos.x() / (float)this->width() - 0.5) / scale + 0.5 - translateX;
    float resultY = (1.0 - (float)qtPos.y() / (float)this->height() - 0.5) / scale + 0.5 - translateY;
    return QPointF(resultX, resultY);
}

// for zooming
void MatrixView::wheelEvent(QWheelEvent *event)
{
    float sensitiveRatio = 0.02;
    scale += (event->delta() / 8.0 / 15.0) * sensitiveRatio;
    if(scale < 0.1) {
        scale = 0.1;
    }

    if (unitBlockSize != getAdjustedBlockSize()) {
        updateUnitBlockSize();
        updateAllDrawingInfo();
    } else if (mouseMode == "Lens" && unitBlockSizeForLens != getAdjustedBlockSizeForLens()) {
        updateUnitBlockSize();
        updateAllDrawingInfoForLens();
    }

    update();
}

void MatrixView::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        selectedXRange.clear();
        selectedYRange.clear();
        leftButtonPressed = true;
    } else if (event->button() == Qt::RightButton) {
        rightButtonPressed = true;
    }
    mousePressPos = event->pos();
}

void MatrixView::mouseMoveEvent(QMouseEvent *event)
{
    QPoint mousePosDelta = event->pos() - mousePressPos;

    if ((mouseMode == "Hand" && leftButtonPressed)|| rightButtonPressed) {
        if(mousePosDelta.manhattanLength() > 5) {
            QPointF glPosCurrent = qtPosToGlPos(event->pos());
            QPointF glPosPre = qtPosToGlPos(mouseMovePos);
            translateX += (glPosCurrent - glPosPre).x();
            translateY += (glPosCurrent - glPosPre).y();
        }
    } else if (mouseMode == "Rect" && leftButtonPressed) {
        QPointF pressPosF = qtPosToGlPos(mousePressPos);
        QPointF movePosF = qtPosToGlPos(event->pos());

        if(mousePosDelta.manhattanLength() > 5) {
            vector<GLfloat> xPositions;
            vector<GLfloat> yPositions;
            xPositions.push_back(qMin(pressPosF.x(), movePosF.x()));
            xPositions.push_back(qMax(pressPosF.x(), movePosF.x()));
            yPositions.push_back(qMin(pressPosF.y(), movePosF.y()));
            yPositions.push_back(qMax(pressPosF.y(), movePosF.y()));
            selectedXRange = xPositions;
            selectedYRange = yPositions;
        }
    } else if (mouseMode == "Lens") {
        QPointF movePosF = qtPosToGlPos(event->pos());
        vector<GLfloat> xPositions;
        vector<GLfloat> yPositions;
        float size = lensSize / (float)lensZoomLevel / (float)scale;
        xPositions.push_back(movePosF.x() - size * 0.5);
        xPositions.push_back(movePosF.x() + size * 0.5);
        yPositions.push_back(movePosF.y() - size * 0.5);
        yPositions.push_back(movePosF.y() + size * 0.5);
        lensTargetXRange = xPositions;
        lensTargetYRange = yPositions;
    }

    mouseMovePos = event->pos();
    update();
}

void MatrixView::mouseReleaseEvent(QMouseEvent *event)
{
    QPoint mousePosDelta = event->pos() - mousePressPos;

    if (event->button() == Qt::LeftButton) {
        if (selectedXRange.size() > 0 && selectedYRange.size() > 0) {
            showContextMenu(event->pos());
        }
    }
    if (event->button() == Qt::RightButton) {
        if(mousePosDelta.manhattanLength() < 5) {
            showContextMenu(event->pos());
        }
    }

    leftButtonPressed = false;
    rightButtonPressed = false;
}

void MatrixView::showContextMenu(const QPoint &pos)
{
    QMenu contextMenu(tr("Context menu"), this);
    QAction actSwitchEdgeDrawingMode("Show edges", this);
    QAction actSwitchRouteDrawingMode("Show routes", this);
    QAction actSwitchFolding("Fold regions not related with displayed routes", this);
    QAction actSwitchResolutionReduction("Turn on reolution reduction", this);
    QAction actSuggestAltRoutesForDisplayedRoutes("Suggest alternative routes", this);
    QAction actApplyAltRoutes("Apply suggested altenative routes", this);
    QAction actZoomInSelectedRange("Zoom in selected area", this);
    QAction actResetDisplayedRange("Reset zoom", this);

    if (edgeDrawingMode != "none") actSwitchEdgeDrawingMode.setText("Hide edges");
    if (routeDrawingMode != "none") actSwitchRouteDrawingMode.setText("Hide routes");
    if (isResolutionReductionTurnedOn) actSwitchResolutionReduction.setText("Turn off resolution reduction");
    if (unitFoldingSize != 1) actSwitchFolding.setText("Unfold all regions");
    if (suggestedRoutes.size() != 0) actSuggestAltRoutesForDisplayedRoutes.setText("Clear alternative routes");

    connect(&actSwitchEdgeDrawingMode, SIGNAL(triggered()), this, SLOT(switchEdgeDrawingMode()));
    connect(&actSwitchRouteDrawingMode, SIGNAL(triggered()), this, SLOT(switchRouteDrawingMode()));
    connect(&actSwitchFolding, SIGNAL(triggered()), this, SLOT(switchFolding()));
    connect(&actSwitchResolutionReduction, SIGNAL(triggered()), this, SLOT(switchResolutionReduction()));
    connect(&actSuggestAltRoutesForDisplayedRoutes, SIGNAL(triggered()), this, SLOT(suggestAltRoutesForDisplayedRoutes()));
    connect(&actApplyAltRoutes, SIGNAL(triggered()), this, SLOT(applyAltRoutes()));
    connect(&actZoomInSelectedRange, SIGNAL(triggered()), this, SLOT(zoomInSelectedRange()));
    connect(&actResetDisplayedRange, SIGNAL(triggered()), this, SLOT(resetDisplayedRange()));

    contextMenu.addAction(&actSwitchEdgeDrawingMode);
    contextMenu.addAction(&actSwitchRouteDrawingMode);
    contextMenu.addAction(&actSwitchFolding);
    contextMenu.addAction(&actSwitchResolutionReduction);
    if (selectedXRange.size() > 0 && selectedYRange.size() > 0) {
        contextMenu.addAction(&actZoomInSelectedRange);
    }
    contextMenu.addAction(&actResetDisplayedRange);

    contextMenu.exec(mapToGlobal(pos));
}

void MatrixView::switchEdgeDrawingMode()
{
    if (edgeDrawingMode == "none") edgeDrawingMode = "straight";
    else edgeDrawingMode = "none";
    update();
}

void MatrixView::switchRouteDrawingMode()
{
    if (routeDrawingMode == "none") routeDrawingMode = "straight";
    else routeDrawingMode = "none";
    update();
}

void MatrixView::switchFolding()
{
    if (unitFoldingSize == 1) {
        // tmp code: folding based regions corespoding 2 dimensions
        QVector<int> shape = displayedAdjMat->getShape();
        QVector<int> order = displayedAdjMat->getOrder();
        unitFoldingSize = shape.at(order.at(0)) * shape.at(order.at(1));
        //routeWidth = 2.0; // for taking video
        //edgeWidth = 3.0; // for taking video
    } else {
        unitFoldingSize = 1;
    }
    updateAllDrawingInfo();
    update();
}

void MatrixView::switchResolutionReduction()
{
    if (isResolutionReductionTurnedOn) {
        isResolutionReductionTurnedOn = false;
        unitBlockSize = 1;
        unitBlockSizeForLens = 1;
    } else {
        isResolutionReductionTurnedOn = true;
        updateUnitBlockSize();
    }

    updateAllDrawingInfo();
    update();
}

void MatrixView::setGraphDir(QString graphDir) { this->graphDir = graphDir; }
void MatrixView::setAdjMat(AdjMatrix* adjMat) { this->adjMat = adjMat; }
void MatrixView::setAggregatedAdjMat(AdjMatrix* aggregatedAdjMat) { this->aggregatedAdjMat = aggregatedAdjMat; }
void MatrixView::setDisplayedAdjMat(AdjMatrix* adjMat) { this->displayedAdjMat = adjMat; }
void MatrixView::setGraph(Graph* graph) { this->graph = graph; }
void MatrixView::setAggregatedGraph(Graph* aggregatedGraph) { this->aggregatedGraph = aggregatedGraph; }
void MatrixView::setDisplayedGraph(Graph* graph) { this->displayedGraph = graph; }
void MatrixView::setColor(Color* color) { this->color = color; }
void MatrixView::setDisplayedVertexIndices(QVector<int> displayedVertexIndices) { this->displayedVertexIndices = displayedVertexIndices; }
void MatrixView::setDisplayedEdgeIndices(QVector<int> displayedEdgeIndices) { this->displayedEdgeIndices = displayedEdgeIndices; }
void MatrixView::setDisplayedRouteIndices(QVector<int> displayedRouteIndices) { this->displayedRouteIndices = displayedRouteIndices; }
void MatrixView::setMouseMode(QString mouseMode)
{
    this->mouseMode = mouseMode;
    if (mouseMode == "Lens") {
        updateAllDrawingInfoForLens();
    } else {
        this->setCursor(Qt::ArrowCursor);
    }
}
void MatrixView::setRoutingGuideType(QString routingGuideType) { this->routingGuideType = routingGuideType; }
void MatrixView::setMappingGuideType(QString mappingGuideType) { this->mappingGuideType = mappingGuideType; }
void MatrixView::setRoutingGuidePriority(QString routingGuidePriority) { this->routingGuidePriority = routingGuidePriority; }
void MatrixView::setRoutingGuideLengthTolerance(int routingGuideLengthTolerance) { this->routingGuideLengthTolerance = routingGuideLengthTolerance; }
void MatrixView::setRoutingGuideLoadTolerance(float routingGuideLoadTolerance) { this->routingGuideLoadTolerance = routingGuideLoadTolerance; }
void MatrixView::setUnitFoldingSize(int unitFoldingSize) { this->unitFoldingSize = unitFoldingSize; }
void MatrixView::setRouteDrawingMode(QString routeDrawingMode) { this->routeDrawingMode = routeDrawingMode; }
void MatrixView::setRscriptPath(QString rscriptPath) { this->rscriptPath = rscriptPath; }
void MatrixView::setSaQapPath(QString saQapPath) { this->saQapPath = saQapPath; }

void MatrixView::resetDisplayedElementsIndices()
{
    displayedVertexIndices.clear();
    displayedEdgeIndices.clear();
    displayedRouteIndices.clear();
}

void MatrixView::resetSuggestedElementsIndices()
{
    suggestedRoutes.clear();
    suggestedRoutesIndices.clear();
    suggestedVertexIndices.clear();
    suggestedEdgeIndices.clear();
}

void MatrixView::resetScaleAndTranslate()
{
    scale = 1.0;
    translateX = 0.0;
    translateY = 0.0;
}

void MatrixView::updateUnitBlockSize()
{
    if (isResolutionReductionTurnedOn) {
        unitBlockSize = getAdjustedBlockSize();
        unitBlockSizeForLens = getAdjustedBlockSizeForLens();
    }
}

void MatrixView::resetSettings()
{
    numOfDisplayedCells = displayedAdjMat->getColSize();
    resetDisplayedElementsIndices();
    resetSuggestedElementsIndices();
    resetScaleAndTranslate();
    updateUnitBlockSize();
    updateAllDrawingInfo();
    //updateAllDrawingInfoForLens();
    update();
}

void MatrixView::updateAdjMatVbo(vector<GLfloat> &adjMatVbo, int unitBlockSize)
{
    adjMatVbo.clear();

    int matSize = displayedAdjMat->getColSize();
    double unitSize = 1.0 * (float)unitBlockSize / matSize;

    for (int i = 0; i < matSize; ++i) {
        for (int j = 0; j < matSize; ++j) {
            if (displayedAdjMat->getValueAt(i, j) == 1) {
                int blockI = i / unitBlockSize;
                int blockJ = j / unitBlockSize;
                adjMatVbo.push_back(blockI * unitSize);
                adjMatVbo.push_back(1.0 - blockJ * unitSize);
                adjMatVbo.push_back((blockI + 1) * unitSize);
                adjMatVbo.push_back(1.0 - blockJ * unitSize);
                adjMatVbo.push_back((blockI + 1) * unitSize);
                adjMatVbo.push_back(1.0 - (blockJ + 1) * unitSize);

                adjMatVbo.push_back(blockI * unitSize);
                adjMatVbo.push_back(1.0 - blockJ * unitSize);
                adjMatVbo.push_back((blockI + 1) * unitSize);
                adjMatVbo.push_back(1.0 - (blockJ + 1) * unitSize);
                adjMatVbo.push_back(blockI * unitSize);
                adjMatVbo.push_back(1.0 - (blockJ + 1) * unitSize);
            }
        }
    }
}

void MatrixView::updateCoordValues(QVector<double> &coordValues, QVector<int> &vertexIndices, int &unitBlockSize)
{
    coordValues.clear();
    foldedRegionNums.clear();
    QVector<int> unfoldedColNums;

    if (unitFoldingSize == 1) { // non folding
        numOfDisplayedCells = displayedAdjMat->getColSize();
        updateUnitBlockSize();

        double unitSize = 1.0 * (float)unitBlockSize / displayedAdjMat->getColSize();

        for (int i = 0; i < displayedAdjMat->getColSize() / unitBlockSize; ++i) {
            for (int j = 0; j < unitBlockSize; ++j) {
                coordValues.push_back(i * unitSize);
            }
        }
        coordValues.push_back(displayedAdjMat->getColSize() * unitSize);

    } else { // folding
        float tmpNumOfDisplayedCells = 0.0;

        // TODO: think faster way
        double coordVal = 0.0;
        for (int i = 0; i < displayedAdjMat->getColSize() / unitFoldingSize; ++i) {
            bool isSomeVertexUsedInUnitFoldingSize = false;

            for (int j = 0; j < unitFoldingSize; ++j) {
                int vIndex = displayedGraph->getVertexIndexWithCoord(displayedAdjMat->colNumToCoord(i * unitFoldingSize + j));
                if (vertexIndices.contains(vIndex)) {
                    isSomeVertexUsedInUnitFoldingSize = true;
                    break;
                }
            }

            if (isSomeVertexUsedInUnitFoldingSize) {
                for (int j = 0; j < unitFoldingSize; ++j) {
                    tmpNumOfDisplayedCells += 1.0;
                    unfoldedColNums.push_back(i * unitFoldingSize + j);
                }
            } else {
                foldedRegionNums.push_back(i);
                tmpNumOfDisplayedCells += 0.5;
            }
        }

        numOfDisplayedCells = tmpNumOfDisplayedCells;
        updateUnitBlockSize();

        double unitSize = 1.0 * (float)unitBlockSize / displayedAdjMat->getColSize();

        for (int i = 0; i < displayedAdjMat->getColSize() / unitFoldingSize; ++i) {
            if (!foldedRegionNums.contains(i)) {
                for (int j = 0; j < unitFoldingSize; ++j) {
                    coordValues.push_back(coordVal);
                    if (j % unitBlockSize == unitBlockSize - 1) {
                        coordVal += unitSize;
                    }
                }
            } else {
                for (int j = 0; j < unitFoldingSize; ++j) {
                    coordValues.push_back(coordVal);
                }
                coordVal += unitSize * 0.5;
            }
        }
        coordValues.push_back(coordVal);

        // adjust size to fit max value = 1.0
        magnifyingRateForFolding  = 1.0 / coordValues.last();
        for (int i = 0; i < coordValues.size(); ++i) {
            coordValues[i] *= magnifyingRateForFolding;
        }
    }
}

void MatrixView::updateFoldedRegionsVbo(vector<vector<GLfloat> > &foldedRegionsVbos, QVector<double> &coordValues)
{
    foldedRegionsVbos.clear();

    foreach(int num, foldedRegionNums) {
        double coordVal1 = coordValues.at(num * unitFoldingSize);
        double coordVal2 = coordValues.at(num * unitFoldingSize + unitFoldingSize);

        // col
        vector<GLfloat> foldedRegionsColVbo;
        foldedRegionsColVbo.push_back(coordVal1);
        foldedRegionsColVbo.push_back(1.0);
        foldedRegionsColVbo.push_back(coordVal2);
        foldedRegionsColVbo.push_back(1.0);
        foldedRegionsColVbo.push_back(coordVal2);
        foldedRegionsColVbo.push_back(0.0);
        foldedRegionsColVbo.push_back(coordVal1);
        foldedRegionsColVbo.push_back(0.0);

        // row
        vector<GLfloat> foldedRegionsRowVbo;
        foldedRegionsRowVbo.push_back(0.0);
        foldedRegionsRowVbo.push_back(1.0 - coordVal1);
        foldedRegionsRowVbo.push_back(1.0);
        foldedRegionsRowVbo.push_back(1.0 - coordVal1);
        foldedRegionsRowVbo.push_back(1.0);
        foldedRegionsRowVbo.push_back(1.0 - coordVal2);
        foldedRegionsRowVbo.push_back(0.0);
        foldedRegionsRowVbo.push_back(1.0 - coordVal2);

        foldedRegionsVbos.push_back(foldedRegionsColVbo);
        foldedRegionsVbos.push_back(foldedRegionsRowVbo);
    }
}

void MatrixView::updateAdjMatVboWithFolding(vector<GLfloat> &adjMatVbo, QVector<double> &coordValues)
{
    adjMatVbo.clear();

    for (int i = 0; i < displayedAdjMat->getColSize(); ++i) {
        for (int j = 0; j < displayedAdjMat->getColSize(); ++j) {
            if (displayedAdjMat->getValueAt(i, j) == 1) {
                adjMatVbo.push_back(coordValues.at(i));
                adjMatVbo.push_back(1.0 - coordValues.at(j));
                adjMatVbo.push_back(coordValues.at(i + 1));
                adjMatVbo.push_back(1.0 - coordValues.at(j));
                adjMatVbo.push_back(coordValues.at(i + 1));
                adjMatVbo.push_back(1.0 - coordValues.at(j + 1));

                adjMatVbo.push_back(coordValues.at(i));
                adjMatVbo.push_back(1.0 - coordValues.at(j));
                adjMatVbo.push_back(coordValues.at(i + 1));
                adjMatVbo.push_back(1.0 - coordValues.at(j + 1));
                adjMatVbo.push_back(coordValues.at(i));
                adjMatVbo.push_back(1.0 - coordValues.at(j + 1));
            }
        }
    }
}

void MatrixView::updateVerticesVbo(vector<GLfloat> &verticesVbo, QVector<int> &vertexIndices, int unitBlockSize)
{
    verticesVbo.clear();

    // TODO: rethink structure here later
    QVector<Vertex> vertices = displayedGraph->getVertices();
    for (int i = 0; i < vertices.size(); ++i) {
        verticesVbo.push_back(-1000000.0);
        verticesVbo.push_back(-1000000.0);
    }

    double unitSize = 1.0 * (float)unitBlockSize / displayedAdjMat->getColSize();

    foreach(int i, vertexIndices) {
        int colNum = displayedAdjMat->coordToColNum(vertices[i].getCoord());
        int blockNum = colNum / unitBlockSize;
        verticesVbo[i * 2] = blockNum * unitSize + 0.5 * unitSize;
        verticesVbo[i * 2 + 1] = 1.0 - (blockNum * unitSize + 0.5 * unitSize);
    }
}

void MatrixView::updateVerticesVboWithFolding(vector<GLfloat> &verticesVbo, QVector<int> &vertexIndices, QVector<double> &coordValues, int unitBlockSize, int unitFoldingSize)
{
    verticesVbo.clear();
    QVector<Vertex> vertices = displayedGraph->getVertices();

    double unitSize = 1.0 * (float)unitBlockSize / displayedAdjMat->getColSize();
    // TODO: want to remove magnifyingRateForFolding
    if (unitFoldingSize != 1) unitSize *= magnifyingRateForFolding;

    for (int i = 0; i < vertices.size(); ++i) {
        verticesVbo.push_back(-1000000.0);
        verticesVbo.push_back(-1000000.0);
    }

    foreach(int i, vertexIndices) {
        QVector<int> coord = vertices[i].getCoord();
        int colNum = displayedAdjMat->coordToColNum(coord);

        verticesVbo[i * 2] = coordValues.at(colNum) + 0.5 * unitSize;
        verticesVbo[i * 2 + 1] = 1.0 - (coordValues.at(colNum) + 0.5 * unitSize);
    }
}

void MatrixView::updateRoutesArcVbos(vector< vector<GLfloat> > &routesArcVbos, vector<GLfloat> &verticesVbo, QVector<Route> &routes, QVector<int> &routeIndices)
{
    routesArcVbos.clear();

    foreach(int rtIndex, routeIndices) {
        vector<GLfloat> vbo;
        QVector<int> arcs = routes[rtIndex].getArcs();

        for (int i = 1; i < arcs.size(); ++i) {
            int vIndex0 = arcs.at(i - 1);
            int vIndex1 = arcs.at(i);

            if (i == 1) {
                vbo.push_back(verticesVbo.at(vIndex0 * 2));
                vbo.push_back(verticesVbo.at(vIndex0 * 2 + 1));
            }
            vbo.push_back(verticesVbo.at(vIndex1 * 2));
            vbo.push_back(verticesVbo.at(vIndex0 * 2 + 1));
            vbo.push_back(verticesVbo.at(vIndex1 * 2));
            vbo.push_back(verticesVbo.at(vIndex1 * 2 + 1));
        }

        routesArcVbos.push_back(vbo);
    }

}

void MatrixView::updateRoutesSrcVbos(vector< vector<GLfloat> > &routesSrcVbos, vector<GLfloat> &verticesVbo, QVector<Route> &routes, QVector<int> &routeIndices, int unitBlockSize)
{
    routesSrcVbos.clear();

    double unitSize = 1.0 * (float)unitBlockSize / displayedAdjMat->getColSize();
    // TODO: want to remove magnifyingRateForFolding
    if (unitFoldingSize != 1) unitSize *= magnifyingRateForFolding;

    foreach(int rtIndex, routeIndices) {
        vector<GLfloat> vbo;
        int src = routes[rtIndex].getSrc();
        vbo.push_back(verticesVbo.at(src * 2) - 0.5 * unitSize);
        vbo.push_back(verticesVbo.at(src * 2 + 1) + 0.5 * unitSize);
        vbo.push_back(verticesVbo.at(src * 2) + 0.5 * unitSize);
        vbo.push_back(verticesVbo.at(src * 2 + 1) + 0.5 * unitSize);
        vbo.push_back(verticesVbo.at(src * 2) + 0.5 * unitSize);
        vbo.push_back(verticesVbo.at(src * 2 + 1) - 0.5 * unitSize);
        vbo.push_back(verticesVbo.at(src * 2) - 0.5 * unitSize);
        vbo.push_back(verticesVbo.at(src * 2 + 1) - 0.5 * unitSize);

        routesSrcVbos.push_back(vbo);
    }
}

void MatrixView::updateRoutesDestVbos(vector< vector<GLfloat> > &routesDestVbos, vector<GLfloat> &verticesVbo, QVector<Route> &routes, QVector<int> &routeIndices, int unitBlockSize)
{
    routesDestVbos.clear();

    double unitSize = 1.0 * (float)unitBlockSize / displayedAdjMat->getColSize();
    // TODO: want to remove magnifyingRateForFolding
    if (unitFoldingSize != 1) unitSize *= magnifyingRateForFolding;

    foreach(int rtIndex, routeIndices) {
        vector<GLfloat> vbo;
        int dest = routes[rtIndex].getDest();

        int numSplit = 20;
        GLfloat twicePi = 2.0f * M_PI;

        for (int i = 0; i <= numSplit; i++) {
            vbo.push_back(verticesVbo.at(dest * 2) - 0.42 * unitSize * cos(i * twicePi / (float)numSplit));
            vbo.push_back(verticesVbo.at(dest * 2 + 1) + 0.42 * unitSize* sin(i * twicePi / (float)numSplit));
        }

        routesDestVbos.push_back(vbo);
    }
}

vector<GLfloat> MatrixView::calcCatmullRomSplineCurvePoints(vector<GLfloat> &routeArcVbo, int numSplits)
{
    vector<GLfloat> result;

    int n = routeArcVbo.size() / 2;

    for (int j = 0; j < n - 1; ++j) {
        double px0, px1, px2, px3;
        double py0, py1, py2, py3;

        if (j == 0) px0 = routeArcVbo.at((n - 1) * 2);
        else px0 = routeArcVbo.at((j - 1) * 2);
        px1 = routeArcVbo.at(j * 2);
        px2 = routeArcVbo.at((j + 1) * 2);
        if (j == n - 1 || j == n - 2) px3 = routeArcVbo.at(0);
        else px3 = routeArcVbo.at((j + 2) * 2);

        if (j == 0) py0 = routeArcVbo.at((n - 1) * 2 + 1);
        else py0 = routeArcVbo.at((j - 1) * 2 + 1);
        py1 = routeArcVbo.at(j * 2 + 1);
        py2 = routeArcVbo.at((j + 1) * 2 + 1);
        if (j == n - 1 || j == n - 2) py3 = routeArcVbo.at(1);
        else py3 = routeArcVbo.at((j + 2) * 2 + 1);

        for (int i = 0; i < numSplits / n; ++i) {
            double t = (double)i / (double)(numSplits / n);
            double tt = t * t;
            double ttt = t * t * t;

            float px = (-0.5 * px0 + 1.5 * px1 - 1.5 * px2 + 0.5 * px3) * ttt +
                         (px0 - 2.5 * px1 + 2.0 * px2 - 0.5 * px3) * tt + (-0.5 * px0 + 0.5 * px2) * t + px1;
            float py = (-0.5 * py0 + 1.5 * py1 - 1.5 * py2 + 0.5 * py3) * ttt +
                         (py0 - 2.5 * py1 + 2.0 * py2 - 0.5 * py3) * tt + (-0.5 * py0 + 0.5 * py2) * t + py1;
            result.push_back(px);
            result.push_back(py);
        }
    }

    return result;
}

void MatrixView::updateRoutesCurveVbos(vector< vector<GLfloat> > &routesCurveVbos, vector< vector<GLfloat> > &routesArcVbos)
{
    routesCurveVbos.clear();

    for (int i = 0; i < (int)routesArcVbos.size(); ++i) {
        routesCurveVbos.push_back(calcCatmullRomSplineCurvePoints(routesArcVbos.at(i)));
    }
}

void MatrixView::updateRoutesCbo(vector<GLfloat> &routesCbo, QVector<Route> &routes, QVector<int> &routeIndices, QList< QPair<float, QColor> > &routeColorOfPoints)
{
    routesCbo.clear();

    foreach (int rtIndex, routeIndices) {
        QColor qColor;

        if (routeColorMetric == "distinguishable" && routeColorOfPoints != color->suggestedRouteColorOfPoints) {
            qColor = color->routeColors.at(rtIndex % color->routeColors.size());
        } else {
            float val = routes[rtIndex].getMetric(routeColorMetric);
            float ratio = val / displayedGraph->getMetric("route", routeColorMetric, "max");

            qColor = color->ratioToHeatMapColor(ratio, routeColorOfPoints);
        }

        routesCbo.push_back(qColor.redF());
        routesCbo.push_back(qColor.greenF());
        routesCbo.push_back(qColor.blueF());
        routesCbo.push_back(qColor.alphaF());
    }
}

// TODO: need to think better structure later
void MatrixView::updateEdgesVbos(vector< vector<GLfloat> > &edgesVbos,  vector<GLfloat> &verticesVbo, QString edgesVboType) // edgesVboType: displayed, suggested
{
    if (edgesVboType == "displayed") {
        edgesVbos.clear();

        // TODO: later change here. route should have edge index in a class member
        // make a set of displayedEdgeIndices and edges used by displayed routes or suggested routes

        displayedEdgeIndices.clear(); // added on June 21st

        foreach(int rtIndex, displayedRouteIndices) {
            QVector<int> arcs = displayedGraph->getRouteAt(rtIndex).getArcs();
            for (int i = 0; i < arcs.size() - 1; ++i) {
                int eIndex = displayedGraph->getEdgeIndexWithSourceTarget(arcs.at(i), arcs.at(i+1));
                if (eIndex >= 0) {
                    displayedEdgeIndices.push_back(eIndex);
                }
            }
        }
        QSet<int> uniqDisplayedEdgeIndicesSet = QSet<int>::fromList(QList<int>::fromVector(displayedEdgeIndices));
        displayedEdgeIndices = uniqDisplayedEdgeIndicesSet.toList().toVector();
        qSort(displayedEdgeIndices);

        foreach(int eIndex, displayedEdgeIndices) {
            vector<GLfloat> vbo;
            int source = displayedGraph->getEdgeAt(eIndex).getSource();
            int target = displayedGraph->getEdgeAt(eIndex).getTarget();

            vbo.push_back(verticesVbo.at(source * 2));
            vbo.push_back(verticesVbo.at(source * 2 + 1));
            vbo.push_back(verticesVbo.at(target * 2));
            vbo.push_back(verticesVbo.at(source * 2 + 1));
            vbo.push_back(verticesVbo.at(target * 2));
            vbo.push_back(verticesVbo.at(target * 2 + 1));

            edgesVbos.push_back(vbo);
        }
    } else if (edgesVboType == "suggested") {
        suggestedEdgesVbos.clear();

        QList<int> tmpSuggestedEdgeIndices;
        // TODO: handling unused edge shoule be reconsidered
        //       to keep consistent, the edge drawn as background is not reflected a load of new altenative route
        //       (it is compicated to handle edges that have load = 0)
//        foreach(Route rt, suggestedRoutes) {
//            QVector<int> arcs = rt.getArcs();
//            for (int i = 0; i < arcs.size() - 1; ++i) {
//                int eIndex = displayedGraph->getEdgeIndexWithSourceTarget(arcs.at(i), arcs.at(i+1));
//                if (eIndex >= 0) {
//                    tmpSuggestedEdgeIndices.push_back(eIndex);
//                }
//            }
//        }

//        QSet<int> tmpSuggestedEdgeIndicesSet = QSet<int>::fromList(tmpSuggestedEdgeIndices);
//        // obtain only indices which are not in the displayed edge indices
//        QSet<int> displayedEdgeIndicesSet = QSet<int>::fromList(QList<int>::fromVector(displayedEdgeIndices));
//        tmpSuggestedEdgeIndicesSet -= displayedEdgeIndicesSet;
//        suggestedEdgeIndices = tmpSuggestedEdgeIndicesSet.toList().toVector();

//        foreach(int eIndex, suggestedEdgeIndices) {
//            vector<GLfloat> vbo;
//            int source = displayedGraph->getEdgeAt(eIndex).getSource();
//            int target = displayedGraph->getEdgeAt(eIndex).getTarget();

//            vbo.push_back(suggestedVerticesVbo.at(source * 2));
//            vbo.push_back(suggestedVerticesVbo.at(source * 2 + 1));
//            vbo.push_back(suggestedVerticesVbo.at(target * 2));
//            vbo.push_back(suggestedVerticesVbo.at(source * 2 + 1));
//            vbo.push_back(suggestedVerticesVbo.at(target * 2));
//            vbo.push_back(suggestedVerticesVbo.at(target * 2 + 1));

//            suggestedEdgesVbos.push_back(vbo);
//        }

        foreach(Route rt, suggestedRoutes) {
            QVector<int> arcs = rt.getArcs();
            for (int i = 0; i < arcs.size() - 1; ++i) {
                vector<GLfloat> vbo;
                int source = arcs.at(i);
                int target = arcs.at(i+1);

                vbo.push_back(suggestedVerticesVbo.at(source * 2));
                vbo.push_back(suggestedVerticesVbo.at(source * 2 + 1));
                vbo.push_back(suggestedVerticesVbo.at(target * 2));
                vbo.push_back(suggestedVerticesVbo.at(source * 2 + 1));
                vbo.push_back(suggestedVerticesVbo.at(target * 2));
                vbo.push_back(suggestedVerticesVbo.at(target * 2 + 1));

                suggestedEdgesVbos.push_back(vbo);
            }
        }

    }
}

// TODO: need to think better structure later
void MatrixView::updateEdgesCbo(vector<GLfloat> &edgesCbo, QString edgesVboType) // edgesVboType: displayed, suggested
{
    if (edgesVboType == "displayed") {
        edgesCbo.clear();

        foreach(int eIndex, displayedEdgeIndices) {
            float val = displayedGraph->getEdgeAt(eIndex).getMetric(edgeColorMetric);
            float ratio = val / displayedGraph->getMetric("edge", edgeColorMetric, "max");
            QColor qColor = color->ratioToHeatMapColor(ratio, color->edgeColorOfPoints);
            edgesCbo.push_back(qColor.redF());
            edgesCbo.push_back(qColor.greenF());
            edgesCbo.push_back(qColor.blueF());
            edgesCbo.push_back(1.0);
        }
    } else if (edgesVboType == "suggested") {
        suggestedEdgesCbo.clear();

        foreach(Route rt, suggestedRoutes) {
            QVector<int> arcs = rt.getArcs();
            float byte = rt.getByte();
            for (int i = 0; i < arcs.size() - 1; ++i) {
                int eIndex = displayedGraph->getEdgeIndexWithSourceTarget(arcs.at(i), arcs.at(i+1));

                float val = 0.0;
                if (eIndex >= 0) val = displayedGraph->getEdgeAt(eIndex).getMetric(edgeColorMetric);
                val += byte;

                float ratio = val / displayedGraph->getMetric("edge", edgeColorMetric, "max");
                QColor qColor = color->ratioToHeatMapColor(ratio, color->edgeColorOfPoints);
                suggestedEdgesCbo.push_back(qColor.redF());
                suggestedEdgesCbo.push_back(qColor.greenF());
                suggestedEdgesCbo.push_back(qColor.blueF());
                suggestedEdgesCbo.push_back(1.0);
            }
        }
    }
}

void MatrixView::updateAllDrawingInfo()
{
    if (displayedVertexIndices.size() == 0) {
        for (int i = 0; i < displayedGraph->getVertices().size(); ++i) {
            displayedVertexIndices.push_back(i);
        }
    }
    if (displayedEdgeIndices.size() == 0) {
        for (int i = 0; i < displayedGraph->getEdges().size(); ++i) {
            displayedEdgeIndices.push_back(i);
        }
    }
    if (displayedRouteIndices.size() == 0) {
        for (int i = 0; i < displayedGraph->getRoutes().size(); ++i) {
            displayedRouteIndices.push_back(i);
        }
    }

    QVector<int> allVertexIndices = displayedVertexIndices;
    allVertexIndices += suggestedVertexIndices;

    updateCoordValues(coordValues, allVertexIndices, unitBlockSize);
    updateFoldedRegionsVbo(foldedRegionsVbos, coordValues);
    updateAdjMatVboWithFolding(adjMatVbo, coordValues);
    updateVerticesVboWithFolding(verticesVbo, displayedVertexIndices, coordValues, unitBlockSize, unitFoldingSize);

    QVector<Route> routes = displayedGraph->getRoutes();
    updateRoutesArcVbos(routesArcVbos, verticesVbo, routes, displayedRouteIndices);
    QList<QPair<float,QColor>> routeColorOfPoints = color->getRouteColorOfPointsForMetric(routeColorMetric);
    updateRoutesCbo(routesCbo, routes, displayedRouteIndices, routeColorOfPoints);
    updateRoutesSrcVbos(routesSrcVbos, verticesVbo, routes, displayedRouteIndices, unitBlockSize);
    updateRoutesDestVbos(routesDestVbos, verticesVbo, routes, displayedRouteIndices, unitBlockSize);
    updateRoutesCurveVbos(routesCurveVbos, routesArcVbos);

    updateEdgesVbos(edgesVbos, verticesVbo, "displayed");
    updateEdgesCbo(edgesCbo, "displayed");

    // alternative routes
    updateVerticesVboWithFolding(suggestedVerticesVbo, suggestedVertexIndices, coordValues, unitBlockSize, unitFoldingSize);
    updateRoutesArcVbos(suggestedRoutesArcVbos, suggestedVerticesVbo, suggestedRoutes, suggestedRoutesIndices); //////////
    updateRoutesCbo(suggestedRoutesCbo, suggestedRoutes, suggestedRoutesIndices, color->suggestedRouteColorOfPoints);
    updateRoutesSrcVbos(suggestedRoutesSrcVbos, suggestedVerticesVbo, suggestedRoutes, suggestedRoutesIndices, unitBlockSize);
    updateRoutesDestVbos(suggestedRoutesDestVbos, suggestedVerticesVbo, suggestedRoutes, suggestedRoutesIndices, unitBlockSize);
    updateRoutesCurveVbos(suggestedRoutesCurveVbos, suggestedRoutesArcVbos);

    updateEdgesVbos(suggestedEdgesVbos, suggestedVerticesVbo, "suggested");
    updateEdgesCbo(suggestedEdgesCbo, "suggested");

    if (mouseMode == "Lens") updateAllDrawingInfoForLens();
}

void MatrixView::updateAllDrawingInfoForLens()
{
    QVector<int> allVertexIndices = displayedVertexIndices;
    allVertexIndices += suggestedVertexIndices;
    updateCoordValues(coordValuesForLens, allVertexIndices, unitBlockSizeForLens);
    updateFoldedRegionsVbo(foldedRegionsVbosForLens, coordValuesForLens);
    updateAdjMatVboWithFolding(adjMatVboForLens, coordValuesForLens);
    updateVerticesVboWithFolding(verticesVboForLens, displayedVertexIndices, coordValuesForLens, unitBlockSize, unitFoldingSize);

    QVector<Route> routes = displayedGraph->getRoutes();
    updateRoutesArcVbos(routesArcVbosForLens, verticesVboForLens, routes, displayedRouteIndices);
    updateRoutesSrcVbos(routesSrcVbosForLens, verticesVboForLens, routes, displayedRouteIndices, unitBlockSizeForLens);
    updateRoutesDestVbos(routesDestVbosForLens, verticesVboForLens, routes, displayedRouteIndices, unitBlockSizeForLens);
    updateRoutesCurveVbos(routesCurveVbosForLens, routesArcVbosForLens);
    updateEdgesVbos(edgesVbosForLens, verticesVboForLens, "displayed");

    updateRoutesArcVbos(suggestedRoutesArcVbosForLens, verticesVboForLens, suggestedRoutes, suggestedRoutesIndices);
    updateRoutesSrcVbos(suggestedRoutesSrcVbosForLens, verticesVboForLens, suggestedRoutes, suggestedRoutesIndices, unitBlockSizeForLens);
    updateRoutesDestVbos(suggestedRoutesDestVbosForLens, verticesVboForLens, suggestedRoutes, suggestedRoutesIndices, unitBlockSizeForLens);
    updateRoutesCurveVbos(suggestedRoutesCurveVbosForLens, suggestedRoutesArcVbosForLens);
    updateEdgesVbos(suggestedEdgesVbosForLens, verticesVboForLens, "suggested");
}

void MatrixView::updateColorsFromStatView()
{
    QVector<Route> routes = displayedGraph->getRoutes();
    QList<QPair<float,QColor>> routeColorOfPoints = color->getRouteColorOfPointsForMetric(routeColorMetric);
    updateRoutesCbo(routesCbo, routes, displayedRouteIndices, routeColorOfPoints);
    updateEdgesCbo(edgesCbo, "displayed");
    update();
}

void MatrixView::updateDisplayedElementsIndicies(QString elemType, QVector<int> indices)
{
    suggestedVerticesVbo.clear();
    suggestedRoutesArcVbos.clear();
    suggestedRoutesCbo.clear();
    suggestedRoutesSrcVbos.clear();
    suggestedRoutesDestVbos.clear();

    updateCoordValues(coordValues, displayedVertexIndices, unitBlockSize);
    updateFoldedRegionsVbo(foldedRegionsVbos, coordValues);
    updateAdjMatVboWithFolding(adjMatVbo, coordValues);
    updateVerticesVboWithFolding(verticesVbo, displayedVertexIndices, coordValues, unitBlockSize, unitFoldingSize);

    if (elemType == "edge") {
        setDisplayedEdgeIndices(indices);

        updateEdgesVbos(edgesVbos, verticesVbo, "displayed");
        updateEdgesCbo(edgesCbo, "displayed");
    } else if (elemType == "route") {
        setDisplayedRouteIndices(indices);
        // TODO: this is probably slow think later
        QSet<int> displayedVertexIndicesSet = QSet<int>::fromList(displayedVertexIndices.toList());
        foreach(int rIndex, displayedRouteIndices) {
            displayedVertexIndicesSet.unite(QSet<int>::fromList(displayedGraph->getRouteAt(rIndex).getArcs().toList()));
        }
        displayedVertexIndices = displayedVertexIndicesSet.toList().toVector();
        updateVerticesVboWithFolding(verticesVbo, displayedVertexIndices, coordValues, unitBlockSize, unitFoldingSize);

        QVector<Route> routes = displayedGraph->getRoutes();
        updateRoutesArcVbos(routesArcVbos, verticesVbo, routes, displayedRouteIndices);
        QList<QPair<float,QColor>> routeColorOfPoints = color->getRouteColorOfPointsForMetric(routeColorMetric);
        updateRoutesCbo(routesCbo, routes, displayedRouteIndices, routeColorOfPoints);
        updateRoutesSrcVbos(routesSrcVbos, verticesVbo, routes, displayedRouteIndices, unitBlockSize);
        updateRoutesDestVbos(routesDestVbos, verticesVbo, routes, displayedRouteIndices, unitBlockSize);
        updateRoutesCurveVbos(routesCurveVbos, routesArcVbos);

        updateEdgesVbos(edgesVbos, verticesVbo, "displayed");
        updateEdgesCbo(edgesCbo, "displayed");

    } else if (elemType == "vertex") {
        setDisplayedVertexIndices(indices);

        updateVerticesVboWithFolding(verticesVbo, displayedVertexIndices, coordValues, unitBlockSize, unitFoldingSize);

        // TODO: consider using ibo
        QVector<Route> routes = displayedGraph->getRoutes();
        updateRoutesArcVbos(routesArcVbos, verticesVbo, routes, displayedRouteIndices);
        QList<QPair<float,QColor>> routeColorOfPoints = color->getRouteColorOfPointsForMetric(routeColorMetric);
        updateRoutesCbo(routesCbo, routes, displayedRouteIndices, routeColorOfPoints);
        updateRoutesSrcVbos(routesSrcVbos, verticesVbo, routes, displayedRouteIndices, unitBlockSize);
        updateRoutesDestVbos(routesDestVbos, verticesVbo, routes, displayedRouteIndices, unitBlockSize);
        updateRoutesCurveVbos(routesCurveVbos, routesArcVbos);

        updateEdgesVbos(edgesVbos, verticesVbo, "displayed");
        updateEdgesCbo(edgesCbo, "displayed");
    }

    if (mouseMode == "Lens") updateAllDrawingInfoForLens();

    update();
}

void MatrixView::suggestAltRoutesForDisplayedRoutes()
{
    //routeGuide.initilize(graph, adjMat);
    routeGuide.initilize(displayedGraph, displayedAdjMat);

    if (suggestedRoutesIndices.size() > 0) {
        // if there is already alt routes, clear alt routes
        suggestedRoutes.clear();
        suggestedRoutesIndices.clear();
        suggestedVertexIndices.clear();
        suggestedEdgeIndices.clear();
    } else {
        suggestedRoutes.clear();
        suggestedRoutesIndices.clear();
        suggestedVertexIndices.clear();

        // to suggest from longer route, prepare decending order routes (routes are sorted with length by inc order)
        // TODO: consider how to make a dec order corresponding "loadFirst"
        QVector<int> decOrderDisplayedRouteIndices = displayedRouteIndices;
        qSort(decOrderDisplayedRouteIndices.begin(), decOrderDisplayedRouteIndices.end(), qGreater<int>());

        ///
        /// here is a main loop for suggesting all alternative routes corresponding to displayed routes
        ///
        int count = 0;
        int improveCount = 0;
        float totalImproveRate = 0;
        float maxImproveRate = 0;
        float minImproveRate = 100;
        foreach(int rtIndex, decOrderDisplayedRouteIndices) {
            /// 1. suggest alt route
            Route originalRoute = displayedGraph->getRouteAt(rtIndex);

            Route altRoute = routeGuide.suggestRoute(routingGuidePriority, originalRoute, routingGuideType, routingGuideLengthTolerance, routingGuideLoadTolerance);

            //qDebug() << "alt: " << altRoute.getArcs();
            if (originalRoute.getMaxEdgeByte() - altRoute.getMaxEdgeByte() != 0) {
                float improveRate = (float)(originalRoute.getMaxEdgeByte() - altRoute.getMaxEdgeByte()) / (float)originalRoute.getMaxEdgeByte() * 100.0;
                qDebug() << "improve " << improveRate << "%";
                if (improveRate > maxImproveRate) maxImproveRate = improveRate;
                if (improveRate < minImproveRate) minImproveRate = improveRate;
                totalImproveRate += improveRate;
                improveCount++;
            }
            //qDebug() << "==== " << rtIndex << "(" << count + 1 << "/" << decOrderDisplayedRouteIndices.size() << ")" << " =====";
            //qDebug() << originalRoute.getLength()
            //         << " => " << altRoute.getLength() << "(improve " << (float)(originalRoute.getLength() - altRoute.getLength()) / (float)originalRoute.getLength() * 100.0 << "%)";
            //qDebug() << originalRoute.getMaxEdgeByte()
            //         << " => " << altRoute.getMaxEdgeByte() << "(improve " << (float)(originalRoute.getMaxEdgeByte() - altRoute.getMaxEdgeByte()) / (float)originalRoute.getMaxEdgeByte() * 100.0 << "%)";
            //qDebug();
            /// 2. update temporary graph in RouteGuite
            routeGuide.updateGraphForUpdate(originalRoute, altRoute);

            // TODO: check this is fine later
            /// 3. store suggested alt route
            suggestedRoutes.push_back(altRoute);
            suggestedRoutesIndices.push_back(count);
            count++;
        }
        qDebug() << improveCount << "/" << decOrderDisplayedRouteIndices.size() << "routes improved";
        qDebug() << "average: " << totalImproveRate / (float)improveCount << "(max:" << maxImproveRate << "%, " << "min:" << minImproveRate << "%)";

        // TODO: should change data structure later
        QSet<int> vertexIndicesSet;
        foreach(Route rt, suggestedRoutes) {
            foreach(int a, rt.getArcs()) {
                vertexIndicesSet.insert(a);
            }
        }
        QVector<int> vertexIndices = vertexIndicesSet.toList().toVector();
        suggestedVertexIndices = vertexIndices;
    }

    int totalLengthBefore = 0;
    int totalLengthAfter = 0;
    float totalMaxEdgeLoadBefore = 0.0;
    float totalMaxEdgeLoadAfter = 0.0;

    foreach(int rtIndex, displayedRouteIndices) {
        Route rt = displayedGraph->getRouteAt(rtIndex);
        totalLengthBefore += rt.getLength();
        totalMaxEdgeLoadBefore += rt.getMaxEdgeByte();
    }
    foreach(Route rt, suggestedRoutes) {
        totalLengthAfter += rt.getLength();
        totalMaxEdgeLoadAfter += rt.getMaxEdgeByte();
    }
    totalLengthWillBeChanged = totalLengthAfter - totalLengthBefore;
    totalMaxEdgeLoadWillBeChanged = totalMaxEdgeLoadAfter - totalMaxEdgeLoadBefore;

    updateAllDrawingInfo();
    emit suggestAltRoutesForDisplayedRoutesCalled(suggestedRoutesIndices.size(), totalLengthWillBeChanged, totalMaxEdgeLoadWillBeChanged);

    update();
}

void MatrixView::suggestAltMapping(QString perfOutput, bool useMaGraphPartition)
{
    mappingGuide.initilize(graph, adjMat);
    mappingGuide.setUseMaGraphPartition(useMaGraphPartition);

    if (suggestedRoutesIndices.size() > 0) {
        // if there is already alt routes, clear alt routes
        suggestedRoutes.clear();
        suggestedRoutesIndices.clear();
        suggestedVertexIndices.clear();
        suggestedEdgeIndices.clear();
    } else {
        suggestedRoutes.clear();
        suggestedRoutesIndices.clear();
        suggestedVertexIndices.clear();

        // TODO: Remapping only can apply for the initial graph now.
        //qDebug() << displayedVertexIndices;
        QVector<int> bijection = mappingGuide.suggestMapping(displayedVertexIndices, mappingGuideType, graphDir + "route_weight_mat.csv", graphDir + "dist_mat.csv",  graphDir + "qap_intermed/", graphDir + "qapresult.txt", 1000, perfOutput);
        qDebug() << bijection;
    }
}

void MatrixView::applyAltRoutes()
{
    routeGuide.updateGraph();
    routeGuide.clearReplacableVertexMat();

    emit applyAltRoutesCalled();
    updateAllDrawingInfo();
    update();
}

void MatrixView::turnOnAutoUpdate()
{
    autoUpdate = true;
}

void MatrixView::turnOffAutoUpdate()
{
    autoUpdate = false;
}

int MatrixView::getDisplayedColSize()
{
    return (int)(numOfDisplayedCells / scale);
}

int MatrixView::getAdjustedBlockSize()
{
    int result = 1;
    int displayedColSize = getDisplayedColSize();
    if (displayedColSize > thresAdjustingResolution) {
        QVector<int> matShape = displayedAdjMat->getShape();
        QVector<int> matOrder = displayedAdjMat->getOrder();

        for (int i = 0; i < matOrder.size(); ++i) {
            // here is not precise calculation, but probably enough to judge block size
            displayedColSize /= matShape.at(matOrder.at(i));
            result *= matShape.at(matOrder.at(i));
            if (displayedColSize <= thresAdjustingResolution) break;
        }
    }

    return result;
}

int MatrixView::getAdjustedBlockSizeForLens()
{
    int result = 1;
    int displayedColSize = getDisplayedColSize();

    if (displayedColSize > thresAdjustingResolution * lensZoomLevel) {
        QVector<int> matShape = displayedAdjMat->getShape();
        QVector<int> matOrder = displayedAdjMat->getOrder();

        for (int i = 0; i < matOrder.size(); ++i) {
            // here is not precise calculation, but probably enough to judge block size
            displayedColSize /= matShape.at(matOrder.at(i));
            result *= matShape.at(matOrder.at(i));
            if (displayedColSize <= thresAdjustingResolution * lensZoomLevel) break;
        }
    }

    return result;
}

void MatrixView::changeRouteColorMapSettings(QString metric, float min, float val)
{
    routeColorMetric = metric;
    updateAllDrawingInfo();
    update();
}

void MatrixView::switchGraphAndAdjMat()
{
    if (displayedGraph == graph) {
        displayedGraph = aggregatedGraph;
    } else {
        displayedGraph = graph;
    }
    if (displayedAdjMat == adjMat) {
        displayedAdjMat = aggregatedAdjMat;
    } else {
        displayedAdjMat = adjMat;
    }

    resetDisplayedElementsIndices();
    updateAllDrawingInfo();

    update();
}
