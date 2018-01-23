#ifndef COLOR_H
#define COLOR_H

#include <QVector>
#include <QColor>
#include <QDebug>

class Color
{
public:
    Color();

    QList< QPair<float, QColor> > vertexColorOfPoints;
    QList< QPair<float, QColor> > edgeColorOfPoints;
    QList< QPair<float, QColor> > routeColorOfPoints1;
    QList< QPair<float, QColor> > routeColorOfPoints2;
    QList< QPair<float, QColor> > routeColorOfPoints3;
    QList< QPair<float, QColor> > suggestedRouteColorOfPoints;
    QList<QColor> routeColors;

    QColor adjMatrixColor;
    QColor foldedRegionBackgroundColor;
    QColor foldedRegionStrokeColor;
    QColor selectionColor;
    QColor matrixSrcColor;
    QColor matrixDestColor;

    QColor ratioToHeatMapColor(float ratio, QList< QPair<float, QColor> > colorOfPoints = QList< QPair<float, QColor> >());
    static bool ratioLessThan(QPair<float, QColor>colorPoint1, QPair<float, QColor> colorPoint2);
    void sortColorPoints(QList< QPair<float, QColor> > &colorPoints);
    QList<QPair<float,QColor>> getRouteColorOfPointsForMetric(QString metric);

private:
    QColor interpolateColor(float ratio, QColor minColor, QColor maxColor);
};

#endif // COLOR_H
