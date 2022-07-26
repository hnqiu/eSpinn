# eSpinn

eSpinn (Evolving SPIking Neural Networks) is a c++ package that simulates Spiking Neural Networks (SNNs) for control problems.


## Quick Start
```sh
git clone https://github.com/hnqiu/eSpinn.git
cd eSpinn
mkdir build && cd build
cmake ..
make
```

`cmake` options:
```sh
cmake -DCMAKE_BUILD_TYPE=Debug ..
cmake -DNDEBUG=ON .. # if run in Ndebug mode
cmake -DESPINN_VERBOSE=ON .. # if run in verbose debug mode
cmake -DUSE_IN_MATLAB=ON .. # if package is used in MATLAB
cmake -DPYTHON_EXECUTABLE="$HOME/anaconda3/bin/python" .. # if package is compiled with conda env, i.e., the pybind module will be interpreted by the specified Python instance
```

## Example: Running Flappy Bird

- [setup](game/README.md)
- `python tasks/sim_flappy.py -t`

https://user-images.githubusercontent.com/28779070/181024313-cab8a9cc-ba9e-4e0d-9e8b-2f8d3e7f2e6d.mp4


## Author
- [Huanneng Qiu](https://github.com/hnqiu)
