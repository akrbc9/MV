#pragma once

#include <vector>
#include <random>
#include <algorithm>
#include <cmath>

class LHSSampler {
public:
    struct ParameterRange {
        double min;
        double max;
    };

    LHSSampler(const std::vector<ParameterRange>& ranges, int num_samples);
    
    // Generate a single sample configuration
    std::vector<double> generateSample();
    
    // Generate all samples
    std::vector<std::vector<double>> generateAllSamples();

private:
    std::vector<ParameterRange> ranges_;
    int num_samples_;
    std::vector<std::vector<double>> permutations_;
    
    void initializePermutations();
    double mapToRange(double value, const ParameterRange& range);
}; 