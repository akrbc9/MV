#! /bin/zsh

# eval "$(conda shell.bash hook)"
# conda activate CMPLX

export CC=/opt/homebrew/opt/llvm/bin/clang
export CXX=/opt/homebrew/opt/llvm/bin/clang++
# export CONDA_PREFIX=/opt/anaconda3/envs/CMPLX

rm -rf build
mkdir build
cd build
cmake -DCMAKE_CXX_COMPILER=/opt/homebrew/opt/llvm/bin/clang++ ..
make
