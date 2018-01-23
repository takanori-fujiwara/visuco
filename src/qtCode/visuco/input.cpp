///
/// This code is modified from source code developed by David Gasquez (https://github.com/davidgasquez/qap)
///

#include "input.h"

Input::Input(const string &filename)
    : filename_(filename), dimension_(0)
{

}

Input::Input(const QString &flowMatFilePath, const QString &distMatFilePath)
{
    filename_ = "";
    read(flowMatFilePath, distMatFilePath);
}

Input::Input(int dimension, vector< vector<float> > distances, vector< vector<float> > flow)
    : filename_("input")
{
    flow_ = flow;
    distances_ = distances;
    dimension_ = dimension;
}

bool Input::read()
{
    ifstream data(filename_.c_str());

    if (!data)
    {
        cerr << "ERROR: Can not open input data file: " << filename_ << endl;
        return false;
    }

    // Obtain dimension
    data >> dimension_;

    // Initialize distance and flow matrix
    distances_.resize(dimension_);
    for (int i = 0; i < dimension_; ++i)
    {
        distances_[i].resize(dimension_);
    }

    flow_.resize(dimension_);
    for (int i = 0; i < dimension_; ++i)
    {
        flow_[i].resize(dimension_);
    }

    // Fill distances and flow matrix
    for (int i = 0; i < dimension_; i++)
    {
        for (int j = 0; j < dimension_; ++j)
        {
            data >> distances_[i][j];
        }
    }

    for (int i = 0; i < dimension_; i++)
    {
        for (int j = 0; j < dimension_; ++j)
        {
            data >> flow_[i][j];
        }
    }

    return true;
}

bool Input::read(const QString &flowMatFilePath, const QString &distMatFilePath)
{
    QFile flowFile(flowMatFilePath);
    QFile distFile(distMatFilePath);

    if (flowFile.open(QIODevice::ReadOnly)) {
        flow_.clear();

        QTextStream ts(&flowFile);

        while (!ts.atEnd()) {
            vector<float> vec;

            QString line = ts.readLine();
            QStringList elements = line.split(",");

            foreach (QString elem, elements) {
                vec.push_back(elem.toFloat());
            }

            flow_.push_back(vec);
        }
        flowFile.close();
    } else {
        qWarning() << "Couldn't open edge file. file path: " << flowMatFilePath;
        return false;
    }

    if (distFile.open(QIODevice::ReadOnly)) {
        distances_.clear();

        QTextStream ts(&distFile);

        while (!ts.atEnd()) {
            vector<float> vec;

            QString line = ts.readLine();
            QStringList elements = line.split(",");

            foreach (QString elem, elements) {
                vec.push_back(elem.toFloat());
            }

            distances_.push_back(vec);
        }
        distFile.close();
    } else {
        qWarning() << "Couldn't open edge file. file path: " << distMatFilePath;
        return false;
    }

    dimension_ = distances_.size();

    return true;
}

string Input::getFilename() const
{
    return filename_;
}

int Input::getDimension() const
{
    return dimension_;
}

vector< vector<float> > Input::getDistances() const
{
    return distances_;
}

vector< vector<float> > Input::getFlow() const
{
    return flow_;
}


void Input::setDimension(int dimension) { this->dimension_ = dimension; }
void Input::setDistances(vector< vector<float> > distances) { this->distances_ = distances; }
void Input::setFlow(vector< vector<float> > flow) { this->flow_ = flow; }
