#include "Color.h"

Color::Color()
{
    vertexColorOfPoints.append(qMakePair(0.1, QColor(44,123,182))); // blue
    vertexColorOfPoints.append(qMakePair(0.3, QColor(171,217,233))); // light blue
    vertexColorOfPoints.append(qMakePair(0.5, QColor(255,255,191))); // yellow
    vertexColorOfPoints.append(qMakePair(0.7, QColor(253,174,97))); // orange
    vertexColorOfPoints.append(qMakePair(0.9, QColor(215,25,28))); // red

    edgeColorOfPoints.append(qMakePair(0.1, QColor(255,255,212,255)));
    edgeColorOfPoints.append(qMakePair(0.3, QColor(254,217,142,255)));
    edgeColorOfPoints.append(qMakePair(0.5, QColor(254,153,41,255)));
    edgeColorOfPoints.append(qMakePair(0.7, QColor(217,95,14,255)));
    edgeColorOfPoints.append(qMakePair(0.9, QColor(153,52,4,255)));

    // for length
    routeColorOfPoints1.append(qMakePair(0.125, QColor(215,181,216,255)));
    routeColorOfPoints1.append(qMakePair(0.375, QColor(223,101,176,255)));
    routeColorOfPoints1.append(qMakePair(0.625, QColor(221,28,119,255)));
    routeColorOfPoints1.append(qMakePair(0.875, QColor(152,0,67,255)));

    // for byte
    routeColorOfPoints2.append(qMakePair(0.1, QColor(237,248,251,255)));
    routeColorOfPoints2.append(qMakePair(0.3, QColor(178,226,226,255)));
    routeColorOfPoints2.append(qMakePair(0.5, QColor(102,194,164,255)));
    routeColorOfPoints2.append(qMakePair(0.7, QColor(44,162,95,255)));
    routeColorOfPoints2.append(qMakePair(0.9, QColor(0,109,44,255)));

    // for hop-byte
    routeColorOfPoints3.append(qMakePair(0.1, QColor(237,248,251,255)));
    routeColorOfPoints3.append(qMakePair(0.3, QColor(179,205,227,255)));
    routeColorOfPoints3.append(qMakePair(0.5, QColor(140,150,198,255)));
    routeColorOfPoints3.append(qMakePair(0.7, QColor(136,86,167,255)));
    routeColorOfPoints3.append(qMakePair(0.9, QColor(129,15,124,255)));

    suggestedRouteColorOfPoints.append(qMakePair(0.0, QColor(110,110,255,255)));
    suggestedRouteColorOfPoints.append(qMakePair(1.0, QColor(110,110,255,255)));

    adjMatrixColor = QColor(93,93,93);
    matrixDestColor = QColor(213,62,79);
    matrixSrcColor = QColor(50,136,189);

    foldedRegionBackgroundColor = QColor(244,244,244);
    foldedRegionStrokeColor = QColor(30, 30, 30, 150);
    selectionColor = QColor(200,70,70,125);

    // for distinguishable color
    routeColors << QColor(212,42,47)
                << QColor(175,200,231)
                << QColor(139,86,76)
                << QColor(253,186,125)
                << QColor(51,159,52)
                << QColor(154,222,141)
                << QColor(147,106,187)
                << QColor(253,153,152)
                << QColor(38,120,178)
                << QColor(197,177,212)
                << QColor(253,127,40)
                << QColor(195,156,149)
                << QColor(225,122,193)
                << QColor(246,183,210)
                << QColor(127,127,127)
                << QColor(199,199,199)
                << QColor(188,188,53)
                << QColor(219,218,145)
                << QColor(41,190,206)
                << QColor(160,218,228);
}

QColor Color::ratioToHeatMapColor(float ratio, QList< QPair<float, QColor> > colorOfPoints)
{
    // TODO: need to be changed later
    QColor outOfRangeColorLow = colorOfPoints.first().second;
    QColor outOfRangeColorHigh = colorOfPoints.last().second;

    if (ratio < colorOfPoints.first().first) {
        return outOfRangeColorLow;
    }

    for (int i = 1; i < colorOfPoints.size(); ++i) {
        if (ratio < (colorOfPoints.at(i-1).first + colorOfPoints.at(i).first) * 0.5) {
            return colorOfPoints.at(i-1).second;
        }
    }

    return outOfRangeColorHigh;
}

QColor Color::interpolateColor(float ratio, QColor minColor, QColor maxColor)
{
    int r = (1.0 - ratio) * minColor.red()   + ratio * maxColor.red();
    int g = (1.0 - ratio) * minColor.green() + ratio * maxColor.green();
    int b = (1.0 - ratio) * minColor.blue()  + ratio * maxColor.blue();
    int a = (1.0 - ratio) * minColor.alpha() + ratio * maxColor.alpha();

    return QColor(r, g, b, a);
}

bool Color::ratioLessThan(QPair<float, QColor>colorPoint1, QPair<float, QColor> colorPoint2) { return colorPoint1.first < colorPoint2.first; }

void Color::sortColorPoints(QList< QPair<float, QColor> > &colorPoints)
{
    qSort(colorPoints.begin(), colorPoints.end(), ratioLessThan);
}

QList<QPair<float,QColor>> Color::getRouteColorOfPointsForMetric(QString metric)
{
    QList<QPair<float,QColor>> result;
    if (metric == "length") result = routeColorOfPoints1;
    else if (metric == "byte") result = routeColorOfPoints2;
    else if (metric == "hopbyte") result = routeColorOfPoints3;
    else qWarning() << "undefined metric: see getRouteColorOfPointsForMetric in Color class";

    return result;
}
