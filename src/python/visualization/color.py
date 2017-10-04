def interpolate(rgba1, rgba2, ratio):
    r = (1.0 - ratio) * rgba1[0] + ratio * rgba2[0]
    g = (1.0 - ratio) * rgba1[1] + ratio * rgba2[1]
    b = (1.0 - ratio) * rgba1[2] + ratio * rgba2[2]
    a = (1.0 - ratio) * rgba1[3] + ratio * rgba2[3]
    return [r, g, b, a]

def interpolateThreeCol(rgba1, rgba2, rgba3, ratio):
    if(ratio < 0.5):
        return interpolate(rgba1, rgba2, ratio * 2.0)
    else:
        return interpolate(rgba2, rgba3, (ratio - 0.5)* 2.0)

def ratioToHeatMapColor(ratio):
    minCol = [0.376, 0.682, 0.933, 0.5]
    midCol = [0.882, 0.882, 0.882, 0.65]
    maxCol = [0.878, 0.424, 0.459, 0.8]
    return interpolateThreeCol(minCol, midCol, maxCol, ratio)
