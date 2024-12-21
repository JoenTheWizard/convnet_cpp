# ConvNet in CPP

Reimplementation of my MLP in C++ as well as attempting to add convolutional layers

To get started

```
make
build/conv.exe
```

## To-do

- [X] Support multiple different activation functions (from `Activation` class)
- [ ] Add convolutional layers
- [ ] Add post-training quantization methods (for later use to be used in FPGAs)
- [ ] Add regularization techniques
    - [ ] Dropout
    - [ ] Early Stopping (save weights and stop training after no improvement)