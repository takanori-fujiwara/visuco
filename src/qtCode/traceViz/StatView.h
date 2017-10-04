#ifndef STATVIEW_H
#define STATVIEW_H

#include <QOpenGLWidget>
#include <QPainter>
#include <QColorDialog>
#include <QMouseEvent>
#include <QComboBox>
#include <QMenu>
#include <QDebug>

#include "Graph.h"
#include "Color.h"

using namespace std;

class StatView: public QOpenGLWidget
{
    Q_OBJECT
public:
    explicit StatView(QWidget *parent = 0);
    void setGraph(Graph* graph);
    void setAggregatedGraph(Graph* aggregatedGraph);
    void setDisplayedGraph(Graph* displayedGraph);
    void setColor(Color* color);
    void setCurrentComboBoxIndex(int index);

    void updateDistriVbo();
    void updateDistriCbo();
    void updateDistriLabels();
    void updateAllDrawingInfo();
    void updateColorPointsVbos();
    void updateColorPointsCbos();
    void resetDisplayedRankingStartEnd();

public slots:
    void changeRouteColorMapSettings(QString metric, float min, float val);
    void switchGraph();

protected:
    void initializeGL();
    void paintGL();
    void resizeGL(int width, int height);

    void drawDistri();
    void paintDistriLabels();
    void drawAxis();
    void drawSelectedArea();
    void drawColorPoints();
    void drawColorMap(float x, float y, float w, float h, QList< QPair<float, QColor> > colorOfPoints);

    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

private:
    Color* color;
    Graph* graph;
    Graph* aggregatedGraph;
    Graph* displayedGraph;
    QVector<Vertex> sortedVertices;
    QVector<Route> sortedRoutes;

    int fontSize;
    float scale;
    float translateX;
    float translateY;

    QString mouseMode;

    QPoint mousePressPos;
    QPoint mouseMovePos;
    QPoint mouseReleasePos;

    float valMin;
    float valMax;
    int displayedRankingStart;
    int displayedRankingEnd;
    float thresRatioShowLabel;
    float zeroValAxisPos;
    float median;
    float medianAxisPos;
    float lowerQuantileAxisPos;
    float upperQuantileAxisPos;

    float chartMarginTop;
    float chartMarginLeft;
    float chartMarginRight;
    float chartMarginBottom;

    QComboBox* comboBox;
    const QString modeEdgeLoad = "Message sizes on links";
    const QString modeRouteLength = "Length of routes";
    const QString modeRouteByte = "Message sizes on routes";
    const QString modeRouteHopbyte = "Hop-bytes of routes";
    const QString modeNodeInDeg = "In-Degree on nodes";
    const QString modeNodeOutDeg = "Out-Degree on nodes";
    const QString modeNodeInMinusOutDeg = "(In-Deg - Out-Deg) on nodes";

    vector<GLfloat> distriVbo;
    vector<GLfloat> distriCbo;
    QVector<QString> distriLabels;
    vector<GLfloat> selectedYRange;
    vector< vector<GLfloat> > colorPointsVbosX;
    vector< vector<GLfloat> > colorPointsInsideVbosX;
    vector< vector<GLfloat> > colorPointsCbosX;

    int selectedColorIndex;
    QPointF qtPosToGlPos(QPoint qtPos);
    void showContextMenu(const QPoint &pos);
    bool isInsidePolygon(QPointF point, vector<GLfloat> selectionAreaGlBuffer);
    int selectColorPoints();
    void selectColor(int colorPointIndex);
    void changeColorPointsPos(int colorPointIndex);

private slots:
    void resetDisplayedRankingStartEndFromComboBox(QString comboBoxText);
    void updateAllDrawingInfoFromComboBox(QString comboBoxText);
    void zoomInSelectedRange();
    void resetDisplayedRange();
    void displaySelectedElementsInOtherViews();
    void emitCurrentComboBoxIndexChanged(int index);

signals:
    void displaySelectedElementsInOtherViewsCalled(QString elemType, QVector<int>selectedIndices);
    void currentComboBoxIndexChanged(int index);
    void colorChanged();
};

#endif // STATVIEW_H
