#include "lhs_sampler.hpp"
#include <random>
#include <algorithm>
#include <numeric>

LHSSampler::LHSSampler(const std::vector<ParameterRange>& ranges, int num_samples)
    : ranges_(ranges), num_samples_(num_samples) {
    initializePermutations();
}

void LHSSampler::initializePermutations() {
    permutations_.resize(ranges_.size());
    
    for (size_t i = 0; i < ranges_.size(); ++i) {
        // Create a vector of equally spaced values
        std::vector<double> values(num_samples_);
        for (int j = 0; j < num_samples_; ++j) {
            values[j] = static_cast<double>(j) / num_samples_;
        }
        
        // Shuffle the values
        std::random_device rd;
        std::mt19937 rng(rd());
        std::shuffle(values.begin(), values.end(), rng);
        
        permutations_[i] = values;
    }
}

std::vector<double> LHSSampler::generateSample() {
    std::vector<double> sample(ranges_.size());
    for (size_t i = 0; i < ranges_.size(); ++i) {
        sample[i] = mapToRange(permutations_[i][0], ranges_[i]);
    }
    return sample;
}

std::vector<std::vector<double>> LHSSampler::generateAllSamples() {
    std::vector<std::vector<double>> samples(num_samples_);
    for (int i = 0; i < num_samples_; ++i) {
        samples[i].resize(ranges_.size());
        for (size_t j = 0; j < ranges_.size(); ++j) {
            samples[i][j] = mapToRange(permutations_[j][i], ranges_[j]);
        }
    }
    return samples;
}

double LHSSampler::mapToRange(double value, const ParameterRange& range) {
    return range.min + value * (range.max - range.min);
} 