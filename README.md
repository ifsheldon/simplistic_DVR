# Assignment 4
Please be kindly noted that this is only compilable on Linux.

## Compilation Notes
To compile the code, `libglfw3` and `libglfw3-dev` should be installed by Linux's package managers, then `cmake` will find them automatically.

## Run Notes
* To run the binary, please `cd` to the build directory, since relative paths are w.r.t to the build directory.
* If you are accessing a Linux machine via VNC or RDP as I do, please run the binary using `vglrun`(e.g. `vglrun ./assignment3`).

## How to play
* Press (`A` ,`D`), (`W`, `S`) and (`Q`, `E`) to rotate objects around different axes.
* Press ArrowUp and ArrowDown to increase and decrease isovalue.
* Press `5` - `0` to load different transfer functions.
* Press `V` to enable/disable DVR.
* Press `M` to enable/disable MIP.
* Press `R` to reset position.
* Press `U` `J` to increase/decrease transfer function window lower bound.
* Press `I` `K` to increase/decrease transfer function window upper bound.
* Press `X` to enable/disable shading.
* Press `-` `=` to decrease/increase step size.
* Scroll to zoom in/out.
* Change `loadData(HEAD_DATA_PATH)` to see different visualization on lobster dataset.