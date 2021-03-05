#pragma once

#include <vector>
#include <string>

class featureIO {
 public:
  featureIO(std::string templatePath);

  int loadTemplateFile(std::string featurePath);

  int loadTemplateVec();
 public:

  std::vector<std::vector<double>> table;
  std::vector<char> charTable;
  std::vector<std::vector<double>> alphabetTable;
  std::vector<char> alphabetCharTable;
  std::vector<std::vector<double>> numberTable;
  std::vector<char> numberCharTable;

};




