#ifndef MATRIXVIEW_H
#define MATRIXVIEW_H

#include <QOpenGLWidget>
#include <QPainter>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QDebug>
#include <QMenu>
#include <QSettings>

#include <QtGlobal>
#include <QTime>
#include <QThread>

#include "Graph.h"
#include "Color.h"
#include "AdjMatrix.h"
#include "RouteGuide.h"
#include "MappingGuide.h"

using namespace std;

class MatrixView: public QOpenGLWidget
{
    Q_OBJECT
public:
    explicit MatrixView(QWidget *parent = 0);

    void setGraphDir(QString graphDir);
    void setAdjMat(AdjMatrix* adjMat);
    void setAggregatedAdjMat(AdjMatrix* aggregatedAdjMat);
    void setDisplayedAdjMat(AdjMatrix* adjMat);
    void setColor(Color* color);
    void setGraph(Graph* graph);
    void setAggregatedGraph(Graph* aggregatedGraph);
    void setDisplayedGraph(Graph* graph);
    void setDisplayedVertexIndices(QVector<int> displayedVertexIndices);
    void setDisplayedEdgeIndices(QVector<int> displayedEdgeIndices);
    void setDisplayedRouteIndices(QVector<int> displayedRouteIndices);
    void setRoutingGuideType(QString routingGuideType);
    void setRoutingGuidePriority(QString routingGuidePriority);
    void setMappingGuideType(QString mappingGuideType);
    void setRoutingGuideLengthTolerance(int routingGuideLengthTolerance);
    void setRoutingGuideLoadTolerance(float routingGuideLoadTolerance);
    void setUnitFoldingSize(int unitFoldingSize);
    void setRouteDrawingMode(QString routeDrawingMode);
    void setRscriptPath(QString rscriptPath);
    void setSaQapPath(QString saQapPath);
    void resetDisplayedElementsIndices();
    void resetSuggestedElementsIndices();
    void resetScaleAndTranslate();
    void updateUnitBlockSize();

    void updateCoordValues(QVector<double> &coordValues, QVector<int> &vertexIndices, int &unitBlockSize);
    void updateFoldedRegionsVbo(vector<vector<GLfloat> > &foldedRegionsVbos, QVector<double> &coordValues);
    void updateAdjMatVbo(vector<GLfloat> &adjMatVbo, int unitBlockSize);
    void updateAdjMatVboWithFolding(vector<GLfloat> &adjMatVbo, QVector<double> &coordValues);
    void updateVerticesVbo(vector<GLfloat> &verticesVbo, QVector<int> &vertexIndices, int unitBlockSize);
    void updateVerticesVboWithFolding(vector<GLfloat> &verticesVbo, QVector<int> &vertexIndices, QVector<double> &coordValues, int unitBlockSize, int unitFoldingSize);
    void updateRoutesArcVbos(vector< vector<GLfloat> > &routesArcVbos, vector<GLfloat> &verticesVbo, QVector<Route> &routes, QVector<int> &routeIndices);
    void updateRoutesSrcVbos(vector< vector<GLfloat> > &routesSrcVbos, vector<GLfloat> &verticesVbo, QVector<Route> &routes, QVector<int> &routeIndices, int unitBlockSize);
    void updateRoutesDestVbos(vector< vector<GLfloat> > &routesDestVbos, vector<GLfloat> &verticesVbo, QVector<Route> &routes, QVector<int> &routeIndices, int unitBlockSize);
    void updateRoutesCurveVbos(vector< vector<GLfloat> > &routesCurveVbos, vector< vector<GLfloat> > &routesArcVbos);
    void updateRoutesCbo(vector<GLfloat> &routesCbo, QVector<Route> &routes, QVector<int> &routeIndices, QList< QPair<float, QColor> > &routeColorOfPoints);
    void updateEdgesVbos(vector< vector<GLfloat> > &edgesVbos, vector<GLfloat> &verticesVbo, QString edgesVboType = "displayed"); // edgesVboType: displayed, suggested
    void updateEdgesCbo(vector<GLfloat> &edgesCbo, QString edgesVboType = "displayed"); // edgesVboType: displayed, suggested
    void updateAllDrawingInfo();  
    void updateAllDrawingInfoForLens();

    void updateVisualEncodingParams(float edgeWidth, float routeWidth);

public slots:
    void setMouseMode(QString mouseMode);
    void updateDisplayedElementsIndicies(QString elemType, QVector<int> indices);
    void resetSettings();
    void suggestAltRoutesForDisplayedRoutes();
    void suggestAltMapping(QString perfOutput = "", bool useMaGraphPartition = true);
    void applyAltRoutes();
    void turnOnAutoUpdate();
    void turnOffAutoUpdate();
    void updateColorsFromStatView();
    void switchGraphAndAdjMat();
    void changeRouteColorMapSettings(QString metric, float min, float val);

protected:
    void initializeGL();
    void paintGL();
    void resizeGL(int width, int height);

    void drawFoldedRegions(vector<vector<GLfloat> > &foldedRegionsVbos);
    void drawAdjMat(vector<GLfloat> &adjMatVb);
    void drawEdges(vector< vector<GLfloat> > &edgesVbos, vector<GLfloat> &edgesCbo, float width = 5.0);
    void drawEdgesForLens(vector< vector<GLfloat> > &edgesVbos, vector<GLfloat> &edgesCbo, float width = 5.0);
    void drawRoutes(vector< vector<GLfloat> > &routesArcVbos, vector< vector<GLfloat> > &routesCurveVbos, vector<GLfloat> &routesCbo, vector< vector<GLfloat> > &routesSrcVbos, vector< vector<GLfloat> > &routesDestVbos, QString type = "displayed", float width = 3.0); // type: displayed, suggested
    void drawColorSymbols();
    void drawColorMap(float x, float y, float w, float h, QList< QPair<float, QColor> > colorOfPoints);
    void drawColorMaps();
    void paintLabels();
    void drawSelectedArea();
    void drawLensContents();
    void drawLensOutLine();

    void wheelEvent(QWheelEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    vector<GLfloat> calcCatmullRomSplineCurvePoints(vector<GLfloat> &routeArcVbo, int numSplits = 200);

private:
    float routeWidth;
    float edgeWidth;

    AdjMatrix* adjMat;
    AdjMatrix* aggregatedAdjMat;
    AdjMatrix* displayedAdjMat;
    Graph* graph;
    Graph* aggregatedGraph;
    Graph* displayedGraph;
    Color* color;
    RouteGuide routeGuide;
    MappingGuide mappingGuide;
    QString graphDir; // TODO: temp code
    QString rscriptPath;
    QString saQapPath;

    QString edgeColorMetric;
    QString routeColorMetric;

    bool isResolutionReductionTurnedOn;
    int unitBlockSize;
    int unitBlockSizeForLens;
    int unitFoldingSize;
    int thresAdjustingResolution;
    int numOfDisplayedCells;
    QString edgeDrawingMode;
    QString routeDrawingMode;

    QVector<double> coordValues;
    double magnifyingRateForFolding;
    QVector<int> foldedRegionNums;
    QVector<double> coordValuesForLens;

    QVector<int> displayedVertexIndices;
    QVector<int> displayedRouteIndices;
    QVector<int> displayedEdgeIndices;

    QString routingGuideType;
    QString routingGuidePriority;
    int routingGuideLengthTolerance;
    float routingGuideLoadTolerance;
    QString mappingGuideType;
    QVector<Route> suggestedRoutes;
    QVector<int> suggestedRoutesIndices;
    QVector<int> suggestedVertexIndices;
    QVector<int> suggestedEdgeIndices;
    int totalLengthWillBeChanged;
    int totalMaxEdgeLoadWillBeChanged;
    bool autoUpdate;

    float scale;
    float translateX;
    float translateY;
    QString mouseMode;
    QPoint mousePressPos;
    QPoint mouseMovePos;
    QPoint mouseReleasePos;
    bool leftButtonPressed;
    bool rightButtonPressed;
    vector<GLfloat> selectedXRange;
    vector<GLfloat> selectedYRange;
    float lensSize;
    int lensZoomLevel;
    vector<GLfloat> lensTargetXRange;
    vector<GLfloat> lensTargetYRange;

    vector<vector<GLfloat> > foldedRegionsVbos;
    vector<vector<GLfloat> > foldedRegionsVbosForLens;
    vector<GLfloat> adjMatVbo;
    vector<GLfloat> verticesVbo;
    vector< vector<GLfloat> > routesArcVbos;
    vector<GLfloat> routesCbo;
    vector< vector<GLfloat> > routesSrcVbos;
    vector< vector<GLfloat> > routesDestVbos;
    vector< vector<GLfloat> > routesCurveVbos;
    vector< vector<GLfloat> > edgesVbos;
    vector<GLfloat> edgesCbo;
    vector<GLfloat> suggestedVerticesVbo;
    vector< vector<GLfloat> > suggestedRoutesArcVbos;
    vector<GLfloat> suggestedRoutesCbo;
    vector< vector<GLfloat> > suggestedRoutesSrcVbos;
    vector< vector<GLfloat> > suggestedRoutesDestVbos;
    vector< vector<GLfloat> > suggestedRoutesCurveVbos;
    vector< vector<GLfloat> > suggestedEdgesVbos;
    vector<GLfloat> suggestedEdgesCbo;

    vector<GLfloat> adjMatVboForLens;
    vector<GLfloat> verticesVboForLens;
    vector< vector<GLfloat> > routesArcVbosForLens;
    vector< vector<GLfloat> > routesSrcVbosForLens;
    vector< vector<GLfloat> > routesDestVbosForLens;
    vector< vector<GLfloat> > routesCurveVbosForLens;
    vector< vector<GLfloat> > edgesVbosForLens;
    vector<GLfloat> suggestedVerticesVboForLens;
    vector< vector<GLfloat> > suggestedRoutesArcVbosForLens;
    vector< vector<GLfloat> > suggestedRoutesSrcVbosForLens;
    vector< vector<GLfloat> > suggestedRoutesDestVbosForLens;
    vector< vector<GLfloat> > suggestedRoutesCurveVbosForLens;
    vector< vector<GLfloat> > suggestedEdgesVbosForLens;

    QPointF qtPosToGlPos(QPoint qtPos);
    void showContextMenu(const QPoint &);
    int getDisplayedColSize();
    int getAdjustedBlockSize();
    int getAdjustedBlockSizeForLens();
    void setCurrentSettings();

private slots:
    void switchEdgeDrawingMode();
    void switchRouteDrawingMode();
    void switchFolding();
    void switchResolutionReduction();
    void zoomInSelectedRange();
    void resetDisplayedRange();

signals:
    void suggestAltRoutesForDisplayedRoutesCalled(int numSuggestedRoutes, int totalChangeOfLength, float totalChangeOfMaxEdgeLoad);
    void applyAltRoutesCalled();
};

#endif // MATRIXVIEW_H
