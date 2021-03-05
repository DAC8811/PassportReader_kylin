

#include "featureIO.h"
#include "mrzChar.h"
#include "utils.h"
#include <fstream>
#include <featureIO.h>

using namespace std;

featureIO::featureIO(std::string templatePath) {
    //loadTemplateFile(templatePath);
    loadTemplateVec();

    charTable = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D',
                 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
                 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', '<'};

    alphabetTable = vector < vector < double >> (table.begin() + 10, table.end());
    alphabetCharTable = vector<char>(charTable.begin() + 10, charTable.end());
    numberTable = vector < vector < double >> (table.begin(), table.begin() + 10);
    numberCharTable = vector<char>(charTable.begin(), charTable.begin() + 10);
}

int featureIO::loadTemplateFile(std::string featurePath) {

    std::vector<std::vector<double>> featureTemplates;
    std::ifstream in;
    in.open(featurePath);

    for (int i = 0; i < 37; ++i) {
        std::vector<double> feature;
        feature.reserve(906);
        int featureSize = 900;
        for (int j = 0; j < featureSize; ++j) {
            double temp;
            in >> temp;
            feature.push_back(temp);
        }
        featureTemplates.push_back(feature);
    }
    table = featureTemplates;
    return 0;
}

int featureIO::loadTemplateVec() {
    extern std::vector<std::vector<double>> templates;
    table = templates;
    return 0;
}
