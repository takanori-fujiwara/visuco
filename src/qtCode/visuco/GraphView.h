#ifndef GRAPHVIEW_H
#define GRAPHVIEW_H

#include <QOpenGLWidget>
#include <QPainter>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QMenu>
#include <QInputDialog>
#include <QtCore/qmath.h>
#include <QDebug>
#include <QDialog>
#include <QLayout>
#include <QLabel>
#include <QComboBox>
#include <QPushButton>
#include <QSettings>

#include <cmath>

#include "Graph.h"
#include "Color.h"

using namespace std;

class GraphView: public QOpenGLWidget
{
    Q_OBJECT
public:
    explicit GraphView(QWidget *parent = 0);
    void setColor(Color* color);
    void setGraph(Graph* graph);
    void setAggregatedGraph(Graph* aggregatedGraph);
    void setDisplayedGraph(Graph* graph);
    void setEdgeDrawingMode(QString edgeDrawingMode);
    void setRouteDrawingMode(QString routeDrawingMode);
    void setDisplayedVertexIndices(QVector<int> displayedVertexIndices);
    void setDisplayedEdgeIndices(QVector<int> displayedEdgeIndices);
    void setDisplayedRouteIndices(QVector<int> displayedRouteIndices);
    void setHighlightedVertexIndices(QVector<int> highlightedVertexIndices);
    void setHighlightedEdgeIndices(QVector<int> highlightedEdgeIndices);
    void setHighlightedRouteIndices(QVector<int> highlightedRouteIndices);
    void setRouteColorMetric(QString metric);
    void resetDisplayedElementsIndices();
    void resetSelectedElementsIndices();
    void resetHilightedElements();
    void resetScaleAndTranslate();
    void updateVerticesVbo();
    void updateVerticesCbo();
    void updateVertexSizes();
    void updateEdgesIbo();
    void updateEdgesCbo();
    void updateEdgeWidths();
    void updateRoutesArcIbos();
    void updateRoutesCurveVbos();
    void updateRoutesCbo();
    void updateRouteWidths();
    void updateAllDrawingInfo(bool graphChanged = false);
    void updateSelectedVerticesVbo();
    void updateSelectedVerticesCbo();
    void updateSelectedVertexSizes();
    void updateSelectedEdgesIbo();
    void updateSelectedEdgesCbo();
    void updateSelectedEdgeWidths();
    void updateSelectedRoutesArcIbos();
    void updateSelectedRoutesCurveVbos();
    void updateSelectedRoutesCbo();
    void updateSelectedRouteWidths();
    void updateSelectedElementDrawingInfo();
    void updateRoutePositionsMap(QString metric);
    void updateVisualEncodingParams(float nodeSizeWeight, float nodeSizeMin, float edgeWidthWeight, float routeWidthWeight, float selfLoopRouteRatio);

public slots:
    void setMouseMode(QString mouseMode);
    void updateDisplayedElementsIndicies(QString elemType, QVector<int> indices);
    void updateHighlightedElementsIndicies(QString elemType, QVector<int> indices);
    void resetSettings();
    void updateColorsFromStatView();
    void updateBasicVisualEncodingParams(QString appName);

protected:
    void initializeGL();
    void paintGL();
    void resizeGL(int width, int height);

    void wheelEvent(QWheelEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

    void drawVertices();
    void drawEdges(QString edgeDrawingMode);
    void drawRoutes(QString routeDrawingMode);
    float basisFunc(float t);
    vector<GLfloat> calcCubicBSplineCurvePoints(vector<GLint> ibo, float curveStrength = 1.0, int numSplits = 100);
    vector<GLfloat> calcCatmullRomSplineCurvePoints(vector<GLint> ibo, int numSplits = 200);
    vector<GLfloat> genMetroPoints(vector<GLint> ibo, int routeIndex);
    void drawLasso();
    void drawColorMap(float x, float y, float w, float h, QList< QPair<float, QColor> > colorOfPoints);
    void drawColorMaps();
    void drawColorSymbols();
    void paintLabels();
    void paintPopup();
    vector<GLfloat> genCirclePoints(float cx, float cy, float r, int numSplit = 6);
    void drawCircleAroundCursor(); // for test
    void drawSelectedVertices();
    void drawSelectedRoutesAndEdges();

private:
    float nodeSizeWeight;
    float edgeWidthWeight;
    float routeWidthWeight;
    float selfLoopRouteRatio;
    float nodeSizeMin;
    float filterOutAlpha;
    float filterOutDarker;
    float nodeSizeWeightForMetro;
    bool showOnlySelectedElems;
    bool expandElemWithZoom;

    Color* color;
    Graph* graph;
    Graph* aggregatedGraph;
    Graph* displayedGraph;

    QDialog* colorMapSettingDialog;
    QComboBox* routeColorMetricComboBox;
    QLineEdit* colorMapMinValLineEdit;
    QLineEdit* colorMapMaxValLineEdit;

    float scale;
    float translateX;
    float translateY;
    QString mouseMode;
    bool isEdgeVisible;
    bool isRouteVisible;
    QString edgeDrawingMode;
    QString routeDrawingMode;
    QString defaultRouteDrawingMode;
    QString vertexDrawingMode;
    float curveStrength;

    float baseValueForRouteWidth;
    float baseValueForRouteColor;

    QString edgeWidthMetric;
    QString edgeColorMetric;
    QString routeWidthMetric;
    QString routeColorMetric;

    QVector<int> displayedVertexIndices;
    QVector<int> displayedEdgeIndices;
    QVector<int> displayedRouteIndices;
    QVector<int> selectedVertexIndices;
    QVector<int> selectedEdgeIndices;
    QVector<int> selectedRouteIndices;
    QPair<QString, int> hoveredSelectedElement;
    QVector<int> highlightedVertexIndices;
    QVector<int> highlightedEdgeIndices;
    QVector<int> highlightedRouteIndices;

    QPoint mousePressPos;
    QPoint mouseMovePos;
    QPoint mouseReleasePos;
    bool rightButtonPressed;

    vector<GLfloat> verticesVbo;
    vector<GLfloat> vertexSizes;
    vector<GLfloat> verticesCbo;
    vector<GLint>   edgesIbo;
    vector<GLfloat> edgesCbo;
    vector<GLfloat> edgeWidths;
    vector< vector<GLint> > routesArcIbos;
    vector< vector<GLfloat> > routesCurveVbos;
    vector<GLfloat> routesCbo;
    vector<GLfloat> routeWidths;
    vector<GLfloat> lassoPoints;
    vector<GLfloat> selectedVerticesVbo;
    vector<GLfloat> selectedVerticesCbo;
    vector<GLfloat> selectedVertexSizes;
    vector<GLint>   selectedEdgesIbo;
    vector<GLfloat> selectedEdgesCbo;
    vector<GLfloat> selectedEdgeWidths;
    vector< vector<GLint> > selectedRoutesArcIbos;
    vector< vector<GLfloat> > selectedRoutesCurveVbos;
    vector<GLfloat> selectedRoutesCbo;
    vector<GLfloat> selectedRouteWidths;
    QVector<bool> displayedVertexPropertyMap;
    QVector<bool> highlightedVertexPropertyMap;
    QMap<int,QMap<QVector<int>,vector<GLfloat>>> routePositionsMap;

    QPointF qtPosToGlPos(QPoint qtPos);
    QPoint glPosToQtPos(QPointF glPos);
    void showContextMenu(const QPoint &);
    bool isInsidePolygon(QPointF point, vector<GLfloat> selectionAreaGlBuffer);
    bool isInsideLasso(QPointF point);
    bool isIntersected(QPointF a, QPointF b, QPointF c, QPointF d);
    bool isIntersectedWithPolygon(QPointF a, QPointF b, vector<GLfloat> selectionAreaGlBuffer);
    bool isIntersectedWithLasso(QPointF a, QPointF b);
    QVector<int> getVertexIndicesInsideLasso();
    int getVertexIndexInsidePolygon(QVector<int> &targetVertexIndices, vector<GLfloat> selectionAreaGlBuffer);
    QVector<int> getEdgeIndicesInsideLasso();
    QVector<int> getEdgeIndicesIntersectedWithLasso();
    int getEdgeIndexIntersectedWithPolygon(QVector<int>& targetEdgeIndices, vector<GLfloat> selectionAreaGlBuffer);
    QVector<int> getRouteIndicesInsideLasso();
    QVector<int> getRouteIndicesIntersectedWithLasso();
    int getRouteIndexIntersectedWithPolygon(QVector<int>& targetRouteIndices, vector<GLfloat> selectionAreaGlBuffer, QString selectTargetType = "displayed");

    void selectElement(vector<GLfloat> selectionAreaGlBuffer, QString selectTargetType = "displayed");
    void setCurrentSettings();

private slots:
    void switchEdgeDrawingMode();
    void switchRouteDrawingMode();
    void switchVertexDrawingMode();
    void switchGraph();
    void switchGraphFromContextMenu();
    void changeCurveStrength();
    void clearSelection();
    void displaySelectedElementsInMatrixView();
    void displayElementsInsideLassoInMatrixView();
    void displayElementsPassingVertexInsideLassoInMatrixView();
    void displaySelectedElementsInOriginalGraph();
    void displayElementsRelatedVerticesInsideLasso();
    void showColorMapSettingsDialog();
    void handleColorMapSettingsDialogCancelBtn();
    void handleColorMapSettingsDialogOkBtn();

signals:
    void displaySelectedElementsInMatrixViewCalled(QString elemMode, QVector<int> indices);
    void resetSettingsCalled();
    void routeColorSettingsChanged(QString metric, float min, float max);
    void switchGraphCalled();
};

#endif // GRAPHVIEW_H
