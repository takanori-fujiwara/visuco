#include "GraphView.h"

// TODO: convert a way for rendering to using shader

 GraphView::GraphView(QWidget *parent)
{
    edgeWidthMetric = "byte";
    edgeColorMetric = "byte";
    routeWidthMetric = "byte";
    routeColorMetric = "byte";

    scale = 0.90;
    translateX = 0.0;
    translateY = 0.0;
    mouseMode = "Lasso";
    rightButtonPressed = false;

    isEdgeVisible = false;
    isRouteVisible = true;
    edgeDrawingMode = "straight"; // "straight"
    routeDrawingMode = "metro"; // "catmull-rom", "bspline", "straight", "metro"
    defaultRouteDrawingMode = "metro"; // "catmull-rom", "bspline", "straight", "metro"
    vertexDrawingMode = "smaller-on-bigger"; // "out-on-in", "in-on-out", "smaller-on-bigger"

    curveStrength = 0.5;

    nodeSizeWeightForMetro = 0.0016;
    showOnlySelectedElems = false;
    expandElemWithZoom = true;

    baseValueForRouteWidth = -1.0;
    baseValueForRouteColor = -1.0;
    //baseValueForRouteWidth = 1.41e9;
    //baseValueForRouteColor = 1.41e9;

    // general
    nodeSizeWeight = 20.0;
    nodeSizeMin = 1.0;
    edgeWidthWeight = 5.0;
    //routeWidthWeight = 0.01;
    routeWidthWeight = 0.005;
    //routeWidthWeight = 0.04;
    selfLoopRouteRatio = 1.0;// 0.4

//    nodeSizeWeight = 20.0;
//    nodeSizeMin = 1.0;
//    edgeWidthWeight = 5.0;
//    routeWidthWeight = 0.0025;
//    nodeSizeWeightForMetro = 0.0008;
//    showOnlySelectedElems = true;
//    expandElemWithZoom = false;

    // general
    //filterOutAlpha = 0.55;
    //filterOutDarker = 200;

    // less emphasis
    filterOutAlpha = 0.35;
    filterOutDarker = 300;

    // imb
//    nodeSizeWeight = 60.0;
//    nodeSizeMin = 3.5; // 3.5
//    edgeWidthWeight = 10.0;
//    routeWidthWeight = 0.03; // 0.03
//    showOnlySelectedElems = true;

    // miniamr
//    nodeSizeWeight = 20.0;
//    nodeSizeMin = 1.0;
//    edgeWidthWeight = 5.0;
//    routeWidthWeight = 0.08;//0.04
//    selfLoopRouteRatio = 0.4;// 0.4

    // multisend
//    nodeSizeWeight = 60.0;
//    nodeSizeMin = 10.0;
//    edgeWidthWeight = 5.0;
//    routeWidthWeight = 0.001;
//    //routeWidthWeight = 0.04;
//    selfLoopRouteRatio = 1.0;// 0.4

    // io
//    nodeSizeWeight = 40.0;
//    nodeSizeMin = 10.0;
//    edgeWidthWeight = 5.0;
//    routeWidthWeight = 0.002;
//    //routeWidthWeight = 0.04;
//    selfLoopRouteRatio = 1.0;// 0.4

    // miniamr theta
//    nodeSizeWeight = 10.0;
//    nodeSizeMin = 1.0;
//    edgeWidthWeight = 5.0;
//    routeWidthWeight = 0.001;//0.04
//    selfLoopRouteRatio = 0.4;// 0.4

    // minimd theta
//    nodeSizeWeight = 30.0;
//    nodeSizeMin = 1.0;
//    edgeWidthWeight = 5.0;
//    routeWidthWeight = 0.01;//0.04
//    selfLoopRouteRatio = 0.4;// 0.4
    setCurrentSettings();
}

void GraphView::updateBasicVisualEncodingParams(QString appName)
{
    // general
    nodeSizeWeight = 20.0;
    nodeSizeMin = 1.0;
    edgeWidthWeight = 5.0;
    routeWidthWeight = 0.01;
    //routeWidthWeight = 0.005;
    //routeWidthWeight = 0.04;
    selfLoopRouteRatio = 1.0;// 0.4
    showOnlySelectedElems = false;

//    nodeSizeWeight = 20.0;
//    nodeSizeMin = 1.0;
//    edgeWidthWeight = 5.0;
//    routeWidthWeight = 0.0025;
//    nodeSizeWeightForMetro = 0.0008;
//    showOnlySelectedElems = true;
//    expandElemWithZoom = false;

    // general
    //filterOutAlpha = 0.55;
    //filterOutDarker = 200;

    // less emphasis
    filterOutAlpha = 0.35;
    filterOutDarker = 300;

    if (appName == "imb-MPI1") {
        nodeSizeWeight = 60.0;
        nodeSizeMin = 3.5; // 3.5
        edgeWidthWeight = 10.0;
        routeWidthWeight = 0.03; // 0.03
        showOnlySelectedElems = true;
    } else if (appName == "miniAMR") {
        nodeSizeWeight = 20.0;
        nodeSizeMin = 1.0;
        edgeWidthWeight = 5.0;
        //routeWidthWeight = 0.08;
        routeWidthWeight = 0.005;//0.04
        //selfLoopRouteRatio = 0.25;// 0.4

        //routeWidthWeight = 0.04;
        //selfLoopRouteRatio = 0.25;

        showOnlySelectedElems = true;
    } else if (appName == "multisend") {
        nodeSizeWeight = 60.0;
        nodeSizeMin = 10.0;
        edgeWidthWeight = 5.0;
        routeWidthWeight = 0.001;
        routeWidthWeight = 0.04;
        selfLoopRouteRatio = 1.0;// 0.4
        showOnlySelectedElems = false;
    } else if (appName == "ioDefault" || appName == "ioOpt") {
        nodeSizeWeight = 40.0;
        nodeSizeMin = 10.0;
        edgeWidthWeight = 5.0;
        routeWidthWeight = 0.002;
        //routeWidthWeight = 0.04;
        selfLoopRouteRatio = 1.0;// 0.4
        showOnlySelectedElems = false;
    } else if (appName == "miniAMRTheta") {
        nodeSizeWeight = 10.0;
        nodeSizeMin = 1.0;
        edgeWidthWeight = 5.0;
        routeWidthWeight = 0.001;//0.04
        selfLoopRouteRatio = 0.4;// 0.4
        showOnlySelectedElems = false;
    } else if (appName == "miniMDTheta") {
        nodeSizeWeight = 30.0;
        nodeSizeMin = 1.0;
        edgeWidthWeight = 5.0;
        routeWidthWeight = 0.01;//0.04
        selfLoopRouteRatio = 0.4;// 0.4
        showOnlySelectedElems = false;
    }

    setCurrentSettings();
}

void GraphView::updateVisualEncodingParams(float nodeSizeWeight, float nodeSizeMin, float edgeWidthWeight, float routeWidthWeight, float selfLoopRouteRatio)
{
    this->nodeSizeWeight = nodeSizeWeight;
    this->nodeSizeMin = nodeSizeMin;
    this->edgeWidthWeight = edgeWidthWeight;
    this->routeWidthWeight = routeWidthWeight;
    this->selfLoopRouteRatio = selfLoopRouteRatio;
    updateAllDrawingInfo(false);
    update();
}

void GraphView::setCurrentSettings()
{
    QSettings settings("ANL_VIDI", "Visuco");

    if (settings.value("dataDirectorySelectionDialog/nodeSizeWeight").isValid()) {
        nodeSizeWeight = settings.value("dataDirectorySelectionDialog/nodeSizeWeight").toFloat();
    }
    if (settings.value("dataDirectorySelectionDialog/nodeSizeMin").isValid()) {
        nodeSizeMin = settings.value("dataDirectorySelectionDialog/nodeSizeMin").toFloat();
    }
    if (settings.value("dataDirectorySelectionDialog/edgeWidthWeight").isValid()) {
        edgeWidthWeight = settings.value("dataDirectorySelectionDialog/edgeWidthWeight").toFloat();
    }
    if (settings.value("dataDirectorySelectionDialog/routeWidthWeight").isValid()) {
        routeWidthWeight = settings.value("dataDirectorySelectionDialog/routeWidthWeight").toFloat();
    }
    if (settings.value("dataDirectorySelectionDialog/selfLoopRouteRatio").isValid()) {
        selfLoopRouteRatio = settings.value("dataDirectorySelectionDialog/selfLoopRouteRatio").toFloat();
    }
}

void GraphView::initializeGL()
{
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glEnable(GL_POINT_SMOOTH);
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_POLYGON_SMOOTH);
    glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
}

void GraphView::resizeGL(int width, int height)
{
    glViewport(0, 0, width, height);
    glLoadIdentity();
    this->update();
}

void GraphView::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA , GL_ONE_MINUS_SRC_ALPHA);

    float aspectRatio = (float)this->width()/(float)this->height();

    glPushMatrix();
    {
        glScalef(scale, scale * aspectRatio, scale);
        glTranslatef(translateX, translateY, 0.0);
        ///
        /// edge drawing uses gl_trianle_fan, for showing wider line routes and cost is not large (gl_line has a small size limit)
        /// route drawing used gl_line_strip, for cost is large
        ///
        drawSelectedVertices();
        drawEdges(edgeDrawingMode);
        drawVertices();
        drawRoutes(routeDrawingMode);
        drawSelectedRoutesAndEdges();

        drawLasso();
        //drawCircleAroundCursor(); // for test
    }
    glPopMatrix();
    glPushMatrix();
    {
        drawColorMaps();
        drawColorSymbols();
    }
    glPopMatrix();
    glClear(GL_DEPTH_BUFFER_BIT);

    paintLabels();
    paintPopup();
}

void GraphView::drawVertices()
{
    float scaleForVertex = 0.90;
    if (expandElemWithZoom) scaleForVertex = scale;

    for (int i = 0; i < (int)verticesVbo.size() / 2; ++i) {
        if (highlightedVertexPropertyMap.at(i)) {
            glColor4f(0.0, 0.0, 0.0, 1.0);
            glPointSize(vertexSizes.at(i) * this->devicePixelRatioF() * scaleForVertex + 3);
            glBegin(GL_POINTS);
            glVertex2d(verticesVbo.at(i * 2),  verticesVbo.at(i * 2 + 1));
            glEnd();
        }
        glColor4f(verticesCbo.at(i * 4), verticesCbo.at(i * 4 + 1), verticesCbo.at(i * 4 + 2), verticesCbo.at(i * 4 + 3));
        glPointSize(vertexSizes.at(i) * this->devicePixelRatioF() * scaleForVertex);
        glBegin(GL_POINTS);
        glVertex2d(verticesVbo.at(i * 2),  verticesVbo.at(i * 2 + 1));
        glEnd();
    }
}

void GraphView::drawEdges(QString edgeDrawingMode)
{
    if (isEdgeVisible) {
        if (edgeDrawingMode == "straight") {
            for (int i = 0; i < displayedEdgeIndices.size(); ++i) {
                glColor4f(edgesCbo.at(i * 4), edgesCbo.at(i * 4 + 1), edgesCbo.at(i * 4 + 2), edgesCbo.at(i * 4 + 3));
                glLineWidth(edgeWidths.at(i) * this->devicePixelRatioF());

                // TODO: This calculation should be done before rendering. i.e. remove sizeVbo and make new version of edgeVbo
                if (displayedVertexPropertyMap.at(edgesIbo.at(i * 2)) && displayedVertexPropertyMap.at(edgesIbo.at(i * 2 + 1))) {
                    float sX = verticesVbo.at(edgesIbo.at(i * 2) * 2);
                    float sY = verticesVbo.at(edgesIbo.at(i * 2) * 2 + 1);
                    float eX = verticesVbo.at(edgesIbo.at(i * 2 + 1) * 2);
                    float eY = verticesVbo.at(edgesIbo.at(i * 2 + 1) * 2 + 1);
                    float w2 = 0.005 * edgeWidths.at(i) * 0.5 / scale;
                    QPointF startPos(sX, sY);
                    QPointF endPos(eX, eY);
                    QLineF vec(startPos, endPos);
                    QLineF normVec = vec.normalVector().unitVector();

                    glBegin(GL_TRIANGLE_FAN);
                    glVertex2d(sX + w2 * normVec.dx(), sY + w2 * normVec.dy());
                    glVertex2d(eX + w2 * normVec.dx(), eY + w2 * normVec.dy());
                    glVertex2d(eX - w2 * normVec.dx(), eY - w2 * normVec.dy());
                    glVertex2d(sX - w2 * normVec.dx(), sY - w2 * normVec.dy());
                    glEnd();
                }
            }
        }
    }
}

void GraphView::drawRoutes(QString routeDrawingMode)
{
    if (isRouteVisible) {
        if (routeDrawingMode == "catmull-rom" || routeDrawingMode == "bspline") {
            for (int i = 0; i < displayedRouteIndices.size(); ++i) {
                glColor4f(routesCbo.at(i * 4), routesCbo.at(i * 4 + 1), routesCbo.at(i * 4 + 2), routesCbo.at(i * 4 + 3));
                glLineWidth(routeWidths.at(i) * this->devicePixelRatioF());
                glLineWidth(2.0 * this->devicePixelRatioF());
                vector<GLint> arcIbo = routesArcIbos.at(i);
                vector<GLfloat> curveVbo = routesCurveVbos.at(i);

                bool isAllVerticesDisplayed = true;
                for (int j = 0; j < (int)arcIbo.size(); ++j) {
                    if (!displayedVertexPropertyMap.at(arcIbo.at(j))) {
                        isAllVerticesDisplayed = false;
                        break;
                    }
                }

                if (isAllVerticesDisplayed) {
                    glBegin(GL_LINE_STRIP);
                    for (int j = 0; j < (int)curveVbo.size() / 2; ++j) {
                        glVertex2d(curveVbo.at(j * 2), curveVbo.at(j * 2 + 1));
                    }
                    glEnd();
                }
            }
        } else if (routeDrawingMode == "straight") {
            for (int i = 0; i < displayedRouteIndices.size(); ++i) {
                glColor4f(routesCbo.at(i * 4), routesCbo.at(i * 4 + 1), routesCbo.at(i * 4 + 2), routesCbo.at(i * 4 + 3));
                glLineWidth(routeWidths.at(i) * this->devicePixelRatioF());

                vector<GLint> arcIbo = routesArcIbos.at(i);
                bool isAllVerticesDisplayed = true;
                for (int j = 0; j < (int)arcIbo.size(); ++j) {
                    if (verticesVbo.at(arcIbo.at(j) * 2) < -1.0) {
                        isAllVerticesDisplayed = false;
                        break;
                    }
                }

                glBegin(GL_LINE_STRIP);
                for (int j = 0; j < (int)arcIbo.size(); ++j) {
                    glVertex2d(verticesVbo.at(arcIbo.at(j) * 2), verticesVbo.at(arcIbo.at(j) * 2 + 1));
                }
                glEnd();
            }
        } else if (routeDrawingMode == "metro") {
            for (int i = 0; i < displayedRouteIndices.size(); ++i) {
                glColor4f(routesCbo.at(i * 4), routesCbo.at(i * 4 + 1), routesCbo.at(i * 4 + 2), routesCbo.at(i * 4 + 3));
                glLineWidth(routeWidths.at(i) * this->devicePixelRatioF());

                vector<GLint> arcIbo = routesArcIbos.at(i);
                vector<GLfloat> curveVbo = routesCurveVbos.at(i);

                bool isAllVerticesDisplayed = true;
                for (int j = 0; j < (int)arcIbo.size(); ++j) {
                    if (verticesVbo.at(arcIbo.at(j) * 2) < -1.0) {
                        isAllVerticesDisplayed = false;
                        break;
                    }
                }

                if (isAllVerticesDisplayed) {
                    glBegin(GL_TRIANGLE_STRIP);
                    for (int j = 0; j < (int)curveVbo.size() / 2; ++j) {
                        glVertex2d(curveVbo.at(j * 2), curveVbo.at(j * 2 + 1));
                    }
                    glEnd();
                }
            }
        }
    }
}

float GraphView::basisFunc(float t)
{
    t = qAbs(t);
    switch(qFloor(t)) {
    case 0:
        return (3.0 * t * t * t - 6 * t * t + 4) / 6.0;
    case 1:
        return -qPow(t - 2.0, 3.0) / 6.0;
    default:
        return 0.0;
    }
}

vector<GLfloat> GraphView::calcCubicBSplineCurvePoints(vector<GLint> ibo, float curveStrength, int numSplits)
{
    vector<GLfloat> result;

    int n = (int)ibo.size();
    for (int i = 0; i < numSplits; ++i) {
        float t = -1.0 + (n + 1) * i / (float)numSplits;
        float x = 0.0;
        float y = 0.0;

        QVector<float> xcps;
        QVector<float> ycps;
        for (int j = -2; j < n + 2; ++j) {
            float xcp, ycp; // control points
            if (j == -2 || j == -1) {
                xcp = verticesVbo.at(ibo.at(0) * 2);
                ycp = verticesVbo.at(ibo.at(0) * 2 + 1);
            } else if (j == n+1 || j == n) {
                xcp = verticesVbo.at(ibo.at(n-1) * 2);
                ycp = verticesVbo.at(ibo.at(n-1) * 2 + 1);
            } else {
                xcp = verticesVbo.at(ibo.at(j) * 2);
                ycp = verticesVbo.at(ibo.at(j) * 2 + 1);
            }

            xcps.push_back(xcp);
            ycps.push_back(ycp);
        }

        // straightening curve by modifying control point
        if (curveStrength != 1.0) {
            for (int j = 2; j < xcps.size() - 2; ++j) {
                xcps[j] = curveStrength * xcps.at(j) + (1.0 - curveStrength) * (xcps.at(0) + (float)j * (xcps.at(xcps.size() - 1) - xcps.at(0)) / (float)(xcps.size() - 1));
                ycps[j] = curveStrength * ycps.at(j) + (1.0 - curveStrength) * (ycps.at(0) + (float)j * (ycps.at(ycps.size() - 1) - ycps.at(0)) / (float)(ycps.size() - 1));
            }
        }

        for (int j = 0; j < xcps.size(); ++j) {
            x += xcps[j] * basisFunc(t - (float)(j - 2));
            y += ycps[j] * basisFunc(t - (float)(j - 2));
        }

        result.push_back(x);
        result.push_back(y);
    }

    return result;
}

vector<GLfloat> GraphView::calcCatmullRomSplineCurvePoints(vector<GLint> ibo, int numSplits)
{
    vector<GLfloat> result;

    int n = (int)ibo.size();

    for (int j = 0; j < n; ++j) {
        double px0, px1, px2, px3;
        double py0, py1, py2, py3;

        if (j == 0) {
            px0 = verticesVbo.at(ibo.at(0    ) * 2) + 0.000001;
            py0 = verticesVbo.at(ibo.at(0    ) * 2 + 1) + 0.000001;
        } else {
            px0 = verticesVbo.at(ibo.at(j - 1) * 2);
            py0 = verticesVbo.at(ibo.at(j - 1) * 2 + 1);
        }

        px1 = verticesVbo.at(ibo.at(j) * 2);
        py1 = verticesVbo.at(ibo.at(j) * 2 + 1);

        if (j == n - 1) {
            px2 = verticesVbo.at(ibo.at(n - 1) * 2) + 0.000001;
            py2 = verticesVbo.at(ibo.at(n - 1) * 2 + 1) + 0.000001;
        } else {
            px2 = verticesVbo.at(ibo.at(j + 1) * 2);
            py2 = verticesVbo.at(ibo.at(j + 1) * 2 + 1);
        }

        if (j == n - 2) {
            px3 = verticesVbo.at(ibo.at(n - 1) * 2) + 0.000001;
            py3 = verticesVbo.at(ibo.at(n - 1) * 2 + 1) + 0.000001;
        } else if (j == n - 1) {
            px3 = verticesVbo.at(ibo.at(n - 1) * 2) - 0.000001;
            py3 = verticesVbo.at(ibo.at(n - 1) * 2 + 1) - 0.000001;
        } else {
            px3 = verticesVbo.at(ibo.at(j + 2) * 2);
            py3 = verticesVbo.at(ibo.at(j + 2) * 2 + 1);
        }

        double alpha = 0.5;
        // TODO: here is temporal code
        alpha = curveStrength;
        double t0 = 0.0;
        double t1 = pow(sqrt((px1 - px0) * (px1 - px0) + (py1 - py0) * (py1 - py0)), alpha) + t0;
        double t2 = pow(sqrt((px2 - px1) * (px2 - px1) + (py2 - py1) * (py2 - py1)), alpha) + t1;
        double t3 = pow(sqrt((px3 - px2) * (px3 - px2) + (py3 - py2) * (py3 - py2)), alpha) + t2;

        for (int i = 0; i <= numSplits; ++i) {
            double t = (double)i / (double)(numSplits / n);
            if (t >= t1 && t <= t2) {

                double t1_t = t1 - t;
                double t2_t = t2 - t;
                double t3_t = t3 - t;
                double t_t0 = t - t0;
                double t_t1 = t - t1;
                double t_t2 = t - t2;
                double t1_t0 = t1 - t0;
                double t2_t1 = t2 - t1;
                double t3_t2 = t3 - t2;
                double t2_t0 = t2 - t0;
                double t3_t1 = t3 - t1;

                double ax1 = px0 * t1_t / t1_t0 + px1 * t_t0 / t1_t0;
                double ax2 = px1 * t2_t / t2_t1 + px2 * t_t1 / t2_t1;
                double ax3 = px2 * t3_t / t3_t2 + px3 * t_t2 / t3_t2;
                double bx1 = ax1 * t2_t / t2_t0 + ax2 * t_t0 / t2_t0;
                double bx2 = ax2 * t3_t / t3_t1 + ax3 * t_t1 / t3_t1;
                double cx  = bx1 * t2_t / t2_t1 + bx2 * t_t1 / t2_t1;

                double ay1 = py0 * t1_t / t1_t0 + py1 * t_t0 / t1_t0;
                double ay2 = py1 * t2_t / t2_t1 + py2 * t_t1 / t2_t1;
                double ay3 = py2 * t3_t / t3_t2 + py3 * t_t2 / t3_t2;
                double by1 = ay1 * t2_t / t2_t0 + ay2 * t_t0 / t2_t0;
                double by2 = ay2 * t3_t / t3_t1 + ay3 * t_t1 / t3_t1;
                double cy  = by1 * t2_t / t2_t1 + by2 * t_t1 / t2_t1;

                result.push_back(cx);
                result.push_back(cy);
            }
        }
    }

    return result;
}

void GraphView::drawLasso()
{
    glColor4f(1.0, 0.1, 0.1, 1.0);
    glLineWidth(2.0 * (float)devicePixelRatio());

    glLineStipple(4, 0xAAAA);
    glEnable(GL_LINE_STIPPLE);
    glVertexPointer(2, GL_FLOAT, 0, &lassoPoints[0]);
    glEnableClientState(GL_VERTEX_ARRAY);
    glDrawArrays(GL_LINE_STRIP, 0, lassoPoints.size() / 2);
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisable(GL_LINE_STIPPLE);
}

void GraphView::drawColorMap(float x, float y, float w, float h, QList< QPair<float, QColor> > colorOfPoints)
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

void GraphView::drawSelectedRoutesAndEdges()
{
    if (selectedEdgeIndices.size() > 0) {
        if (edgeDrawingMode == "straight") {
            for (int i = 0; i < selectedEdgeIndices.size(); ++i) {
                glColor4f(selectedEdgesCbo.at(i * 4), selectedEdgesCbo.at(i * 4 + 1), selectedEdgesCbo.at(i * 4 + 2), selectedEdgesCbo.at(i * 4 + 3));
                glLineWidth(selectedEdgeWidths.at(i) * this->devicePixelRatioF());
                if (verticesVbo.at(selectedEdgesIbo.at(i * 2) * 2) >= -1.0 && verticesVbo.at(selectedEdgesIbo.at(i * 2 + 1) * 2) >= -1.0) {
                    glBegin(GL_LINES);
                    glVertex2d(verticesVbo.at(selectedEdgesIbo.at(i * 2) * 2), verticesVbo.at(selectedEdgesIbo.at(i * 2) * 2 + 1));
                    glVertex2d(verticesVbo.at(selectedEdgesIbo.at(i * 2 + 1) * 2), verticesVbo.at(selectedEdgesIbo.at(i * 2 + 1) * 2 + 1));
                    glEnd();
                }
            }
        }
    }
    if (selectedRouteIndices.size() > 0 ) {
        if (routeDrawingMode == "catmull-rom" || routeDrawingMode == "bspline") {
            for (int i = 0; i < selectedRouteIndices.size(); ++i) {
                glColor4f(selectedRoutesCbo.at(i * 4), selectedRoutesCbo.at(i * 4 + 1), selectedRoutesCbo.at(i * 4 + 2), selectedRoutesCbo.at(i * 4 + 3));
                glLineWidth(selectedRouteWidths.at(i) * this->devicePixelRatioF());

                glBegin(GL_LINE_STRIP);
                vector<GLfloat> curveVbo = selectedRoutesCurveVbos.at(i);

                for (int j = 0; j < (int)curveVbo.size() / 2; ++j) {
                    glVertex2d(curveVbo.at(j * 2), curveVbo.at(j * 2 + 1));
                }
                glEnd();
            }
        } else if (routeDrawingMode == "straight") {
            for (int i = 0; i < selectedRouteIndices.size(); ++i) {
                glColor4f(selectedRoutesCbo.at(i * 4), selectedRoutesCbo.at(i * 4 + 1), selectedRoutesCbo.at(i * 4 + 2), selectedRoutesCbo.at(i * 4 + 3));
                glLineWidth(selectedRouteWidths.at(i) * this->devicePixelRatioF());

                glBegin(GL_LINE_STRIP);
                vector<GLint> arcIbo = selectedRoutesArcIbos.at(i);
                for (int j = 0; j < (int)arcIbo.size(); ++j) {
                        glVertex2d(verticesVbo.at(arcIbo.at(j) * 2), verticesVbo.at(arcIbo.at(j) * 2 + 1));
                }
                glEnd();
            }
        } else if (routeDrawingMode == "metro") {
            for (int i = 0; i < selectedRouteIndices.size(); ++i) {
                glColor4f(selectedRoutesCbo.at(i * 4), selectedRoutesCbo.at(i * 4 + 1), selectedRoutesCbo.at(i * 4 + 2), selectedRoutesCbo.at(i * 4 + 3));
                glLineWidth(selectedRouteWidths.at(i) * this->devicePixelRatioF());


                vector<GLfloat> curveVbo = selectedRoutesCurveVbos.at(i);
                glBegin(GL_TRIANGLE_STRIP);
                for (int j = 0; j < (int)curveVbo.size() / 2; ++j) {
                    glVertex2d(curveVbo.at(j * 2), curveVbo.at(j * 2 + 1));
                }
                glEnd();
            }
        }
    }
}

void GraphView::drawSelectedVertices()
{
    if (selectedVerticesVbo.size() > 0) {
        for (int i = 0; i < selectedVerticesVbo.size() / 2; ++i) {
            glColor4f(selectedVerticesCbo.at(i * 4), selectedVerticesCbo.at(i * 4 + 1), selectedVerticesCbo.at(i * 4 + 2), selectedVerticesCbo.at(i * 4 + 3));
            glPointSize(selectedVertexSizes.at(i) * this->devicePixelRatioF() * scale);
            glBegin(GL_POINTS);
            glVertex2d(selectedVerticesVbo.at(i * 2),  selectedVerticesVbo.at(i * 2 + 1));
            glEnd();
        }
    }
}

void GraphView::drawColorMaps()
{   
    if (isEdgeVisible) {
        drawColorMap(0.63, -0.84, 0.35, 0.03, color->edgeColorOfPoints);
    }
    if (isRouteVisible && routeColorMetric != "distinguishable") {
        drawColorMap(0.63, -0.90, 0.35, 0.03, color->getRouteColorOfPointsForMetric(routeColorMetric));
    }
    drawColorMap(0.63, -0.96, 0.35, 0.03, color->vertexColorOfPoints);
}

void GraphView::drawColorSymbols()
{
//    QColor qColor1 = color->ratioToHeatMapColor(1.0, color->vertexColorOfPoints);
//    QColor qColor2 = color->ratioToHeatMapColor(0.0, color->vertexColorOfPoints);

//    glPointSize(10.0 * this->devicePixelRatioF());
//    glColor4f(qColor1.redF(), qColor1.greenF(), qColor1.blueF(), qColor1.alphaF());
//    glBegin(GL_POINTS);
//    glVertex2d(-0.97, -0.926);
//    glEnd();
//    glColor4f(qColor2.redF(), qColor2.greenF(), qColor2.blueF(), qColor2.alphaF());
//    glBegin(GL_POINTS);
//    glVertex2d(-0.97, -0.97);
//    glEnd();
}

void GraphView::paintLabels()
{
    // edge and route colors
    QPainter painter(this);

    float edgeValMax = 0.0;
    float edgeValMin = 0.0;
    if (displayedGraph->getNumEdges() > 0) edgeValMax = displayedGraph->getEdgeAt(displayedGraph->getNumEdges() - 1).getMetric(edgeColorMetric);

    float routeValMax = 0.0;
    float routeValMin = 0.0;

    if (displayedGraph->getNumRoutes() > 0) {
        if (baseValueForRouteColor < 0.0) {
            // default
            if (routeColorMetric == "byte") routeValMax = displayedGraph->getMaxRoutesByte();
            else if (routeColorMetric == "length") routeValMax = displayedGraph->getMaxLengthOfRoutes();
            else if (routeColorMetric == "hopbyte") routeValMax = displayedGraph->getMaxRoutesHopbyte();
        } else {
            routeValMax = baseValueForRouteColor;
        }
    }

    painter.setFont(QFont("San Francisco", 9));

    int fontW = painter.fontMetrics().ascent();
    int fontH = painter.fontMetrics().height();

    painter.setBrush(Qt::black);

    if (isEdgeVisible) {
        painter.drawText(this->width() - 13 * fontW, this->height() - 5.1 * fontH, 5 * fontW, fontH,
                         Qt::AlignVCenter | Qt::AlignLeft, QString::number(edgeValMin, 'g', 3));
        painter.drawText(this->width() - 5.5 * fontW, this->height() - 5.1 * fontH, 5 * fontW, fontH,
                         Qt::AlignVCenter | Qt::AlignRight, QString::number(edgeValMax, 'g', 3));
        painter.drawText(this->width() - 23 * fontW, this->height() - 5.9 * fontH, 9.0 * fontW, fontH,
                         Qt::AlignVCenter | Qt::AlignRight, edgeColorMetric + " on edge");
    }

    if (isRouteVisible && routeColorMetric != "distinguishable") {
        QString title = "";
        if (routeColorMetric == "byte") title = "message size";
        else if (routeColorMetric == "length") title = "length of route";
        else if (routeColorMetric == "hopbyte") title = "hop-byte";

        if (routeColorMetric == "length") {
            painter.drawText(this->width() - 13 * fontW, this->height() - 3.1 * fontH, 5 * fontW, fontH,
                             Qt::AlignVCenter | Qt::AlignLeft, QString::number((int)routeValMin));
            painter.drawText(this->width() - 5.5 * fontW, this->height() - 3.1 * fontH, 5 * fontW, fontH,
                             Qt::AlignVCenter | Qt::AlignRight, QString::number((int)routeValMax));
            painter.drawText(this->width() - 23 * fontW, this->height() - 3.9 * fontH, 9.0 * fontW, fontH,
                             Qt::AlignVCenter | Qt::AlignRight, title);
        } else {
            painter.drawText(this->width() - 13 * fontW, this->height() - 3.1 * fontH, 5 * fontW, fontH,
                             Qt::AlignVCenter | Qt::AlignLeft, QString::number(routeValMin, 'g', 3));
            painter.drawText(this->width() - 5.5 * fontW, this->height() - 3.1 * fontH, 5 * fontW, fontH,
                             Qt::AlignVCenter | Qt::AlignRight, QString::number(routeValMax, 'g', 3));
            painter.drawText(this->width() - 23 * fontW, this->height() - 3.9 * fontH, 9.0 * fontW, fontH,
                             Qt::AlignVCenter | Qt::AlignRight, title);
        }
    }

    painter.drawText(this->width() - 13 * fontW, this->height() - fontH, 5 * fontW, fontH,
                     Qt::AlignVCenter | Qt::AlignLeft, "0%");
    painter.drawText(this->width() - 5.5 * fontW, this->height() - fontH, 5 * fontW, fontH,
                     Qt::AlignVCenter | Qt::AlignRight, "100%");
    painter.drawText(this->width() - 23 * fontW, this->height() - 1.9 * fontH, 9.0 * fontW, fontH,
                     Qt::AlignVCenter | Qt::AlignRight, "ratio of in-degree");

    painter.end();
}

void GraphView::paintPopup()
{
    // edge and route colors
    QPainter painter(this);
    QColor transWhite(222, 222, 222, 220);
    painter.setBrush(transWhite);

    painter.setFont(QFont("San Francisco", 9));
    int fontH = painter.fontMetrics().height();

    int x = mouseMovePos.x();
    int y = mouseMovePos.y();
    QPolygon poly;
    int rightSpace  = this->width()  - x;
    int bottomSpace = this->height() - y;

    QStringList infos;

    if (hoveredSelectedElement.first == "vertex") {
        Vertex v = displayedGraph->getVertexAt(hoveredSelectedElement.second);

        QString indexInfo = "Index: " + QString::number(v.getIndex());
        QString rankInfo = "Ranks: ";

        if (v.getRanks().size() <= 2) {
            foreach(int r, v.getRanks()) {
                rankInfo += QString::number(r) + ",";
            }
            rankInfo.chop(1);
        } else {
            for (int i = 0; i <= 4 ; ++i) {
                if (i < 4) {
                    rankInfo += QString::number(v.getRanks().at(i)) + ",";
                } else if (i == 4) {
                    rankInfo.chop(1);
                    rankInfo += "... (" + QString::number(v.getRanks().size()) + " counts)";
                }
            }
        }

        QString coordInfo = "Coords:";
        if (v.getChildCoords().size() <= 2) {
            foreach (QVector<int> coord, v.getChildCoords()) {
                coordInfo += "(";
                foreach(int i, coord) {
                    coordInfo += QString::number(i) + ",";
                }
                coordInfo.chop(1);
                coordInfo += "),";
            }
            coordInfo.chop(1);
        } else {
            for (int i = 0; i <= 2 ; ++i) {
                if (i < 2) {
                    coordInfo += "(";
                    foreach(int i, v.getChildCoords().at(i)) {
                        coordInfo += QString::number(i) + ",";
                    }
                    coordInfo.chop(1);
                    coordInfo += "),";
                } else if (i == 2) {
                    coordInfo.chop(1);
                    coordInfo += "... (" + QString::number(v.getChildCoords().size()) + " counts)";
                }
            }
        }

        QString inDegInfo = "In-degree: " + QString::number(v.getInDegOfEdgesByte(), 'g', 3);
        QString outDegInfo = "Out-degree: " + QString::number(v.getOutDegOfEdgesByte(), 'g', 3);

        infos << indexInfo << rankInfo << coordInfo << inDegInfo << outDegInfo;

    } else if (hoveredSelectedElement.first == "edge") {
        Edge e = displayedGraph->getEdgeAt(hoveredSelectedElement.second);

        QString sourceInfo = "Source: " + QString::number(e.getSource());
        QString targetInfo = "Target: " + QString::number(e.getTarget());
        QString loadInfo = "Load: " + QString::number(e.getLoad());

        infos << sourceInfo << targetInfo << loadInfo;
    } else if (hoveredSelectedElement.first == "route") {
        Route rt = displayedGraph->getRouteAt(hoveredSelectedElement.second);

        QString hopInfo = "Hops: ";
        foreach(int i, rt.getArcs()) {
            hopInfo += QString::number(i) + "-";
        }
        hopInfo.chop(1);

        QString lengthInfo = "Length: " + QString::number(rt.getLength());
        QString routeLoadInfo = "Message Size: " + QString::number(rt.getByte(), 'g', 3);
        QString heaviestLoadInfo = "Heaviest Message Size on Links: " + QString::number(rt.getMaxEdgeByte(), 'g', 3);

        infos << hopInfo << lengthInfo << routeLoadInfo << heaviestLoadInfo;

        // display related vertex indices near vertices
        foreach(int i, rt.getArcs()) {
            Vertex v = graph->getVertexAt(i);
            QPoint pos = glPosToQtPos(v.getGraphPos());
            QString indexStr = QString::number(v.getIndex());
            float r = 0.0;
            int infoPixelsWide = painter.fontMetrics().width(indexStr) + 5;
            painter.drawText(pos.x() - 0.5 * infoPixelsWide, pos.y() - 0.5 * fontH - r, infoPixelsWide, fontH, Qt::AlignVCenter | Qt::AlignLeft, indexStr);
        }
    }

    int numOfInfo = infos.size();
    QString longestText = "";
    foreach(QString s, infos) {
        if (s.size() > longestText.size()) longestText = s;
    }
    int infoPixelsWide = painter.fontMetrics().width(longestText) + 5;

    if (numOfInfo > 0) {
        // Changing popup position by the left space
        if (rightSpace  > 10 + infoPixelsWide && bottomSpace > 15 + numOfInfo * fontH) {
            poly << QPoint(x + 0,                      y + 0)
                 << QPoint(x + 8,                      y + 3)
                 << QPoint(x + 8 + infoPixelsWide, y + 3)
                 << QPoint(x + 8 + infoPixelsWide, y + 13 + numOfInfo * fontH)
                 << QPoint(x + 3,                      y + 13 + numOfInfo * fontH)
                 << QPoint(x + 3,                      y + 8);
        } else if (rightSpace <= 10 + infoPixelsWide && bottomSpace > 15 + numOfInfo * fontH) {
            poly << QPoint(x - 0,                      y + 0)
                 << QPoint(x - 8,                      y + 3)
                 << QPoint(x - 8 - infoPixelsWide, y + 3)
                 << QPoint(x - 8 - infoPixelsWide, y + 13 + numOfInfo * fontH)
                 << QPoint(x - 3,                      y + 13 + numOfInfo * fontH)
                 << QPoint(x - 3,                      y + 8);
        } else if (rightSpace > 10 + infoPixelsWide && bottomSpace <= 15 + numOfInfo * fontH) {
            poly << QPoint(x + 0,                      y - 0)
                 << QPoint(x + 8,                      y - 3)
                 << QPoint(x + 8 + infoPixelsWide, y - 3)
                 << QPoint(x + 8 + infoPixelsWide, y - 13 - numOfInfo * fontH)
                 << QPoint(x + 3,                      y - 13 - numOfInfo * fontH)
                 << QPoint(x + 3,                      y - 8);
        } else {
            poly << QPoint(x - 0,                      y - 0)
                 << QPoint(x - 8,                      y - 3)
                 << QPoint(x - 8 - infoPixelsWide, y - 3)
                 << QPoint(x - 8 - infoPixelsWide, y - 13 - numOfInfo * fontH)
                 << QPoint(x - 3,                      y - 13 - numOfInfo * fontH)
                 << QPoint(x - 3,                      y - 8);
        }
        QPainterPath path;
        path.addPolygon(poly);
        painter.drawPolygon(poly);

        // draw node info
        QString infosStr = "";
        foreach(QString s, infos) {
            infosStr += s + "\n";
        }
        infosStr.chop(1);

        // Changing popup position by the left space
        if (rightSpace  > 10 + infoPixelsWide && bottomSpace > 15 + numOfInfo * fontH) {
            painter.drawText(x + 10, y + 7, infoPixelsWide, numOfInfo * fontH, Qt::AlignVCenter | Qt::AlignLeft, infosStr);
        } else if (rightSpace <= 10 + infoPixelsWide && bottomSpace > 15 + numOfInfo * fontH) {
            painter.drawText(x - 5 - infoPixelsWide, y + 7, infoPixelsWide, numOfInfo * fontH, Qt::AlignVCenter | Qt::AlignLeft, infosStr);
        } else if (rightSpace > 10 + infoPixelsWide && bottomSpace <= 15 + numOfInfo * fontH) {
            painter.drawText(x + 10, y - 7 - numOfInfo * fontH, infoPixelsWide, numOfInfo * fontH, Qt::AlignVCenter | Qt::AlignLeft, infosStr);
        } else {
            painter.drawText(x - 5 - infoPixelsWide, y - 7 - numOfInfo * fontH, infoPixelsWide, numOfInfo * fontH, Qt::AlignVCenter | Qt::AlignLeft, infosStr);
        }
    }

    painter.end();
}

vector<GLfloat> GraphView::genCirclePoints(float cx, float cy, float r, int numSplit)
{
    vector<GLfloat> result;

    int i;

    GLfloat twicePi = 2.0f * M_PI;

    for(i = 0; i <= numSplit; i++) {
        result.push_back(cx + r * cos(i * twicePi / (float)numSplit));
        result.push_back(cy + r * sin(i * twicePi / (float)numSplit));
    }

    return result;
}

void GraphView::drawCircleAroundCursor()
{
    QPointF mouseReleasePosF = qtPosToGlPos(mouseReleasePos);
    vector<GLfloat> circlePoints = genCirclePoints(mouseReleasePosF.x(), mouseReleasePosF.y(), 0.015 / scale);

    glColor4f(0.0, 0.0, 0.0, 1.0);
    glLineWidth(1.0);
    glVertexPointer(2, GL_FLOAT, 0, &circlePoints[0]);
    glEnableClientState(GL_VERTEX_ARRAY);
    glDrawArrays(GL_LINE_STRIP, 0, circlePoints.size() / 2);
    glDisableClientState(GL_VERTEX_ARRAY);
}

QPointF GraphView::qtPosToGlPos(QPoint qtPos)
{
    float resultX =   (-1.0 + 2.0 * (float)qtPos.x() / (float)this->width()) / scale - translateX;
    float resultY = - (-1.0 + 2.0 * (float)qtPos.y() / (float)this->height()) / scale - translateY;
    return QPointF(resultX, resultY);
}

QPoint GraphView::glPosToQtPos(QPointF glPos)
{
    float resultX = 0.5 * (float)this->width()  * ( scale * (glPos.x() + translateX) + 1.0);
    float resultY = 0.5 * (float)this->height() * (-scale * (glPos.y() + translateY) + 1.0);
    return QPoint((int)resultX, (int)resultY);
}

// for zooming
void GraphView::wheelEvent(QWheelEvent *event)
{
    float sensitiveRatio = 0.02;
    scale += (event->delta() / 8.0 / 15.0) * sensitiveRatio;
    if(scale < 0.1) {
        scale = 0.1;
    }

    update();
}

void GraphView::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton) {
        if(lassoPoints.size() > 0) {
            lassoPoints.clear();
        }
    } else if (event->button() == Qt::RightButton) {
        rightButtonPressed = true;
    }

    mousePressPos = event->pos();
}

void GraphView::mouseMoveEvent(QMouseEvent *event)
{
    QPoint mousePosDelta = event->pos() - mousePressPos;
    QPointF eventPosF = qtPosToGlPos(event->pos());

    if (event->buttons()) {
        if (mouseMode == "Hand" || rightButtonPressed) {
            if (mousePosDelta.manhattanLength() > 5) {
                QPointF glPosCurrent = qtPosToGlPos(event->pos());
                QPointF glPosPre = qtPosToGlPos(mouseMovePos);
                translateX += (glPosCurrent - glPosPre).x();
                translateY += (glPosCurrent - glPosPre).y();
            }
        } else if (mouseMode == "Lasso") {
            QPointF glPos = qtPosToGlPos(event->pos());
            lassoPoints.push_back(glPos.x());
            lassoPoints.push_back(glPos.y());
        }
    } else if(!event->buttons()) {
        // hover
        vector<GLfloat> circlePoints = genCirclePoints(eventPosF.x(), eventPosF.y(), 0.03 / scale);
        selectElement(circlePoints, "selected");
    }

    mouseMovePos = event->pos();

    update();
}

void GraphView::mouseReleaseEvent(QMouseEvent *event)
{
    QPoint mousePosDelta = event->pos() - mousePressPos;
    QPointF eventPosF = qtPosToGlPos(event->pos());

    if(event->button() == Qt::LeftButton) {
        if (lassoPoints.size() > 1) {
            lassoPoints.push_back(lassoPoints.at(0));
            lassoPoints.push_back(lassoPoints.at(1));
        } else {
            if (mousePosDelta.manhattanLength() < 5) {
                vector<GLfloat> circlePoints = genCirclePoints(eventPosF.x(), eventPosF.y(), 0.03 / scale);
                selectElement(circlePoints, "displayed");
            }
        }
    } else if (event->button() == Qt::RightButton) {
        if (mousePosDelta.manhattanLength() < 5) {
            showContextMenu(event->pos());
        }
    }

    rightButtonPressed = false;
    mouseReleasePos = event->pos();

    //update();
}

bool GraphView::isInsidePolygon(QPointF point, vector<GLfloat> selectionAreaGlBuffer)
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
              isInside ^=
                      (selectionAreaGlBuffer.at(i) + (y - selectionAreaGlBuffer.at(i + 1)) / (selectionAreaGlBuffer.at(j + 1) - selectionAreaGlBuffer.at(i + 1)) * (selectionAreaGlBuffer.at(j) - selectionAreaGlBuffer.at(i))) < x;
          }
          j = i;
    }

    return isInside;
}

bool GraphView::isInsideLasso(QPointF point)
{
    return isInsidePolygon(point, lassoPoints);
}

bool GraphView::isIntersected(QPointF a, QPointF b, QPointF c, QPointF d)
{
    float ax = a.x();
    float ay = a.y();
    float bx = b.x();
    float by = b.y();
    float cx = c.x();
    float cy = c.y();
    float dx = d.x();
    float dy = d.y();

    float ta = (cx - dx) * (ay - cy) + (cy - dy) * (cx - ax);
    float tb = (cx - dx) * (by - cy) + (cy - dy) * (cx - bx);
    float tc = (ax - bx) * (cy - ay) + (ay - by) * (ax - cx);
    float td = (ax - bx) * (dy - ay) + (ay - by) * (ax - dx);

    return tc * td < 0 && ta * tb < 0;
}

bool GraphView::isIntersectedWithPolygon(QPointF a, QPointF b, vector<GLfloat> selectionAreaGlBuffer)
{
    bool result = false;
    for (int i = 0; i < (int)selectionAreaGlBuffer.size() - 2; i += 2) {
        QPointF c(selectionAreaGlBuffer.at(i), selectionAreaGlBuffer.at(i + 1));
        QPointF d(selectionAreaGlBuffer.at(i + 2), selectionAreaGlBuffer.at(i + 3));

        if (isIntersected(a, b, c, d)) {
            result = true;
            break;
        }
    }

    return result;
}

bool GraphView::isIntersectedWithLasso(QPointF a, QPointF b)
{
    return isIntersectedWithPolygon(a, b, lassoPoints);
}

QVector<int> GraphView::getVertexIndicesInsideLasso()
{
    QVector<int> result;

    foreach(int i, displayedVertexIndices) {
        if (isInsideLasso(displayedGraph->getVertexAt(i).getGraphPos())) {
            result.push_back(i);
        }
    }

    return result;
}

int GraphView::getVertexIndexInsidePolygon(QVector<int>& targetVertexIndices, vector<GLfloat> selectionAreaGlBuffer)
{
    int result = -1;

    for (int i = 0; i < targetVertexIndices.size(); ++i) {
        int index = targetVertexIndices.at(targetVertexIndices.size() - 1 - i); // to select vertex displayed front
        if (isInsidePolygon(displayedGraph->getVertexAt(index).getGraphPos(), selectionAreaGlBuffer)) {
            result = index;
            break;
        }
    }

    return result;
}

QVector<int> GraphView::getEdgeIndicesInsideLasso()
{
    QVector<int> result;
    QVector<int> vertexIndicesInsideLasso;

    foreach(int i, getVertexIndicesInsideLasso()) {
        vertexIndicesInsideLasso.push_back( displayedGraph->getVertexAt(i).getIndex() );
    }

    if (isEdgeVisible) {
        foreach(int i, displayedEdgeIndices) {
            Edge edge = displayedGraph->getEdgeAt(i);
            int source = edge.getSource();
            int target = edge.getTarget();

            if (vertexIndicesInsideLasso.contains(source) && vertexIndicesInsideLasso.contains(target)) {
                result.push_back(i);
            }
        }
    }

    return result;
}

QVector<int> GraphView::getEdgeIndicesIntersectedWithLasso()
{
    QVector<int> result;

    if (isEdgeVisible) {
        foreach(int i, displayedEdgeIndices) {
            Edge edge = displayedGraph->getEdgeAt(i);
            int source = edge.getSource();
            int target = edge.getTarget();

            if (isIntersectedWithLasso(displayedGraph->getVertexAt(source).getGraphPos(), displayedGraph->getVertexAt(target).getGraphPos())) {
                result.push_back(i);
            }
        }
    }

    return result;
}

int GraphView::getEdgeIndexIntersectedWithPolygon(QVector<int>& targetEdgeIndices, vector<GLfloat> selectionAreaGlBuffer)
{
    int result = -1;

    if (isEdgeVisible) {
        for (int i = 0; i < targetEdgeIndices.size(); ++i) {
            int index = targetEdgeIndices.at(targetEdgeIndices.size() - 1 - i); // to select heavier load edge first
            Edge edge = displayedGraph->getEdgeAt(index);
            int source = edge.getSource();
            int target = edge.getTarget();

            if (isIntersectedWithPolygon(displayedGraph->getVertexAt(source).getGraphPos(), displayedGraph->getVertexAt(target).getGraphPos(), selectionAreaGlBuffer)) {
                result = index;
                break;
            }
        }
    }

    return result;
}

QVector<int> GraphView::getRouteIndicesInsideLasso()
{
    QVector<int> result;
    QVector<int> vertexIndicesInsideLasso;

    foreach(int i, getVertexIndicesInsideLasso()) {
        vertexIndicesInsideLasso.push_back( displayedGraph->getVertexAt(i).getIndex() );
    }

    if (isRouteVisible) {
        foreach(int i, displayedRouteIndices) {
            Route route = displayedGraph->getRouteAt(i);
            QVector<int> arcs = route.getArcs();
            bool isAllArcsIncluded = true;

            // all arcs are inside lasso
            foreach(int arc, arcs) {
                if (!vertexIndicesInsideLasso.contains(arc)) {
                    isAllArcsIncluded = false;
                    break;
                }
            }

            if (isAllArcsIncluded) {
                result.push_back(i);
            }
        }
    }

    return result;
}

QVector<int> GraphView::getRouteIndicesIntersectedWithLasso()
{
    QVector<int> result;

    if (isRouteVisible) {
        foreach(int i, displayedRouteIndices) {
            Route route = displayedGraph->getRouteAt(i);
            QVector<int> arcs = route.getArcs();

            // TODO: this is not precise calculation (route is cureved line, but here considered as a straight line)
            // at least one edge in route is intersected with lasso
            for (int j = 0; j < arcs.size() - 1; ++j) {
                int source = arcs.at(j);
                int target = arcs.at(j + 1);
                if (isIntersectedWithLasso(displayedGraph->getVertexAt(source).getGraphPos(), displayedGraph->getVertexAt(target).getGraphPos())) {
                    result.push_back(i);
                    break;
                }
            }
        }
    }

    return result;
}

int GraphView::getRouteIndexIntersectedWithPolygon(QVector<int>& targetRouteIndices, vector<GLfloat> selectionAreaGlBuffer, QString selectTargetType)
{
    int result = -1;

    if (routeDrawingMode == "catmull-rom" || routeDrawingMode == "bspline") {
        vector< vector<GLfloat> > curveVbos;
        if (selectTargetType == "displayed") {
            curveVbos = routesCurveVbos;
        } else {
            curveVbos = selectedRoutesCurveVbos;
        }

        for (int i = 0; i < targetRouteIndices.size(); ++i) {
            int index = targetRouteIndices.size() - 1 - i; // to select longer length route first
            vector<float> vbo = curveVbos.at(index);

            for (int j = 0; j < (int)vbo.size() / 2 - 1; ++j) {
                QPointF sourcePos(vbo.at(j * 2),     vbo.at(j * 2 + 1));
                QPointF targetPos(vbo.at(j * 2 + 2), vbo.at(j * 2 + 3));

                if (isIntersectedWithPolygon(sourcePos, targetPos, selectionAreaGlBuffer)) {
                    result = targetRouteIndices.at(index);
                    break;
                }
            }
            if (result >= 0) break;
        }
    } else if (routeDrawingMode == "straight") {
        for (int i = 0; i < targetRouteIndices.size(); ++i) {
            int index = targetRouteIndices.size() - 1 - i; // to select longer length route first
            Route route = displayedGraph->getRouteAt(targetRouteIndices.at(index));
            QVector<int> arcs = route.getArcs();

            for (int j = 0; j < arcs.size() - 1; ++j) {
                int source = arcs.at(j);
                int target = arcs.at(j + 1);
                if (isIntersectedWithPolygon(displayedGraph->getVertexAt(source).getGraphPos(), displayedGraph->getVertexAt(target).getGraphPos(), selectionAreaGlBuffer)) {
                    result = index;
                    break;
                }
            }

            if (result >= 0) break;
        }
    } else if (routeDrawingMode == "metro") {
        vector< vector<GLfloat> > curveVbos;
        if (selectTargetType == "displayed") {
            curveVbos = routesCurveVbos;
        } else {
            curveVbos = selectedRoutesCurveVbos;
        }

        for (int i = 0; i < targetRouteIndices.size(); ++i) {
            int index = targetRouteIndices.size() - 1 - i; // to select longer length route first
            vector<float> vbo = curveVbos.at(index);

            for (int j = 0; j < (int)vbo.size() / 8; ++j) {
                QPointF sourcePos(vbo.at(j * 8),     vbo.at(j * 8 + 1));
                QPointF targetPos(vbo.at(j * 8 + 4), vbo.at(j * 8 + 5));

                if (isIntersectedWithPolygon(sourcePos, targetPos, selectionAreaGlBuffer)) {
                    result = targetRouteIndices.at(index);
                    break;
                }
            }
            if (result >= 0) break;
        }
    }
    return result;
}

void GraphView::selectElement(vector<GLfloat> selectionAreaGlBuffer, QString selectTargetType)
{
    QVector<int> targetVertexIndices;
    QVector<int> targetEdgeIndices;
    QVector<int> targetRouteIndices;
    if (selectTargetType == "displayed") {
        targetVertexIndices = displayedVertexIndices;
        targetEdgeIndices = displayedEdgeIndices;
        targetRouteIndices = displayedRouteIndices;
    } else if (selectTargetType == "selected") {
        targetVertexIndices = selectedVertexIndices;
        targetEdgeIndices = selectedEdgeIndices;
        targetRouteIndices = selectedRouteIndices;
    }

    // selection priority: 1. vertex, 2. route, 3. edge
    // if route is selected, vertices are used for the route also selected. Edges used for the route are also displayed in matrix view (but not selected)
    // if edge is selected, vertices used for the edge are also selected
    // TODO: updating indices should be separated in different function

    int vIndex = getVertexIndexInsidePolygon(targetVertexIndices, selectionAreaGlBuffer);
    if (vIndex >= 0) {
        if (selectTargetType == "displayed") {
            int ind = selectedVertexIndices.indexOf(vIndex);
            if (ind >= 0) {
                selectedVertexIndices.removeAt(ind);
            } else {
                selectedVertexIndices.push_back(vIndex);
            }
            updateSelectedElementDrawingInfo();
        } else if (selectTargetType == "selected") {
            hoveredSelectedElement.first = "vertex";
            hoveredSelectedElement.second = vIndex;
        }
        return;
    }

    int rtIndex = getRouteIndexIntersectedWithPolygon(targetRouteIndices, selectionAreaGlBuffer, selectTargetType);

    if (rtIndex >= 0) {
        if (selectTargetType == "displayed") {
            int ind = selectedRouteIndices.indexOf(rtIndex);
            if (ind >= 0) {
                selectedRouteIndices.removeAt(ind);
                foreach(int vIndex, displayedGraph->getRouteAt(rtIndex).getArcs()) {
                    int ind = selectedVertexIndices.indexOf(vIndex);
                    if (ind >= 0) {
                        selectedVertexIndices.removeAt(ind);
                    }
                }
            } else {
                selectedRouteIndices.push_back(rtIndex);
                foreach(int vIndex, displayedGraph->getRouteAt(rtIndex).getArcs()) {
                    if (!selectedVertexIndices.contains(vIndex)) {
                        selectedVertexIndices.push_back(vIndex);
                    }
                }
            }
            updateSelectedElementDrawingInfo();
        } else if (selectTargetType == "selected") {
            hoveredSelectedElement.first = "route";
            hoveredSelectedElement.second = rtIndex;
        }
        return;
    }

    int eIndex = getEdgeIndexIntersectedWithPolygon(targetEdgeIndices, selectionAreaGlBuffer);
    if (eIndex >= 0) {
        if (selectTargetType == "displayed") {
            int ind = selectedEdgeIndices.indexOf(eIndex);
            Edge edge = displayedGraph->getEdgeAt(eIndex);

            if (ind >= 0) {
                selectedEdgeIndices.removeAt(ind);
                foreach(int vIndex, QVector<int>({edge.getSource(), edge.getTarget()})) {
                    int i = selectedVertexIndices.indexOf(vIndex);
                    if (i >= 0) {
                        selectedVertexIndices.removeAt(i);
                    }
                }
            } else {
                selectedEdgeIndices.push_back(eIndex);
                foreach(int vIndex, QVector<int>({edge.getSource(), edge.getTarget()})) {
                    if (!selectedVertexIndices.contains(vIndex)) {
                        selectedVertexIndices.push_back(vIndex);
                    }
                }
            }
            updateSelectedElementDrawingInfo();
        } else if (selectTargetType == "selected") {
            hoveredSelectedElement.first = "edge";
            hoveredSelectedElement.second = eIndex;
        }
        return;
    }

    // if hovered element is not found
    hoveredSelectedElement = QPair<QString, int>();
}

void GraphView::showContextMenu(const QPoint &pos)
{
    QMenu contextMenu(tr("Context menu"), this);
    QAction actSwitchEdgeDrawingMode("Show edges", this);
    QAction actSwitchRouteDrawingMode("Show routes", this);
    QAction actSwitchVertexDrawingMode("Switch node superposition mode (out-deg on in-deg)", this);
    QAction actChangeCurveStrength("Change curve strength", this);
    QAction actResetSettings("Reset settings", this);
    QAction actClearSlection("Clear selection",this);
    QAction actDisplaySelectedElementsInMatrixView("Display selected elements in matrix view", this);
    QAction actDisplayElementsInsideLassoInMatrixView("Display elements inside lasso in matrix view", this);
    QAction actDisplayElementsPassingVertexInsideLassoInMatrixView("Display elements intersected with or inside lasso in matrix view", this);
    QAction actDisplaySelectedElementsInOriginalGraph("Display selected elements in original graph", this);
    QAction actDisplayElementsInsideLassoInOriginalGraph("Display elements related with nodes inside lasso in original graph", this);
    QAction actSwitchGraph("Switch to original graph", this);
    QAction actChangeColorMapSettings("Change color map settings", this);
    if (isEdgeVisible) actSwitchEdgeDrawingMode.setText("Hide edges");
    if (isRouteVisible) actSwitchRouteDrawingMode.setText("Hide routes");
    if (vertexDrawingMode == "out-on-in") actSwitchVertexDrawingMode.setText("Invert Node Superposition (in-deg on on-deg)");
    if (vertexDrawingMode == "in-on-out") actSwitchVertexDrawingMode.setText("Invert Node Superposition (smaller on bigger)");
    if (displayedGraph == graph) actSwitchGraph.setText("Switch to aggregated graph");

    connect(&actSwitchEdgeDrawingMode, SIGNAL(triggered()), this, SLOT(switchEdgeDrawingMode()));
    connect(&actSwitchRouteDrawingMode, SIGNAL(triggered()), this, SLOT(switchRouteDrawingMode()));
    connect(&actSwitchVertexDrawingMode, SIGNAL(triggered()), this, SLOT(switchVertexDrawingMode()));
    connect(&actChangeCurveStrength, SIGNAL(triggered()), this, SLOT(changeCurveStrength()));
    connect(&actResetSettings, SIGNAL(triggered()), this, SLOT(resetSettings()));
    connect(&actClearSlection, SIGNAL(triggered()), this, SLOT(clearSelection()));
    connect(&actDisplaySelectedElementsInMatrixView, SIGNAL(triggered()), this, SLOT(displaySelectedElementsInMatrixView()));
    connect(&actDisplayElementsInsideLassoInMatrixView, SIGNAL(triggered()), this, SLOT(displayElementsInsideLassoInMatrixView()));
    connect(&actDisplayElementsPassingVertexInsideLassoInMatrixView, SIGNAL(triggered()), this, SLOT(displayElementsPassingVertexInsideLassoInMatrixView()));
    connect(&actDisplaySelectedElementsInOriginalGraph, SIGNAL(triggered()), this, SLOT(displaySelectedElementsInOriginalGraph()));
    connect(&actDisplayElementsInsideLassoInOriginalGraph, SIGNAL(triggered()), this, SLOT(displayElementsRelatedVerticesInsideLasso()));
    connect(&actSwitchGraph, SIGNAL(triggered()), this, SLOT(switchGraphFromContextMenu()));
    connect(&actChangeColorMapSettings, SIGNAL(triggered()), this, SLOT(showColorMapSettingsDialog()));

    if (displayedGraph == aggregatedGraph) {
        if (lassoPoints.size() > 0) {
            contextMenu.addAction(&actDisplayElementsInsideLassoInOriginalGraph);
        }
        if (selectedRouteIndices.size() > 0 || selectedEdgeIndices.size() > 0 || selectedVertexIndices.size() > 0) {
            contextMenu.addAction(&actDisplaySelectedElementsInOriginalGraph);
        }
    }

    if (lassoPoints.size() > 0) {
        contextMenu.addAction(&actDisplayElementsInsideLassoInMatrixView);
        contextMenu.addAction(&actDisplayElementsPassingVertexInsideLassoInMatrixView);
    }
    if (selectedRouteIndices.size() > 0 || selectedEdgeIndices.size() > 0 || selectedVertexIndices.size() > 0) {
        contextMenu.addAction(&actDisplaySelectedElementsInMatrixView);
    }

    if (selectedRouteIndices.size() > 0 || selectedEdgeIndices.size() > 0 || selectedVertexIndices.size() > 0) {
        contextMenu.addAction(&actClearSlection);
    }
    contextMenu.addAction(&actSwitchEdgeDrawingMode);
    contextMenu.addAction(&actSwitchRouteDrawingMode);

    contextMenu.addAction(&actResetSettings);
    contextMenu.addAction(&actSwitchGraph);

    contextMenu.exec(mapToGlobal(pos));
}

void GraphView::switchEdgeDrawingMode()
{
    isEdgeVisible = !isEdgeVisible;
    update();
}

void GraphView::switchRouteDrawingMode()
{
    isRouteVisible = !isRouteVisible;
    update();
}

void GraphView::switchVertexDrawingMode()
{
    if (vertexDrawingMode == "smaller-on-bigger") vertexDrawingMode = "out-on-in";
    else if (vertexDrawingMode == "out-on-in") vertexDrawingMode = "in-on-out";
    else vertexDrawingMode = "smaller-on-bigger";
    update();
}

void GraphView::changeCurveStrength()
{
    bool ok;
    double value = QInputDialog::getDouble(this, "Curve Strength Setting", "Curve strength value (min: 0.0, max: 1.5)", curveStrength, 0.0, 1.5, 3, &ok);
    if (ok) {
        curveStrength = value;
        updateRoutesCurveVbos();
        update();
    }
}

void GraphView::displaySelectedElementsInMatrixView()
{
    highlightedRouteIndices.clear();
    highlightedEdgeIndices.clear();
    highlightedVertexIndices.clear();

    highlightedRouteIndices = selectedRouteIndices;
    highlightedEdgeIndices = selectedEdgeIndices;
    highlightedVertexIndices = selectedVertexIndices;

    updateRoutesCbo();
    updateEdgesCbo();
    updateVerticesCbo();

    emit displaySelectedElementsInMatrixViewCalled("vertex", highlightedVertexIndices);
    emit displaySelectedElementsInMatrixViewCalled("route", highlightedRouteIndices);
    emit displaySelectedElementsInMatrixViewCalled("edge", highlightedEdgeIndices);
    update();
}

void GraphView::displayElementsInsideLassoInMatrixView()
{
    highlightedRouteIndices.clear();
    highlightedEdgeIndices.clear();
    highlightedVertexIndices.clear();

    highlightedRouteIndices = getRouteIndicesInsideLasso();
    highlightedEdgeIndices = getEdgeIndicesInsideLasso();
    highlightedVertexIndices = getVertexIndicesInsideLasso();

    updateRoutesCbo();
    updateEdgesCbo();
    updateVerticesCbo();

    emit displaySelectedElementsInMatrixViewCalled("vertex", highlightedVertexIndices);
    emit displaySelectedElementsInMatrixViewCalled("route", highlightedRouteIndices);
    emit displaySelectedElementsInMatrixViewCalled("edge", highlightedEdgeIndices);
    update();
}

void GraphView::displayElementsPassingVertexInsideLassoInMatrixView()
{
    highlightedRouteIndices.clear();
    highlightedEdgeIndices.clear();
    highlightedVertexIndices.clear();

    QVector<int> indices = getRouteIndicesInsideLasso();
    indices.append(getRouteIndicesIntersectedWithLasso());

    // TODO: remove duplicate. maybe not necessary. think theretical things again.
    QSet<int> uniqueIndices = QSet<int>::fromList(QList<int>::fromVector(indices));
    indices = uniqueIndices.toList().toVector();

    highlightedRouteIndices = indices;
    highlightedEdgeIndices = getEdgeIndicesInsideLasso();
    highlightedEdgeIndices.append(getEdgeIndicesIntersectedWithLasso());
    highlightedVertexIndices = getVertexIndicesInsideLasso();

    // TODO: maybe using QSet is faster
    QVector<Route> routes = displayedGraph->getRoutes();
    foreach(int rtIndex, highlightedRouteIndices) {
        foreach(int vIndex, routes[rtIndex].getArcs()) {
            if (!highlightedVertexIndices.contains(vIndex)) {
                highlightedVertexIndices.push_back(vIndex);
            }
        }
    }
    QVector<Edge> edges = displayedGraph->getEdges();
    foreach(int eIndex, highlightedEdgeIndices) {
        foreach(int vIndex, QVector<int>({edges[eIndex].getSource(), edges[eIndex].getTarget()})) {
            if (!highlightedVertexIndices.contains(vIndex)) {
                highlightedVertexIndices.push_back(vIndex);
            }
        }
    }

    updateRoutesCbo();
    updateEdgesCbo();
    updateVerticesCbo();

    emit displaySelectedElementsInMatrixViewCalled("vertex", highlightedVertexIndices);
    emit displaySelectedElementsInMatrixViewCalled("route", highlightedRouteIndices);
    emit displaySelectedElementsInMatrixViewCalled("edge", highlightedEdgeIndices);
    update();
}

void GraphView::displaySelectedElementsInOriginalGraph()
{
    QVector<QVector<QVector<int>>> allChildCoords;
    foreach (int vIndex, selectedVertexIndices) {
        QVector<QVector<int>> childCoords = aggregatedGraph->getVertexAt(vIndex).getChildCoords();
        allChildCoords.push_back(childCoords);
    }
    selectedVertexIndices.clear();

    foreach (QVector<QVector<int>> childCoords, allChildCoords) {
        foreach (QVector<int> childCoord, childCoords) {
            selectedVertexIndices.push_back(graph->getVertexIndexWithCoord(childCoord));
        }
    }

    // update highlight indices
    highlightedVertexIndices = selectedVertexIndices;
    highlightedRouteIndices.clear();
    highlightedEdgeIndices.clear();

    QSet<int> higlightedVertexSet = QSet<int>::fromList(highlightedVertexIndices.toList());
    foreach (Route r, graph->getRoutes()) {
        QSet<int> arcsSet = QSet<int>::fromList(r.getArcs().toList());

        // when r includes highlighted v
        if (arcsSet.intersect(higlightedVertexSet).size() > 0) {
            highlightedRouteIndices.push_back(r.getIndex());
        }
    }

    foreach (Edge e, graph->getEdges()) {
        int source = e.getSource();
        int target = e.getTarget();

        if (highlightedVertexIndices.contains(source) || highlightedVertexIndices.contains(target)) {
            highlightedEdgeIndices.push_back(e.getIndex());
        }
    }

    switchGraph();

    emit displaySelectedElementsInMatrixViewCalled("vertex", highlightedVertexIndices);
    emit displaySelectedElementsInMatrixViewCalled("route", highlightedRouteIndices);
    emit displaySelectedElementsInMatrixViewCalled("edge", highlightedEdgeIndices);
}

void GraphView::displayElementsRelatedVerticesInsideLasso()
{

    QVector<QVector<QVector<int>>> allChildCoords;
    foreach (int vIndex, getVertexIndicesInsideLasso()) {
        QVector<QVector<int>> childCoords = aggregatedGraph->getVertexAt(vIndex).getChildCoords();
        allChildCoords.push_back(childCoords);
    }

    foreach (QVector<QVector<int>> childCoords, allChildCoords) {
        foreach (QVector<int> childCoord, childCoords) {
            highlightedVertexIndices.push_back(graph->getVertexIndexWithCoord(childCoord));
        }
    }

    // update highlight indices
    highlightedRouteIndices.clear();
    highlightedEdgeIndices.clear();

    QSet<int> higlightedVertexSet = QSet<int>::fromList(highlightedVertexIndices.toList());
    foreach (Route r, graph->getRoutes()) {
        QSet<int> arcsSet = QSet<int>::fromList(r.getArcs().toList());

        // when r includes highlighted v
        if (arcsSet.intersect(higlightedVertexSet).size() > 0) {
            highlightedRouteIndices.push_back(r.getIndex());
        }
    }

    foreach (Edge e, graph->getEdges()) {
        int source = e.getSource();
        int target = e.getTarget();

        if (highlightedVertexIndices.contains(source) || highlightedVertexIndices.contains(target)) {
            highlightedEdgeIndices.push_back(e.getIndex());
        }
    }

    switchGraph();
}

void GraphView::showColorMapSettingsDialog()
{
    colorMapSettingDialog = new QDialog;
    colorMapSettingDialog->setWindowTitle("ColorMmap Settings");
    routeColorMetricComboBox = new QComboBox;
    routeColorMetricComboBox->addItems(QStringList({"byte", "length", "hopbyte", "distinguishable"}));
    QVBoxLayout *vBoxLayout = new QVBoxLayout;
    QHBoxLayout *hBoxLayoutMain = new QHBoxLayout;
    QHBoxLayout *hBoxLayoutCancelOk = new QHBoxLayout;
    colorMapSettingDialog->setLayout(vBoxLayout);
    vBoxLayout->addWidget(routeColorMetricComboBox);
    vBoxLayout->addLayout(hBoxLayoutMain);
    vBoxLayout->addLayout(hBoxLayoutCancelOk);

    colorMapMinValLineEdit = new QLineEdit;
    colorMapMinValLineEdit->setText(QString::number(3000));
    colorMapMaxValLineEdit = new QLineEdit;
    colorMapMaxValLineEdit->setText(QString::number(10000));

    QSpacerItem *hSpacer = new QSpacerItem(20, 40, QSizePolicy::Expanding, QSizePolicy::Minimum);
    QPushButton *cancelBtn = new QPushButton("Cancel");
    cancelBtn->setFocusPolicy(Qt::NoFocus);
    QPushButton *okBtn = new QPushButton("OK");
    hBoxLayoutCancelOk->addSpacerItem(hSpacer);
    hBoxLayoutCancelOk->addWidget(cancelBtn);
    hBoxLayoutCancelOk->addWidget(okBtn);

    connect(cancelBtn, &QPushButton::clicked, this, &GraphView::handleColorMapSettingsDialogCancelBtn);
    connect(okBtn, &QPushButton::clicked, this, &GraphView::handleColorMapSettingsDialogOkBtn);

    colorMapSettingDialog->show();
}

void GraphView::handleColorMapSettingsDialogCancelBtn()
{
    colorMapSettingDialog->close();
    routeColorMetricComboBox->close();
    colorMapMinValLineEdit->close();
    colorMapMaxValLineEdit->close();
}

void GraphView::handleColorMapSettingsDialogOkBtn()
{
    routeColorMetric = routeColorMetricComboBox->currentText();
    float min = colorMapMinValLineEdit->text().toFloat();
    float max = colorMapMaxValLineEdit->text().toFloat();

    displayedGraph->reorderRoutesByMetric(routeColorMetric);

    updateAllDrawingInfo();

    emit routeColorSettingsChanged(routeColorMetric, min, max);

    colorMapSettingDialog->close();
    routeColorMetricComboBox->close();
    colorMapMinValLineEdit->close();
    colorMapMaxValLineEdit->close();

    update();
}

void GraphView::setGraph(Graph* graph) { this->graph = graph; }
void GraphView::setAggregatedGraph(Graph* aggregatedGraph) { this->aggregatedGraph = aggregatedGraph; }
void GraphView::setDisplayedGraph(Graph* graph) { this->displayedGraph = graph; }
void GraphView::switchGraph()
{
    lassoPoints.clear();
    if (displayedGraph == graph) {
        displayedGraph = aggregatedGraph;
        resetSelectedElementsIndices();
        resetHilightedElements();
        resetDisplayedElementsIndices();
    } else {
        displayedGraph = graph;
    }
    updateAllDrawingInfo(true);

    emit switchGraphCalled();

    emit displaySelectedElementsInMatrixViewCalled("vertex", highlightedVertexIndices);
    emit displaySelectedElementsInMatrixViewCalled("route", highlightedRouteIndices);
    emit displaySelectedElementsInMatrixViewCalled("edge", highlightedEdgeIndices);

    update();
}
void GraphView::switchGraphFromContextMenu()
{
    resetSelectedElementsIndices();
    resetHilightedElements();
    resetDisplayedElementsIndices();
    switchGraph();
}

void GraphView::setColor(Color* color) { this->color = color; }
void GraphView::setMouseMode(QString mouseMode) { this->mouseMode = mouseMode; }
void GraphView::setEdgeDrawingMode(QString edgeDrawingMode) { this->edgeDrawingMode = edgeDrawingMode; }
void GraphView::setRouteDrawingMode(QString routeDrawingMode) { this->routeDrawingMode = routeDrawingMode; }
void GraphView::setRouteColorMetric(QString metric) { this->routeColorMetric = metric; }

void GraphView::setDisplayedVertexIndices(QVector<int> displayedVertexIndices) { this->displayedVertexIndices = displayedVertexIndices; }
void GraphView::setDisplayedEdgeIndices(QVector<int> displayedEdgeIndices) { this->displayedEdgeIndices = displayedEdgeIndices; }
void GraphView::setDisplayedRouteIndices(QVector<int> displayedRouteIndices) { this->displayedRouteIndices = displayedRouteIndices; }
void GraphView::setHighlightedVertexIndices(QVector<int> highlightedVertexIndices) { this->highlightedVertexIndices = highlightedVertexIndices; }
void GraphView::setHighlightedEdgeIndices(QVector<int> highlightedEdgeIndices) { this->highlightedEdgeIndices = highlightedEdgeIndices; }
void GraphView::setHighlightedRouteIndices(QVector<int> highlightedRouteIndices) { this->highlightedRouteIndices = highlightedRouteIndices; }

void GraphView::resetDisplayedElementsIndices()
{
    displayedVertexIndices.clear();
    displayedEdgeIndices.clear();
    displayedRouteIndices.clear();
}

void GraphView::resetSelectedElementsIndices()
{
    selectedVertexIndices.clear();
    selectedEdgeIndices.clear();
    selectedRouteIndices.clear();
}

void GraphView::resetHilightedElements()
{
    highlightedVertexIndices.clear();
    highlightedEdgeIndices.clear();
    highlightedRouteIndices.clear();
}

void GraphView::resetScaleAndTranslate()
{
    scale = 0.90;
    translateX = 0.0;
    translateY = 0.0;
}

void GraphView::resetSettings()
{
    resetDisplayedElementsIndices();
    resetSelectedElementsIndices();
    resetHilightedElements();
    resetScaleAndTranslate();
    edgeDrawingMode = "straight";
    routeDrawingMode = defaultRouteDrawingMode;
    vertexDrawingMode = "smaller-on-bigger";
    updateAllDrawingInfo();

    highlightedVertexIndices.clear();
    highlightedRouteIndices.clear();
    highlightedEdgeIndices.clear();

    emit resetSettingsCalled();
    update();
}

void GraphView::clearSelection()
{
    resetSelectedElementsIndices();
    updateAllDrawingInfo();
    update();
}

void GraphView::updateVerticesVbo()
{
    verticesVbo.clear();
    displayedVertexPropertyMap.clear();

    // TODO: rethink structure here later
    QVector<Vertex> vertices = displayedGraph->getVertices();
    for (int i = 0; i < vertices.size(); ++i) {
        verticesVbo.push_back(-1000000.0);
        verticesVbo.push_back(-1000000.0);
        displayedVertexPropertyMap.push_back(false);
    }

    foreach(int i, displayedVertexIndices) {
        QPointF pos = vertices[i].getGraphPos();
        verticesVbo[i * 2] = pos.x();
        verticesVbo[i * 2 + 1] = pos.y();
        displayedVertexPropertyMap[i] = true;
    }
}

void GraphView::updateSelectedVerticesVbo()
{
    selectedVerticesVbo.clear();

    foreach(int vIndex, selectedVertexIndices) {
        QPointF pos = displayedGraph->getVertexAt(vIndex).getGraphPos();
        selectedVerticesVbo.push_back(pos.x());
        selectedVerticesVbo.push_back(pos.y());
    }
}

void GraphView::updateVerticesCbo()
{
    verticesCbo.clear();
    highlightedVertexPropertyMap.clear();

    // TODO: rethink structure here later
    QVector<Vertex> vertices = displayedGraph->getVertices();

    for (int i = 0; i < vertices.size(); ++i) {
        highlightedVertexPropertyMap.push_back(false);
    }

    foreach (int i, highlightedVertexIndices) {
        highlightedVertexPropertyMap[i] = true;
    }

    // TODO: this is temp code. something is wrong in highlighted index control
    if (highlightedVertexIndices.size() == 0) {
        for (int i = 0; i < vertices.size(); ++i) {
            highlightedVertexPropertyMap[i] = true;
        }
    }

    for (int i = 0; i < vertices.size(); ++i) {
        float wDegIn = vertices[i].getInDegOfEdgesByte();
        float wDegOut = vertices[i].getOutDegOfEdgesByte();
        float ratioOfInDeg = wDegIn / (wDegIn + wDegOut);
        QColor qColor = color->ratioToHeatMapColor(ratioOfInDeg, color->vertexColorOfPoints);

        // TODO: should be considered faster algorithm
        if (!highlightedVertexPropertyMap.at(i)) {
            if (showOnlySelectedElems) {
                qColor.setAlphaF(0.0);
            } else {
                qColor = qColor.darker(filterOutDarker);
                qColor.setAlphaF(qColor.alphaF() * filterOutAlpha);
            }
        }

        verticesCbo.push_back(qColor.redF());
        verticesCbo.push_back(qColor.greenF());
        verticesCbo.push_back(qColor.blueF());
        verticesCbo.push_back(qColor.alphaF());
    }
}

void GraphView::updateSelectedVerticesCbo()
{
    selectedVerticesCbo.clear();

    foreach (int vIndex, selectedVertexIndices) {
        selectedVerticesCbo.push_back(color->selectionColor.redF());
        selectedVerticesCbo.push_back(color->selectionColor.greenF());
        selectedVerticesCbo.push_back(color->selectionColor.blueF());
        selectedVerticesCbo.push_back(color->selectionColor.alphaF());
    }
}

void GraphView::updateVertexSizes()
{
    vertexSizes.clear();

    QVector<Vertex> vertices = displayedGraph->getVertices();
    float maxTotalDeg = displayedGraph->getMaxInDegOfEdgesByte() + displayedGraph->getMaxOutDegOfEdgesByte();
    for (int i = 0; i < vertices.size(); ++i) {
        float wDegIn = vertices[i].getInDegOfEdgesByte();
        float wDegOut = vertices[i].getOutDegOfEdgesByte();
        float wDegTotal = wDegIn + wDegOut;
        vertexSizes.push_back(max(nodeSizeMin, nodeSizeWeight * wDegTotal / maxTotalDeg));
    }
}

void GraphView::updateSelectedVertexSizes()
{
    selectedVertexSizes.clear();

    float maxTotalDeg = displayedGraph->getMaxInDegOfEdgesByte() + displayedGraph->getMaxOutDegOfEdgesByte();
    foreach(int vIndex, selectedVertexIndices) {
        Vertex v = displayedGraph->getVertexAt(vIndex);
        float wDegIn = v.getInDegOfEdgesByte();
        float wDegOut = v.getOutDegOfEdgesByte();
        float wDegTotal = wDegIn + wDegOut;
        selectedVertexSizes.push_back(max(nodeSizeMin, nodeSizeWeight * wDegTotal / maxTotalDeg) + 10);
    }
}

void GraphView::updateEdgesIbo()
{
    edgesIbo.clear();
    QVector<Edge> edges = displayedGraph->getEdges();

    foreach(int i, displayedEdgeIndices) {
        int source = edges[i].getSource();
        int target = edges[i].getTarget();

        edgesIbo.push_back(source);
        edgesIbo.push_back(target);
    }
}

void GraphView::updateSelectedEdgesIbo()
{
    selectedEdgesIbo.clear();

    foreach(int eIndex, selectedEdgeIndices) {
        Edge e = displayedGraph->getEdgeAt(eIndex);
        selectedEdgesIbo.push_back(e.getSource());
        selectedEdgesIbo.push_back(e.getTarget());
    }
}

void GraphView::updateEdgesCbo()
{
    edgesCbo.clear();

    QVector<Edge> edges = displayedGraph->getEdges();
    foreach (int i, displayedEdgeIndices) {
        float val = edges[i].getMetric(edgeColorMetric);
        float ratio = val / (float)displayedGraph->getMetric("edge", edgeColorMetric, "max");
        QColor qColor = color->ratioToHeatMapColor(ratio, color->edgeColorOfPoints);

        // TODO: should be considered faster algorithm
        if (highlightedEdgeIndices.size() > 0) {
            if (!highlightedEdgeIndices.contains(i)) {
                if (showOnlySelectedElems) {
                    qColor.setAlphaF(0.0);
                } else {
                    qColor = qColor.darker(filterOutDarker);
                    qColor.setAlphaF(qColor.alphaF() * filterOutAlpha);
                }
            }
        }

        edgesCbo.push_back(qColor.redF());
        edgesCbo.push_back(qColor.greenF());
        edgesCbo.push_back(qColor.blueF());
        edgesCbo.push_back(qColor.alphaF());
    }
}

void GraphView::updateSelectedEdgesCbo()
{
    selectedEdgesCbo.clear();

    foreach (int eIndex, selectedEdgeIndices) {
        selectedEdgesCbo.push_back(color->selectionColor.redF());
        selectedEdgesCbo.push_back(color->selectionColor.greenF());
        selectedEdgesCbo.push_back(color->selectionColor.blueF());
        selectedEdgesCbo.push_back(color->selectionColor.alphaF());
    }
}

void GraphView::updateEdgeWidths()
{
    edgeWidths.clear();

    QVector<Edge> edges = displayedGraph->getEdges();
    foreach(int i, displayedEdgeIndices) {
        float val = edges[i].getMetric(edgeWidthMetric);
        edgeWidths.push_back(edgeWidthWeight * val / displayedGraph->getMetric("edge", edgeWidthMetric, "max"));
    }
}

void GraphView::updateSelectedEdgeWidths()
{
    selectedEdgeWidths.clear();

    foreach(int eIndex, selectedEdgeIndices) {
        float val = displayedGraph->getEdgeAt(eIndex).getMetric(edgeWidthMetric);
        selectedEdgeWidths.push_back(12.0 * val / displayedGraph->getMetric("edge", edgeWidthMetric, "max"));
    }
}

void GraphView::updateRoutesArcIbos()
{
    routesArcIbos.clear();

    QVector<Route> routes = displayedGraph->getRoutes();
    foreach(int i, displayedRouteIndices) {
        vector<int> arcs = routes[i].getArcs().toStdVector();
        routesArcIbos.push_back(arcs);
    }
}

void GraphView::updateSelectedRoutesArcIbos()
{
    selectedRoutesArcIbos.clear();

    QVector<Route> routes = displayedGraph->getRoutes();
    foreach(int rtIndex, selectedRouteIndices) {
        vector<int> arcs = routes[rtIndex].getArcs().toStdVector();
        selectedRoutesArcIbos.push_back(arcs);
    }
}

void GraphView::updateRoutesCurveVbos()
{
    routesCurveVbos.clear();

    for (int i = 0; i < displayedRouteIndices.size(); ++i) {
        vector<GLint> arcIbo = routesArcIbos.at(i);
        vector<GLfloat> curveVbo;
        if (routeDrawingMode == "catmull-rom") {
            curveVbo = calcCatmullRomSplineCurvePoints(arcIbo);
        } else if (routeDrawingMode == "bspline") {
            curveVbo = calcCubicBSplineCurvePoints(arcIbo, curveStrength);
        } else if (routeDrawingMode == "metro") {
            curveVbo = genMetroPoints(arcIbo, displayedRouteIndices.at(i));
        }
        routesCurveVbos.push_back(curveVbo);
    }
}

void GraphView::updateSelectedRoutesCurveVbos()
{
    selectedRoutesCurveVbos.clear();

    for (int i = 0; i < selectedRouteIndices.size(); ++i) {
        vector<GLint> arcIbo = selectedRoutesArcIbos.at(i);
        vector<GLfloat> curveVbo;
        if (routeDrawingMode == "catmull-rom") curveVbo = calcCatmullRomSplineCurvePoints(arcIbo);
        else if (routeDrawingMode == "bspline") curveVbo = calcCubicBSplineCurvePoints(arcIbo, curveStrength);
        else if (routeDrawingMode == "metro") curveVbo = genMetroPoints(arcIbo, selectedRouteIndices.at(i));

        selectedRoutesCurveVbos.push_back(curveVbo);
    }
}

void GraphView::updateRoutesCbo()
{
    routesCbo.clear();

    QVector<Route> routes = displayedGraph->getRoutes();
    foreach (int i, displayedRouteIndices) {
        QColor qColor;
        if (routeColorMetric == "distinguishable") {
            qColor = color->routeColors.at(i % color->routeColors.size());
        } else {
            float val = routes[i].getMetric(routeColorMetric);

            float ratio = val;
            if (baseValueForRouteColor < 0.0) {
                // default
                ratio /= displayedGraph->getMetric("route", routeColorMetric, "max");
            } else {
                // user set base value
                ratio /= baseValueForRouteColor;
            }

            qColor = color->ratioToHeatMapColor(ratio, color->getRouteColorOfPointsForMetric(routeColorMetric));
        }

        // TODO: should be considered faster algorithm
        if (highlightedRouteIndices.size() > 0) {
            if (!highlightedRouteIndices.contains(i)) {
                if (showOnlySelectedElems) {
                    qColor.setAlphaF(0.0);
                } else {
                    qColor = qColor.darker(filterOutDarker);
                    qColor.setAlphaF(qColor.alphaF() * filterOutAlpha);
                }
            }
        }

        routesCbo.push_back(qColor.redF());
        routesCbo.push_back(qColor.greenF());
        routesCbo.push_back(qColor.blueF());
        routesCbo.push_back(qColor.alphaF());
    }
}

void GraphView::updateSelectedRoutesCbo()
{
    selectedRoutesCbo.clear();

    foreach (int rtIndex, selectedRouteIndices) {
        selectedRoutesCbo.push_back(color->selectionColor.redF());
        selectedRoutesCbo.push_back(color->selectionColor.greenF());
        selectedRoutesCbo.push_back(color->selectionColor.blueF());
        selectedRoutesCbo.push_back(color->selectionColor.alphaF());
    }
}

void GraphView::updateRouteWidths()
{
    routeWidths.clear();

    QVector<Route> routes = displayedGraph->getRoutes();
    foreach (int i, displayedRouteIndices) {
        float val = routes[i].getMetric(routeWidthMetric);

        if (baseValueForRouteWidth < 0.0) {
            //default
            routeWidths.push_back(4.0 * val / displayedGraph->getMetric("route", routeWidthMetric, "max"));
        } else {
            // user set value for max
            routeWidths.push_back(4.0 * val / baseValueForRouteWidth);
        }
    }
}

void GraphView::updateSelectedRouteWidths()
{
    selectedRouteWidths.clear();

    QVector<Route> routes = displayedGraph->getRoutes();
    foreach (int rtIndex, selectedRouteIndices) {
        float val = routes[rtIndex].getMetric(routeWidthMetric);
        selectedRouteWidths.push_back(6.0 * val / displayedGraph->getMetric("route", routeWidthMetric, "max"));
    }
}

void GraphView::updateAllDrawingInfo(bool graphChanged)
{
    if (graphChanged) {
        displayedVertexIndices.clear();
        displayedEdgeIndices.clear();
        displayedRouteIndices.clear();
    }

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

    if (highlightedVertexIndices.size() == 0) {
        for (int i = 0; i < displayedGraph->getVertices().size(); ++i) {
            highlightedVertexIndices.push_back(i);
        }
    }

    if (highlightedEdgeIndices.size() == 0) {
        for (int i = 0; i < displayedGraph->getEdges().size(); ++i) {
            highlightedEdgeIndices.push_back(i);
        }
    }

    if (highlightedRouteIndices.size() == 0) {
        for (int i = 0; i < displayedGraph->getRoutes().size(); ++i) {
            highlightedRouteIndices.push_back(i);
        }
    }

    updateVerticesVbo();
    updateVerticesCbo();
    updateVertexSizes();
    updateEdgesIbo();
    updateEdgesCbo();
    updateEdgeWidths();
    updateRoutesArcIbos();

    if (routeDrawingMode == "metro") updateRoutePositionsMap(routeColorMetric);
    updateRoutesCurveVbos();
    updateRoutesCbo();
    updateRouteWidths();

    updateSelectedElementDrawingInfo();
}

void GraphView::updateSelectedElementDrawingInfo()
{
    updateSelectedVerticesVbo();
    updateSelectedVerticesCbo();
    updateSelectedVertexSizes();
    updateSelectedEdgesIbo();
    updateSelectedEdgesCbo();
    updateSelectedEdgeWidths();
    updateSelectedRoutesArcIbos();
    updateSelectedRoutesCurveVbos();
    updateSelectedRoutesCbo();
    updateSelectedRouteWidths();
}

void GraphView::updateDisplayedElementsIndicies(QString elemType, QVector<int> indices)
{
    // TODO: These are temp for demo
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

    if (highlightedVertexIndices.size() == 0) {
        for (int i = 0; i < displayedGraph->getVertices().size(); ++i) {
            highlightedVertexIndices.push_back(i);
        }
    }

    if (highlightedEdgeIndices.size() == 0) {
        for (int i = 0; i < displayedGraph->getEdges().size(); ++i) {
            highlightedEdgeIndices.push_back(i);
        }
    }

    if (highlightedRouteIndices.size() == 0) {
        for (int i = 0; i < displayedGraph->getRoutes().size(); ++i) {
            highlightedRouteIndices.push_back(i);
        }
    }

    if (elemType == "edge") {
        QSet<int> indicesSet = QSet<int>::fromList(QList<int>::fromVector(indices));
        QSet<int> highlightedEdgeIndicesSet = QSet<int>::fromList(QList<int>::fromVector(highlightedEdgeIndices));
        QSet<int> intersectSet = indicesSet.intersect(highlightedEdgeIndicesSet);

        setHighlightedEdgeIndices(intersectSet.toList().toVector());
        updateEdgesIbo();
        updateEdgesCbo();
        updateEdgeWidths();

        displaySelectedElementsInMatrixViewCalled(elemType, intersectSet.toList().toVector());

    } else if (elemType == "route") {
        QSet<int> indicesSet = QSet<int>::fromList(QList<int>::fromVector(indices));
        QSet<int> highlightedRouteIndicesSet = QSet<int>::fromList(QList<int>::fromVector(highlightedRouteIndices));
        QSet<int> intersectSet = indicesSet.intersect(highlightedRouteIndicesSet);
        qDebug() << "size" << intersectSet.size();
        setHighlightedRouteIndices(intersectSet.toList().toVector());
        updateRoutesArcIbos();
        updateRoutePositionsMap(routeColorMetric);
        updateRoutesCurveVbos();
        updateRoutesCbo();
        updateRouteWidths();

        displaySelectedElementsInMatrixViewCalled(elemType, intersectSet.toList().toVector());

    } else if (elemType == "vertex") {
        QSet<int> indicesSet = QSet<int>::fromList(QList<int>::fromVector(indices));
        QSet<int> highlightedVertexIndicesSet = QSet<int>::fromList(QList<int>::fromVector(highlightedVertexIndices));
        QSet<int> intersectSet = indicesSet.intersect(highlightedVertexIndicesSet);

        setHighlightedVertexIndices(intersectSet.toList().toVector());
        updateVerticesVbo();
        updateVerticesCbo();
        updateVertexSizes();

        displaySelectedElementsInMatrixViewCalled(elemType, intersectSet.toList().toVector());
    }

    update();
}

void GraphView::updateHighlightedElementsIndicies(QString elemType, QVector<int> indices)
{
    if (elemType == "edge") {
        setHighlightedEdgeIndices(indices);

        updateEdgesIbo();
        updateEdgesCbo();
        updateEdgeWidths();
    } else if (elemType == "route") {
        setHighlightedRouteIndices(indices);

        updateRoutesArcIbos();
        updateRoutePositionsMap(routeColorMetric);
        updateRoutesCurveVbos();
        updateRoutesCbo();
        updateRouteWidths();
    } else if (elemType == "vertex") {
        setHighlightedVertexIndices(indices);

        updateVerticesVbo();
        updateVerticesCbo();
        updateVertexSizes();
    }

    update();
}

void GraphView::updateColorsFromStatView()
{
    updateVerticesCbo();
    updateEdgesCbo();
    updateRoutesCbo();
    update();
}

void GraphView::updateRoutePositionsMap(QString metric)
{
    routePositionsMap.clear();

    foreach (Edge e, displayedGraph->getEdges()) {
        int s = e.getSource();
        int t = e.getTarget();
        QPointF startPos = displayedGraph->getVertexAt(s).getGraphPos();
        QPointF endPos = displayedGraph->getVertexAt(t).getGraphPos();

        QVector<Route> rts = displayedGraph->getRoutesContainingSourceTarget(s, t);
        // TODO: this should be improved to apply any metric type
        Graph::sortRoutesByMetric(rts, metric);

        float d = sqrt(pow(startPos.x() - endPos.x(), 2.0) + pow(startPos.y() - endPos.y(), 2.0));
        QPointF v = endPos - startPos; // vec

        if (s != t) {
            if (d == 0) {
                qWarning() << "d == 0 in updateRoutePositionsMap";
            } else {
                v = v / d;
            }
        } else {
            v = QPointF(1.0, 0.0);
        }

        QPointF n = QPointF(-v.y(), v.x()); // normal vec

        float unitWidth = 0.0;
        if (baseValueForRouteWidth < 0.0) {
            //default
            unitWidth = routeWidthWeight / displayedGraph->getMetric("route", routeWidthMetric, "max");
        } else {
            //user set max value
            unitWidth = routeWidthWeight / baseValueForRouteWidth;
        }
        float rSource = vertexSizes.at(s) * nodeSizeWeightForMetro;
        float rTarget = vertexSizes.at(t) * nodeSizeWeightForMetro;

        float totalWidth = 0.0;

        for (int i = 0; i < rts.size(); ++i) {
            int rIndex = rts[rts.size() - 1 - i].getIndex();
            float val = rts[rts.size() - 1 - i].getMetric(routeWidthMetric);

            float width = val * unitWidth;

            if (s != t) {
                // to avoid sqrt(0)
                float deltaSPos1 = pow(rSource, 2.0) - pow(totalWidth, 2.0);
                float deltaTPos1 = pow(rTarget, 2.0) - pow(totalWidth, 2.0);
                float deltaSPos2 = pow(rSource, 2.0) - pow((totalWidth + width), 2.0);
                float deltaTPos2 = pow(rTarget, 2.0) - pow((totalWidth + width), 2.0);
                float deltaDistSPos1 = deltaSPos1 > 0.0 ? sqrt(deltaSPos1) : 0.0;
                float deltaDistTPos1 = deltaTPos1 > 0.0 ? sqrt(deltaTPos1) : 0.0;
                float deltaDistSPos2 = deltaSPos2 > 0.0 ? sqrt(deltaSPos2) : 0.0;
                float deltaDistTPos2 = deltaTPos2 > 0.0 ? sqrt(deltaTPos2) : 0.0;

                QPointF sPos1 = startPos + totalWidth * n + deltaDistSPos1 * v;
                QPointF tPos1 = endPos   + totalWidth * n - deltaDistTPos1 * v;
                QPointF sPos2 = startPos + (totalWidth + width) * n + deltaDistSPos2 * v;
                QPointF tPos2 = endPos   + (totalWidth + width) * n - deltaDistTPos2 * v;

                // make map
                routePositionsMap[rIndex].insert(QVector<int>({s, t}),
                                                 vector<GLfloat>({(float)sPos1.x(), (float)sPos1.y(),
                                                                  (float)sPos2.x(), (float)sPos2.y(),
                                                                  (float)tPos1.x(), (float)tPos1.y(),
                                                                  (float)tPos2.x(), (float)tPos2.y()}));

            } else {
                QPointF sPos1 = startPos + totalWidth * n;
                QPointF tPos1 = sPos1 + 0.01 * v;
                QPointF sPos2 = startPos + width * n + (totalWidth + width) * n;
                QPointF tPos2 = sPos2 + 0.01 * v;

                // make map
                routePositionsMap[rIndex].insert(QVector<int>({s, t}),
                                                 vector<GLfloat>({(float)sPos1.x(), (float)sPos1.y(),
                                                                  (float)sPos2.x(), (float)sPos2.y(),
                                                                  (float)tPos1.x(), (float)tPos1.y(),
                                                                  (float)tPos2.x(), (float)tPos2.y()}));
            }
            totalWidth += width;
        }
    }
}

vector<GLfloat> GraphView::genMetroPoints(vector<GLint> arcIbo, int routeIndex)
{
    vector<GLfloat> curveVbo;

    for (int j = 0; j < (int)arcIbo.size() - 1; ++j) {
        int s = arcIbo.at(j);
        int t = arcIbo.at(j + 1);
        vector<GLfloat> positions = routePositionsMap[routeIndex][QVector<int>({s, t})];
        QVector<Route> rts = displayedGraph->getRoutesContainingSourceTarget(s, t);

        if (s != t) {
            if (positions.size() < 8) {
                qWarning() << "not enough positions in genMetroPoints for rIndex:" << routeIndex << "s:" << s << "t:" << t;
            } else {
                curveVbo.push_back(positions.at(0));
                curveVbo.push_back(positions.at(1));
                curveVbo.push_back(positions.at(2));
                curveVbo.push_back(positions.at(3));
                curveVbo.push_back(positions.at(4));
                curveVbo.push_back(positions.at(5));
                curveVbo.push_back(positions.at(6));
                curveVbo.push_back(positions.at(7));
            }
        } else {
            QPointF startPos = displayedGraph->getVertexAt(s).getGraphPos();
            float r = vertexSizes.at(s) * nodeSizeWeightForMetro;
            QPointF s1(positions.at(0), positions.at(1));
            QPointF s2(positions.at(2), positions.at(3));

            float w = sqrt(pow(s1.y() - s2.y(), 2.0));
            QPointF v = s2 - s1;
            v = v / w; // unit vec
            QPointF center = startPos;

            int numSplit = 20;
            GLfloat twicePi = 2.0f * M_PI;
            vector<GLfloat> circlePoint1;
            vector<GLfloat> circlePoint2;

            for (int i = 0; i <= numSplit; i++) {
                circlePoint1.push_back(center.x() - r * cos(i * twicePi / (float)numSplit));
                circlePoint1.push_back(center.y() + r * sin(i * twicePi / (float)numSplit));
                circlePoint2.push_back(center.x() - (r + (s2.y() - s1.y()) * selfLoopRouteRatio) * cos(i * twicePi / (float)numSplit));
                circlePoint2.push_back(center.y() + (r + (s2.y() - s1.y()) * selfLoopRouteRatio) * sin(i * twicePi / (float)numSplit));
            }

            for (int i = 0; i < (int)circlePoint1.size() / 2; ++i) {
                curveVbo.push_back(circlePoint1.at(i * 2));
                curveVbo.push_back(circlePoint1.at(i * 2 + 1));
                curveVbo.push_back(circlePoint2.at(i * 2));
                curveVbo.push_back(circlePoint2.at(i * 2 + 1));
            }
        }
    }
    return curveVbo;
}
