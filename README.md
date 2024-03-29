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


## Prerequisite

- Boost serialization
  ```sh
  # should have already been available on Ubuntu, otherwise:
  sudo apt install libboost-all-dev
  ```
- [pybind11](https://github.com/pybind/pybind11)
  ```sh
  conda install -c conda-forge pybind11
  ```
  Refer to [this post](https://hnqiu.github.io/2020/05/19/pybind11/) for more details on installing pybind11.


## Example: Running Flappy Bird

- [setup](game/README.md)
- `python tasks/sim_flappy.py -t`

https://user-images.githubusercontent.com/28779070/182014642-b0b06a78-212d-4d45-b222-3146e83b8c26.mp4


## Citation

To use this work, please cite:

```bibtex
@InProceedings{Qiu2020,
  author    = {Qiu, Huanneng and Garratt, Matthew and Howard, David and Anavatti, Sreenatha},
  booktitle = {Proceedings of the 2020 Genetic and Evolutionary Computation Conference},
  title     = {Towards Crossing the Reality Gap with Evolved Plastic Neurocontrollers},
  year      = {2020},
  address   = {New York, NY, USA},
  pages     = {130-138},
  publisher = {Association for Computing Machinery},
  series    = {GECCO '20},
  doi       = {10.1145/3377930.3389843},
  isbn      = {9781450371285},
  keywords  = {hebbian plasticity, neuroevolution, spiking neural networks, UAV control, evolutionary robotics},
  location  = {Canc\'{u}n, Mexico},
  numpages  = {9},
  url       = {https://doi.org/10.1145/3377930.3389843},
}
```


## Author
- [Huanneng Qiu](https://github.com/hnqiu)
